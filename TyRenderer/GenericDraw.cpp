#include "GenericDraw.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Camera.h"
#include "TyRenderer.h"

#include "GLUtils.h"

static TYint CircleIndices = 20;
static TYint SphereIndices = 0;

struct Character
{
	TYuint TextureID;
	TYivec2 Size;
	TYivec2 Bearing;
	TYuint Advance;
};

static FT_Library library;
static FT_Face face;

static TYmap<TYint, Character> CharsEng;

static TYbool tmpTextInit = false;

TYvoid GenericDraw::Destroy()
{
	if (tmpTextInit)
	{
		for (auto& ch : CharsEng)
		{
			glDeleteTextures(1, &ch.second.TextureID);
		}
		
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	glDeleteVertexArrays(1, &CubeHandle);
	glDeleteVertexArrays(1, &SphereHandle);
	glDeleteVertexArrays(1, &QuadHandle);
	glDeleteVertexArrays(1, &QuadUVHandle);
	glDeleteVertexArrays(1, &CircleHandle);
	glDeleteVertexArrays(1, &CircleUVHandle);

	glDeleteBuffers(1, &fsQuadHandle);
	glDeleteBuffers(1, &vboCubeHandle);

	glDeleteVertexArrays(1, &LineVAO);
	glDeleteBuffers(1, &LineVBO);

	delete ColorShader;
	delete Color2DShader;
	delete TextureShader;
	delete Texture2DShader;
	delete TextShader;
}

TYvoid GenericDraw::Init()
{
	ColorShader = new Shader("color.vs", "color.fs");
	Color2DShader = new Shader("color2d.vs", "color2d.fs");
	TextureShader = new Shader("texture.vs", "texture.fs");
	Texture2DShader = new Shader("texture2d.vs", "texture2d.fs");
	TextShader = new Shader("text.vs", "text.fs");

	// Sphere
	{
		TYvector3 SphereVertices;

		TYvec prev;
		TYvec cur(0.0f);

		for (TYfloat d = 0; d < 360; d += 12)
		{
			prev = cur;
			cur.x = cosf(glm::radians(d));
			cur.z = sinf(glm::radians(d));

			if (d > 0)
			{
				SphereVertices.push_back(prev);
				SphereVertices.push_back(cur);
			}
		}
		SphereVertices.push_back(SphereVertices.back());
		SphereVertices.push_back(SphereVertices.front());

		TYvector3 ye = SphereVertices;
		for (TYint i = 1; i <= 3; i++)
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1), 1.57079632679f * i, TYvec(0, 0, 1));
			for (TYvec p : ye)
				SphereVertices.push_back(rot * glm::vec4(p, 1));
		}

		glm::mat4 rot = glm::rotate(glm::mat4(1), 1.57079632679f, TYvec(1, 0, 0));
		for (TYvec p : ye)
			SphereVertices.push_back(rot * glm::vec4(p, 1));

		TYuint wireVbo = 0;
		glGenVertexArrays(1, &SphereHandle);
		glGenBuffers(1, &wireVbo);

		glBindVertexArray(SphereHandle);
		glBindBuffer(GL_ARRAY_BUFFER, wireVbo);

		glBufferData(GL_ARRAY_BUFFER, SphereVertices.size() * sizeof(TYvec), &SphereVertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);

		glBindVertexArray(0);

		glDeleteBuffers(1, &wireVbo);

		SphereIndices = (TYint)SphereVertices.size();
	}

	// Cube
	{
		glGenVertexArrays(1, &CubeHandle);
		glGenBuffers(1, &vboCubeHandle);

		glBindVertexArray(CubeHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vboCubeHandle);

		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);

		glBindVertexArray(0);
	}

	// FS Quad
	{
		glGenBuffers(1, &fsQuadHandle);
		glBindBuffer(GL_ARRAY_BUFFER, fsQuadHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FSQuadVertices), &FSQuadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)(2 * sizeof(TYfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Quad
	{
		TYuint vbo, ibo;
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &QuadHandle);
		glGenBuffers(1, &ibo);

		glBindVertexArray(QuadHandle);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices2), QuadVertices2, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(TYfloat), (TYvoid*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);

		// Textured
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &QuadUVHandle);
		glGenBuffers(1, &ibo);

		glBindVertexArray(QuadUVHandle);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)(2 * sizeof(TYfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	// Circle
	{
		TYuint vbo;

		TYvector<TYvec2> CircleVertices;
		TYvector<TYvec2> CircleVertices2;

		TYfloat radius = 0.5f;
		TYint numSeqments = 20;

		for (TYint i = 0; i < CircleIndices; i++)
		{
			TYfloat theta = 2.0f * TYpi * TYfloat(i) / TYfloat(CircleIndices);

			TYfloat x = radius * glm::cos(theta);
			TYfloat y = radius * glm::sin(theta);

			CircleVertices.push_back({ x, y });
			CircleVertices2.push_back({ x, y });

			TYfloat tx = (x / radius + 1.0f) * 0.5f;
			TYfloat ty = (y / radius + 1.0f) * 0.5f;

			CircleVertices.push_back({ tx, ty });
		}

		glGenVertexArrays(1, &CircleHandle);
		glGenBuffers(1, &vbo);

		glBindVertexArray(CircleHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, CircleVertices2.size() * sizeof(TYvec2), &CircleVertices2[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(TYfloat), (TYvoid*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vbo);

		// Textured
		glGenVertexArrays(1, &CircleUVHandle);
		glGenBuffers(1, &vbo);

		glBindVertexArray(CircleUVHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, CircleVertices.size() * sizeof(TYvec2), &CircleVertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)2);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vbo);
	}
}

TYvoid GenericDraw::Update(TYfloat dt)
{
	for (TYpair<TYvec, TYfloat> sphere: RecurringSpheres)
	{
		GenericDraw::DrawSphere(sphere.first, sphere.second, TYvec(0, 0, 0));
	}
}

TYvoid GenericDraw::DrawCube(TYvec pos, TYvec size, TYvec color, TYfloat width)
{
	ColorShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, size );

	ColorShader->Uniforms["Model"](model);
	ColorShader->Uniforms["View"](MainCamera->view);
	ColorShader->Uniforms["Proj"](MainCamera->proj);
	ColorShader->Uniforms["oColor"](color);

	glBindVertexArray(CubeHandle);

	/*glBindBuffer(GL_ARRAY_BUFFER, vboCubeHandle);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);*/

	if (width > 0.0f)
	{
		glLineWidth(width);
		glDrawArrays(GL_LINES, 0, 24);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 24);
	}

	glBindVertexArray(0);
}

