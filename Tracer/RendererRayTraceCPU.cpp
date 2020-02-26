#include "stdafx.h"

#include "RendererRayTraceCPU.h"

#include "Utils.h"
#include "Globals.h"
#include "Geometry.h"

#define MAX_RAY_DEPTH 3 

TYint counter = 0;

TYfloat mix(TYfloat a, TYfloat b, TYfloat mix)
{
	return b * mix + a * (1 - mix);
}

TYvector<Geometry*> spheres;
PixelColorF RenderRayTraceCPU::Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth)
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	TYfloat tnear = TYinf;
	const Geometry* sphere = TYnull;
	// find intersection of this ray with the sphere in the scene
	for (TYuint i = 0; i < spheres.size(); ++i)
	{
		TYfloat t0 = TYinf, t1 = TYinf;
		if (spheres[i]->Intersect(rayOrigin, rayDir, t0, t1))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}

			if (t0 < tnear) 
			{
				tnear = t0;
				sphere = spheres[i];
			}
		}
	}

	// if there's no intersection return black or background color
	if (!sphere) return clearColor;

	PixelColorF surfaceColor = PixelColorF(); // color of the ray/surfaceof the object intersected by the ray 
	TYvec phit = rayOrigin + rayDir * tnear; // point of intersection 
	TYvec nhit = phit - sphere->center; // normal at the intersection point 
	nhit = glm::normalize(nhit); // normalize normal direction 

	// If the normal and the view direction are not opposite to each other
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.
	TYfloat bias = 1e-4f; // add some bias to the point from which we will be tracing 
	TYbool inside = false;

	if (glm::dot(rayDir, nhit) > 0)
	{
		nhit = -nhit;
		inside = true;
	}

	if ((sphere->transparency > 0 || sphere->reflection > 0) && rayDepth < MAX_RAY_DEPTH)
	{
		TYfloat facingratio = -glm::dot(rayDir, nhit);
		// change the mix value to tweak the effect
		TYfloat fresEffect = mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.1f);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		TYvec refldir = rayDir - nhit * 2.0f * glm::dot(rayDir, nhit);
		refldir = glm::normalize(refldir);
		PixelColorF reflect = Trace(phit + nhit * bias, refldir, rayDepth + 1);
		PixelColorF refract = PixelColorF();

		// if the sphere is also transparent compute refraction ray (transmission)
		if (sphere->transparency)
		{
			TYfloat ior = 1.1f, eta = (inside) ? ior : 1.0f / ior; // are we inside or outside the surface? 
			TYfloat cosi = -glm::dot(nhit, rayDir);
			TYfloat k = 1.0f - eta * eta * (1.0f - cosi * cosi);
			TYvec refrdir = rayDir * eta + nhit * (eta * cosi - sqrt(k));
			refrdir = glm::normalize(refrdir);

			refract = Trace(phit - nhit * bias, refrdir, rayDepth + 1);
		}

		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflect * fresEffect + refract * (1.0f - fresEffect) * sphere->transparency) * sphere->surfaceColor;
	}
	else
	{
		// it's a diffuse object, no need to raytrace any further
		for (TYuint i = 0; i < spheres.size(); ++i)
		{
			if (spheres[i]->emissionColor.r > 0)
			{
				// this is a light
				PixelColorF trans = PixelColorF(1.0f);
				TYvec lightDir = spheres[i]->center - phit;
				lightDir = glm::normalize(lightDir);

				for (TYuint j = 0; j < spheres.size(); ++j)
				{
					if (i != j)
					{
						TYfloat t0, t1;
						if (spheres[j]->Intersect(phit + nhit * bias, lightDir, t0, t1))
						{
							trans = PixelColorF();
							break;
						}
					}
				}

				surfaceColor += sphere->surfaceColor * trans * std::max(0.0f, glm::dot(nhit, lightDir)) * spheres[i]->emissionColor;
			}
		}
	}
	counter++;
	return surfaceColor + sphere->emissionColor;
}

TYvoid RenderRayTraceCPU::TraceRays()
{
	Layout layout = engine->GetWindow()->GetLayout();
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
	}
}

TYvoid RenderRayTraceCPU::PreRender()
{

}

