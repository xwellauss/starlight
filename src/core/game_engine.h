#pragma once

#include "window.h"
#include "scene.h"
#include "../audio/audio.h"

typedef struct
{
	Window current_window;
	Audio_Engine audio_engine;
	Scene* current_scene;
	float deltatime;
	struct { char* key; Scene* value; }* scenes;
} GameEngine;

extern GameEngine game_engine;

void run_application();
