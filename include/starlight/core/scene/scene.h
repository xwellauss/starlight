#pragma once

typedef struct
{
	char* scene_name;

	void (*init)();
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)();
	void (*render)();
	void (*build_ui)();
	void (*process_input)();
} Scene;

