#include "stdafx.h"

#include "Utils.h"

TYint Hash(TYstring str)
{
	TYint seed = 131;//31  131 1313 13131131313 etc//
	TYint hash = 0;

	for (TYint i = 0; i < str.length(); i++)
	{
		hash = (hash * seed) + (str[i]);
	}

	return hash & (0x7FFFFFFF);
}