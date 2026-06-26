#include "scene_default.h"

#include <starlight/core/window/input.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>


static void init()
{
	//audio_play(ASSETS_DIR"sounds/song1.wav");
}

static void activate()
{
}

static void update()
{
}

static void build_ui()
{
}

static void render()
{
}

static void process_input()
{
	if(window_input_key_is_down(INPUT_KEY_A))
	{
		log_debug("A CLicked\n");
	}
}

static void deactivate()
{
}

static void destroy()
{
}

Scene scene_default = {"SceneDefault", init, destroy, activate, deactivate, update, render, build_ui, process_input};
