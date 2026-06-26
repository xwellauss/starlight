#include <starlight/utils/math_utils.h>

vec4s hex_to_rgb(char colorcode[7], float alpha)
{
	char hex_string[9] = {};

	hex_string[0] = '0';
	hex_string[1] = 'x';
	hex_string[8] = '\0';

	for(size_t i = 0; i < 6; i++)
	{
		hex_string[i+2] = colorcode[i+1];
	}
	
	int hexcode = strtol(hex_string, NULL, 16);

	return (vec4s){((hexcode >> 16) & 0xFF)/255.0f, ((hexcode >> 8) & 0xFF)/255.0f, ((hexcode) & 0xFF)/255.0f, alpha};
}
