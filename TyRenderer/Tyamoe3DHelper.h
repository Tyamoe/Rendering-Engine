#pragma once

#ifndef TYAMOE3D

#else

#define IDENT(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define PATH(x,y) STR(IDENT(x)IDENT(y))

#define Dir2 ../../

#define EngineInc(x) PATH(Dir2,x)

#endif // TYAMOE3D