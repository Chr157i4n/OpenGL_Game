#include "Model.h"
#include "Logger.h"


Mesh::Mesh(std::vector<Vertex>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices, Material material, Shader* shader) {
	this->material = material;
	this->shader = shader;
	this->numIndices = numIndices;

	dimension = calculateDimension(vertices);

	vertexBuffer = new VertexBuffer(vertices.data(), numVertices);
	indexBuffer = new IndexBuffer(indices.data(), numIndices, sizeof(indices[0]));

	diffuseLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.diffuse"));
	specularLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.specular"));
	emissiveLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.emissive"));
	shininessLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_material.shininess"));
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
	glUniform3fv(diffuseLocation, 1, (float*)&material.diffuse);
	glUniform3fv(specularLocation, 1, (float*)&material.specular);
	glUniform3fv(emissiveLocation, 1, (float*)&material.emissive);
	glUniform1f(shininessLocation, material.shininess);
	GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
}




void Model::init(const char* filename, Shader* shader) {
	uint64 numMeshes = 0;
	std::ifstream input = std::ifstream(filename, std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		Logger::log("Modelfile "+ std::string(filename) +" not found");
		return;
	}

	input.read((char*)&numMeshes, sizeof(uint64));

	for (uint64 i = 0; i < numMeshes; i++) {
		Material material;
		std::vector<Vertex> vertices;
		uint64 numVertices = 0;
		std::vector<uint32> indices;
		uint64 numIndices = 0;

		input.read((char*)&material, sizeof(Material));
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
			vertices.push_back(vertex);
		}
		for (uint64 i = 0; i < numIndices; i++) {
			uint32 index;
			input.read((char*)&index, sizeof(uint32));
			indices.push_back(index);
		}

		Mesh* mesh = new Mesh(vertices, numVertices, indices, numIndices, material, shader);
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

	boundingBoxDimension.x = dimension.x + dimension.y + dimension.z;
	boundingBoxDimension.y = dimension.y;
	boundingBoxDimension.z = dimension.x + dimension.y + dimension.z;


}

void Model::render()
{
	for (Mesh* mesh : meshes) {
		mesh->render();
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
