#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <cglm/struct.h>

typedef uint8_t NetworkByte;

typedef struct
{
	NetworkByte* buffer;
	size_t capacity;
	size_t cursor;
} NetworkWriter;

typedef struct
{
	const NetworkByte* buffer;
	size_t capacity;
	size_t cursor;
} NetworkReader;

// Writer
void network_writer_init(NetworkWriter* writer, NetworkByte* buffer, size_t capacity);
size_t network_writer_remaining(NetworkWriter* writer);
void network_writer_reset(NetworkWriter* writer);

bool network_write_u8(NetworkWriter* writer, uint8_t value);
bool network_write_i8(NetworkWriter* writer, int8_t value);
bool network_write_u16(NetworkWriter* writer, uint16_t value);
bool network_write_i16(NetworkWriter* writer, int16_t value);
bool network_write_u32(NetworkWriter* writer, uint32_t value);
bool network_write_i32(NetworkWriter* writer, int32_t value);
bool network_write_u64(NetworkWriter* writer, uint64_t value);
bool network_write_i64(NetworkWriter* writer, int64_t value);

bool network_write_f32(NetworkWriter* writer, float value);
bool network_write_f64(NetworkWriter* writer, double value);

bool network_write_bool(NetworkWriter* writer, bool value);

bool network_write_vec2(NetworkWriter* writer, vec2s value);
bool network_write_vec3(NetworkWriter* writer, vec3s value);
bool network_write_vec4(NetworkWriter* writer, vec4s value);
bool network_write_mat2(NetworkWriter* writer, mat2s value);
bool network_write_mat3(NetworkWriter* writer, mat3s value);
bool network_write_mat4(NetworkWriter* writer, mat4s value);

bool network_write_string(NetworkWriter* writer, const char* value);
bool network_write_bytes(NetworkWriter* writer, const void* value, size_t size);

// Reader
void network_reader_init(NetworkReader* reader, const NetworkByte* buffer, size_t capacity);
size_t network_reader_remaining(NetworkReader* reader);
void network_reader_reset(NetworkReader* reader);

bool network_read_u8(NetworkReader* reader, uint8_t* out);
bool network_read_i8(NetworkReader* reader, int8_t* out);
bool network_read_i16(NetworkReader* reader, int16_t* out);
bool network_read_u16(NetworkReader* reader, uint16_t* out);
bool network_read_i32(NetworkReader* reader, int32_t* out);
bool network_read_u32(NetworkReader* reader, uint32_t* out);
bool network_read_i64(NetworkReader* reader, int64_t* out);
bool network_read_u64(NetworkReader* reader, uint64_t* out);

bool network_read_f32(NetworkReader* reader, float* out);
bool network_read_f64(NetworkReader* reader, double* out);

bool network_read_bool(NetworkReader* reader, bool* out);

bool network_read_vec2(NetworkReader* reader, vec2s* out);
bool network_read_vec3(NetworkReader* reader, vec3s* out);
bool network_read_vec4(NetworkReader* reader, vec4s* out);
bool network_read_mat2(NetworkReader* reader, mat2s* out);
bool network_read_mat3(NetworkReader* reader, mat3s* out);
bool network_read_mat4(NetworkReader* reader, mat4s* out);

bool network_read_string(NetworkReader* reader, char* out, size_t out_capacity);
// Reads exactly 'size' raw bytes with no length prefix, unlike network_read_string.
bool network_read_bytes(NetworkReader* reader, void* out, size_t size);
