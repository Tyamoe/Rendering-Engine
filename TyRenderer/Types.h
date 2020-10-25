#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <limits>

#include "MathUtils.h"

#define TYmaxf std::numeric_limits<float>::max()
#define TYpi 3.141592653589793f
#define TYinf 1e8f
#define TYepsilon glm::epsilon<float>() 

#define TYlog std::cout
#define TYlogbreak std::endl

#define TYnull nullptr

#define TYstack std::stack
#define TYqueue std::queue
#define TYvector std::vector
#define TYmap std::map
#define TYset std::set
#define TYumap std::unordered_map
#define TYuset std::unordered_set
#define TYpair std::pair
#define TYtuple std::tuple

typedef char* TYcharPtr;
typedef const char* TYcstring;
typedef int TYint;
typedef char TYchar;
typedef short TYshort;
typedef size_t TYsizet;
typedef unsigned int TYuint;
typedef unsigned char TYuchar;
typedef unsigned char TYubyte;
typedef unsigned short TYushort;
typedef float TYfloat;
typedef double TYdouble;
typedef std::string TYstring;
typedef bool TYbool;
typedef void TYvoid;
typedef long long int TYlint;
typedef long long int TYlong;
typedef unsigned long long int TYulint;

typedef std::vector<TYint> TYvectorI;
typedef std::vector<TYuint> TYvectorUI;
typedef std::vector<TYfloat> TYvectorF;
typedef std::vector<TYbool> TYvectorB;
typedef std::vector<TYstring> TYvectorS;
typedef std::vector<glm::vec3> TYvector3;

typedef glm::vec2 TYvec2;
typedef glm::vec3 TYvec;
typedef glm::vec3 TYvec3;
typedef glm::vec4 TYvec4;
typedef glm::mat3 TYmat3;
typedef glm::mat4 TYmat;
typedef glm::mat4 TYmat4;

typedef glm::quat TYquaternion;

typedef glm::ivec2 TYivec2;
typedef glm::ivec3 TYivec3;
typedef glm::ivec4 TYivec4;

typedef glm::uvec4 TYuvec4; 