TYvoid RenderRayTraceCPU::Render(TYfloat dt)
{
	Layout layout = engine->GetWindow()->GetLayout();

	/*
	// Ray Tracing to file
	TraceRays();

	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::ofstream ofs("./outputImages/frame.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << layout.width << " " << layout.height << "\n255\n";
	for (TYint i = 0; i < layout.width * layout.height; i++)
	{
		ofs << ImageBuffer[i].r << ImageBuffer[i].g << ImageBuffer[i].b;
	}
	ofs.close();
	*/

	glBindTexture(GL_TEXTURE_2D, Frame);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	PixelBuffer = (PixelColor*)glMapNamedBuffer(PBO, GL_WRITE_ONLY);

	// Clear Color Buffer
	//std::fill(PixelBuffer, PixelBuffer + pixelCount, clearColor);

	TraceRays();

	counter = 0;

	glUnmapNamedBuffer(PBO);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Draw to buffer
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glViewport(0, 0, layout.width, layout.height);

	glClearColor(0.9f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QuadShader->Use();
	QuadShader->DrawQuad(Frame);
}

TYvoid RenderRayTraceCPU::PostRender()
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);
}

TYuint RenderRayTraceCPU::AddMesh(Mesh& mesh)
{
	TYuint offset = Scene.size();
	Scene += mesh;

	return offset;
}

TYvoid RenderRayTraceCPU::Init()
{
	Layout layout = engine->GetWindow()->GetLayout();

	pixelCount = layout.width * layout.height;
	GLsizei byteCount = pixelCount * sizeof(PixelColor);

	ImageBuffer = new PixelColor[pixelCount];

	glCreateTextures(GL_TEXTURE_2D, 1, &Frame);
	glBindTexture(GL_TEXTURE_2D, Frame);
	glTextureStorage2D(Frame, 1, GL_RGBA8, layout.width, layout.height);

	glCreateBuffers(1, &PBO);
	glNamedBufferStorage(PBO, byteCount, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	PixelBuffer = (PixelColor*)glMapNamedBuffer(PBO, GL_WRITE_ONLY);
	std::fill(PixelBuffer, PixelBuffer + pixelCount, clearColor);

	glUnmapNamedBuffer(PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// FrameBuffer
	glGenFramebuffers(1, &RenderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);

	glGenTextures(1, &RenderTexture);
	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layout.width, layout.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in RaytraceCPU Renderer is not complete!" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderRayTraceCPU::RenderRayTraceCPU()
{
	QuadShader = new Shader("quad.vs", "quad.fs");

	// position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back(new Sphere(TYvec( 0.0, -10004, -20), -10000.0f, PixelColorF(0.20f, 0.20f, 0.20f),	0.1f, 0.0f));
	spheres.push_back(new Sphere(TYvec( 0.0, 0, -20),      4.0f,	 PixelColorF(1.0f, 0.32f, 0.36f),	1.0f, 0.5));
	spheres.push_back(new Sphere(TYvec( 5.0, -1, -15),	  2.0f,		 PixelColorF(0.90f, 0.82f, 0.36f),	1.0f, 0.0f));
	spheres.push_back(new Sphere(TYvec( 5.0, 0, -25),      3.0f,	 PixelColorF(0.65f, 0.57f, 0.97f),	1.0f, 0.0f));
	//spheres.push_back(new Sphere(TYvec(-5.5, 0, -15),     3.0f,		 PixelColorF(0.30f, 0.90f, 0.50f),	1.0f, 0.4f));
	spheres.push_back(new Sphere(TYvec(15.0, 0, -25), 3.0f, PixelColorF(0.97f, 0.27f, 0.97f), 1.0f, 0.0f));
	//spheres.push_back(new Sphere(TYvec(-5.5, 2, -30), 1.0f, PixelColorF(0.30f, 0.90f, 0.90f), 1.0f, 0.0f));
	// light
	spheres.push_back(new Sphere(TYvec(0.0, 20, -25), 3.0f, PixelColorF(), 0, 0.0, PixelColorF(3.0f)));


	spheres.push_back(new Triangle(TYvec(-10.5, 2, -30),   Vertex(TYvec(-10.5, 4, -30)), Vertex(TYvec(-9.0f, 1, -30)), Vertex(TYvec(-12.0f, 1, -30)),        PixelColorF(0.30f, 0.90f, 0.90f), 1.0f, 0.0f));
}

RenderRayTraceCPU::~RenderRayTraceCPU()
{

}