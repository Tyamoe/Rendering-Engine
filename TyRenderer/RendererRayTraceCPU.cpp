/*

	Ray Trace SDF
	https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html
	http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/

*/

#include "RendererRayTraceCPU.h"

#include "Window.h"
#include "Input.h"

#include "GenericDraw.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Geometry.h"

#include "Entity.h"
#include "Mesh.h"
#include "Material.h"

#include "Octree.h"
#include "BoundingVolume.h"

#include "Utils.h"
#include "GLUtils.h"
#include "ImGuiUtils.h"
#include "Globals.h"

TYvoid TraceThread(TYint x, TYint y, TYint sx, TYint sy, PixelColor*& PixelBuffer, RenderRayTraceCPU* rt, TYint index);

struct ThreadData
{
	ThreadData(TYint x, TYint y, TYint sx, TYint sy, PixelColor*& PB, RenderRayTraceCPU* rt, TYint index) :
		x_(x), y_(y), sx_(sx), sy_(sy), PB_(PB), rt_(rt), index_(index)
	{}

	TYint x_;
	TYint y_;
	TYint sx_;
	TYint sy_;
	TYint index_;
	PixelColor*& PB_;
	RenderRayTraceCPU* rt_;
};

#define MAX_RAY_DEPTH 2 

TYvector<TyAtomic<TYint>> rayCount;

TYvoid RenderRayTraceCPU::UpdateData()
{
	//Layout layout = engine->GetWindow()->GetLayout();
	TYint width = TYint(traceData.width);
	TYint height = TYint(traceData.height);

	TYfloat fov = Global::FOV;

	traceData.aspect = traceData.width / traceData.height;
	traceData.angle = tanf(0.5f * glm::radians(Global::FOV));

	traceData.invWidth = 1.0f / traceData.width;
	traceData.invHeight = 1.0f / traceData.height;

	traceData.Origin = camera->position;/// camera->view* TYvec4(0.0f, 0.0f, 0.0f, 1.0f);

	camera->dim.y = glm::tan(fov / 2.0f);
	camera->dim.x = camera->dim.y * traceData.aspect;

	/*int i = 0;
	for (TYint y = height - 1; y >= 0; y--)
	{
		for (TYint x = 0; x < width; x++, i++)
		{
			TYfloat xx = (2.0f * ((x + 0.5f) * traceData.invWidth) - 1.0f) * traceData.aspect * traceData.angle;
			TYfloat yy = (1.0f - 2.0f * ((y + 0.5f) * traceData.invHeight)) * traceData.angle;

			// Ray direction considering camera
			TYvec ir = (camera->view * TYvec4(xx, yy, -1.0f, 1.0f));
			ir = ir - traceData.Origin;
			ir = glm::normalize(ir);

			traceData.Direction[i] = ir;
		}
	}*/
}

