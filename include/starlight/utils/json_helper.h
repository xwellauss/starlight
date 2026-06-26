#pragma once

#include <cJSON.h>
#include <stdbool.h>

cJSON* json_parse(const char* json_data_string);

cJSON* json_read_file(const char* path);

char* json_print(cJSON* root);

cJSON* json_create_object();
bool json_add_item_to_object(cJSON* root, char* string, cJSON* item);
cJSON* json_get_object(cJSON* cjson, const char* string);
void json_delete_object(cJSON* cjson);

double json_get_double(cJSON* cjson, const char* string);
int json_get_int(cJSON* cjson, const char* string);
char* json_get_string(cJSON* cjson, const char* string);


cJSON* json_create_array();
cJSON* json_get_array_item(cJSON* array, int index);
bool json_add_item_to_array(cJSON* array, cJSON* item);
int json_get_array_length(cJSON* array);

cJSON* json_create_float_array(float* data, int count);