TYvoid GenericDraw::DrawSphere(TYvec pos, TYfloat radius, TYvec color, TYfloat width)
{
	ColorShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, TYvec(radius));

	ColorShader->Uniforms["Model"](model);
	ColorShader->Uniforms["View"](MainCamera->view);
	ColorShader->Uniforms["Proj"](MainCamera->proj);
	ColorShader->Uniforms["oColor"](color);

	glBindVertexArray(SphereHandle);

	if (width > 0.0f)
	{
		glLineWidth(width);
		glDrawArrays(GL_LINES, 0, SphereIndices);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, SphereIndices);
	}

	glBindVertexArray(0);
}

TYvoid GenericDraw::DrawLine(TYvec s, TYvec e, TYvec color, TYfloat width)
{
	if (LineVAO == 0)
	{
		TYfloat quadVertices[] =
		{
			s.x,  s.y, s.z,
			e.x,  e.y, e.z
		};

		//glDeleteBuffers(1, &quadVBO);
		//glDeleteBuffers(1, &quadVAO);

		glGenVertexArrays(1, &LineVAO);
		glBindVertexArray(LineVAO);

		glGenBuffers(1, &LineVBO);

		glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);
	}
	else
	{
		TYfloat quadVertices[] =
		{
			s.x,  s.y, s.z,
			e.x,  e.y, e.z
		};

		glBindVertexArray(LineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, LineVBO);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), &quadVertices);
	}

	ColorShader->Use();

	TYmat model(1.0f);

	ColorShader->Uniforms["Model"](model);
	ColorShader->Uniforms["View"](MainCamera->view);
	ColorShader->Uniforms["Proj"](MainCamera->proj);
	ColorShader->Uniforms["oColor"](color);

	glBindVertexArray(LineVAO);
	glLineWidth(width);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}

