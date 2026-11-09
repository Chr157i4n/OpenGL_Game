#include "Model.h"

#include <algorithm>
#include <cctype>
#include <functional>

#include "Logger.h"
#include "Game.h"
#include "Renderer.h"

#ifdef OG_ASSIMP_ENABLED
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Load a texture file referenced by an Assimp material into a GL texture ID.
// Tries the path as-is first, then a "textures/<basename>" fallback.
// Load a texture referenced by an Assimp material into a GL texture ID.
// Handles embedded GLB textures (Assimp path "*N") as well as file paths.
static GLuint loadAssimpTexture(const std::string& path, const aiScene* scene) {
    int width = 0, height = 0, channels = 0;
    unsigned char* data = nullptr;

    // Embedded texture: Assimp reports the path as "*<index>" (1-based) and
    // stores the encoded image bytes in scene->mTextures[index-1].
    if (!path.empty() && path[0] == '*') {
        size_t idx = 0;
        try {
            idx = static_cast<size_t>(std::stoul(path.substr(1)));
        } catch (...) {
            idx = 0;
        }
        if (idx < scene->mNumTextures) {
            const aiTexture* tex = scene->mTextures[idx];
            if (tex && tex->pcData) {
                // Compressed embedded texture: mHeight == 0, mWidth = byte count.
                data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(tex->pcData),
                                           static_cast<int>(tex->mWidth),
                                           &width, &height, &channels, 4);
            }
        }
    } else {
        std::vector<std::string> candidates;
        candidates.push_back(path);

        size_t slash = path.find_last_of("\\/");
        std::string base = (slash != std::string::npos) ? path.substr(slash + 1) : path;
        candidates.push_back("textures/" + base);

        for (const std::string& candidate : candidates) {
            data = stbi_load(candidate.c_str(), &width, &height, &channels, 4);
            if (data) {
                break;
            }
        }
    }

    if (data) {
        GLuint texture = 0;
        GLCALL(glGenTextures(1, &texture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        stbi_image_free(data);
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
        return texture;
    }

    Logger::log("Assimp: texture not found: " + path);
    return 0;
}

bool Model::initAssimp(const char* filename, Shader* shader) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename,
        // aiProcess_OptimizeGraph is incompatible with aiProcess_PreTransformVertices
        // (Assimp aborts with an ai_assert if both are set). PreTransformVertices already
        // flattens the scene graph, so OptimizeGraph is unnecessary here.
        aiProcess_PreTransformVertices |
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_OptimizeMeshes |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_CalcTangentSpace);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        Logger::log("Assimp failed to load model " + std::string(filename) + ": " + importer.GetErrorString());
        return false;
    }

    stbi_set_flip_vertically_on_load(true);

    // ---- Materials ----
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* mat = scene->mMaterials[i];

        Material material = {};

        aiColor3D diffuse(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
            material.material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        }

        aiColor3D specular(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, specular)) {
            material.material.specular = glm::vec3(specular.r, specular.g, specular.b);
        }

        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)) {
            material.material.emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
        }

        float shininess = 0.0f;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess)) {
            material.material.shininess = shininess;
        }

        float shininessStrength = 1.0f;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength)) {
            material.material.specular *= shininessStrength;
        }

        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString diffusePath;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
            std::string path = diffusePath.C_Str();
            if (path.find(".png") != std::string::npos) {
                hasTransparentTexture = true;
            }
            material.diffuseMap = loadAssimpTexture(path, scene);
        }

        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
            aiString normalPath;
            mat->GetTexture(aiTextureType_NORMALS, 0, &normalPath);
            material.normalMap = loadAssimpTexture(normalPath.C_Str(), scene);
        }

        materials.push_back(material);
    }

    // ---- Meshes ----
    std::function<void(aiNode*)> processNode = [&](aiNode* node) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];

            std::vector<Vertex> vertices;
            std::vector<uint32> indices;

            for (unsigned int v = 0; v < aiMesh->mNumVertices; v++) {
                Vertex vertex;
                vertex.position = glm::vec3(aiMesh->mVertices[v].x, aiMesh->mVertices[v].y, aiMesh->mVertices[v].z);

                if (aiMesh->mNormals) {
                    vertex.normal = glm::vec3(aiMesh->mNormals[v].x, aiMesh->mNormals[v].y, aiMesh->mNormals[v].z);
                } else {
                    vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
                }

                if (aiMesh->mTangents) {
                    vertex.tangent = glm::vec3(aiMesh->mTangents[v].x, aiMesh->mTangents[v].y, aiMesh->mTangents[v].z);
                } else {
                    vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                }

                if (aiMesh->mTextureCoords[0]) {
                    vertex.textureCoord = glm::vec2(aiMesh->mTextureCoords[0][v].x, aiMesh->mTextureCoords[0][v].y);
                } else {
                    vertex.textureCoord = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
            }

            for (unsigned int f = 0; f < aiMesh->mNumFaces; f++) {
                const aiFace& face = aiMesh->mFaces[f];
                for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            uint64 materialIndex = aiMesh->mMaterialIndex;
            if (materialIndex >= materials.size()) {
                materialIndex = 0;
            }

            Mesh* mesh = new Mesh(vertices, vertices.size(), indices, indices.size(), materials[materialIndex], shader);
            meshes.push_back(mesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i]);
        }
    };

    processNode(scene->mRootNode);

    if (meshes.empty()) {
        Logger::log("Assimp: no meshes found in model " + std::string(filename));
        return false;
    }

    glm::vec3 max = glm::vec3(0, 0, 0);
    glm::vec3 min = glm::vec3(0, 0, 0);

    for (Mesh* mesh : meshes) {
        glm::vec3 d = mesh->getDimension();
        if (d.x > max.x) max.x = d.x;
        if (d.y > max.y) max.y = d.y;
        if (d.z > max.z) max.z = d.z;
        if (d.x < min.x) min.x = d.x;
        if (d.y < min.y) min.y = d.y;
        if (d.z < min.z) min.z = d.z;
    }

    dimension = max - min;
    boundingBoxDimension.x = dimension.x + dimension.z;
    boundingBoxDimension.y = dimension.y * 2;
    boundingBoxDimension.z = dimension.x + dimension.z;

    return true;
}
#endif


