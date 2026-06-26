#pragma once

#include <miniaudio.h>

typedef struct
{
	ma_engine engine;
} Audio_Engine;

void audio_init(Audio_Engine* audio_engine);
void audio_play(Audio_Engine* audio_engine, char* filename);
void audio_destroy(Audio_Engine* audio_engine);
