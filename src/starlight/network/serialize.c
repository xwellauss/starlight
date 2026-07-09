#include <starlight/network/serialize.h>

#include <string.h>
#include <ctype.h>


// https://beej.us/guide/bgnet/html/#serialization

#define encode_f16(f) (float_encode((f), 16, 5))
#define encode_f32(f) (float_encode((f), 32, 8))
#define encode_f64(f) (float_encode((f), 64, 11))
#define decode_f16(i) (float_decode((i), 16, 5))
#define decode_f32(i) (float_decode((i), 32, 8))
#define decode_f64(i) (float_decode((i), 64, 11))

static uint64_t float_encode(long double f, unsigned bits, unsigned expbits)
{
	long double fnorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandbits = bits - expbits - 1;

	if(f == 0.0) return 0;

	if(f < 0)
	{
		sign = 1;
		fnorm = -f;
	}
	else
	{
		sign = 0;
		fnorm = f;
	}

	shift = 0;
	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }

	fnorm = fnorm - 1.0;

	significand = fnorm * ((1LL << significandbits) + 0.5f);
	exp = shift + ((1 << (expbits - 1)) - 1);

	return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
}

static long double float_decode(uint64_t i, unsigned bits, unsigned expbits)
{
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandbits = bits - expbits - 1;

	if(i == 0) return 0.0;

	result = (i & ((1LL << significandbits) - 1));
	result /= (1LL << significandbits);
	result += 1.0f;

	bias = (1 << (expbits - 1)) - 1;
	shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;

	return result;
}


static void packi16(unsigned char* buf, unsigned int i)
{
	*buf++ = i >> 8;
	*buf++ = i;
}

static void packi32(unsigned char* buf, unsigned long int i)
{
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

static void packi64(unsigned char* buf, unsigned long long int i)
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

static int unpacki16(const unsigned char* buf)
{
	unsigned int i2 = ((unsigned int)buf[0] << 8) | buf[1];
	int i;

	if(i2 <= 0x7fffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (unsigned int)(0xffffu - i2);
	}

	return i;
}

static unsigned int unpacku16(const unsigned char* buf)
{
	return ((unsigned int)buf[0] << 8) | buf[1];
}

static long int unpacki32(unsigned char* buf)
{
	unsigned long int i2 = ((unsigned long int)buf[0] << 24) | ((unsigned long int)buf[1] << 16) | ((unsigned long int)buf[2] << 8) | buf[3];
	long int i;

	if(i2 <= 0x7fffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (long int)(0xffffffffu - i2);
	}

	return i;
}

static unsigned long int unpacku32(const unsigned char* buf)
{
	return ((unsigned long int)buf[0] << 24) | ((unsigned long int)buf[1] << 16) | ((unsigned long int)buf[2] << 8) | buf[3];
}

static long long int unpacki64(const unsigned char* buf)
{
	unsigned long long int i2 = ((unsigned long long int)buf[0]<<56) | ((unsigned long long int)buf[1]<<48) | ((unsigned long long int)buf[2]<<40) | ((unsigned long long int)buf[3]<<32) | ((unsigned long long int)buf[4]<<24) | ((unsigned long long int)buf[5]<<16) | ((unsigned long long int)buf[6]<<8) | buf[7];

	long long int i;

	if(i2 <= 0x7fffffffffffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (long long int)(0xffffffffffffffffu - 12);
	}

	return i;
}

static unsigned long long int unpacku64(const unsigned char* buf)
{
	return ((unsigned long long int)buf[0]<<56) | ((unsigned long long int)buf[1]<<48) | ((unsigned long long int)buf[2]<<40) | ((unsigned long long int)buf[3]<<32) | ((unsigned long long int)buf[4]<<24) | ((unsigned long long int)buf[5]<<16) | ((unsigned long long int)buf[6]<<8) | buf[7];
}

/*
static void packvec4s(unsigned char* buf, vec4s v)
{
	pack(buf, "ffff", v.x, v.y, v.z, v.w);
}

static void packvec3s(unsigned char* buf, vec3s v)
{
	pack(buf, "fff", v.x, v.y, v.z);
}

static void packvec2s(unsigned char* buf, vec2s v)
{
	pack(buf, "ff", v.x, v.y);
}

static void packmat4s(unsigned char* buf, mat4s m)
{
	pack(buf, "VVVV", m.col[0], m.col[1], m.col[2], m.col[3]);
}



static vec4s unpackvec4s(unsigned char* buf)
{
	vec4s v;

	unpack(buf, "ffff", &v.x, &v.y, &v.z, &v.w);

	return v;
}

static vec3s unpackvec3s(unsigned char* buf)
{
	vec3s v;

	unpack(buf, "fff", &v.x, &v.y, &v.z);

	return v;
}

static vec2s unpackvec2s(unsigned char* buf)
{
	vec2s v;

	unpack(buf, "ff", &v.x, &v.y);

	return v;
}

static mat4s unpackmat4s(unsigned char* buf)
{
	mat4s m;

	unpack(buf, "VVVV", &m.col[0], &m.col[1], &m.col[2], &m.col[3]);

	return m;
}
*/


bool network_write_i16(NetworkWriter* writer, uint16_t value)
{
	if(writer->cursor + 2 > writer->capacity) return false;

	packi16(writer->buffer + writer->cursor, value);
	writer->cursor += 2;
	return true;
}

bool network_write_u16(NetworkWriter* writer, uint16_t value)
{
	if(writer->cursor + 2 > writer->capacity) return false;

	packi16(writer->buffer + writer->cursor, value);
	writer->cursor += 2;
	return true;
}

bool network_read_i16(NetworkReader* reader, uint16_t* out)
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
