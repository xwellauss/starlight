#pragma once

#include "../platform.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	bool indexed;
} VertexBuffer;

typedef struct { char* key; VertexBuffer value; }* VertexBufferHashMap;

enum Buffers
{
	BUFFER_VAO,
	BUFFER_VBO,
	BUFFER_EBO
};

void vertex_buffer_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void vertex_buffer_bind(VertexBuffer* vb, enum Buffers buffer);
void vertex_buffer_unbind(enum Buffers buffer);
void vertex_buffer_unbind_all();
void vertex_buffer_update(VertexBuffer* vb, void* data, long size, int offset);
void vertex_buffer_draw(VertexBuffer* vb, GLenum mode, int count, int offset);
void vertex_buffer_draw_indexed(VertexBuffer* vb, GLenum mode, GLenum type, int count, void* offset);
void vertex_buffer_destroy(VertexBuffer* vb);

