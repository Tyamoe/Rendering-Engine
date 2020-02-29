#include "stdafx.h"

#include "Utils.h"
#include "Globals.h"

#include "RendererRayTraceCPU.h"

#define MAX_RAY_DEPTH 3 

TYvoid RenderRayTraceCPU::UpdateData()
{
	Layout layout = engine->GetWindow()->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	TYfloat fov = Global::FOV;
	TYfloat aspect = width / TYfloat(height);

	TYfloat angle = tanf(TYpi * 0.5f * fov / 180.0f);

	TYfloat invWidth = 1.0f / TYfloat(width), invHeight = 1.0f / TYfloat(height);

	// Perspective and inverse
	TYmat persp = glm::perspective(glm::radians(Global::FOV), TYfloat(width) / height, 0.1f, 1000.0f);
	persp = glm::inverse(persp);

	// Ray origin considering camera
	traceData.Origin = camera->view * TYvec4(0.0f, 0.0f, 0.0f, 1.0f);

	int i = 0;
	for (TYint y = height - 1; y >= 0; y--)
	{
		for (TYint x = 0; x < width; x++, i++)
		{
			TYfloat xx = (2.0f * ((x + 0.5f) * invWidth) - 1.0f) * aspect * angle;
			TYfloat yy = (1.0f - 2.0f * ((y + 0.5f) * invHeight)) * angle;

			TYvec dir(xx, yy, -1.0f);
			dir = glm::normalize(dir);

			// Ray direction considering camera
			//TYvec ir = (persp * TYvec4(x / width, y / height, 0.0f, 1.0f));
			TYvec ir = (camera->view * TYvec4(xx, yy, -1.0f, 1.0f));
			ir = ir - traceData.Origin;
			ir = glm::normalize(ir);

			traceData.Direction[i] = ir;
		}
	}
}

PixelColorF RenderRayTraceCPU::Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth, TYint modifier)
{
	TYvec normal = TYvec(0.0f);
	TYfloat tnear = TYinf;
	Geometry* hit = TYnull;

	// Intersect ray with scene
	for (TYuint i = 0; i < Scene.size() - modifier; i++)
	{
		TYvec norm = TYvec(0.0f);
		TYfloat t0 = TYinf, t1 = TYinf;
		if (Scene[i]->Intersect(rayOrigin, rayDir, t0, t1, norm))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}

			if (t0 < tnear) 
			{
				tnear = t0;
				hit = Scene[i];
				normal = norm;
			}
		}
	}

	// No intersection
	if (!hit) return clearColor;

	// Color of the ray/surfaceof the object intersected by the ray 
	PixelColorF surfaceColor = PixelColorF(); 

	// Compute point of intersection 
	TYvec phit = rayOrigin + rayDir * tnear; 

	// Compute normal at the intersection point 
	TYvec nhit = normal;// phit - sphere->center;

	/*if (sphere->GetType() == geoTriangle)
	{
		nhit = glm::normalize(glm::cross(sphere->vertices[1].vertex - sphere->vertices[0].vertex, sphere->vertices[2].vertex - sphere->vertices[0].vertex));
	}*/
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

	if ((hit->transparency > 0 || hit->reflection > 0) && rayDepth < MAX_RAY_DEPTH)
	{
		// Compute reflection direction
		TYvec refldir = rayDir - nhit * 2.0f * DdotN;
		refldir = glm::normalize(refldir);

		// Compute reflection color
		PixelColorF reflect = Trace(phit + nhit * bias, refldir, rayDepth + 1);

		PixelColorF refract = PixelColorF();

		TYfloat facingratio = -DdotN;
		TYfloat fresEffect = Mix<TYfloat>(pow<TYfloat>(1.0f - facingratio, 3.0f), 1.0f, 0.1f);

		// Compute refraction ray
		if (hit->transparency)
		{
			TYfloat ior = 1.1f, eta = (inside) ? ior : 1.0f / ior; // are we inside or outside the surface? 
			TYfloat cosi = -glm::dot(nhit, rayDir);

			TYfloat k = 1.0f - eta * eta * (1.0f - cosi * cosi);

			// Compute refraction direction
			TYvec refrdir = rayDir * eta + nhit * (eta * cosi - sqrt(k));
			refrdir = glm::normalize(refrdir);

			// Compute refraction color
			refract = Trace(phit - nhit * bias, refrdir, rayDepth + 1);
		}

		// Mix of reflection and refraction
		surfaceColor = (reflect * fresEffect + refract * (1.0f - fresEffect) * hit->transparency) * hit->surfaceColor;
	}
	else
	{
		for (TYuint i = 0; i < Scene.size(); i++)
		{
			if (Scene[i]->emissionColor.r > 0.0f) // It is a light
			{
				PixelColorF trans = PixelColorF(1.0f);

				TYvec lightDir = Scene[i]->center - phit;
				lightDir = glm::normalize(lightDir);

				for (TYuint j = 0; j < Scene.size() - modifier; j++)
				{
					if (i != j)
					{
						TYfloat t0, t1;
						if (Scene[j]->Intersect(phit + nhit * bias, lightDir, t0, t1, normal))
						{
							trans = PixelColorF();
							break;
						}
					}
				}
				surfaceColor += hit->surfaceColor * trans * std::max(0.0f, glm::dot(nhit, lightDir)) * Scene[i]->emissionColor;
			}
		}
	}
	return surfaceColor + hit->emissionColor;
}

