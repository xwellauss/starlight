#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <cglm/struct.h>
#include <stdint.h>

typedef enum
{
	PRIMITIVE_TRIANGLES = 0,
	PRIMITIVE_TRIANGLE_STRIP,
	PRIMITIVE_TRIANGLE_FAN,
	PRIMITIVE_LINES,
	PRIMITIVE_LINE_STRIP,
	PRIMITIVE_LINE_LOOP,
	PRIMITIVE_POINTS
} DrawPrimitive;

typedef enum
{
	ATTRIB_TYPE_FLOAT,
    ATTRIB_TYPE_INT,
    ATTRIB_TYPE_UINT,
    ATTRIB_TYPE_BYTE,
    ATTRIB_TYPE_UBYTE,
    ATTRIB_TYPE_SHORT,
    ATTRIB_TYPE_USHORT
} AttribType;

typedef struct
{
	int location;
	int count;
	AttribType type;
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

void vertex_buffer_3d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void vertex_buffer_2d_init(VertexBuffer* vb, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);


void vertex_buffer_bind(VertexBuffer* vb, enum Buffers buffer);
void vertex_buffer_unbind(enum Buffers buffer);
void vertex_buffer_unbind_all();

void vertex_buffer_update(VertexBuffer* vb, void* data, long size, int offset);
void vertex_buffer_draw(VertexBuffer* vb, DrawPrimitive primitive, int count, int offset);
void vertex_buffer_draw_indexed(VertexBuffer* vb, DrawPrimitive primitive, int count, void* offset);

void vertex_buffer_destroy(VertexBuffer* vb);

