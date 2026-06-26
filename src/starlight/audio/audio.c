#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>

#include <miniaudio.h>

typedef struct
{
	ma_engine engine;
} AudioEngine;

static AudioEngine audio_engine = {0};

void audio_init()
{
	ma_result result;

    result = ma_engine_init(NULL, &audio_engine.engine);
    if(result != MA_SUCCESS)
	{
        log_debug("Failed to init audio!\n");
    }
}

void audio_play(const char* filename)
{
	ma_result result;

	result = ma_engine_play_sound(&audio_engine.engine, filename, NULL);
	if(result != MA_SUCCESS)
	{
        log_error("File not found!\n");
    }
}

void audio_destroy()
{
	ma_engine_uninit(&audio_engine.engine);
}
