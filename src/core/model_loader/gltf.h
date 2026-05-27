#pragma once

 #include "model_loader.h"

#include "cgltf.h"

void gltf_load_file(Model* model, const char* filepath);
void gltf_model_free(Model* model);
