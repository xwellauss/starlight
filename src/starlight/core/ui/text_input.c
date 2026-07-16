#include <starlight/core/ui/widget.h>
#include <starlight/core/engine.h>
#include <starlight/core/window/input.h>
#include <starlight/utils/logger.h>

#include "internal.h"

enum
{
	UI_KEY_LEFT = 0x10000,
	UI_KEY_RIGHT,
	UI_KEY_UP,
	UI_KEY_DOWN,
	UI_KEY_PGUP,
	UI_KEY_PGDOWN,
	UI_KEY_LINESTART,
	UI_KEY_LINEEND,
	UI_KEY_TEXTSTART,
	UI_KEY_TEXTEND,
	UI_KEY_DELETE,
	UI_KEY_BACKSPACE,
	UI_KEY_UNDO,
	UI_KEY_REDO,
	UI_KEY_WORDLEFT,
	UI_KEY_WORDRIGHT,
};


#define UI_KEY_SHIFT_BIT 0x100000

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_UNDOSTATECOUNT 8
#define STB_TEXTEDIT_UNDOCHARCOUNT 256

#include <stb_textedit.h> // Include in header mode
static void ui_text_layout_row(StbTexteditRow* row, UITextInputState* s, int n)
{
	row->x0 = 0.0f;
	row->x1 = ui_backend_font_measure_text_width(s->buffer + n, s->length - n);
	// row->x1 = font_atlas_measure_text_width(s->font_atlas, s->buffer, s->length);
	row->baseline_y_delta = 1.0f;
	row->ymin = 0.0f;
	row->ymax = 1.0f;
	row->num_chars = s->length - n;
	// row->num_chars = s->length;
}


#define STB_TEXTEDIT_STRING UITextInputState
#define STB_TEXTEDIT_STRINGLEN(s) ((s)->length)
#define STB_TEXTEDIT_GETCHAR(s,i) ((s)->buffer[i])
#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_KEYTOTEXT(k) ((k) < 0x10000 ? (k) : -1)
#define STB_TEXTEDIT_IS_SPACE(c) ((c) == ' ')

#define STB_TEXTEDIT_GETWIDTH(s,n,i) ui_backend_font_glyph_advance((s)->buffer[(n)+(i)])

// Delete and insert chars
#define STB_TEXTEDIT_LAYOUTROW(r,obj,n) ui_text_layout_row(r, obj, n)
#define STB_TEXTEDIT_DELETECHARS(s,i,n) memmove(&(s)->buffer[(i)], &(s)->buffer[(i)+(n)], (s)->length - (i) - (n)), (s)->length -=(n), (s)->buffer[(s)->length] = '\0'
#define STB_TEXTEDIT_INSERTCHARS(s,i,text,n) ((s)->length + (n) < UI_TEXT_INPUT_MAX ? (memmove(&(s)->buffer[(i)+(n)], &(s)->buffer[(i)], (s)->length-(i)), memcpy(&(s)->buffer[(i)], (text), (n)), (s)->length += (n), (s)->buffer[(s)->length] = '\0', 1) : 0)

#define STB_TEXTEDIT_K_SHIFT UI_KEY_SHIFT_BIT 
#define STB_TEXTEDIT_K_LEFT UI_KEY_LEFT 
#define STB_TEXTEDIT_K_RIGHT UI_KEY_RIGHT
#define STB_TEXTEDIT_K_UP UI_KEY_UP
#define STB_TEXTEDIT_K_DOWN UI_KEY_DOWN
#define STB_TEXTEDIT_K_PGUP UI_KEY_PGUP
#define STB_TEXTEDIT_K_PGDOWN UI_KEY_PGDOWN
#define STB_TEXTEDIT_K_LINESTART UI_KEY_LINESTART
#define STB_TEXTEDIT_K_LINEEND UI_KEY_LINEEND
#define STB_TEXTEDIT_K_TEXTSTART UI_KEY_TEXTSTART
#define STB_TEXTEDIT_K_TEXTEND UI_KEY_TEXTEND
#define STB_TEXTEDIT_K_DELETE UI_KEY_DELETE
#define STB_TEXTEDIT_K_BACKSPACE UI_KEY_BACKSPACE
#define STB_TEXTEDIT_K_UNDO UI_KEY_UNDO
#define STB_TEXTEDIT_K_REDO UI_KEY_REDO
#define STB_TEXTEDIT_K_WORDLEFT UI_KEY_WORDLEFT
#define STB_TEXTEDIT_K_WORDRIGHT UI_KEY_WORDRIGHT

