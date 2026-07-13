#include "scene_menu.h"

#include <starlight/core/window/input.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>
#include <starlight/core/scene/scene_manager.h>

#include <stb_ds.h>


static const char** scenes_list = NULL;

static bool* scene_button_list = NULL;

static UIStyle style1;
static UIStyle style2;

static void init()
{
    style1.base.bg_color = hex_to_rgba("#000000", 0.0f);
	style1.base.fg_color = hex_to_rgba("#ffffff", 1.0f);
	style1.base.font_size = 64;
	style1.base.corner_radius = (vec4s){2.0f, 2.0f, 2.0f, 2.0f};
	style1.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style1.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style1.base.layout.child_gap = 4;
	style1.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_CENTER;
	style1.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style1.base.layout.sizing.x = ui_style_size_fit(0, 400);
	style1.base.layout.sizing.y = ui_style_size_grow(0, 200);
	style1.is_interactive = false;

	style2.base.bg_color = hex_to_rgba("#9a8040", 0.0f);
	style2.base.fg_color = hex_to_rgba("#4287f5", 1.0f);
	style2.base.font_size = 32;
	style2.base.corner_radius = (vec4s){2.0f, 2.0f, 2.0f, 2.0f};
	style2.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style2.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style2.base.layout.child_gap = 4;
	style2.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_LEFT;
	style2.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style2.base.layout.sizing.x = ui_style_size_fit(0, 400);
	style2.base.layout.sizing.y = ui_style_size_grow(0, 200);
	style2.is_interactive = true;
	style2.interactive.bg_hover_color = hex_to_rgba("#d7a8f8", 0.0f);
	style2.interactive.bg_press_color = hex_to_rgba("#f7a8f8", 0.0f);
	style2.interactive.fg_hover_color = hex_to_rgba("#4e42f5", 1.0f);
	style2.interactive.fg_press_color = hex_to_rgba("#2c258f", 1.0f);
}

static void activate()
{
    scenes_list = scene_manager_get_scenes_list();

	for(size_t i = 0; i < arrlen(scenes_list); i++)
	{
		arrput(scene_button_list, false);
	}
}

static void render()
{
}

static void build_ui()
{
    ui_style_push(&style1);
	ui_container_begin("Scenes", NULL);
	ui_widget_text("Scenes");
	ui_style_push(&style2);
	for(size_t i = 0; i < arrlen(scenes_list); i++)
	{
		if(strcmp(scenes_list[i], "SceneMenu") == 0)
		{
			continue;
		}

		ui_widget_button(scenes_list[i], &scene_button_list[i]);

		if(scene_button_list[i])
		{
			scene_manager_switch_scene(scenes_list[i]);
		}
	}
	ui_style_pop();
	ui_container_end();
	ui_style_pop();
}

static void update()
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
	arrfree(scene_button_list);
}


Scene scene_menu = {"SceneMenu", .init=init, .destroy=destroy, .activate=activate, .deactivate=deactivate, .update=update, .render=render, .build_ui=build_ui, .process_input=process_input};
