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
	style1.base.bg_color = hex_to_rgb("#333333", 1.0f);
	style1.base.fg_color = hex_to_rgb("#ffffff", 1.0f);
	style1.base.font_size = 16;
	style1.base.corner_radius = (vec4s){2.0f, 2.0f, 2.0f, 2.0f};

	style1.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style1.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style1.base.layout.child_gap = 0;
	style1.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_CENTER;
	style1.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style1.base.layout.sizing.x = ui_style_size_fit(0, 400);
	style1.base.layout.sizing.y = ui_style_size_grow(0, 200);

	style1.is_interactive = true;
	style1.interactive.bg_hover_color = hex_to_rgb("#666666", 1.0f);
	style1.interactive.bg_press_color = hex_to_rgb("#666666", 1.0f);
	style1.interactive.fg_hover_color = hex_to_rgb("#ffffff", 1.0f);
	style1.interactive.fg_press_color = hex_to_rgb("#ffffff", 1.0f);



	style2.base.bg_color = hex_to_rgb("#a7a8f8", 1.0f);
	style2.base.fg_color = hex_to_rgb("#000000", 1.0f);
	style2.is_interactive = true;
	style2.base.font_size = 16;
	style2.base.corner_radius = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style2.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style2.interactive.bg_hover_color = hex_to_rgb("#d7a8f8", 1.0f);
	style2.interactive.bg_press_color = hex_to_rgb("#f7a8f8", 1.0f);
	style2.interactive.fg_hover_color = hex_to_rgb("#444444", 1.0f);
	style2.interactive.fg_press_color = hex_to_rgb("#222222", 1.0f);
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
	//ui_style_push(&style1);
	ui_begin_container("Container 1", NULL);
		ui_text("This is a button!");
		//ui_style_push(&style2);
		ui_button("Click me!", &button_clicked);
		//ui_style_pop();
	ui_end_container();
	//ui_style_pop();
	ui_begin_container("Container 2", NULL);
		ui_text("This is a button!");
		//ui_button("Click me!", &button_clicked);
	ui_end_container();
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
