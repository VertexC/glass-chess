#ifndef _CONFIG_H__
#define _CONFIG_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#ifdef __APPLE__ // include Mac OS X verions of headers
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#endif // __APPLE__

enum objType
{
    TRIANGLE = 0,
    PLANE = 1
};

const float precision = 0.01;
const float inf = 10000000.0;



float min_float(float a, float b)
{
    return a > b ? b : a;
}

float max_float(float a, float b)
{
    return  a > b ? a : b;
}

glm::vec3 min_vec3(glm::vec3 a, glm::vec3 b)
{
    return glm::vec3(
        min_float(a.x, b.x),
        min_float(a.y, b.y),
        min_float(a.z, b.z));
}

glm::vec3 max_vec3(glm::vec3 a, glm::vec3 b)
{
    return glm::vec3(
        max_float(a.x, b.x),
        max_float(a.y, b.y),
        max_float(a.z, b.z));
}

#endif