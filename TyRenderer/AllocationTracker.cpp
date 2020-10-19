/******************************************************************************/
/*!
\file   MemoryManager.cpp
\author Chris Thomas
\par    email: chris.thomas\@digipen.edu
\par    DigiPen login: chris.thomas
\par    Course: GAM200
\par    Copyright © 2018 DigiPen (USA) Corporation.
\brief
	 Implementation for Memory manager system.

*/
/******************************************************************************/

#include "stdafx.h"

#include "AllocationTracker.h"
#include "Debugger.h"

void* __CRTDECL operator new(size_t n)
{
	Debug::Memory::allocs++;
	Debug::Memory::bytesUsed += n + sizeof(size_t);
	Debug::Memory::bytesInUse += n + sizeof(size_t);

	TYsizet *p = (TYsizet*)malloc(n + sizeof(TYsizet));

	p[0] = n;
	return (TYvoid*)(&p[1]);
}

TYvoid operator delete(TYvoid* ptr) noexcept
{
	if (!ptr)
	{
		return;
	}

	Debug::Memory::deallocs++;

	TYsizet *p = (TYsizet*)ptr;
	TYsizet size = p[-1];

	Debug::Memory::bytesInUse -= size + sizeof(TYsizet);

	TYvoid *p2 = (TYvoid*)(&p[-1]);
	free(p2);
}