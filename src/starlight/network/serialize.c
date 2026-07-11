#include <starlight/network/serialize.h>

#include <stdint.h>
#include <string.h>
#include <ctype.h>


// https://beej.us/guide/bgnet/html/#serialization

// #define encode_f16(f) (float_encode((f), 16, 5))
// #define encode_f32(f) (float_encode((f), 32, 8))
// #define encode_f64(f) (float_encode((f), 64, 11))
// #define decode_f16(i) (float_decode((i), 16, 5))
// #define decode_f32(i) (float_decode((i), 32, 8))
// #define decode_f64(i) (float_decode((i), 64, 11))

// static uint64_t float_encode(long double f, unsigned bits, unsigned expbits)
// {
// 	long double fnorm;
// 	int shift;
// 	long long sign, exp, significand;
// 	unsigned significandbits = bits - expbits - 1;

// 	if(f == 0.0) return 0;

// 	if(f < 0)
// 	{
// 		sign = 1;
// 		fnorm = -f;
// 	}
// 	else
// 	{
// 		sign = 0;
// 		fnorm = f;
// 	}

// 	shift = 0;
// 	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
// 	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }

// 	fnorm = fnorm - 1.0;

// 	significand = fnorm * ((1LL << significandbits) + 0.5);
// 	exp = shift + ((1 << (expbits - 1)) - 1);

// 	return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
// }

// static long double float_decode(uint64_t i, unsigned bits, unsigned expbits)
// {
// 	long double result;
// 	long long shift;
// 	unsigned bias;
// 	unsigned significandbits = bits - expbits - 1;

// 	if(i == 0) return 0.0;

// 	result = (i & ((1LL << significandbits) - 1));
// 	result /= (1LL << significandbits);
// 	result += 1.0f;

// 	bias = (1 << (expbits - 1)) - 1;
// 	shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
// 	while(shift > 0) { result *= 2.0; shift--; }
// 	while(shift < 0) { result /= 2.0; shift++; }

// 	result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;

// 	return result;
// }


static void packu16(NetworkByte* buf, uint16_t i)
{
	*buf++ = i >> 8;
	*buf++ = i;
}

