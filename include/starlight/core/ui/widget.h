#pragma once

#include <starlight/core/ui/style.h>

#include <stdbool.h>

void ui_container_begin(const char* label, bool* clicked);
void ui_container_end();

void ui_widget_text(const char* label);
void ui_widget_button(const char* label, bool* clicked);

