#ifndef _OBJECT_H__
#define _OBJECT_H__

#include "config.h"

class Object
{
public:
  int index;
  objType type;

  glm::vec3 mat_ambient;
  glm::vec3 mat_diffuse;
  glm::vec3 mat_specular;

  float mat_shineness;
  float reflectance;
  float refractance;

  Object(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, float refr)
      : index(id), mat_ambient(amb), mat_diffuse(dif), mat_specular(spe),
        mat_shineness(shine), reflectance(refl), refractance(refr)
  {
    type = TRIANGLE;
  };

  virtual float intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit) = 0;

  virtual glm::vec3 getNormal(glm::vec3 point) = 0;
};

#endif