TYvoid TraceThread(TYint y, PixelColor*& PixelBuffer, TYint width, RenderRayTraceCPUPtr rt)
{
	while (rt->traceData.tracing)
	{
		// Wait for trace to trigger
		rt->block->Wait();

		TYint i = y * width;
		for (TYint x = 0; x < width; x++, i++)
		{
			TYint mod = x > width / 2 ? 0 : 0;
			PixelColor pixel = static_cast<PixelColor>(rt->Trace(rt->traceData.Origin, rt->traceData.Direction[i], 0, mod));
			pixel.Clamp();
			PixelBuffer[i] = pixel;
		}

		rt->count++;

		// All threads traced
		rt->countLock.lock();
		if (rt->count == rt->traceData.height)
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
	// Try to trace
	traceWait->Wait();

	// Trigger trace threads
	for (TYint i = 0; i < traceData.height; i++)
	{
		block->Notify();
	}

	/*Layout layout = engine->GetWindow()->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	TYfloat fov = Global::FOV;
	TYfloat aspect = width / TYfloat(height);
	TYfloat angle = tanf(TYpi * 0.5f * fov / 180.0f);
	TYfloat invWidth = 1.0f / TYfloat(width), invHeight = 1.0f / TYfloat(height);

	int i = 0;
	for (TYint y = height; y > 0; y--)
	{
		for (TYint x = 0; x < width; x++, i++)
		{
			TYfloat xx = (2.0f * ((x + 0.5f) * invWidth) - 1.0f) * angle * aspect;
			TYfloat yy = (1.0f - 2.0f * ((y + 0.5f) * invHeight)) * angle;

			TYvec dir(xx, yy, -1.0f);
			dir = glm::normalize(dir);

			PixelBuffer[i] = static_cast<PixelColor>(Trace(TYvec(0.0f), dir, 0));
			PixelBuffer[i].Clamp();
		}
	}*/
}

TYvoid RenderRayTraceCPU::PreRender()
{
	Global::CulledTries = 0;
	Global::TriCount = 0;
}

TYvoid RenderRayTraceCPU::Render(TYfloat dt)
{
	Layout layout = engine->GetWindow()->GetLayout();

	camera->Update(dt);

	glBindTexture(GL_TEXTURE_2D, Frame);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	PixelBuffer = (PixelColor*)glMapNamedBuffer(PBO, GL_WRITE_ONLY);

	// Update rays
	UpdateData();

	// Trace frame
	TraceRays();

	// Try to copy data to texture
	traceWait->Wait();

	glUnmapNamedBuffer(PBO);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Allow next frame to be traced
	traceWait->Notify();

	// Draw to buffer
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glViewport(0, 0, layout.width, layout.height);

	glClearColor(0.9f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BloomShader->Use();
	BloomShader->DrawQuad(Frame);
}

TYvoid RenderRayTraceCPU::PostRender()
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);
}

TYuint RenderRayTraceCPU::AddMesh(Mesh& mesh)
{
	TYuint offset = Scene.size();
	//Scene += mesh;

	return offset;
}

