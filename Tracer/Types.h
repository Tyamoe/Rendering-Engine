#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define TYpi 3.141592653589793f
#define TYinf 1e8 
#define TYepsilon glm::epsilon<float>() 

#define TYlog std::cout
#define TYlogbreak std::endl

#define TYnull nullptr

#define TYvector std::vector
#define TYmap std::map
#define TYset std::set
#define TYumap std::unordered_map
#define TYuset std::unordered_set

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
typedef unsigned long long int TYulint;

typedef std::vector<TYint> TYvectorI;
typedef std::vector<TYuint> TYvectorUI;
typedef std::vector<TYfloat> TYvectorF;
typedef std::vector<TYbool> TYvectorB;
typedef std::vector<TYstring> TYvectorS;
typedef std::vector<glm::vec3> TYvector3;

typedef glm::vec3 TYvec;
typedef glm::mat4 TYmat;
typedef glm::vec2 TYvec2;
typedef glm::vec4 TYvec4;
typedef glm::mat3 TYmat3;
typedef glm::tquat<glm::highp_float> TYquaternion;