Mesh::Mesh(std::vector<Vertex>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices, Material material, Shader* shader) {
	this->material = material;
	this->shader = shader;
	this->numIndices = numIndices;

	dimension = calculateDimension(vertices);

	vertexBuffer = new VertexBuffer(vertices.data(), numVertices, VertexType::_Vertex);
	indexBuffer = new IndexBuffer(indices.data(), numIndices, sizeof(indices[0]));

	diffuseLocation =		GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.diffuse"));
	specularLocation =		GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.specular"));
	emissiveLocation =		GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.emissive"));
	shininessLocation =		GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.shininess"));
	diffuseMapLocation =	GLCALL(glGetUniformLocation(shader->getShaderId(), "u_diffuse_map"));
	normalMapLocation =		GLCALL(glGetUniformLocation(shader->getShaderId(), "u_normal_map"));
}

Mesh::~Mesh() {
	delete vertexBuffer;
	delete indexBuffer;
}

glm::vec3 Mesh::calculateDimension(std::vector<Vertex>& vertices)
{
	glm::vec3 max = glm::vec3(0, 0, 0);
	glm::vec3 min = glm::vec3(0, 0, 0);


	for (Vertex vertex : vertices)
	{
		if (vertex.position.x > max.x) max.x = vertex.position.x;
		if (vertex.position.y > max.y) max.y = vertex.position.y;
		if (vertex.position.z > max.z) max.z = vertex.position.z;

		if (vertex.position.x < min.x) min.x = vertex.position.x;
		if (vertex.position.y < min.y) min.y = vertex.position.y;
		if (vertex.position.z < min.z) min.z = vertex.position.z;
	}

	glm::vec3 dimension_c = max - min;

	return dimension_c;
}

glm::vec3 Mesh::getDimension()
{
	return dimension;
}