TYvoid RenderRayTraceCPU::Init()
{
	Layout layout = engine->GetWindow()->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	traceData.width = width;
	traceData.height = height;

	traceData.Direction = TYvector3(width * height, TYvec(0));

	camera = new Camera(engine->GetWindow()->GetInput(), true);

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

	// Setup threads
	tracingThreads = new std::thread[height];
	threadCount = height;

	block = new Semaphore();
	traceWait = new Semaphore(1);

	traceBarrier = new Barrier(height);

	int i = 0;
	for (TYint y = height - 1; y >= 0; y--)
	{
		tracingThreads[i] = std::thread(TraceThread, y, std::ref(PixelBuffer), width, this);
		i++;
	}
}

RenderRayTraceCPU::RenderRayTraceCPU()
{
	QuadShader = new Shader("quad.vs", "quad.fs");
	BloomShader = new Shader("bloom.vs", "bloom.fs");

	// position, radius, surface color, reflectivity, transparency, emission color
	//Scene.push_back(new Sphere(TYvec( 0.0, -10007, -20), -10000.0f, PixelColorF(0.20f, 0.20f, 0.20f),	0.1f, 0.0f));
	//Scene.push_back(new Sphere(TYvec( 0.0, -1, -20),      4.0f,	 PixelColorF(1.0f, 0.32f, 0.36f),	1.0f, 0.5));
	//Scene.push_back(new Sphere(TYvec( 5.0, -2, -15),	  2.0f,		 PixelColorF(0.90f, 0.82f, 0.36f),	1.0f, 0.0f));
	//Scene.push_back(new Sphere(TYvec( 5.0, -1, -25),      3.0f,	 PixelColorF(0.65f, 0.57f, 0.97f),	1.0f, 0.0f));
	//Scene.push_back(new Sphere(TYvec(-5.5, -2, -15),     3.0f,		 PixelColorF(0.30f, 0.90f, 0.50f),	1.0f, 0.4f));
	//Scene.push_back(new Sphere(TYvec(15.0, -1, -25), 3.0f, PixelColorF(0.97f, 0.27f, 0.97f), 1.0f, 0.0f));
	//Scene.push_back(new Sphere(TYvec(-5.5, -3, -30), 1.0f, PixelColorF(0.30f, 0.90f, 0.90f), 1.0f, 0.0f));
	// light
	//Scene.push_back(new Sphere(TYvec(0.0, 20, -25), 3.0f, PixelColorF(0.8f, 0.75f, 0.15f), 0, 0.0, PixelColorF(1.0f, 1.0f, 0.3f)));


	//Scene.push_back(new Triangle(TYvec(-10.5, -3, -20),	Vertex(TYvec(-10.5, 4, -25)),	Vertex(TYvec(-9.0f, 1, -20)),	Vertex(TYvec(-12.0f, 1, -20)),  PixelColorF(0.10f, 0.90f, 0.10f), 1.0f, 0.0f));
	//Scene.push_back(new Triangle(TYvec(10.5, -3, -22),		Vertex(TYvec(10.5, 4, -30)),	Vertex(TYvec(9.0f, 1, -22)),	Vertex(TYvec(12.0f, 1, -22)),	PixelColorF(0.10f, 0.10f, 0.90f), 1.0f, 0.4f));
	//Scene.push_back(new Triangle(TYvec(0.5, -3, -22),		Vertex(TYvec(0.5, 5, -20)),		Vertex(TYvec(3.0f, 0, -21)),	Vertex(TYvec(-2.0f, 1, -19)),	PixelColorF(0.90f, 0.10f, 0.10f), 1.0f, 0.0f));
	//Scene.push_back(new Triangle(TYvec(11.5, -2, -15),		Vertex(TYvec(11.15, 3, -15)),	Vertex(TYvec(16.0f, -2, -10)),	Vertex(TYvec(10.0f, -2, -10)),	PixelColorF(0.90f, 0.90f, 0.10f), 1.0f, 0.84f));
	//
	//Scene.push_back(new Model("./resources/models/bunny.obj", PixelColorF(0.14f, 0.12f, 0.562f), 1.0f, 0.0f));
}

RenderRayTraceCPU::~RenderRayTraceCPU()
{
	traceData.tracing = false;
	for (TYint i = 0; i < threadCount; i++)
	{
		tracingThreads[i].join();
	}
	delete[] tracingThreads;
}