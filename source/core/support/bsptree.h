//******************************************************************************
///
/// @file core/support/bsptree.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_CORE_BSPTREE_H
#define POVRAY_CORE_BSPTREE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include <vector>
#include <list>
#include <cstdio>

#include "core/bounding/boundingbox.h"

namespace pov
{

class BSPTree
{
    public:
        class Mailbox
        {
                friend class BSPTree;
            public:
                inline Mailbox(unsigned int range) : objects((range >> 5) + 1), count(0) { }
                inline void clear() { count = 0; memset(&objects[0], 0, objects.size() * sizeof(unsigned int)); } // using memset here as std::fill may not be fast with every standard libaray [trf]
                inline unsigned int size() const { return count; }

                inline bool insert(unsigned int i)
                {
                    if((objects[i >> 5] & (1 << (i & 31))) == 0)
                    {
                        objects[i >> 5] |= (1 << (i & 31));
                        count++;
                        return true;
                    }
                    return false;
                }
            private:
                /// bit marking object (by index) in mailbox
                vector<unsigned int> objects;
                /// number of objects in mailbox
                unsigned int count;

                /// unavailable
                Mailbox();
        };

        class Objects
        {
            public:
                Objects() { }
                virtual ~Objects() { }

                virtual unsigned int size() const = 0;
                virtual float GetMin(unsigned int axis, unsigned int) const = 0;
                virtual float GetMax(unsigned int axis, unsigned int) const = 0;
        };

        class Intersect
        {
            public:
                Intersect() { }
                virtual ~Intersect() { }

                virtual bool operator()(unsigned int index, double& maxdist) = 0;
                virtual bool operator()() const = 0;
        };

        class Inside
        {
            public:
                Inside() { }
                virtual ~Inside() { }

                virtual bool operator()(unsigned int index) = 0;
                virtual bool operator()() const = 0;
        };

        class Progress
        {
            public:
                Progress() { }
                virtual ~Progress() { }

                virtual void operator()(unsigned int nodes) const = 0;
        };

        BSPTree(unsigned int md = 0, float oic = 0.0f, float bac = 0.0f, float cac = 0.0f, float mc = 0.0f);
        virtual ~BSPTree();

        bool operator()(const BasicRay& ray, Intersect& isect, Mailbox& mailbox, double maxdist);
        bool operator()(const Vector3d& origin, Inside& inside, Mailbox& mailbox, bool earlyExit = false);

        void build(const Progress& progress, const Objects& objects,
                   unsigned int& nodes, unsigned int& splitNodes, unsigned int& objectNodes, unsigned int& emptyNodes,
                   unsigned int& maxObjects, float& averageObjects, unsigned int& maxDepth, float& averageDepth,
                   unsigned int& aborts, float& averageAborts, float& averageAbortObjects, const UCS2String& inputFile);

        void clear();
    private:
        struct Node
        {
            enum NodeType
            {
                Split = 0,
                Object = 1
            };

            // data if type is split node
            enum SplitData
            {
                AxisX = 0,
                AxisY = 1,
                AxisZ = 2,
                NoAxis = 3
            };

            // data if type is object node
            enum ObjectData
            {
                Empty = 0,
                SingleObject = 1,
                DoubleObject = 2,
                ObjectList = 3
            };

            // four bytes
            unsigned type : 1;
            unsigned data : 2;
            unsigned index : 29;

            // four bytes (required)
            union
            {
                float plane;
                unsigned int index2;
            };
        };

        struct Split
        {
            enum Side
            {
                Min = 0,
                Max = 1
            };

            // four bytes
            unsigned se : 1;
            unsigned index : 31;

            // four bytes (required)
            float plane;

            inline Split() : se(Min), index(0), plane(0.0f) { }
            inline Split(Side s, unsigned int i, float p) : se(s), index(i), plane(p) { }

            inline bool operator<(const Split& r) const { return (plane < r.plane); }

            struct CompareIndex
            {
                inline bool operator()(const Split& left, const Split& right) const { return (left.index < right.index); }
            };
        };

        struct TraceStack
        {
            unsigned int inode;
            float rentry;
            float rexit;
        };

        /// array of all nodes
        vector<Node> nodes;
        /// array of all object pointer lists
        vector<unsigned int> lists;
        /// splits, only used while building tree
        vector<Split> splits[3];
        /// lower left corner of bounding box
        Vector3d bmin;
        /// upper right corner of bounding box
        Vector3d bmax;
        /// user-defined max depth (<= MAX_BSP_TREE_LEVEL)
        const unsigned int maxDepth;
        /// user-defined object intersection cost
        const float objectIsectCost;
        /// user-defined base access cost
        const float baseAccessCost;
        /// user-defined child access cost
        const float childAccessCost;
        /// user-define miss chance
        const float missChance;
        /// last node progress counter
        unsigned int lastProgressNodeCounter;
        /// maximum objects in node
        unsigned int maxObjectsInNode;
        /// maximum tree depth
        unsigned int maxTreeDepth;
        /// maximum tree depth nodes
        unsigned int maxTreeDepthNodes;
        /// empty node counter
        unsigned int emptyNodeCounter;
        /// object node counter
        unsigned int objectNodeCounter;
        /// objects in tree counter
        POV_LONG objectsInTreeCounter;
        /// objects at maximum depth counter
        POV_LONG objectsAtMaxDepthCounter;
        /// tree depth counter
        POV_LONG treeDepthCounter;
        /// object index list (only used while building tree)
        vector<unsigned int> indices;

        void BuildRecursive(const Progress& progress, const Objects& objects, unsigned int inode, unsigned int indexbegin, unsigned int indexend, MinMaxBoundingBox& cell, unsigned int maxlevel);
        void SetObjectNode(unsigned int inode, unsigned int indexbegin, unsigned int indexend);

        void ReadRecursive(const Progress& progress, FILE *infile, unsigned int inode, unsigned int level, unsigned int maxIndex);
        char *GetLine(char *str, int len, FILE *infile);
        void ValidateBounds(FILE *infile, const Objects& objects);
};


class BSPIntersectFunctor : public BSPTree::Intersect
{
    public:
        BSPIntersectFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t);
        virtual bool operator()(unsigned int index, double& maxdist);
        virtual bool operator()() const;

    private:
        bool found;
        vector<ObjectPtr>& objects;
        Intersection& bestisect;
        const Ray& ray;
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;
        TraceThreadData *traceThreadData;
};

class BSPIntersectCondFunctor : public BSPTree::Intersect
{
    public:
        BSPIntersectCondFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t,
                                const RayObjectCondition& prec, const RayObjectCondition& postc);
        virtual bool operator()(unsigned int index, double& maxdist);
        virtual bool operator()() const;

    private:
        bool found;
        vector<ObjectPtr>& objects;
        Intersection& bestisect;
        const Ray& ray;
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;
        TraceThreadData *traceThreadData;
        const RayObjectCondition& precondition;
        const RayObjectCondition& postcondition;
};

class BSPInsideCondFunctor : public BSPTree::Inside
{
    public:
        BSPInsideCondFunctor(Vector3d o, vector<ObjectPtr>& objs, TraceThreadData *t,
                             const PointObjectCondition& prec, const PointObjectCondition& postc);
        virtual bool operator()(unsigned int index);
        virtual bool operator()() const;

    private:
        bool found;
        vector<ObjectPtr>& objects;
        Vector3d origin;
        const PointObjectCondition& precondition;
        const PointObjectCondition& postcondition;
        TraceThreadData *threadData;
};


}

#endif // POVRAY_CORE_BSPTREE_H
