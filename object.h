#ifndef _OBJECT_H__
#define _OBJECT_H__

#include "config.h"
#include "iostream"
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

  float refract_factor;

  Object(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, float refr, float factor)
      : index(id), mat_ambient(amb), mat_diffuse(dif), mat_specular(spe),
        mat_shineness(shine), reflectance(refl), refractance(refr), refract_factor(factor)
  {
    type = TRIANGLE;
  };

  virtual float intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit) = 0;

  virtual glm::vec3 getNormal(glm::vec3 point) = 0;

  glm::vec3 getRefract(glm::vec3 inRay, glm::vec3 point);

  virtual glm::vec3 getAmbient(glm::vec3 point) = 0;

  virtual glm::vec3 getDiffuse(glm::vec3 point) = 0;

  virtual glm::vec3 getSpecular(glm::vec3 point) = 0;
};

glm::vec3 Object::getRefract(glm::vec3 inRay, glm::vec3 point)
{
  glm::vec3 outRay;
  inRay = glm::normalize(-inRay); //inverse the inRay
  glm::vec3 surf_norm = getNormal(point);
  float r1;
  float r2;
  // std::cout << type << std::endl;
  if (glm::dot(inRay, surf_norm) > 0)
  {
    // into object
    r1 = 1.0;
    r2 = refract_factor;
  }
  else
  {
    // outside object
    surf_norm = -surf_norm;
    r1 = refract_factor;
    r2 = 1.0;
  }

  float ratio = r1 / r2;

  float delta = 1 - pow(ratio, 2) * (1 - pow(glm::dot(inRay, surf_norm), 2));

  outRay = float((ratio * glm::dot(inRay, surf_norm) - sqrt(delta))) * surf_norm - ratio * inRay;

  return outRay;
}

#endif