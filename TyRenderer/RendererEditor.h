#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Mesh.h"

#define wEditProperties           (TYuint)1		// 2^0, bit 0
#define wDisplayEntities          (TYuint)2		// 2^1, bit 1
#define wRayTraceSettings         (TYuint)4		// 2^2, bit 2
#define wFlatColorSettings        (TYuint)8		// 2^3, bit 3
#define wLoadModel				  (TYuint)16	// 2^4, bit 4
#define wAcceleratedStructure     (TYuint)32    // 2^5, bit 5
#define wDefault				  (TYuint)64	// 2^6, bit 6
#define wAll					  (TYuint)128	// 2^7, bit 7

class Windows
{
	public:
		Windows() { value = wDefault; }
		Windows(TYuint w) : value(w) {}

		TYvoid operator+=(TYuint rhs)	// Add Flag
		{
			value |= rhs;
		}

		TYvoid operator-=(TYuint rhs)	 // Remove Flag
		{
			value &= ~rhs;
		}

		TYbool operator==(TYuint rhs)	// Check if flag is set
		{
			return (value & rhs) == rhs;
		}

	private:
		TYuint value = wDefault;
};

class RenderEditor : public Renderer
{
public:
	RenderEditor(TYbool pReadonly = false, Windows pActiveWindows = Windows(wAll));
	~RenderEditor();

	TYvoid Init();

	TYvoid PreRender();
	TYvoid Render(TYfloat dt);
	TYvoid PostRender();

private:
	TYbool readonly = false;
	Windows activeWindows;

	ImGuiContext* context = TYnull;
};

typedef RenderEditor* RenderEditorPtr;