PixelColorF RenderRayTraceCPU::Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth, TYint& rayCounter, TYint ignore = -1)
{
	rayCounter++;
	Global::DevCounter++;
	TYvec normal = TYvec(0.0f);
	TYfloat tnear = TYinf;
	Geometry* hit = TYnull;
	TYint hitIndex = -1;

	// Intersect ray with scene
	//for (TYuint i = 0; i < scene->geometry.size(); i++)
	for (TYuint i = 0; i < scene->entityList.size(); i++)
	{
		if (i == ignore) continue;
		TYvec norm = TYvec(0.0f);
		TYfloat t0 = TYinf, t1 = TYinf;

		//if (scene->geometry[i]->Intersect(rayOrigin, rayDir, t0, t1, norm))
		if (Geometry::Intersect(scene->entityList[i], rayOrigin, rayDir, t0, t1, norm))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}

			if (t0 < tnear) 
			{
				Mesh* mesh = scene->entityList[i]->Get<Mesh*>();

				tnear = t0;
				hit = mesh->GetGeometry(-1);// scene->geometry[i];
				normal = norm;
				hitIndex = i;
			}
		}
	}

	// No intersection
	if (!hit) return clearColor;
	if (hit->emissionColor.r > 0.0f) return hit->emissionColor;

	// Color of the ray/surfaceof the object intersected by the ray 
	PixelColorF surfaceColor = PixelColorF(); 

	// Compute point of intersection 
	TYvec phit = rayOrigin + rayDir * tnear; 
	TYvec nhit = normal;

	nhit = glm::normalize(nhit);

	TYfloat DdotN = glm::dot(rayDir, nhit);
	TYfloat bias = 1e-4f;
	TYbool inside = false;
	
	/*	
		If the normal and the direction are not opposite to each other
		reverse the normal direction. That also means we are inside the sphere so set
		the inside bool to true. Finally reverse the sign of IdotN which we want
		positive.
	*/
	if (DdotN > 0)
	{
		inside = true;

		nhit = -nhit;
		DdotN = glm::dot(rayDir, nhit);
	}

	/*************************************/
	if (hit->GetType() == geoModel)
	{
		//surfaceColor = glm::normalize(phit);
		if (Global::DevBool)
		{
			lineLock.lock();

			lines.push_back(phit);
			lines.push_back(phit + nhit * bias);

			lineLock.unlock();
		}
	}

	PixelColorF incomingColor = PixelColorF();

	if ((hit->transparency > 0 || hit->reflection > 0) && rayDepth < MAX_RAY_DEPTH)
	{
		// Compute reflection direction
		TYvec refldir = rayDir - nhit * 2.0f * DdotN;
		refldir = glm::normalize(refldir);

		TYint rays = 0;

		// Compute reflection color
		PixelColorF reflect = Trace(phit + nhit * bias, refldir, rayDepth + 1, rays, hitIndex);

		PixelColorF refract = PixelColorF();

		TYfloat facingratio = -DdotN;
		TYfloat fresEffect = Mix<TYfloat>(pow<TYfloat>(1.0f - facingratio, 3.0f), 1.0f, 0.1f);

		rayCounter += rays;

		// Compute refraction ray
		if (hit->transparency)
		{
			TYfloat ior = hit->transparency, eta = (inside) ? ior : 1.0f / ior; // are we inside or outside the surface? 
			TYfloat cosi = -glm::dot(nhit, rayDir);

			TYfloat k = 1.0f - eta * eta * (1.0f - cosi * cosi);

			// Compute refraction direction
			TYvec refrdir = rayDir * eta + nhit * (eta * cosi - sqrt(k));
			refrdir = glm::normalize(refrdir);

			TYint rays1 = 0;

			// Compute refraction color
			refract = Trace(phit - nhit * bias, refrdir, rayDepth + 1, rays1, hitIndex);

			rayCounter += rays1;
		}

		// Mix of reflection and refraction
		incomingColor = ((reflect * hit->reflection) * fresEffect + refract * (1.0f - fresEffect) * 1.0f) * hit->surfaceColor;
	}

	// Phong Step
	TYvec lightDir = scene->geometry[0]->center - phit;
	lightDir = normalize(lightDir);

	TYfloat diff = glm::max(glm::dot(nhit, lightDir) + 0.15f, 0.0f);
	PixelColorF diffuse = diff * scene->geometry[0]->emissionColor;

	TYfloat specularStrength = 0.4f;
	TYvec viewDir = glm::normalize(rayOrigin - phit);
	TYvec reflectDir = glm::reflect(-lightDir, nhit);
	TYfloat spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), 32.0f);
	PixelColorF specular = specularStrength * spec * scene->geometry[0]->emissionColor;

	surfaceColor = (diffuse + specular) * hit->surfaceColor;

	TYfloat mixVal = glm::clamp(hit->reflection + hit->transparency, 0.0f, 1.0f);
	surfaceColor = Mix(surfaceColor, incomingColor, PixelColorF(mixVal));

	// Shadow Step
	PixelColorF trans = PixelColorF(1.0f);

	//for (TYuint j = 1; j < scene->geometry.size(); j++)
	for (TYuint j = 0; j < scene->entityList.size(); j++)
	{
		TYfloat t0, t1;
		//if (scene->geometry[j]->Intersect(phit + nhit * bias, lightDir, t0, t1, normal))
		if (Geometry::Intersect(scene->entityList[j], phit + nhit * bias, lightDir, t0, t1, normal))
		{
			Mesh* mesh = scene->entityList[j]->Get<Mesh*>();
			Material* mat = mesh->Get<Material*>();

			TYfloat t = mat->GetTransparency();// scene->geometry[j]->transparency;
			if (t != 0.0f)
			{
				t = t <= 1.0f ? 1.0f : glm::clamp(1.0f - (t - 1.0f), 0.4f, 0.95f);
			}
			else
			{
				t = 0.15f;
			}

			trans = PixelColorF(t);
			break;
		}
	}
	surfaceColor = surfaceColor * trans;
	PixelColorF emis = PixelColorF();// hit->emissionColor == PixelColorF() ? (hit->surfaceColor* 0.02f) : hit->emissionColor;
	return surfaceColor + emis;
}

