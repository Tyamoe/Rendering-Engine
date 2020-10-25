#pragma once

#define DEBUG_MODE 0

#define UNREFERENCED_PARAMETER(P) (P)

#define IDENT(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define PATH(x,y) STR(IDENT(x)IDENT(y))

//#define Dir ../../TyEngine/

//#define EngineInc(x) PATH(Dir,x)