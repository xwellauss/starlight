#include "scene_default.h"

#include <starlight/utils/logger.h>


static void init()
{
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
	//font_renderer_render_text("Starlight", 0.0f, 0.0f, 1.0f, "#ffffff", 1.0f);
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
