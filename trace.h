#ifndef _TRACE_H__
#define _TRACE_H__

#include <cstdio>
#include <GL/glut.h>
#include <cmath>

#include <cstdlib>
#include <time.h>

// GLM lib for matrix calculation
#include "config.h"

#include "global.h"
#include "object.h"

#include "iostream"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern glm::vec3 frame[WIN_HEIGHT][WIN_WIDTH];

extern float image_width;
extern float image_height;

extern glm::vec3 eye_pos;
extern float image_plane;

extern glm::vec3 null_clr;

extern Scene *scene;

extern int shadow_on;
extern int reflect_on;
extern int step_max;
extern int refract_on;
extern int difref_on;
extern int antiAlias_on;
extern int triangle_on;

extern int objectCount;

float max(float a, float b)
{
    return a > b ? a : b;
}

/**
 * phong illumination
*/
glm::vec3 phong(glm::vec3 q, glm::vec3 view, glm::vec3 surf_norm, Object *obj)
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

/**
 * recursive ray tracer
*/
glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray, int step)
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
        // std::cout << color.x << " " << color.y << " " << color.z << std::endl;
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

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *********************************************************************/
void ray_trace()
{
    int i, j;
    float x_grid_size = image_width / float(win_width);
    float y_grid_size = image_height / float(win_height);
    float x_start = -0.5 * image_width;
    float y_start = -0.5 * image_height;
    glm::vec3 ret_color;
    glm::vec3 cur_pixel_pos;

    // ray is cast through center of pixel
    cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
    cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
    cur_pixel_pos.z = image_plane;

    for (i = 0; i < win_height; i++)
    {
        for (j = 0; j < win_width; j++)
        {
            ret_color = glm::vec3(0, 0, 0);

            //ray = get_vec(eye_pos, cur_pixel_pos);
            glm::vec3 ray = cur_pixel_pos - eye_pos;

            //normalize(&ray);
            ray = glm::normalize(ray);

            //
            // You need to change this!!!
            //

            ret_color = recursive_ray_trace(eye_pos, ray, step_max);
            //else ret_color = background_clr; // just background for now

            // Parallel rays can be cast instead using below
            //
            // ray.x = ray.y = 0;
            // ray.z = -1.0;
            // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

            // Checkboard for testing
            // glm::vec3 clr = glm::vec3(float(i/32), 0, float(j/32));
            //ret_color = clr;

            frame[i][j] = ret_color;

            cur_pixel_pos.x += x_grid_size;
        }

        cur_pixel_pos.y += y_grid_size;
        cur_pixel_pos.x = x_start;
    }
}

#endif