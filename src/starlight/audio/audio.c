#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>

#include <miniaudio.h>

void audio_init(Audio_Engine* audio_engine)
{
	ma_result result;

    result = ma_engine_init(NULL, &audio_engine->engine);
    if(result != MA_SUCCESS)
	{
        log_debug("Failed to init audio!\n");
    }
}

void audio_play(Audio_Engine* audio_engine, char* filename)
{
	ma_result result;

	result = ma_engine_play_sound(&audio_engine->engine, filename, NULL);
	if(result != MA_SUCCESS)
	{
        log_debug("File not found!\n");
    }
}

void audio_destroy(Audio_Engine* audio_engine)
{
	ma_engine_uninit(&audio_engine->engine);
}
