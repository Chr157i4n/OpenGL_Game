#pragma once

#include "defines.h"
#include <GL/glew.h>
#include <stddef.h>
#include <vector>


struct VertexBuffer
{
	VertexBuffer(void *data, uint32 numVertices)
	{
		this->numVertices = numVertices;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


		glBindVertexArray(0);
	}

	virtual ~VertexBuffer()
	{
		glDeleteBuffers(1, &bufferId);
	}

	void bind()
	{
		glBindVertexArray(vao);
	}

	void unbind()
	{
		glBindVertexArray(0);
	}

	/*int64 getNumVertices()
	{
		return numVertices;
	}*/

	/*std::vector<Vertex> getBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);

		std::vector<Vertex> data;
		data.resize(numVertices);

		glGetBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex), &data);

		return data;
	}

	void updateBuffer(void* data) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex), data);
	}*/

private:
	GLuint bufferId;
	GLuint vao;
	int64 numVertices;
};