TYvoid GenericDraw::DrawQuad(TYvec2 pos, TYvec2 size, TYuint texture)
{
	glDisable(GL_CULL_FACE);

	Texture2DShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, { pos, 0.0f });
	model = glm::scale(model, { size, 1.0f });

	Texture2DShader->Uniforms["texture1"](0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Texture2DShader->Uniforms["repeat"](1.0f);

	Texture2DShader->Uniforms["Model"](model);
	Texture2DShader->Uniforms["View"](MainCamera->view);
	Texture2DShader->Uniforms["Proj"](MainCamera->proj);

	glBindVertexArray(QuadUVHandle);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
}

TYvoid GenericDraw::DrawQuad(TYvec2 pos, TYvec2 size, TYvec color)
{
	glDisable(GL_CULL_FACE);

	Color2DShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, { pos, 0.0f });
	model = glm::scale(model, { size, 1.0f });

	Color2DShader->Uniforms["Model"](model);
	Color2DShader->Uniforms["View"](MainCamera->view);
	Color2DShader->Uniforms["Proj"](MainCamera->proj);
	Color2DShader->Uniforms["oColor"](color);

	glBindVertexArray(QuadHandle);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
}

TYvoid GenericDraw::DrawCircle(TYvec2 pos, TYfloat radius, TYuint texture)
{
	glDisable(GL_CULL_FACE);

	Texture2DShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, { pos, 0.0f });
	model = glm::scale(model, { radius, radius, 1.0f });

	Texture2DShader->Uniforms["texture1"](0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Texture2DShader->Uniforms["repeat"](1.0f);

	Texture2DShader->Uniforms["Model"](model);
	Texture2DShader->Uniforms["View"](MainCamera->view);
	Texture2DShader->Uniforms["Proj"](MainCamera->proj);

	glBindVertexArray(CircleHandle);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CircleIndices);

	glEnable(GL_CULL_FACE);
}

