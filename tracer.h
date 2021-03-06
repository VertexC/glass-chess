#ifndef _TRACER_H__
#define _TRACER_H__

#include "scene.h"
#include "config.h"
#include "intersectInfo.h"
#include "bvh.h"
class Tracer
{
  public:
    Tracer(Scene *sce, glm::vec3 **fra, float x_s, float y_s, int h, int w, float x_grid, float y_grid, float z, glm::vec3 eye_pos, int max,
           int x_lb, int y_lb, int x_rt, int y_rt, Bvh *bvh)
        : scene(sce), frame(fra), x_start(x_s), y_start(y_s), height(h), width(w),
          x_grid_size(x_grid), y_grid_size(y_grid), image_z(z), eye_pos(eye_pos), step_max(max),
          x_lb(x_lb), y_lb(y_lb), x_rt(x_rt), y_rt(y_rt), bvh(bvh)
    {
        intersect_count = 0;
    };

    void setScene(Scene *myscene);
    void setFrame(glm::vec3 **frame);
    int getIntersectCount()
    {
        return intersect_count;
    }
    void setIntersectCount(int count)
    {
        intersect_count = count;
    }
    void AddIntersectCount(int count)
    {
        intersect_count += count;
    }
    void ray_trace();
    bool in_range(int x, int y);
    glm::vec3 phong(glm::vec3 q, glm::vec3 view, glm::vec3 surf_norm, Object *obj);
    glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray, int step);

  private:
    int intersect_count;

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

    int x_lb;
    int y_lb;
    int x_rt;
    int y_rt;
    Bvh *bvh;
};

void Tracer::setScene(Scene *myscene)
{
    scene = myscene;
}

void Tracer::setFrame(glm::vec3 **myframe)
{
    frame = myframe;
}

bool Tracer::in_range(int x, int y)
{
    return x >= x_lb && x <= x_rt && y >= y_lb && y <= y_rt;
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
    if (bvh == NULL)
    {
        int count = 0;
        if (scene->intersectScene(q, l, NULL, &count) != NULL)
        {
            // std::cout << "shadow" << std::endl;
            glm::vec3 color = {ga.r + la.r,
                               ga.g + la.g,
                               ga.b + la.b};
            // color = {1.0f, 0.0f, 0.0f};
            AddIntersectCount(count);
            return color;
        }
    }
    else
    {
        int count = 0;
        IntersectInfo intersect_info;
        if (bvh->getIntersection(q, l, &intersect_info, &count))
        {
            // std::cout << "shadow" << std::endl;
            glm::vec3 color = {ga.r + la.r,
                               ga.g + la.g,
                               ga.b + la.b};
            // color = {1.0f, 0.0f, 0.0f};
            AddIntersectCount(count);
            return color;
        }
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
    // simply assume that board is placed belowed the triangles
    glm::vec3 hit_board;
    Object *obj_board = NULL;
    float distance = scene->board->intersect(eye, ray, &hit_board);

    if (distance > 0.0f)
    {
        obj_board = scene->board;
    }

    glm::vec3 hit;
    Object *obj = NULL;
    distance = 0;
    if (bvh != NULL)
    {
        IntersectInfo intersect_info;
        int count = 0; // count for intersection
        bvh->getIntersection(eye, ray, &intersect_info, &count);
        obj = intersect_info.object;
        hit = intersect_info.hit;
        AddIntersectCount(count);
        // std::cout << "get object!" << std::endl;
    }
    else
    {
        int count = 0; // count for intersection
        obj = scene->intersectScene(eye, ray, &hit, &count);
        AddIntersectCount(count);
    }

    if (obj == NULL && obj_board != NULL)
    {
        obj = obj_board;
        hit = hit_board;
    }

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
            if (!in_range(i, j))
            {
                continue;
            }
            cur_pixel_pos.x = j * x_grid_size + x_start;
            cur_pixel_pos.y = i * y_grid_size + y_start;
            ret_color = glm::vec3(0, 0, 0);

            glm::vec3 ray = cur_pixel_pos - eye_pos;
            ray = glm::normalize(ray);

            ret_color = recursive_ray_trace(eye_pos, ray, step_max);
            *((glm::vec3 *)frame + width * i + j) = ret_color;
        }
    }
}

#endif