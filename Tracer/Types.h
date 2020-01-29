#pragma once

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

#define TYvector std::vector
#define TYmap std::map
#define TYset std::set
#define TYumap std::unordered_map
#define TYuset std::unordered_set

typedef int TYint;
typedef char TYchar;
typedef short TYshort;
typedef unsigned int TYuint;
typedef unsigned char TYuchar;
typedef unsigned short TYushort;
typedef float TYfloat;
typedef double TYdouble;
typedef std::string TYstring;
typedef bool TYbool;
typedef void TYvoid;
typedef long long int TYlint;
typedef unsigned long long int TYulint;

typedef std::vector<TYfloat> TYvectorf;
typedef std::vector<TYint> TYvectori;
typedef std::vector<TYbool> TYvectorb;
typedef std::vector<TYstring> TYvectors;
typedef std::vector<glm::vec3> TYvector3;

typedef glm::vec3 TYvec;
typedef glm::mat4 TYmat;
typedef glm::vec2 TYvec2;
typedef glm::vec4 TYvec4;
typedef glm::mat3 TYmat3;
typedef glm::tquat<glm::highp_float> TYquaternion;