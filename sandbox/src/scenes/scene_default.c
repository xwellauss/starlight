#include "scene_default.h"

#include <starlight/core/window/input.h>
#include <starlight/core/ui/ui.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>

static bool button_clicked = false;


static void init()
{
	//audio_play("sounds/song1.wav");
}

static void activate()
{
}

static void update()
{
	if(button_clicked)
	{
		log_debug("Bro clicked the button!\n");
		button_clicked = false;
	}
}

static void build_ui()
{
	ui_button("Click me!", &button_clicked);
}

static void render()
{
}

static void process_input()
{
}

static void deactivate()
{
}

static void destroy()
{
}

Scene scene_default = {"SceneDefault", init, destroy, activate, deactivate, update, render, build_ui, process_input};
