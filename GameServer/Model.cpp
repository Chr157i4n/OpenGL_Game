#include "Model.h"

#include "Logger.h"
#include "Game.h"

Mesh::Mesh(std::vector<glm::vec3>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices)
{	
	this->material = material;
	this->numIndices = numIndices;

	dimension = calculateDimension(vertices);
}


glm::vec3 Mesh::calculateDimension(std::vector<glm::vec3>& vertices)
{
	glm::vec3 max = glm::vec3(0, 0, 0);
	glm::vec3 min = glm::vec3(0, 0, 0);


	for (glm::vec3 vertex : vertices)
	{
		if (vertex.x > max.x) max.x = vertex.x;
		if (vertex.y > max.y) max.y = vertex.y;
		if (vertex.z > max.z) max.z = vertex.z;

		if (vertex.x < min.x) min.x = vertex.x;
		if (vertex.y < min.y) min.y = vertex.y;
		if (vertex.z < min.z) min.z = vertex.z;
	}

	glm::vec3 dimension_c = max - min;

	return dimension_c;
}

glm::vec3 Mesh::getDimension()
{
	return dimension;
}




bool Model::init(const char* filename) {
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
	}


	// Meshes
	input.read((char*)&numMeshes, sizeof(uint64));

	for (uint64 i = 0; i < numMeshes; i++) {
		std::vector<glm::vec3> vertices;
		uint64 numVertices = 0;
		std::vector<uint32> indices;
		uint64 numIndices = 0;
		uint64 materialIndex = 0;

		input.read((char*)&materialIndex, sizeof(uint64));
		input.read((char*)&numVertices, sizeof(uint64));
		input.read((char*)&numIndices, sizeof(uint64));

		for (uint64 i = 0; i < numVertices; i++) {
			glm::vec3 vertex;
			float skipthisvalue;
			input.read((char*)&vertex.x, sizeof(float));
			input.read((char*)&vertex.y, sizeof(float));
			input.read((char*)&vertex.z, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			input.read((char*)&skipthisvalue, sizeof(float));
			vertices.push_back(vertex);
		}
		for (uint64 i = 0; i < numIndices; i++) {
			uint32 index;
			input.read((char*)&index, sizeof(uint32));
			indices.push_back(index);
		}

		Mesh* mesh = new Mesh(vertices, numVertices, indices, numIndices);
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

glm::vec3 Model::getDimension()
{
	return dimension;
}

glm::vec3 Model::getBoundingBoxDimension()
{
	return boundingBoxDimension;
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
