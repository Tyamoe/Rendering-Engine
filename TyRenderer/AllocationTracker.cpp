#include "AllocationTracker.h"

#include "Debugger.h"

void* operator new(size_t n)
{
	Debug::Memory::allocs++;
	Debug::Memory::bytesUsed += n + sizeof(size_t);
	Debug::Memory::bytesInUse += n + sizeof(size_t);

	TYsizet* p = (TYsizet*)malloc(n + sizeof(TYsizet));

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

	TYsizet* p = (TYsizet*)ptr;
	TYsizet size = p[-1];

	Debug::Memory::bytesInUse -= size + sizeof(TYsizet);

	TYvoid* p2 = (TYvoid*)(&p[-1]);
	free(p2);
}