TYvoid TraceThread(TYint x, TYint y, TYint sx, TYint sy, PixelColor*& PixelBuffer, RenderRayTraceCPU* rt, TYint index)
{
	while (rt->traceData.tracing)
	{
		// Wait for trace to trigger
		rt->block->Wait();

		TYint rayc = 0;

		// Evalute Pixels in this thread group
		//for (TYint y_ = y; y_ > y - sy; y_--)
		for (TYint y_ = y - sy + 1;  y_ < y + 1;  y_++)
		{
			TYint i = y_ * TYint(rt->traceData.width) + x;
			for (TYint x_ = x; x_ < sx + x; x_++, i++)
			{
				if (rt->traceData.dirty)
				{
					/*TYfloat xx = (2.0f * ((x_ + 0.5f) * rt->traceData.invWidth) - 1.0f) * rt->traceData.aspect * rt->traceData.angle;
					TYfloat yy = (1.0f - 2.0f * ((y_ + 0.5f) * rt->traceData.invHeight)) * rt->traceData.angle;

					// Ray direction considering camera
					TYvec ir = (rt->camera->view * TYvec4(xx, yy, -1.0f, 1.0f));
					ir = ir - rt->traceData.Origin;
					ir = glm::normalize(ir);

					rt->traceData.Direction[i] = ir;*/

					/////////////////////////////////////////////
					TYvec2 screenCoord;
					screenCoord.x = (2.0f * x_) / rt->traceData.width - 1.0f;
					screenCoord.y = (-2.0f * (rt->traceData.height - y_)) / rt->traceData.height + 1.0f;

					TYvec dir = rt->camera->front + screenCoord.x * rt->camera->dim.x * rt->camera->right + 
						screenCoord.y * rt->camera->dim.y * rt->camera->up;

					rt->traceData.Direction[i] = glm::normalize(dir);
					/////////////////////////////////////////////
				}

				TYint rays = 0;
				PixelColor pixel = static_cast<PixelColor>(rt->Trace(rt->traceData.Origin, rt->traceData.Direction[i], 0, rays));
				rayc += rays;
				pixel.Clamp();
				PixelBuffer[i] = pixel;
			}
		}

		std::atomic<int> a_i(rayc);
		rayCount[index] = a_i;

		rt->count++;

		// All threads traced
		rt->countLock.lock();
		if (rt->count == rt->traceData.ThreadCount)
		{
			rt->count = 0;

			// Unblock copy to texture
			rt->traceWait->Notify();
		}
		rt->countLock.unlock();

		// Wait for all threads
		rt->traceBarrier->Wait();
	}
}

TYvoid RenderRayTraceCPU::TraceRays()
{
	if (Global::MultiThreadBool)
	{
		// Try to trace
		traceWait->Wait();

		// Trigger trace threads
		block->NotifyAll(traceData.ThreadCount);
	}
	else
	{
		Layout layout = window->GetLayout();
		TYint width = layout.width;
		TYint height = layout.height;

		TYfloat fov = Global::FOV;
		TYfloat aspect = width / TYfloat(height);
		TYfloat angle = tanf(TYpi * 0.5f * fov / 180.0f);
		TYfloat invWidth = 1.0f / TYfloat(width), invHeight = 1.0f / TYfloat(height);

		TYint rays = 0;

		int i = 0;
		for (TYint y = height; y > 0; y--)
		{
			for (TYint x = 0; x < width; x++, i++)
			{
				PixelColor pixel = static_cast<PixelColor>(this->Trace(this->traceData.Origin, this->traceData.Direction[i], 0, rays));
				pixel.Clamp();
				PixelBuffer[i] = pixel;
			}
		}
	}
}

