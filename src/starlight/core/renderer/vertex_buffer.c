#include <starlight/core/renderer/vertex_buffer.h>
#include <starlight/utils/logger.h>

#include "gl_platform.h"

static GLenum attrib_type_to_gl(AttribType type)
{
	switch(type)
    {
        case ATTRIB_TYPE_FLOAT:  return GL_FLOAT;
        case ATTRIB_TYPE_INT:    return GL_INT;
        case ATTRIB_TYPE_UINT:   return GL_UNSIGNED_INT;
        case ATTRIB_TYPE_BYTE:   return GL_BYTE;
        case ATTRIB_TYPE_UBYTE:  return GL_UNSIGNED_BYTE;
        case ATTRIB_TYPE_SHORT:  return GL_SHORT;
        case ATTRIB_TYPE_USHORT: return GL_UNSIGNED_SHORT;
        default:                 return GL_FLOAT;
    }
}

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
		glVertexAttribPointer(vat->location, vat->count, attrib_type_to_gl(vat->type), GL_FALSE, layout.stride, (void*)vat->offset);
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
		{0, 3, ATTRIB_TYPE_FLOAT, offsetof(Vertex3D, position)},
		{1, 4, ATTRIB_TYPE_FLOAT, offsetof(Vertex3D, color)},
		{2, 2, ATTRIB_TYPE_FLOAT, offsetof(Vertex3D, tex_coord)},
		{3, 3, ATTRIB_TYPE_FLOAT, offsetof(Vertex3D, normal)},
	};

	VertexLayout layout = {attribs, 4, sizeof(Vertex3D)};
	vertex_buffer_init_with_layout(vb, vertex_data, vertex_data_size, index_data, index_data_size, indexed, layout);
}

void vertex_buffer_2d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	VertexAttrib attribs[] =
	{
		{0, 2, ATTRIB_TYPE_FLOAT, offsetof(Vertex2D, position)},
		{1, 4, ATTRIB_TYPE_FLOAT, offsetof(Vertex2D, color)},
		{2, 2, ATTRIB_TYPE_FLOAT, offsetof(Vertex2D, tex_coord)},
	};

	VertexLayout layout = {attribs, 3, sizeof(Vertex2D)};
	vertex_buffer_init_with_layout(vb, vertex_data, vertex_data_size, index_data, index_data_size, indexed, layout);
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
	vertex_buffer_bind(vb, BUFFER_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void vertex_buffer_draw(VertexBuffer* vb, DrawPrimitive primitive, int count, int offset)
{
	vertex_buffer_bind(vb, BUFFER_VAO);

	switch(primitive)
	{
		case PRIMITIVE_TRIANGLES: glDrawArrays(GL_TRIANGLES, offset, count); break;
		case PRIMITIVE_TRIANGLE_STRIP: glDrawArrays(GL_TRIANGLE_STRIP, offset, count); break;
		case PRIMITIVE_TRIANGLE_FAN: glDrawArrays(GL_TRIANGLE_FAN, offset, count); break;
		case PRIMITIVE_LINES: glDrawArrays(GL_LINES, offset, count); break;
		case PRIMITIVE_LINE_STRIP: glDrawArrays(GL_LINE_STRIP, offset, count); break;
		case PRIMITIVE_LINE_LOOP: glDrawArrays(GL_LINE_LOOP, offset, count); break;
		case PRIMITIVE_POINTS: glDrawArrays(GL_POINTS, offset, count); break;
	}
}

void vertex_buffer_draw_indexed(VertexBuffer* vb, DrawPrimitive primitive, int count, void* offset)
{
	vertex_buffer_bind(vb, BUFFER_VAO);

	switch(primitive)
	{
		case PRIMITIVE_TRIANGLES: glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_TRIANGLE_STRIP: glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_TRIANGLE_FAN: glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_LINES: glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_LINE_STRIP: glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_LINE_LOOP: glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_INT, offset); break;
		case PRIMITIVE_POINTS: glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, offset); break;
	}
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

