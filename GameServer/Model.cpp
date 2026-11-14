#include "Model.h"

#include "Geometry.h"
#include "Logger.h"

#ifdef OG_ASSIMP_ENABLED
	#include <assimp/Importer.hpp>
	#include <assimp/scene.h>
	#include <assimp/postprocess.h>
#endif

bool Model::init(const char* filename)
{
	if (!filename)
	{
		return false;
	}

#ifdef OG_ASSIMP_ENABLED
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		filename,
		aiProcess_Triangulate | aiProcess_GenNormals
	);

	if (!scene || !scene->mRootNode || scene->mNumMeshes == 0)
	{
		Logger::log("Could not load model " + std::string(filename));
		return false;
	}

	std::vector<glm::vec3> vertices;

	for (uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		const aiMesh* mesh = scene->mMeshes[meshIndex];

		if (!mesh || mesh->mNumVertices == 0)
		{
			continue;
		}

		for (uint32 vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			const aiVector3D& vertex = mesh->mVertices[vertexIndex];
		 vertices.emplace_back(vertex.x, vertex.y, vertex.z);
		}
	}

	if (vertices.empty())
	{
		Logger::log("Model " + std::string(filename) + " has no vertices");
		return false;
	}

	dimension = calculateMeshBoundingBox(vertices);
	boundingBoxDimension = calculateModelBoundingBox(vertices);

	for (uint32 materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		const aiMaterial* material = scene->mMaterials[materialIndex];

		if (!material) continue;

		// Texture paths are not needed on the headless server, but the old BMF
		// parser marked .png diffuse textures as transparent.
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                std::string texturePath(path.C_Str());

                if (texturePath.find(".png") != std::string::npos)
                {
                    hasTransparentTexture = true;
                }
            }
        }
	}

	return true;
#else
	Logger::log("Assimp is not linked into GameServer; using fallback dimensions for model " + std::string(filename));

	dimension = glm::vec3(2, 2, 2);
	boundingBoxDimension = glm::vec3(4, 4, 4);
	hasTransparentTexture = false;

	return true;
#endif
}