TYvoid RenderRayTraceCPU::PreRender()
{
	Global::CulledTries = 0;
	Global::TriCount = 0;

	Global::DevCounter = 0;

	Global::DevFrameCount++;
}

TYvoid RenderRayTraceCPU::Render(TYfloat dt)
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	traceData.dirty = camera->Update(dt);

	glBindTexture(GL_TEXTURE_2D, Frame);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	PixelBuffer = (PixelColor*)glMapNamedBuffer(PBO, GL_WRITE_ONLY);

	// Update rays
	if(traceData.dirty)
		UpdateData();

	// Trace frame
	TraceRays();

	// Try to copy data to texture
	if (Global::MultiThreadBool)
		traceWait->Wait();

	glUnmapNamedBuffer(PBO);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Allow next frame to be traced
	if (Global::MultiThreadBool)
		traceWait->Notify();

	// Draw to buffer
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glViewport(0, 0, layout.width, layout.height);

	glClearColor(0.9f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BloomShader->Use();
	BloomShader->DrawQuad(Frame, true);

	ImGui::Begin("Details");

	ImGui::Text("Camera Pos: %.3f, %.3f, %.3f", camera->position.x, camera->position.y, camera->position.z);
	ImGui::Text("Camera Front: %.3f, %.3f, %.3f", camera->front.x, camera->front.y, camera->front.z);

	ImGui::End();
}

TYvoid RenderRayTraceCPU::PostRender()
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);

	GenericDraw::DrawSphere(TYvec(0.0f), 2.0f, TYvec(1.0f, 0.0f, 0.0f));

	if (Input::isMouseReleased(MouseRight))
	{
		for (TYuint i = 0; i < scene->geometry.size(); i++)
		{
			if (scene->geometry[i]->GetType() == geoModel)
			{
				if (((Model*)scene->geometry[i])->octree)
				{
					((Model*)scene->geometry[i])->octree->Traverse(((Model*)scene->geometry[i])->octree->root);
				}
			}
		}
	}

	/* Draw BVH */
	if (Global::DevBool)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (BVH* b : scene->bvh)
		{
			b->head->Draw();
		}

		GenericDraw::DrawSphere(TYvec(0.0f), 2.0f, TYvec(1.0f, 1.0f, 1.0f));

		for (TYsizet i = 0; i < lines.size(); i += 2)
		{
			GenericDraw::DrawSphere(lines[i], 0.1f, TYvec(0.0f, 0.25f, 0.5f));
			GenericDraw::DrawSphere(lines[i + 1], 0.1f, TYvec(0.6f, 0.4f, 0.0f));
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		lines.clear();
	}
}

TYvoid RenderRayTraceCPU::ThreadHeight()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	traceData.ThreadCount = height;

	rayCount = TYvector<TyAtomic<TYint>>(traceData.ThreadCount);

	tracingThreads = new std::thread[traceData.ThreadCount];

	block = new Semaphore();
	traceWait = new Semaphore(1);

	traceBarrier = new Barrier(traceData.ThreadCount);

	int i = 0;
	for (TYint y = height - 1; y >= 0; y--)
	{
		tracingThreads[i] = std::thread(TraceThread, 0, y, width, 1, std::ref(PixelBuffer), this, i);
		i++;
	}
}

TYvoid RenderRayTraceCPU::ThreadWidth()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	traceData.ThreadCount = width;

	rayCount = TYvector< TyAtomic<TYint> >(traceData.ThreadCount);

	tracingThreads = new std::thread[traceData.ThreadCount];

	block = new Semaphore();
	traceWait = new Semaphore(1);

	traceBarrier = new Barrier(traceData.ThreadCount);

	int i = 0;
	for (TYint x = 0; x < width; x++)
	{
		tracingThreads[i] = std::thread(TraceThread, x, height - 1, 1, height, std::ref(PixelBuffer), this, i);
		i++;
	}
}

