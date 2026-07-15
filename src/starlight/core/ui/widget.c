#include <starlight/core/ui/widget.h>
#include <starlight/utils/logger.h>

#include "internal.h"

void ui_container_begin(const char* label, bool* clicked)
{
	UINode* node = ui_node_append(UI_NODE_CONTAINER, label);
	if(!node)
	{
		log_error("UI: Container: could not append node");
		return;
	}

	node->user_data = clicked;

	if(!ui_node_push_parent(node))
	{
		log_error("UI: could not create container!\n");
	}
}

void ui_container_end()
{
	if(!ui_node_pop_parent())
	{
		log_error("UI: Mismatched ui_container_end!\n");
	}
}

void ui_widget_button(const char* label, bool* clicked)
{
	UINode* node = ui_node_append(UI_NODE_BUTTON, label);
	if(!node)
	{
		log_error("UI: Button: could not append node");
		return;
	}

	node->user_data = clicked;
}

void ui_widget_text(const char* label)
{
	UINode* node = ui_node_append(UI_NODE_TEXT, label);
	if(!node)
	{
		log_error("UI: Text: could not append node");
		return;
	}
}

void ui_widget_text_input(const char* label, UITextInputState* state)
{
	UINode* node = ui_node_append(UI_NODE_TEXT_INPUT, label);
	if(!node)
	{
		log_error("UI: Text Input: could not append node");
		return;
	}

	state->label = label;

	ui_text_input_update(state);
	
	node->user_data = state;
}
