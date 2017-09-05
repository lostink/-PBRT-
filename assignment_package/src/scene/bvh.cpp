#include "bvh.h"
#include <QTime>
#include <iostream>
// Feel free to ignore these structs entirely!
// They are here if you want to implement any of PBRT's
// methods for BVH construction.

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
};

struct BVHBuildNode {
    // BVHBuildNode Public Methods
    void InitLeaf(int first, int n, const Bounds3f &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }
    Bounds3f bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct MortonPrimitive {
    int primitiveIndex;
    unsigned int mortonCode;
};

struct LBVHTreelet {
    int startIndex, nPrimitives;
    BVHBuildNode *buildNodes;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};


BVHAccel::~BVHAccel()
{
    delete [] nodes;
}

// Constructs an array of BVHPrimitiveInfos, recursively builds a node-based BVH
// from the information, then optimizes the memory of the BVH
BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive> > &p, int maxPrimsInNode)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
    //TODO
    QTime Timer;
    if(primitives.size() == 0)
        return;

    //Build BVH from primitives:
    //1. Initialize primitiveInfo array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for(size_t i = 0; i < primitives.size(); ++i)
        primitiveInfo[i] = {i, primitives[i]->shape->WorldBound()};
    //2. Build BVH tree for primitives using primitiveInfo
    int totalNodes = 0;
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    BVHBuildNode *root;
    Timer.start();
    root = recursiveBuild(primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrims);
    primitives.swap(orderedPrims);

    //3. Compute representation of depth-first traversal of BVH tree
    nodes = new LinearBVHNode[totalNodes];
    int offset = 0;
    flattenBVHTree(root, &offset);
    std::cout << "Total time to build the BVH tree: " << Timer.elapsed()  << "ms" << std::endl;
}

bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    //TODO
    bool hit = false;
    Vector3f invDir(1/ray.direction.x, 1/ray.direction.y, 1/ray.direction.z);
    int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

    //Follow ray through BVH nodes to find primitive intersections
    int toVisitOffset = 0, currentNodeIndex = 0;
    int nodesToVisit[64];
    Intersection TempResult;
    isect->t = -1.f;
    while(true)
    {
        const LinearBVHNode *node = &nodes[currentNodeIndex];

        //Check ray against BVH node
        if(node->bounds.IntersectP(ray, invDir, dirIsNeg))
        {
            if(node->nPrimitives > 0)
            {
                //Intersect ray with primitives in leaf BVH node
                for(int i = 0; i < node->nPrimitives; ++i)
                    if(primitives[node->primitivesOffset + i]->Intersect(ray, &TempResult))
                    {
                        hit = true;
                        if ((isect->t == -1.f)||(TempResult.t < isect->t))
                        {
                            //Choose the nearest intersection
                            (*isect) = TempResult;
                            isect->wo = glm::normalize(TempResult.point - ray.origin);
                        }
                    }
                if(toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else
            {
                //Put far BVH node on nodesToVisist stack, advance to near node
                if(dirIsNeg[node->axis])
                {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->secondChildOffset;
                }
                else
                {
                    nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        }
        else
        {
            if(toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }

    return hit;
}

BVHBuildNode *BVHAccel::recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo, int start,
                                       int end, int *totalNodes, std::vector<std::shared_ptr<Primitive>> &orderedPrims)
{
    BVHBuildNode *node = new BVHBuildNode;
    (*totalNodes)++;
    //Compute bounds of all primitives in BVH node
    Bounds3f bounds = primitiveInfo[start].bounds;
    for(int i = start; i < end; ++i)
        bounds = Union(bounds, primitiveInfo[i].bounds);

    int nPrimitives = end - start;
    if(nPrimitives == 1)
    {
        //Create leaf BVHBuildNode
        int firstPrimOffset = orderedPrims.size();
        for(int i = start; i < end; ++i)
        {
            int primNum = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[primNum]);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
        return node;
    }
    else
    {
        //Compute bound of primitive centroids, choose split dimension dim
        Bounds3f centroidBounds = Bounds3f(primitiveInfo[start].centroid);
        for(int i = start; i < end; ++i)
            centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
        int dim = centroidBounds.MaximumExtent();

        //Partition primitives into two sets and build children
        int mid = (start + end) / 2;
        if(centroidBounds.max[dim] == centroidBounds.min[dim])
        {
            //Create leaf BVHBuildNode
            int firstPrimOffset = orderedPrims.size();
            for(int i = start; i < end; ++i)
            {
                int primNum = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
            return node;

        }
        else
        {
            //Partition primitives based on splitMethod
            //In this homework, we use SAH
            if(nPrimitives <= 4)
            {
                //Partition primitives into equally sized subsets
                mid = (start + end) / 2;
                std::nth_element(&primitiveInfo[start],
                                 &primitiveInfo[mid],
                                 &primitiveInfo[end-1]+1,
                        [dim](const BVHPrimitiveInfo &a,const BVHPrimitiveInfo &b){
                    return a.centroid[dim] < b.centroid[dim];
                });

            }
            else
            {
                //1. Allocate BucketInfo for SAH partition buckets
                constexpr int nBuckets = 12;
                struct BucketInfo
                {
                    int count = 0;
                    Bounds3f bounds;
                };
                BucketInfo buckets[nBuckets];

                //2. Initialize BucketInfo for SAH partition buckets
                for(int i = start; i < end; ++i)
                {
                    int b = nBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[dim];
                    if(b == nBuckets)
                        b = nBuckets - 1;
                    buckets[b].count++;
                    buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
                }

                //3. Compute costs for splitting after each bucket
                float cost[nBuckets - 1];
                for(int i = 0; i < nBuckets - 1; ++i)
                {
                    Bounds3f b0, b1;
                    int count0 = 0, count1 = 0;
                    for(int j = 0; j <= i; ++j)
                    {
                        b0 = Union(b0, buckets[j].bounds);
                        count0 += buckets[j].count;
                    }
                    for(int j = i+1; j < nBuckets; ++j)
                    {
                        b1 = Union(b1, buckets[j].bounds);
                        count1 += buckets[j].count;
                    }
                    cost[i] = 1.f +
                            (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) /
                            bounds.SurfaceArea();
                }

                //4. Find bucket to split at that minimizes SAH metric
                float minCost = cost[0];
                int minCostSplitBucket = 0;
                for(int i = 1; i < nBuckets - 1; ++i)
                {
                    if(cost[i] < minCost)
                    {
                        minCost = cost[i];
                        minCostSplitBucket = i;
                    }
                }

                //5. Either create leaf or split primitives at selected SAH bucket
                float leafCost = nPrimitives;
                if(nPrimitives > maxPrimsInNode || minCost < leafCost)
                {
                    BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start], &primitiveInfo[end-1]+1,
                            [=](const BVHPrimitiveInfo &pi)
                    {
                        int b = nBuckets * centroidBounds.Offset(pi.centroid)[dim];
                        if(b == nBuckets)
                            b = nBuckets - 1;
                        return b <= minCostSplitBucket;
                    });
                    mid = pmid - &primitiveInfo[0];
                }
                else
                {
                    //Create leaf BVHBuildNode
                    int firstPrimOffset = orderedPrims.size();
                    for(int i = start; i < end; ++i)
                    {
                        int primNum = primitiveInfo[i].primitiveNumber;
                        orderedPrims.push_back(primitives[primNum]);
                    }
                    node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                    return node;
                }

            }



            node->InitInterior(dim,
                               recursiveBuild(primitiveInfo, start, mid, totalNodes, orderedPrims),
                               recursiveBuild(primitiveInfo, mid, end, totalNodes, orderedPrims));
        }

    }
    return node;
}

int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *offset)
{
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;
    if(node->nPrimitives > 0)
    {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }
    else
    {
        //Create interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flattenBVHTree(node->children[0], offset);
        linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
        linearNode->bounds = Union(node->children[0]->bounds,node->children[1]->bounds);
    }
    return myOffset;
}