inline void Mesh::render()
{
	vertexBuffer->bind();
	indexBuffer->bind();
	glUniform3fv(diffuseLocation, 1, (float*)&material.material.diffuse);
	glUniform3fv(specularLocation, 1, (float*)&material.material.specular);
	glUniform3fv(emissiveLocation, 1, (float*)&material.material.emissive);
	glUniform1f(shininessLocation, material.material.shininess);

	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, material.diffuseMap));
	GLCALL(glUniform1i(diffuseMapLocation, 0));

	GLCALL(glActiveTexture(GL_TEXTURE1));
	GLCALL(glBindTexture(GL_TEXTURE_2D, material.normalMap));
	GLCALL(glUniform1i(normalMapLocation, 1));

	GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));

}

inline void Mesh::renderShadowMap()
{
	vertexBuffer->bind();
	indexBuffer->bind();
	
	GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
}

inline void Mesh::renderEnvMap()
{
	vertexBuffer->bind();
	indexBuffer->bind();
	int diffuseLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_material.diffuse"));
	glUniform3fv(diffuseLocation, 1, (float*)&material.material.diffuse);
	int specularLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_material.specular"));
	glUniform3fv(specularLocation, 1, (float*)&material.material.specular);
	int emissiveLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_material.emissive"));
	glUniform3fv(emissiveLocation, 1, (float*)&material.material.emissive);
	int shininessLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_material.shininess"));
	glUniform1f(shininessLocation, material.material.shininess);

	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, material.diffuseMap));
	int diffuseMapLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_diffuse_map"));
	GLCALL(glUniform1i(diffuseMapLocation, 0));

	GLCALL(glActiveTexture(GL_TEXTURE1));
	GLCALL(glBindTexture(GL_TEXTURE_2D, material.normalMap));
	int normalMapLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::envmap)->getShaderId(), "u_normal_map"));
	GLCALL(glUniform1i(normalMapLocation, 1));

	GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));

}




