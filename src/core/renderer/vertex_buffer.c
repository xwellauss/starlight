#include "vertex_buffer.h"
#include "../../utils/utils.h"

void vertex_buffer_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	vb->indexed = indexed;

	glGenVertexArrays(1, &vb->VAO);
	vertex_buffer_bind(vb, BUFFER_VAO);

	glGenBuffers(1, &vb->VBO);
	vertex_buffer_bind(vb, BUFFER_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);

	if(vb->indexed)
	{
		glGenBuffers(1, &vb->EBO);
		vertex_buffer_bind(vb, BUFFER_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_DYNAMIC_DRAW);
	}

	vertex_buffer_unbind_all();
}

void vertex_buffer_bind(VertexBuffer* vb, enum Buffers buffer)
{
	switch(buffer)
	{
		case BUFFER_VAO:
			glBindVertexArray(vb->VAO);
			break;
		case BUFFER_VBO:
			glBindBuffer(GL_ARRAY_BUFFER, vb->VBO);
			break;
		case BUFFER_EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->EBO);
			break;
	}
}

void vertex_buffer_unbind(enum Buffers buffer)
{
	switch(buffer)
	{
		case BUFFER_VAO:
			glBindVertexArray(0);
			break;
		case BUFFER_VBO:
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			break;
		case BUFFER_EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			break;
	}
}

void vertex_buffer_unbind_all()
{
	vertex_buffer_unbind(BUFFER_VAO);
	vertex_buffer_unbind(BUFFER_VBO);
	vertex_buffer_unbind(BUFFER_EBO);
}

void vertex_buffer_update(VertexBuffer* vb, void* data, long size, int offset)
{
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void vertex_buffer_draw(VertexBuffer* vb, GLenum mode, int count, int offset)
{
	glDrawArrays(mode, offset, count);
}

void vertex_buffer_draw_indexed(VertexBuffer* vb, GLenum mode, GLenum type, int count, void* offset)
{
	glDrawElements(mode, count, type, offset);
}


void vertex_buffer_destroy(VertexBuffer* vb)
{
	glDeleteVertexArrays(1, &vb->VAO);
	glDeleteBuffers(1, &vb->VBO);

	if(vb->indexed)
	{
		glDeleteBuffers(1, &vb->EBO);
	}
}

