#include <starlight/starlight.h>
#include <starlight/audio/audio.h>
#include <stdlib.h>

#include "scenes/scene_default.h"

#define WINDOW_TITLE "Starlight Sandbox"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define UI_FONT_FILE ASSETS_DIR"fonts/monocraft.ttf"

int main()
{
	EngineConfig config = {0};
	config.window_title = WINDOW_TITLE;
	config.window_width = WINDOW_WIDTH;
	config.window_height = WINDOW_HEIGHT;
	config.enable_ui = true;
	config.enable_network = true;
	config.enable_audio = true;
	config.font_path = UI_FONT_FILE;

	engine_init(config);
	scene_manager_register_scene(&scene_default);
	scene_manager_switch_scene("SceneDefault");

	engine_run();

	engine_destroy();

	return EXIT_SUCCESS;
}
