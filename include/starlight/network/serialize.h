#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t NetworkByte;

typedef struct
{
	const NetworkByte* buffer;
	size_t capacity;
	size_t cursor;
} NetworkReader;

typedef struct
{
	NetworkByte* buffer;
	size_t capacity;
	size_t cursor;
} NetworkWriter;

bool network_write_u16(NetworkWriter* writer, uint16_t value);
bool network_write_i16(NetworkWriter* writer, int16_t value);
bool network_write_u32(NetworkWriter* writer, uint32_t value);
bool network_write_i32(NetworkWriter* writer, int32_t value);
bool network_write_u64(NetworkWriter* writer, uint64_t value);
bool network_write_i64(NetworkWriter* writer, int64_t value);
bool network_write_f32(NetworkWriter* writer, float value);
bool network_write_f64(NetworkWriter* writer, double value);
bool network_write_string(NetworkWriter* writer, const char* value);

bool network_read_i16(NetworkReader* reader, int16_t* out);
bool network_read_u16(NetworkReader* reader, uint16_t* out);
bool network_read_i32(NetworkReader* reader, int32_t* out);
bool network_read_u32(NetworkReader* reader, uint32_t* out);
bool network_read_i64(NetworkReader* reader, int64_t* out);
bool network_read_u64(NetworkReader* reader, uint64_t* out);
bool network_read_f32(NetworkReader* reader, float* out);
bool network_read_f64(NetworkReader* reader, double* out);
bool network_read_string(NetworkReader* reader, char* out, size_t out_capacity);