static void packu32(NetworkByte* buf, uint32_t i)
{
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

static void packu64(NetworkByte* buf, uint64_t i)
{
	*buf++ = i >> 56;
	*buf++ = i >> 48;
	*buf++ = i >> 40;
	*buf++ = i >> 32;
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

static uint16_t unpacku16(const NetworkByte* buf)
{
	return ((uint16_t)buf[0] << 8) | buf[1];
}

static int16_t unpacki16(const NetworkByte* buf)
{
	uint16_t i2 = unpacku16(buf);
	int16_t i;

	if(i2 <= 0x7fffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (uint16_t)(0xffffu - i2);
	}

	return i;
}

static uint32_t unpacku32(const NetworkByte* buf)
{
	return ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];
}

static int32_t unpacki32(const NetworkByte* buf)
{
	uint32_t i2 = unpacku32(buf);
	int32_t i;

	if(i2 <= 0x7fffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (int32_t)(0xffffffffu - i2);
	}

	return i;
}

static uint64_t unpacku64(const NetworkByte* buf)
{
	return ((uint64_t)buf[0]<<56) | ((uint64_t)buf[1]<<48) | ((uint64_t)buf[2]<<40) | ((uint64_t)buf[3]<<32) | ((uint64_t)buf[4]<<24) | ((uint64_t)buf[5]<<16) | ((uint64_t)buf[6]<<8) | buf[7];
}

static int64_t unpacki64(const NetworkByte* buf)
{
	uint64_t i2 = unpacku64(buf);
	int64_t i;

	if(i2 <= 0x7fffffffffffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (int64_t)(0xffffffffffffffffu - i2);
	}

	return i;
}

// Writer

bool network_write_u16(NetworkWriter* writer, uint16_t value)
{
	if(writer->cursor + 2 > writer->capacity) return false;

	packu16(writer->buffer + writer->cursor, value);
	writer->cursor += 2;
	return true;
}

bool network_write_u32(NetworkWriter* writer, uint32_t value)
{
	if(writer->cursor + 4 > writer->capacity) return false;

	packu32(writer->buffer + writer->cursor, value);
	writer->cursor += 4;
	return true;
}

bool network_write_u64(NetworkWriter* writer, uint64_t value)
{
	if(writer->cursor + 8 > writer->capacity) return false;

	packu64(writer->buffer + writer->cursor, value);
	writer->cursor += 8;
	return true;
}

bool network_write_i16(NetworkWriter* writer, int16_t value)
{
	return network_write_u16(writer, (uint16_t)value);
}

bool network_write_i32(NetworkWriter* writer, int32_t value)
{
	return network_write_u32(writer, (uint32_t)value);
}

bool network_write_i64(NetworkWriter* writer, int64_t value)
{
	return network_write_u64(writer, (uint64_t)value);
}

// bool network_write_f16(NetworkWriter* writer, float value)
// {
// 	if(writer->cursor + 2 > writer->capacity) return false;

// 	packu16(writer->buffer + writer->cursor, (uint16_t)encode_f16(value));
// 	writer->cursor += 2;
// 	return true;
// }

bool network_write_f32(NetworkWriter* writer, float value)
{
	if(writer->cursor + 4 > writer->capacity) return false;

	uint32_t bits;
	memcpy(&bits, &value, sizeof(bits));
	packu32(writer->buffer + writer->cursor, bits);
	writer->cursor += 4;

	return true;
}

bool network_write_f64(NetworkWriter* writer, double value)
{
	if(writer->cursor + 8 > writer->capacity) return false;

	uint64_t bits;
	memcpy(&bits, &value, sizeof(bits));
	packu64(writer->buffer + writer->cursor, bits);
	writer->cursor += 8;

	return true;
}

// Strings are assumed to be "small", i.e uint16_t
bool network_write_string(NetworkWriter* writer, const char* value)
{
	size_t len = strlen(value);
	if(len > 0xFFFu) return false;

	if(!network_write_u16(writer, (uint16_t)len)) return false;

	if(writer->cursor + len > writer->capacity) return false;
	memcpy(writer->buffer + writer->cursor, value, len);
	writer->cursor += len;

	return true;
}

bool network_write_vec2(NetworkWriter* writer, vec2s value)
{
	bool result = network_write_f32(writer, value.x);
	result &= network_write_f32(writer, value.y);

	return result;
}

bool network_write_vec3(NetworkWriter* writer, vec3s value)
{
	bool result = network_write_f32(writer, value.x);
	result &= network_write_f32(writer, value.y);
	result &= network_write_f32(writer, value.z);

	return result;
}

bool network_write_vec4(NetworkWriter* writer, vec4s value)
{
	bool result = network_write_f32(writer, value.x);
	result &= network_write_f32(writer, value.y);
	result &= network_write_f32(writer, value.z);
	result &= network_write_f32(writer, value.w);

	return result;
}

bool network_write_mat2(NetworkWriter* writer, mat2s value)
{
	bool result = network_write_vec2(writer, value.col[0]);
	result &= network_write_vec2(writer, value.col[1]);

	return result;
}

bool network_write_mat3(NetworkWriter* writer, mat3s value)
{
	bool result = network_write_vec3(writer, value.col[0]);
	result &= network_write_vec3(writer, value.col[1]);
	result &= network_write_vec3(writer, value.col[2]);

	return result;
}

bool network_write_mat4(NetworkWriter* writer, mat4s value)
{
	bool result = network_write_vec4(writer, value.col[0]);
	result &= network_write_vec4(writer, value.col[1]);
	result &= network_write_vec4(writer, value.col[2]);
	result &= network_write_vec4(writer, value.col[3]);

	return result;
}

// Reader

bool network_read_i16(NetworkReader* reader, int16_t* out)
{
	if(reader->cursor + 2 > reader->capacity) return false;

	*out = unpacki16(reader->buffer + reader->cursor);
	reader->cursor += 2;

	return true;
}

bool network_read_u16(NetworkReader* reader, uint16_t* out)
{
	if(reader->cursor + 2 > reader->capacity) return false;

	*out = unpacku16(reader->buffer + reader->cursor);
	reader->cursor += 2;

	return true;
}

bool network_read_i32(NetworkReader* reader, int32_t* out)
{
	if(reader->cursor + 4 > reader->capacity) return false;

	*out = unpacki32(reader->buffer + reader->cursor);
	reader->cursor += 4;

	return true;
}

bool network_read_u32(NetworkReader* reader, uint32_t* out)
{
	if(reader->cursor + 4 > reader->capacity) return false;

	*out = unpacku32(reader->buffer + reader->cursor);
	reader->cursor += 4;

	return true;
}

bool network_read_i64(NetworkReader* reader, int64_t* out)
{
	if(reader->cursor + 8 > reader->capacity) return false;

	*out = unpacki64(reader->buffer + reader->cursor);
	reader->cursor += 8;

	return true;
}

bool network_read_u64(NetworkReader* reader, uint64_t* out)
{
	if(reader->cursor + 8 > reader->capacity) return false;

	*out = unpacku64(reader->buffer + reader->cursor);
	reader->cursor += 8;

	return true;
}

// bool network_read_f16(NetworkReader* reader, float* out)
// {
// 	if(reader->cursor + 2 > reader->capacity) return false;

// 	*out = (float)decode_f16(unpacku16(reader->buffer + reader->cursor));
// 	reader->cursor += 2;
// 	return true;
// }

bool network_read_f32(NetworkReader* reader, float* out)
{
	if(reader->cursor + 4 > reader->capacity) return false;

	uint32_t bits = unpacku32(reader->buffer + reader->cursor);
	memcpy(out, &bits, sizeof(bits));
	reader->cursor += 4;

	return true;
}

bool network_read_f64(NetworkReader* reader, double* out)
{
	if(reader->cursor + 8 > reader->capacity) return false;

	uint64_t bits = unpacku64(reader->buffer + reader->cursor);
	memcpy(out, &bits, sizeof(bits));
	reader->cursor += 8;

	return true;
}

bool network_read_string(NetworkReader* reader, char* out, size_t out_capacity)
{
	uint16_t len;
	if(!network_read_u16(reader, &len)) return false;

	if(reader->cursor + len > reader->capacity) return false;
	if(len + 1 > out_capacity) return false;

	memcpy(out, reader->buffer + reader->cursor, len);
	out[len] = '\0';
	reader->cursor += len;

	return true;
}

bool network_read_vec2(NetworkReader* reader, vec2s* out)
{
	bool result = network_read_f32(reader, &out->x);
	result &= network_read_f32(reader, &out->y);

	return result;
}

bool network_read_vec3(NetworkReader* reader, vec3s* out)
{
	bool result = network_read_f32(reader, &out->x);
	result &= network_read_f32(reader, &out->y);
	result &= network_read_f32(reader, &out->z);

	return result;
}

bool network_read_vec4(NetworkReader* reader, vec4s* out)
{
	bool result = network_read_f32(reader, &out->x);
	result &= network_read_f32(reader, &out->y);
	result &= network_read_f32(reader, &out->z);
	result &= network_read_f32(reader, &out->w);

	return result;
}

bool network_read_mat2(NetworkReader* reader, mat2s* out)
{
	bool result = network_read_vec2(reader, &out->col[0]);
	result &= network_read_vec2(reader, &out->col[1]);

	return result;
}

bool network_read_mat3(NetworkReader* reader, mat3s* out)
{
	bool result = network_read_vec3(reader, &out->col[0]);
	result &= network_read_vec3(reader, &out->col[1]);
	result &= network_read_vec3(reader, &out->col[2]);

	return result;
}

bool network_read_mat4(NetworkReader* reader, mat4s* out)
{
	bool result = network_read_vec4(reader, &out->col[0]);
	result &= network_read_vec4(reader, &out->col[1]);
	result &= network_read_vec4(reader, &out->col[2]);
	result &= network_read_vec4(reader, &out->col[3]);

	return result;
}
