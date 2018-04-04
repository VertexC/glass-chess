#ifndef _BVH_H__
#define _BVH_H__

#include "bbox.h"
#include "object.h"
#include <iostream>
#include <vector>
#include <stdint.h>

struct BvhNode
{
    BBox bbox;
    uint32_t start, prim_num, offset;
};

class Bvh
{
  public:
    uint32_t node_num, leaf_num, leaf_size;
    std::vector<Object *> *objectList_ptr;
    BvhNode *bvhTree;

    void build();

    Bvh(std::vector<Object *> *objectList_ptr, uint32_t leaf_size = 4);
    ~Bvh();

    bool getIntersection();
};

// struct BvhState
// {
//     uint32_t i;
//     float min_time;
//     BvhState() {}
//     BvhState(uint32_t _i, float _min_time) : i(_i), min_time(_min_time) {}
// };

Bvh::~Bvh()
{
    delete bvhTree;
}

Bvh::Bvh(std::vector<Object *> *_objectList_ptr, uint32_t _leaf_size)
    : node_num(0), leaf_num(0), leaf_size(_leaf_size), objectList_ptr(_objectList_ptr), bvhTree(NULL)
{
    // start buil the Tree
    build();

    std::cout << "Hierachical Tree Built." << std::endl;
}

struct BvhEntry
{
    // index of the parent, if non-zero
    uint32_t parent;
    // The range of objects in objecList covered by node
    uint32_t start, end;
};

void Bvh::build()
{
    BvhEntry stack[128];
    uint32_t stack_ptr = 0;
    // just make sure the value is large enough
    const uint32_t untouched = 0xffffffff;
    const uint32_t touchedTwice = 0xfffffffd;

    stack[stack_ptr].start = 0;
    stack[stack_ptr].end = objectList_ptr->size();
    stack[stack_ptr].parent = 0xfffffffc;
    stack_ptr++;

    BvhNode node;
    std::vector<BvhNode> nodeList;
    nodeList.reserve(objectList_ptr->size() * 2);

    while (stack_ptr > 0)
    {
        BvhEntry &entry(stack[--stack_ptr]);
        uint32_t start = entry.start;
        uint32_t end = entry.end;
        uint32_t prim_num = end - start;

        node_num++;
        node.start = start;
        node.prim_num = prim_num;
        node.offset = untouched;

        // get the bounding box
        BBox object_box((*objectList_ptr)[start]->getBBox());
        BBox centroid_box((*objectList_ptr)[start]->getCentroid());
        for (uint32_t p = start + 1; p < end; p++)
        {
            object_box.expandToInclude((*objectList_ptr)[p]->getBBox());
            centroid_box.expandToInclude((*objectList_ptr)[p]->getCentroid());
        }

        // if number of prims less than leaf size, let it be leaf
        if (prim_num <= leaf_size)
        {
            node.offset = 0;
            leaf_num++;
        }

        // push node into nodeList
        nodeList.push_back(node);

        // dfs way to left
        // keep update the offset, inorder to know the index of right node in nodeList
        // if the condition of if statement is satisfied
        // the node currently processing is the rightnode
        // which means all nodes in the right subtree has been push into nodeList
        if (entry.parent != 0xfffffffc)
        {
            nodeList[entry.parent].offset--;

            if (nodeList[entry.parent].offset == touchedTwice)
            {
                nodeList[entry.parent].offset = node_num - 1 - entry.parent;
            }
        }

        // continue if it is leaf node
        if (node.offset == 0)
        {
            continue;
        }

        uint32_t dim = centroid_box.maxDimension();
        float split_p = 0.5f * (centroid_box.min_p[dim] + centroid_box.max_p[dim]);

        // sort according to  the centroid point of object
        uint32_t mid = start;
        for (uint32_t i = start; i < end; i++)
        {
            if ((*objectList_ptr)[i]->getCentroid()[dim] < split_p)
            {
                std::swap((*objectList_ptr)[i], (*objectList_ptr)[mid]);
                mid++;
            }
        }

        // bad split
        if (mid == start || mid == end)
        {
            mid = start + (end - start) / 2;
        }

        // right child
        stack[stack_ptr].start = mid;
        stack[stack_ptr].end = end;
        stack[stack_ptr].parent = node_num - 1;

        // left child
        stack[stack_ptr].start = start;
        stack[stack_ptr].end = mid;
        stack[stack_ptr].parent = node_num - 1;
    }

    // copy the result
    // as the lifetime of vector will end after build
    bvhTree = new BvhNode[node_num];
    for (uint32_t i = 0; i < node_num; i++)
    {
        bvhTree[i] = nodeList[i];
    }
}

#endif