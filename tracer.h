#ifndef _TRACER_H__
#define _TRACER_H__

#include "scene.h"
#include "config.h"

class Tracer
{
  public:
    Tracer(Scene *sce, glm::vec3 **fra, float x, float y, int h, int w, float x_grid, float y_grid, float z, glm::vec3 eye_pos, int max)
        : scene(sce), frame(fra), x_start(x), y_start(y), height(h), width(w),
          x_grid_size(x_grid), y_grid_size(y_grid), image_z(z), eye_pos(eye_pos), step_max(max){};

    void setScene(Scene *myscene);
    void setFrame(glm::vec3 **frame);
    void ray_trace();
    glm::vec3 phong(glm::vec3 q, glm::vec3 view, glm::vec3 surf_norm, Object *obj);
    glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray, int step);

  private:
    Scene *scene;
    glm::vec3 **frame;
    float x_start;
    float y_start;
    int height;
    int width;
    float x_grid_size;
    float y_grid_size;
    float image_z;
    glm::vec3 eye_pos;
    int step_max;
};

void Tracer::setScene(Scene *myscene)
{
    scene = myscene;
}

void Tracer::setFrame(glm::vec3 **myframe)
{
    frame = myframe;
}

float max(float a, float b)
{
    return a > b ? a : b;
}

glm::vec3 Tracer::phong(glm::vec3 q, glm::vec3 view, glm::vec3 surf_norm, Object *obj)
{
    // I = global_ambient + local_ambient + f_decay(diffuse + specular)

    // global ambient
    glm::vec3 ga = scene->global_ambient * obj->getAmbient(q);
    // local ambient
    glm::vec3 la = scene->light_ambient * obj->getAmbient(q);

    // glm::vec3 to light
    glm::vec3 l = scene->light_position - q;
    float distance = glm::length(l);
    l = glm::normalize(l);

    // shadow ray
    if (scene->intersectScene(q, l, NULL) != NULL)
    {
        // std::cout << "shadow" << std::endl;
        glm::vec3 color = {ga.r + la.r,
                           ga.g + la.g,
                           ga.b + la.b};
        // color = {1.0f, 0.0f, 0.0f};
        return color;
    }

    // parameter for diffuse and specular
    float decay = scene->decay_a + scene->decay_b * distance + scene->decay_c * pow(distance, 2);
    float nl = max(glm::dot(surf_norm, l), 0.0);

    float theta = glm::dot(surf_norm, l);

    glm::vec3 r = 2 * theta * surf_norm - l;
    r = glm::normalize(r);

    float rv = max(glm::dot(r, view), 0.0);
    float rvN = pow(rv, obj->mat_shineness);

    // diffuse
    glm::vec3 diffuse = (scene->light_diffuse * obj->getDiffuse(q)) * nl / decay;

    // specular
    glm::vec3 specular = scene->light_specular * obj->getSpecular(q) * rvN / decay;

    glm::vec3 color = ga + la + diffuse + specular;

    return color;
}

glm::vec3 Tracer::recursive_ray_trace(glm::vec3 eye, glm::vec3 ray, int step)
{
    glm::vec3 hit;
    Object *obj = scene->intersectScene(eye, ray, &hit);
    glm::vec3 color;

    // for debug
    bool reflect_on = true;
    bool refract_on = true;

    if (obj == NULL)
    {
        color = scene->background_clr;
    }
    else
    {
        glm::vec3 view = glm::normalize(eye - hit);
        glm::vec3 surf_norm = obj->getNormal(hit);

        color = phong(hit, view, surf_norm, obj);
        if (step > 0 && reflect_on)
        {
            glm::vec3 reflected_view = glm::normalize(glm::rotate(view, glm::radians(180.0f), surf_norm));
            glm::vec3 reflected_color = recursive_ray_trace(hit, reflected_view, step - 1);

            color += reflected_color * obj->reflectance;
        }

        if (step > 0 && refract_on)
        {
            glm::vec3 refracted_view = glm::normalize(obj->getRefract(ray, hit));
            glm::vec3 refracted_color = recursive_ray_trace(hit, refracted_view, step - 1);

            color += refracted_color * 1.0f;
        }
    }

    return color;
}

void Tracer::ray_trace()
{
    int i, j;
    glm::vec3 ret_color;
    glm::vec3 cur_pixel_pos;

    // ray is cast through center of pixel
    cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
    cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
    cur_pixel_pos.z = image_z;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            ret_color = glm::vec3(0, 0, 0);

            glm::vec3 ray = cur_pixel_pos - eye_pos;
            ray = glm::normalize(ray);

            ret_color = recursive_ray_trace(eye_pos, ray, step_max);
            *((glm::vec3*)frame + width*i + j) = ret_color;

            cur_pixel_pos.x += x_grid_size;
        }
        cur_pixel_pos.y += y_grid_size;
        cur_pixel_pos.x = x_start;
    }
}

#endif