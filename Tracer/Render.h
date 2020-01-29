#pragma once

#include "stdafx.h"
#include "Shader.h"
#include "Model.h"

void InitRender();
void CleanRender();

void RenderModels();
void Draw(ModelPtr model);

extern std::vector<ModelPtr> Models;