bool Model::init(const char* filename, Shader* shader) {
    std::string f(filename);
    // Lowercase the extension for a robust comparison.
    std::string ext;
    size_t dot = f.find_last_of('.');
    if (dot != std::string::npos) {
        ext = f.substr(dot + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }

#ifdef OG_ASSIMP_ENABLED
    if (ext != "bmf") {
        return initAssimp(filename, shader);
    }
#endif

    return initBMF(filename, shader);
}

bool Model::initBMF(const char* filename, Shader* shader) {
	uint64 numMeshes = 0;
	uint64 numMaterials = 0;
	std::ifstream input = std::ifstream(filename, std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		Logger::log("Modelfile "+ std::string(filename) +" not found");
		return false;
	}

	// Materials
	input.read((char*)&numMaterials, sizeof(uint64));
	for (uint64 i = 0; i < numMaterials; i++) {
		Material material = {};
		input.read((char*)&material, sizeof(BMFMaterial));

		uint64 diffuseMapNameLength = 0;
		input.read((char*)&diffuseMapNameLength, sizeof(uint64));
		std::string diffuseMapName(diffuseMapNameLength, '\0');
		input.read((char*)&diffuseMapName[0], diffuseMapNameLength);
		std::replace(diffuseMapName.begin(), diffuseMapName.end(), '\\', '/');


		if (diffuseMapName.find(".png") != std::string::npos) {
			hasTransparentTexture = true;
		}

		uint64 normalMapNameLength = 0;
		input.read((char*)&normalMapNameLength, sizeof(uint64));
		std::string normalMapName(normalMapNameLength, '\0');
		input.read((char*)&normalMapName[0], normalMapNameLength);
		std::replace(normalMapName.begin(), normalMapName.end(), '\\', '/');

		assert(diffuseMapNameLength > 0);
		assert(normalMapNameLength > 0);

		int32 textureWidth = 0;
		int32 textureHeight = 0;
		int32 bitsPerPixel = 0;
		GLCALL(glGenTextures(2, &material.diffuseMap));
		stbi_set_flip_vertically_on_load(true);
		{
			auto textureBuffer = stbi_load(diffuseMapName.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4);
			assert(textureBuffer);
			assert(material.diffuseMap);

			GLCALL(glBindTexture(GL_TEXTURE_2D, material.diffuseMap));

			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

			GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer));

			if (textureBuffer) {
				stbi_image_free(textureBuffer);
			}
		}

		{
			auto textureBuffer = stbi_load(normalMapName.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4);
			assert(textureBuffer);
			assert(material.normalMap);

			GLCALL(glBindTexture(GL_TEXTURE_2D, material.normalMap));

			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

			GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer));

			if (textureBuffer) {
				stbi_image_free(textureBuffer);
			}
		}

		GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
		materials.push_back(material);
	}


	// Meshes
	input.read((char*)&numMeshes, sizeof(uint64));

	for (uint64 i = 0; i < numMeshes; i++) {
		std::vector<Vertex> vertices;
		uint64 numVertices = 0;
		std::vector<uint32> indices;
		uint64 numIndices = 0;
		uint64 materialIndex = 0;

		input.read((char*)&materialIndex, sizeof(uint64));
		input.read((char*)&numVertices, sizeof(uint64));
		input.read((char*)&numIndices, sizeof(uint64));

		for (uint64 i = 0; i < numVertices; i++) {
			Vertex vertex;
			input.read((char*)&vertex.position.x, sizeof(float));
			input.read((char*)&vertex.position.y, sizeof(float));
			input.read((char*)&vertex.position.z, sizeof(float));
			input.read((char*)&vertex.normal.x, sizeof(float));
			input.read((char*)&vertex.normal.y, sizeof(float));
			input.read((char*)&vertex.normal.z, sizeof(float));
			input.read((char*)&vertex.tangent.x, sizeof(float));
			input.read((char*)&vertex.tangent.y, sizeof(float));
			input.read((char*)&vertex.tangent.z, sizeof(float));
			input.read((char*)&vertex.textureCoord.x, sizeof(float));
			input.read((char*)&vertex.textureCoord.y, sizeof(float));
			vertices.push_back(vertex);
		}
		for (uint64 i = 0; i < numIndices; i++) {
			uint32 index;
			input.read((char*)&index, sizeof(uint32));
			indices.push_back(index);
		}

		Mesh* mesh = new Mesh(vertices, numVertices, indices, numIndices, materials[materialIndex], shader);
		meshes.push_back(mesh);
	}


	glm::vec3 max = glm::vec3(0, 0, 0);
	glm::vec3 min = glm::vec3(0, 0, 0);

	for (Mesh* mesh : meshes)
	{
		if (mesh->getDimension().x > max.x) max.x = mesh->getDimension().x;
		if (mesh->getDimension().y > max.y) max.y = mesh->getDimension().y;
		if (mesh->getDimension().z > max.z) max.z = mesh->getDimension().z;

		if (mesh->getDimension().x < min.x) min.x =mesh->getDimension().x;
		if (mesh->getDimension().y < min.y) min.y =mesh->getDimension().y;
		if (mesh->getDimension().z < min.z) min.z =mesh->getDimension().z;
			
	}
	dimension = max - min;

	boundingBoxDimension.x = dimension.x + dimension.z;
	boundingBoxDimension.y = dimension.y*2;
	boundingBoxDimension.z = dimension.x + dimension.z;

	return true;

}

void Model::render()
{
	for (Mesh* mesh : meshes) {
		mesh->render();
	}
}

void Model::renderShadowMap()
{
	for (Mesh* mesh : meshes) {
		mesh->renderShadowMap();
	}
}

void Model::renderEnvMap()
{
	for (Mesh* mesh : meshes) {
		mesh->renderEnvMap();
	}
}

glm::vec3 Model::getDimension()
{
	return dimension;
}

glm::vec3 Model::getBoundingBoxDimension()
{
	return boundingBoxDimension;
}

Model::~Model() {
	for (Mesh* mesh : meshes) {
		delete mesh;
	}
}

void Model::setModelName(std::string newModelName)
{
	modelName = newModelName;
}

std::string Model::getModelName()
{
	return modelName;
}

void Model::setModelID(int newModelID)
{
	modelID = newModelID;
}

int Model::getModelID()
{
	return modelID;
}
