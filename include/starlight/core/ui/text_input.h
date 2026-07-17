#pragma once

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
} UITextInputState;

void ui_text_input_state_init(UITextInputState* state);
void ui_text_input_state_destroy(UITextInputState* state);
void ui_text_input_state_reset(UITextInputState* state);