TYvoid RenderRayTraceCPU::ThreadGrid()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	TYint grid = Global::DevThreadCount; // 4;

	traceData.ThreadCount = grid * grid;

	rayCount = TYvector< TyAtomic<TYint> >(traceData.ThreadCount);

	tracingThreads = new std::thread[traceData.ThreadCount];

	block = new Semaphore();
	traceWait = new Semaphore(1);

	traceBarrier = new Barrier(traceData.ThreadCount);

	TYint sx = width / grid;
	TYint sy = height / grid;

	TYint c = 0;
	for (TYint i = 0; i < grid; i++)
	{
		for (TYint j = 0; j < grid; j++, c++)
		{
			tracingThreads[c] = std::thread(TraceThread, sx * j, height - sy * i - 1, sx, sy, std::ref(PixelBuffer), this, c);
		}
	}
}

TYvoid RenderRayTraceCPU::ThreadStrip()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	traceData.ThreadCount = Global::DevThreadCount; //6;

	rayCount = TYvector< TyAtomic<TYint> >(traceData.ThreadCount);

	tracingThreads = new std::thread[traceData.ThreadCount];

	block = new Semaphore();
	traceWait = new Semaphore(1);

	traceBarrier = new Barrier(traceData.ThreadCount);

	TYint sx = width;

	TYint sy = height / traceData.ThreadCount;

	for (TYint i = 0; i < traceData.ThreadCount; i++)
	{
		tracingThreads[i] = std::thread(TraceThread, 0, height - sy * i - 1, sx, sy, std::ref(PixelBuffer), this, i);
	}
}

TYvoid RenderRayTraceCPU::Init()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	traceData.width = TYfloat(width);
	traceData.height = TYfloat(height);

	traceData.Direction = TYvector3(width * height, TYvec(0));

	camera = new Camera(true);

	pixelCount = width * height;
	GLsizei byteCount = pixelCount * sizeof(PixelColor);

	// Setup pixel buffer texture
	glCreateTextures(GL_TEXTURE_2D, 1, &Frame);
	glBindTexture(GL_TEXTURE_2D, Frame);
	glTextureStorage2D(Frame, 1, GL_RGBA8, layout.width, layout.height);

	// Setup pixel buffer
	glCreateBuffers(1, &PBO);
	glNamedBufferStorage(PBO, byteCount, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	PixelBuffer = (PixelColor*)glMapNamedBuffer(PBO, GL_WRITE_ONLY);
	std::fill(PixelBuffer, PixelBuffer + pixelCount, clearColor);

	glUnmapNamedBuffer(PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Setup frameBuffer
	glGenFramebuffers(1, &RenderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);

	// Setup render texture
	glGenTextures(1, &RenderTexture);
	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in RaytraceCPU Renderer is not complete!" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ThreadStrip();
}

RenderRayTraceCPU::RenderRayTraceCPU() : Renderer()
{
	SetType(RendererType::RayTraceCPU);

	Mesh::GenHandles = false;

	QuadShader = new Shader("quad.vs", "quad.fs");
	BloomShader = new Shader("bloom.vs", "bloom.fs");
	ColorShader = new Shader("color.vs", "color.fs");

	scene = new Scene(1);
	scene->GenOctree();
}

RenderRayTraceCPU::~RenderRayTraceCPU()
{
	traceData.tracing = false;
	for (TYint i = 0; i < traceData.ThreadCount; i++)
	{
		tracingThreads[i].join();
	}
	delete[] tracingThreads;

	delete block;
	delete traceWait;
	delete traceBarrier;

	delete BloomShader;
	delete QuadShader;
	delete ColorShader;

	glDeleteBuffers(1, &PBO);

	glDeleteTextures(1, &RenderTexture);
	glDeleteTextures(1, &Frame);

	glDeleteFramebuffers(1, &RenderBuffer);
}