#ifndef _SCENE_H__
#define _SCENE_H__

#include "config.h"
#include <vector>
#include "object.h"
#include "triangle.h"
void SMF_reader(const char *filepath, int *vertexNum, int *faceNum, glm::vec3 *&vertexes, int *&indexes);

// global values
class Scene
{
  public:
    std::vector<Object *> objectList;
    glm::vec3 light_position;
    glm::vec3 light_ambient;
    glm::vec3 light_diffuse;
    glm::vec3 light_specular;
    glm::vec3 global_ambient;

    glm::vec3 background_clr;
    float decay_a;
    float decay_b;
    float decay_c;
    int objectCount;

    Scene()
    {
        light_position = glm::vec3(-2.0, 5.0, 1.0);
        light_ambient = glm::vec3(0.1, 0.1, 0.1);
        light_diffuse = glm::vec3(1.0, 1.0, 1.0);
        light_specular = glm::vec3(1.0, 1.0, 1.0);

        global_ambient = glm::vec3(0.2, 0.2, 0.2);
        background_clr = glm::vec3(0.0, 0.0, 0.0);

        decay_a = 0.5;
        decay_b = 0.3;
        decay_c = 0.0;

        objectCount = 0;
    }

    void set_chess();
    Object *intersectScene(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit);
};

Object *Scene::intersectScene(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit)
{
    glm::vec3 intersect_hit;
    Object *intersect_obj = NULL;
    float min_distance = inf;

    bool is_intersect = false;

    for (int i = 0; i < objectList.size(); i++)
    {

        glm::vec3 temp_hit;
        float temp_distance;

        temp_distance = objectList[i]->intersect(eye, ray, &temp_hit);
        if (temp_distance > precision)
        {
            if (temp_distance < min_distance)
            {
                min_distance = temp_distance;
                intersect_obj = objectList[i];
                intersect_hit = temp_hit;
                is_intersect = true;
            }
        }
    }

    if (is_intersect)
    {
        hit->x = intersect_hit.x;
        hit->y = intersect_hit.y;
        hit->z = intersect_hit.z;
    }

    return intersect_obj;
}

void Scene::set_chess()
{
    int vertexNum = 0;
    int faceNum = 0;

    glm::vec3 *vertexes = NULL;
    int *indexes = NULL;

    SMF_reader("./chess_pieces/chess_piece.smf", &vertexNum, &faceNum, vertexes, indexes);

    glm::vec3 mat_ambient = glm::vec3(0.75, 0.5, 0.5);
    glm::vec3 mat_diffuse = glm::vec3(0.1, 0.5, 0.5);
    glm::vec3 mat_specular = glm::vec3(1.0, 1.0, 1.0);
    float shineness = 40;
    float reflectance = 0.6;
    float refractance = 0.2;

    glm::vec3 center = glm::vec3(0.0, -1, -1);
    glm::vec3 point[3];
    int objectCount = 0;

    for (int i = 0; i < faceNum; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int index = indexes[i * 3 + j];
            point[j] = glm::vec3(vertexes[index].x + center.x, vertexes[index].y + center.y, vertexes[index].z + center.z);
        }
        objectList.push_back(new Triangle(++objectCount, mat_ambient, mat_diffuse, mat_specular, shineness, reflectance, refractance,
                                          point[0], point[1], point[2]));
    }
    printf("sizeof object in scene:%d", objectList.size());
}

void SMF_reader(const char *filepath, int *vertexNum, int *faceNum, glm::vec3 *&vertexes, int *&indexes)
{
    FILE *fileptr = NULL;
    fileptr = fopen(filepath, "r");
    if (fileptr == NULL)
    {
        printf("Error: Cannot open file %s\n", filepath);
        fclose(fileptr);
        exit(1);
    }

    if (fscanf(fileptr, " # %d %d", vertexNum, faceNum) < 2)
    {
        printf("Error: No Vertex and Face numer read.\n");
        fclose(fileptr);
        exit(1);
    }

    vertexes = new glm::vec3[*vertexNum * 3];
    for (int i = 0; i < *vertexNum; i++)
        if (fscanf(fileptr, " v %f %f %f", &(vertexes[i].x), &(vertexes[i].y), &(vertexes[i].z)) < 3)
        {
            printf("Error: Cannot read the vertexes\n");
            fclose(fileptr);
            exit(1);
        }

    indexes = new int[*faceNum * 3];

    for (int i = 0; i < *faceNum; i++)
    {
        int a, b, c;
        if (fscanf(fileptr, " f %d %d %d", &a, &b, &c) < 3)
        {
            printf("Error: Cannot read the indexes\n");
            fclose(fileptr);
            exit(1);
        }
        indexes[i * 3] = a - 1;
        indexes[i * 3 + 1] = b - 1;
        indexes[i * 3 + 2] = c - 1;
    }

    printf("read SMF done\n");
    fclose(fileptr);
}

#endif