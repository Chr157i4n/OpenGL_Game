#include "Model.h"

#include "Logger.h"
#include "Game.h"
#include "Renderer.h"

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


		if (diffuseMapName.find(".png") != std::string::npos) {
			hasTransparentTexture = true;
		}

		uint64 normalMapNameLength = 0;
		input.read((char*)&normalMapNameLength, sizeof(uint64));
		std::string normalMapName(normalMapNameLength, '\0');
		input.read((char*)&normalMapName[0], normalMapNameLength);

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
