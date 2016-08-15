#ifndef PRT_hpp
#define PRT_hpp

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

//typedef glm::vec3 float3;
//typedef glm::vec2 float2;
//typedef glm::mat4 float4x4;
//typedef glm::mat3 float3x3;

#define Property(type, name, var) \
public: \
	void Set#name (const type& value) { var = value; } \
	type Get#name () const { return var; } \
private: \
	type var;

#endif