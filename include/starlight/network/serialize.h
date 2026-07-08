#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	const unsigned char* buffer;
	size_t capacity;
	size_t cursor;
} NetworkReader;

typedef struct
{
	unsigned char* buffer;
	size_t capacity;
	size_t cursor;
} NetworkWriter;

bool network_write_i16(NetworkWriter* writer, uint16_t value);
bool network_write_u16(NetworkWriter* writer, uint16_t value);

bool network_read_i16(NetworkReader* reader, uint16_t* out);
bool network_read_u16(NetworkReader* reader, uint16_t* out);