#define STB_TEXTEDIT_IMPLEMENTATION
#include <stb_textedit.h> // Include in implementation mode

#include <string.h>
#include <stdlib.h>

struct UITextEditState
{
    STB_TexteditState stb_state;
};

static UITextInputState* focused_text_input = NULL;
static uint32_t char_queue[UI_TEXT_INPUT_MAX];
static int char_queue_len = 0;

static void ui_text_input_process_keys(UITextInputState* state)
{
	bool shift = window_input_mod_active(INPUT_MOD_SHIFT);
	bool ctrl = window_input_mod_active(INPUT_MOD_CONTROL);
	STB_TexteditState* s = &state->edit_state->stb_state;

	if(ctrl && window_input_key_just_pressed(INPUT_KEY_A))
	{
		s->select_start = 0;
		s->select_end = state->length;
		s->cursor = state->length;
		s->has_preferred_x = 0;
		return;
	}

	if(window_input_key_just_pressed(INPUT_KEY_LEFT))
		stb_textedit_key(state, s, ctrl ? UI_KEY_WORDLEFT : (UI_KEY_LEFT | (shift ? UI_KEY_SHIFT_BIT : 0)));
	if(window_input_key_just_pressed(INPUT_KEY_RIGHT))
		stb_textedit_key(state, s, ctrl ? UI_KEY_WORDRIGHT : (UI_KEY_RIGHT | (shift ? UI_KEY_SHIFT_BIT : 0)));
	if(window_input_key_just_pressed(INPUT_KEY_HOME))
		stb_textedit_key(state, s, (ctrl ? UI_KEY_TEXTSTART : UI_KEY_LINESTART) | (shift ? UI_KEY_SHIFT_BIT : 0));
	if(window_input_key_just_pressed(INPUT_KEY_END))
		stb_textedit_key(state, s, (ctrl ? UI_KEY_TEXTEND : UI_KEY_LINEEND) | (shift ? UI_KEY_SHIFT_BIT : 0));
	if(window_input_key_just_pressed(INPUT_KEY_BACKSPACE))
		stb_textedit_key(state, s, UI_KEY_BACKSPACE);
	if(window_input_key_just_pressed(INPUT_KEY_DELETE))
		stb_textedit_key(state, s, UI_KEY_DELETE);
	if(ctrl && window_input_key_just_pressed(INPUT_KEY_Z))
		stb_textedit_key(state, s, UI_KEY_UNDO);
	if(ctrl && window_input_key_just_pressed(INPUT_KEY_Y))
		stb_textedit_key(state, s, UI_KEY_REDO);
}

void ui_text_input_focus(UITextInputState* state)
{
	focused_text_input = state;
	state->focused = true;
}

void ui_text_input_unfocus(UITextInputState* state)
{
	if(focused_text_input == state)
	{
		focused_text_input = NULL;
		state->focused = false;
	}
}

bool ui_text_input_is_focused(UITextInputState* state)
{
	return focused_text_input == state;
}

void ui_text_input_click(UITextInputState* state, float local_x)
{
	stb_textedit_click(state, &state->edit_state->stb_state, local_x, 0.0f);
}

void ui_text_input_drag(UITextInputState* state, float local_x)
{
	stb_textedit_drag(state, &state->edit_state->stb_state, local_x, 0.0f);
}

int ui_text_input_get_cursor_index(UITextInputState* s)
{
	return s->edit_state->stb_state.cursor;
}

void ui_text_input_char_queue_reset()
{
	char_queue_len = window_input_get_key_char_queue(char_queue, UI_TEXT_INPUT_MAX);
}

void ui_text_input_state_init(UITextInputState* state)
{
	memset(state, 0, sizeof(*state));
	state->edit_state = malloc(sizeof(UITextEditState));
	stb_textedit_initialize_state(&state->edit_state->stb_state, 1);
}

void ui_text_input_state_destroy(UITextInputState* state)
{
	free(state->edit_state);
	state->edit_state = NULL;
}

void ui_text_input_update(UITextInputState* state)
{
	if(!state->focused) return;

	STB_TexteditState* s = &state->edit_state->stb_state;

	for(int i = 0; i < char_queue_len; i++)
	{
		uint32_t c = char_queue[i];
		if(c >= 32 && c < 127)
		{
			stb_textedit_key(state, s, (int)c);
		}
	}

	ui_text_input_process_keys(state);

	if(window_input_key_just_pressed(INPUT_KEY_ENTER))
	{
		state->focused = false;
	}

	state->blink_timer += engine_get_deltatime();
}

