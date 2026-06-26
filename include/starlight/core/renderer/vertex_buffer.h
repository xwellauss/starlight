#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <cglm/struct.h>
#include <stdint.h>

typedef struct
{
	int location;
	int count;
	uint32_t type;
	size_t offset;
} VertexAttrib;

typedef struct
{
	VertexAttrib* attribs;
	int attrib_count;
	size_t stride;
} VertexLayout;

typedef struct
{
	vec3s position;
	vec4s color;
	vec2s tex_coord;
	vec3s normal;
} Vertex3D;

typedef Vertex3D Vertex;

typedef struct
{
	vec2s position;
	vec4s color;
	vec2s tex_coord;
} Vertex2D;

typedef struct
{
	Vertex2D vertices[4];
} Vertex2DQuad;

typedef struct
{
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	bool indexed;
} VertexBuffer;

typedef struct { char* key; VertexBuffer value; }* VertexBufferHashMap;

enum Buffers
{
	BUFFER_VAO,
	BUFFER_VBO,
	BUFFER_EBO
};

void vertex_buffer_init_with_layout(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed, VertexLayout layout);

void vertex_buffer_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void vertex_buffer_3d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void vertex_buffer_2d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);


void vertex_buffer_bind(VertexBuffer* vb, enum Buffers buffer);
void vertex_buffer_unbind(enum Buffers buffer);
void vertex_buffer_unbind_all();

void vertex_buffer_update(VertexBuffer* vb, void* data, long size, int offset);
void vertex_buffer_draw(VertexBuffer* vb, uint32_t mode, int count, int offset);
void vertex_buffer_draw_indexed(VertexBuffer* vb, uint32_t mode, uint32_t type, int count, void* offset);

void vertex_buffer_destroy(VertexBuffer* vb);

