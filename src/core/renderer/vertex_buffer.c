#include "vertex_buffer.h"
#include "../../utils/utils.h"

void vertex_buffer_init_with_layout(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed, VertexLayout layout)
{
	vb->indexed = indexed;

	glGenVertexArrays(1, &vb->VAO);
	vertex_buffer_bind(vb, BUFFER_VAO);

	glGenBuffers(1, &vb->VBO);
	vertex_buffer_bind(vb, BUFFER_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_DYNAMIC_DRAW);

	for(int i = 0; i < layout.attrib_count; i++)
	{
		VertexAttrib* vat = &layout.attribs[i];
		glVertexAttribPointer(vat->location, vat->count, vat->type, GL_FALSE, layout.stride, (void*)vat->offset);
		glEnableVertexAttribArray(vat->location);
	}

	if(vb->indexed)
	{
		glGenBuffers(1, &vb->EBO);
		vertex_buffer_bind(vb, BUFFER_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_DYNAMIC_DRAW);
	}

	vertex_buffer_unbind_all();
}

void vertex_buffer_3d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	VertexAttrib attribs[] =
	{
		{0, 3, GL_FLOAT, offsetof(Vertex3D, position)},
		{1, 4, GL_FLOAT, offsetof(Vertex3D, color)},
		{2, 2, GL_FLOAT, offsetof(Vertex3D, tex_coord)},
		{3, 3, GL_FLOAT, offsetof(Vertex3D, normal)},
	};

	VertexLayout layout = {attribs, 4, sizeof(Vertex3D)};
	vertex_buffer_init_with_layout(vb, vertex_data, vertex_data_size, index_data, index_data_size, indexed, layout);
}

void vertex_buffer_2d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	VertexAttrib attribs[] =
	{
		{0, 2, GL_FLOAT, offsetof(Vertex2D, position)},
		{1, 4, GL_FLOAT, offsetof(Vertex2D, color)},
		{2, 2, GL_FLOAT, offsetof(Vertex2D, tex_coord)},
	};

	VertexLayout layout = {attribs, 3, sizeof(Vertex2D)};
	vertex_buffer_init_with_layout(vb, vertex_data, vertex_data_size, index_data, index_data_size, indexed, layout);
}

// TODO: Compatibility
void vertex_buffer_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	vertex_buffer_3d_init(vb, vertex_data, vertex_data_size, index_data, index_data_size, indexed);
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

