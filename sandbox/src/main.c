#include <starlight/starlight.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "scenes.h"

#define WINDOW_TITLE "Starlight Sandbox"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define UI_FONT_FILE "fonts/monocraft.ttf"

int main()
{
	if(strlen(ASSETS_DIR) > 0) chdir(ASSETS_DIR);

	EngineConfig config = {0};
	config.window_title = WINDOW_TITLE;
	config.window_width = WINDOW_WIDTH;
	config.window_height = WINDOW_HEIGHT;
	config.enable_ui = true;
	config.enable_network = true;
	config.enable_audio = true;
	config.font_path = UI_FONT_FILE;

	engine_init(config);

	sandbox_scenes();
	scene_manager_switch_scene("SceneMenu");

	engine_run();

	engine_destroy();

	return EXIT_SUCCESS;
}
