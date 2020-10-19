/******************************************************************************/
/*!
\file   MemoryManager.h
\author Chris Thomas
\par    email: chris.thomas\@digipen.edu
\par    DigiPen login: chris.thomas
\par    Course: GAM200
\par    Copyright © 2018 DigiPen (USA) Corporation.
\brief
	 Interface for the memory manager

*/
/******************************************************************************/

#pragma once

#include "Types.h"

#include <new>
#include <stdlib.h>

TYvoid* __CRTDECL operator new(TYsizet n);

TYvoid operator delete(TYvoid* ptr) noexcept;