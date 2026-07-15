#pragma once

#include <starlight/core/ui/style.h>
#include <starlight/core/resources/font_atlas.h>

#include <stdbool.h>

#define UI_TEXT_INPUT_MAX 256

typedef struct UITextEditState UITextEditState;

typedef struct
{
	const char* label;
	char buffer[UI_TEXT_INPUT_MAX];
	int length;
	UITextEditState* edit_state;
	bool focused;
	float blink_timer;

	FontAtlas* font_atlas;
} UITextInputState;


void ui_container_begin(const char* label, bool* clicked);
void ui_container_end();

void ui_widget_text(const char* label);
void ui_widget_button(const char* label, bool* clicked);

void ui_text_input_state_init(UITextInputState* state);
void ui_text_input_state_destroy(UITextInputState* state);
void ui_widget_text_input(const char* label, UITextInputState* state);