TYvoid GenericDraw::DrawCircle(TYvec2 pos, TYfloat radius, TYvec color)
{
	static TYmat proj(1.0f);
	if (proj == TYmat(1.0f))
	{
		proj = glm::ortho(0.0f, TYfloat(TyRenderer::width), 0.0f, TYfloat(TyRenderer::height), -1.0f, 1.0f);
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Color2DShader->Use();

	TYmat ss(1.0f);
	TYmat model(1.0f);
	model = glm::translate(model, { pos, 0.0f });
	model = glm::scale(model, { radius, radius, 1.0f });

	Color2DShader->Uniforms["Model"](model);
	Color2DShader->Uniforms["View"](ss);
	Color2DShader->Uniforms["Proj"](proj);
	Color2DShader->Uniforms["oColor"](color);

	glBindVertexArray(CircleHandle);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CircleIndices);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

TYvoid GenericDraw::DrawFSQuad(TYuint texture)
{
	Texture2DShader->Use();

	TYmat model(1.0f);

	Texture2DShader->Uniforms["texture1"](0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Texture2DShader->Uniforms["repeat"](1.0f);

	Texture2DShader->Uniforms["Model"](model);
	Texture2DShader->Uniforms["View"](model);
	Texture2DShader->Uniforms["Proj"](model);

	{
		static TYuint vbo = 0;

		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &fsQuadHandle);

		glGenVertexArrays(1, &fsQuadHandle);
		glGenBuffers(1, &vbo);

		glBindVertexArray(fsQuadHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FSQuadVertices), &FSQuadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), (TYvoid*)(2 * sizeof(TYfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(fsQuadHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static TYfloat AvgCharacterSize = 0.0f;

static TYcstring eng_font = "./resources/fonts/Audiowide/Audiowide-Regular.ttf";
static TYcstring eng_font2 = "./resources/fonts/Noto_Sans_JP/NotoSansJP-Regular.otf";

static TYuint TextVAO = 0;
static TYuint TextVBO = 0;

#define PIXEL_TO_SCALE(x) (x / 12.0f) * 0.04f

TYvoid GenericDraw::DrawText(TYstring text, TYvec2 position, TYfloat scale, TYvec4 color)
{
	if (!tmpTextInit)
	{
		tmpTextInit = true;

		int error = FT_Init_FreeType(&library);
		if (error) TYlog << "Init_FreeType Error " << error << TYlogbreak;

		error = FT_New_Face(library, eng_font2, 0, &face);

		if (error == FT_Err_Unknown_File_Format)
		{
			TYlog << "FT_Err_Unknown_File_Format" << TYlogbreak;
		}

		error = FT_Set_Pixel_Sizes(face, 0, 10 * 48);

		CharsEng.clear();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (TYubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			TYuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character
			{
					texture,
					TYivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					TYivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					(TYuint)face->glyph->advance.x
			};

			//AvgCharacterSize += (TYfloat)character.Size.x;
			AvgCharacterSize += (TYfloat)character.Size.x + ((TYfloat)character.Bearing.x * 2.0f);

			CharsEng.insert(TYpair<TYchar, Character>(c, character));
		}
		//AvgCharacterSize /= 128.0f;
		AvgCharacterSize /= 128.0f;

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenVertexArrays(1, &TextVAO);
		glGenBuffers(1, &TextVBO);

		glBindVertexArray(TextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(TYfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// 2D Text
	//scale = PIXEL_TO_SCALE(scale);

	//position += TYvec2((TYfloat)TyRenderer::width / 2.0f, (TYfloat)TyRenderer::height / 2.0f);
	//position.x -= (AvgCharacterSize * scale) * (TYfloat(text.size()) / 2.0f);

	TextShader->Use();

	// 3D Text
	TextShader->Uniforms["View"](MainCamera->view);
	TextShader->Uniforms["Proj"](MainCamera->proj);

	TextShader->Uniforms["textColor"](color);

	glBindVertexArray(TextVAO);

	// Iterate through all characters
	TYstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = CharsEng[*c];

		GLfloat xpos = position.x + ch.Bearing.x * scale;
		GLfloat ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		GLfloat vertices[6][4] =
		{
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		TextShader->Uniforms["text"](0);

		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		position.x += (ch.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

TYvoid GenericDraw::DrawText(TYstring text, TYvec position, TYfloat scale, TYvec4 color)
{
	if (!tmpTextInit)
	{
		tmpTextInit = true;

		int error = FT_Init_FreeType(&library);
		if (error) TYlog << "Init_FreeType Error " << error << TYlogbreak;

		error = FT_New_Face(library, eng_font2, 0, &face);

		if (error == FT_Err_Unknown_File_Format)
		{
			TYlog << "FT_Err_Unknown_File_Format" << TYlogbreak;
		}

		error = FT_Set_Pixel_Sizes(face, 0, 10 * 48);

		CharsEng.clear();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (TYubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			TYuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character
			{
					texture,
					TYivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					TYivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					(TYuint)face->glyph->advance.x
			};

			//AvgCharacterSize += (TYfloat)character.Size.x;
			AvgCharacterSize += (TYfloat)character.Size.x + ((TYfloat)character.Bearing.x * 2.0f);

			CharsEng.insert(TYpair<TYchar, Character>(c, character));
		}
		AvgCharacterSize /= 128.0f;

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenVertexArrays(1, &TextVAO);
		glGenBuffers(1, &TextVBO);

		glBindVertexArray(TextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(TYfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(TYfloat), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	TextShader->Use();

	TYmat model = glm::translate(TYmat(1.0f), position);

	// 3D Text
	TextShader->Uniforms["Model"](model);
	TextShader->Uniforms["View"](MainCamera->view);
	TextShader->Uniforms["Proj"](MainCamera->proj);

	TextShader->Uniforms["textColor"](color);

	glBindVertexArray(TextVAO);

	TYvec2 pPos = {};

	// Iterate through all characters
	TYstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = CharsEng[*c];

		GLfloat xpos = pPos.x + ch.Bearing.x * scale;
		GLfloat ypos = pPos.y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		GLfloat vertices[6][4] =
		{
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		TextShader->Uniforms["text"](0);

		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		pPos.x += (ch.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}