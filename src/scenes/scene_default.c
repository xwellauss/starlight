#include "scene_default.h"

static void init()
{
	log_debug("Scene init...\n");
}

static void activate()
{
	log_debug("Scene activate...\n");
}

static void update()
{
	log_debug("Scene update...\n");
}

static void render()
{
	log_debug("Scene render...\n");
}

static void process_input()
{
	log_debug("Scene process input...\n");
}

static void deactivate()
{
	log_debug("Scene deactivate...\n");
}

static void destroy()
{
	log_debug("Scene destroy...\n");
}

Scene scene_default = {"SceneDefault", init, destroy, activate, deactivate, update, render, process_input};
