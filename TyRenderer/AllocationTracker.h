#pragma once

#include "Types.h"

#include <new>
#include <stdlib.h>

TYvoid* operator new(TYsizet n);

TYvoid operator delete(TYvoid* ptr) noexcept;