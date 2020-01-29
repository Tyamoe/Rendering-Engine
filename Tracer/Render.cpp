#include "stdafx.h"

#include "Shader.h"
#include "Render.h"
#include "Model.h"
#include "Camera.h"

ShaderPtr ShaderFlatColor;
ShaderPtr ShaderPhong;

CameraPtr camera;
glm::mat4 Projection = glm::mat4(1.0f);

std::vector<ModelPtr> Models;

void InitRender()
{
	ShaderPhong = new Shader("phong.vs", "phong.fs");

	ShaderFlatColor = new Shader("FlatColor.vs", "FlatColor.fs");
	
	camera = new Camera();
}

void CleanRender()
{
	delete ShaderPhong;
	delete ShaderFlatColor;
	delete camera;
}

void RenderModels()
{
	camera->Update(1.0f/60.0f);

	Projection = glm::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f);
	camera->view = glm::lookAt(camera->Position, camera->Position + camera->Front, camera->Up);

	for(int i = 0; i < Models.size(); i++)
	{
		Draw(Models[i]);
	}
}

void Draw(ModelPtr model)
{
	glm::mat4 modelMatrix = model->GetMatrix();
	glm::mat4 proj = Projection * camera->view;

	glm::mat4 MVP = proj * modelMatrix;

	ShaderPhong->Use();

	glm::mat4 inv = glm::inverse(model->GetMatrix());
	inv = glm::transpose(inv);

	ShaderPhong->setMat4(ShaderPhong->Uniforms["MVP"], MVP);
	ShaderPhong->setMat4(ShaderPhong->Uniforms["Model"], model->GetMatrix());
	ShaderPhong->setMat4(ShaderPhong->Uniforms["InvTrModel"], inv);

	ShaderPhong->setVec3(ShaderPhong->Uniforms["lightPos"], glm::vec3(0.f, 10.f, 1.f));
	ShaderPhong->setVec3(ShaderPhong->Uniforms["lightColor"], glm::vec3(1.f, 1.f, 1.f));
	ShaderPhong->setVec3(ShaderPhong->Uniforms["viewPos"], camera->Position);
	ShaderPhong->setVec4(ShaderPhong->Uniforms["oColor"], glm::vec4(0.4f, 0.4f, 0.4f, 1.f));

	glBindVertexArray(model->VAO);

	glDrawElements(GL_TRIANGLES, model->Indices.size(), GL_UNSIGNED_INT, 0);
}
