#include "scene_default.h"

#include <starlight/core/window/input.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

static bool button_clicked = false;

static UIStyle style1;
static UIStyle style2;


static void init()
{
	style1 = ui_style_button_default_style();
	style1.base.bg_color = hex_to_rgba("#ff0000", 1.0f);

	style2 = ui_style_button_default_style();
	style2.base.bg_color = hex_to_rgba("#00ff00", 1.0f);
}

static void activate()
{
}

static void update()
{
	if(button_clicked)
	{
		log_debug("Bro clicked the button!\n");
	}
}

static void build_ui()
{
	ui_container_begin("Container 1", NULL);
		ui_style_push(&style1);
		ui_widget_button("Click me!", &button_clicked);
		ui_style_pop();

		ui_style_push(&style2);
		ui_widget_button("Click me!", &button_clicked);
		ui_style_pop();
	ui_container_end();
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

Scene scene_default = {"SceneDefault", .init=init, .destroy=destroy, .activate=activate, .deactivate=deactivate, .update=update, .render=render, .build_ui=build_ui, .process_input=process_input};
