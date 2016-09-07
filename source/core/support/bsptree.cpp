//******************************************************************************
///
/// @file core/support/bsptree.cpp
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/bsptree.h"

#include <vector>
#include <list>

#include "base/pov_err.h"

#include "core/render/ray.h"
#include "core/shape/box.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

#ifndef BSP_READNODES
    #define BSP_READNODES     0
#endif

#ifndef BSP_WRITEBOUNDS
    #define BSP_WRITEBOUNDS   0
#endif

#ifndef BSP_WRITETREE
    #define BSP_WRITETREE     0
#endif

#define MAX_BSP_TREE_LEVEL  128
#define OBJECT_ISECT_COST   150.0f
#define BASE_ACCESS_COST    1.0f
#define CHILD_ACCESS_COST   5.0f
#define MISS_CHANCE         0.2f
#define BSP_TOLERANCE       0.00001f

const unsigned int NODE_PROGRESS_INTERVAL = 1000;

// we allow the values to be set by users to promote experimentation with tree
// building. at a later date we may remove this facility since using compile-time
// constants is more efficient.
//
// we use 0.0f as defaults rather than making the defaults equal to the above
// #defines to avoid having to expose the above values to any code that wants
// to construct a BSPTree (e.g. if it only wanted to set mc, for example, it
// would have to know the other values).
//
// missChance is only ever used with 1.0f added to it, so we do that now as well
//
// NB all these values are declared const in the class definition
BSPTree::BSPTree(unsigned int md, float oic, float bac, float cac, float mc) :
    maxDepth((md == 0) || (md > MAX_BSP_TREE_LEVEL) ? MAX_BSP_TREE_LEVEL : md),
    objectIsectCost(oic == 0.0f ? OBJECT_ISECT_COST : oic),
    baseAccessCost(bac == 0.0f ? BASE_ACCESS_COST : bac),
    childAccessCost(cac == 0.0f ? CHILD_ACCESS_COST : cac),
    missChance(mc == 0.0f ? MISS_CHANCE + 1.0f : mc + 1.0f)
{
}

BSPTree::~BSPTree()
{
}

static FILE *gFile = NULL;

bool BSPTree::operator()(const BasicRay& ray, Intersect& isect, Mailbox& mailbox, double maxdist)
{
    TraceStack tstack[MAX_BSP_TREE_LEVEL];
    Vector3d rayorigin(ray.GetOrigin());
    Vector3d raydir(ray.GetDirection());
    Vector3d invraydir(Vector3d(1.0) / raydir);
    double rentry, rexit;
    int ignore1, ignore2;
    unsigned int tstackpos = 0;

    if(Box::Intersect(ray, NULL, bmin, bmax, &rentry, &rexit, &ignore1, &ignore2) == false)
        return false; // no objects hit

    unsigned int inode = 0;

    while(rentry < maxdist)
    {
        // descend into child
        if(nodes[inode].type == Node::Split)
        {
            unsigned int axis = nodes[inode].data;
            unsigned int ileft = nodes[inode].index;
            unsigned int iright = ileft + 1;
            float plane = nodes[inode].plane;
            float rdist = (plane - rayorigin[axis]) * invraydir[axis];

            // decide which child to descend into
            if((rayorigin[axis] > plane) || ((rdist == 0.0f) && (raydir[axis] < 0)))
                std::swap(ileft, iright);

            // determine which child is next
            if((rdist < 0.0f) || (rdist > rexit))
                inode = ileft;
            else if(rdist < rentry)
                inode = iright;
            // if both children are intersected, remember one and continue with the other
            else
            {
                // remember right child
                tstack[tstackpos].inode = iright;
                tstack[tstackpos].rentry = rdist;
                tstack[tstackpos].rexit = rexit;
                tstackpos++;

                // continue with left child
                inode = ileft;
                rexit = rdist;
            }
        }
        else
        {
            // insert objects into mailbox
            switch(nodes[inode].data)
            {
                case Node::Empty:
                    // nothing to do
                    break;
                case Node::SingleObject:
                    if(mailbox.insert(nodes[inode].index) == true)
                        isect(nodes[inode].index, maxdist);
                    break;
                case Node::DoubleObject:
                    if(mailbox.insert(nodes[inode].index) == true)
                        isect(nodes[inode].index, maxdist);
                    if(mailbox.insert(nodes[inode].index2) == true)
                        isect(nodes[inode].index2, maxdist);
                    break;
                case Node::ObjectList:
                    for(unsigned int i = nodes[inode].index2, e = i + nodes[inode].index; i < e; i++)
                    {
                        if(mailbox.insert(lists[i]) == true)
                            isect(lists[i], maxdist);
                    }
                    break;
            }

            // see if there is another node to process
            if(tstackpos > 0)
            {
                tstackpos--;
                inode = tstack[tstackpos].inode;
                rentry = tstack[tstackpos].rentry;
                rexit = tstack[tstackpos].rexit;
            }
            // no nodes left, so terminate loop
            else
                break;
        }
    }

    return isect(); // see if any objects were hit
}

bool BSPTree::operator()(const Vector3d& origin, Inside& inside, Mailbox& mailbox, bool earlyExit)
{
    unsigned int tstackpos = 0;
    unsigned int tstack[MAX_BSP_TREE_LEVEL];

    // make sure the origin is within the bounded volume
    if ((origin[X] < bmin[X]) || (origin[Y] < bmin[Y]) || (origin[Z] < bmin[Z]) ||
        (origin[X] > bmax[X]) || (origin[Y] > bmax[Y]) || (origin[Z] > bmax[Z]))
        return false;

    tstack[tstackpos++] = 0;
    while(tstackpos > 0)
    {
        unsigned int inode = tstack[--tstackpos];
        if(nodes[inode].type == Node::Split)
        {
            if(origin[nodes[inode].data] <= nodes[inode].plane)
                tstack[tstackpos++] = nodes[inode].index;
            if(origin[nodes[inode].data] >= nodes[inode].plane)
                tstack[tstackpos++] = nodes[inode].index + 1;
        }
        else
        {
            // insert objects into mailbox
            switch(nodes[inode].data)
            {
                case Node::Empty:
                    // nothing to do
                    break;
                case Node::SingleObject:
                    if(mailbox.insert(nodes[inode].index) == true)
                        inside(nodes[inode].index);
                    break;
                case Node::DoubleObject:
                    if(mailbox.insert(nodes[inode].index) == true)
                        inside(nodes[inode].index);
                    if(mailbox.insert(nodes[inode].index2) == true)
                        inside(nodes[inode].index2);
                    break;
                case Node::ObjectList:
                    for(unsigned int i = nodes[inode].index2, e = i + nodes[inode].index; i < e; i++)
                        if(mailbox.insert(lists[i]) == true)
                            inside(lists[i]);
                    break;
            }
            if (earlyExit && inside())
                return true;
        }
    }

    return inside();
}

void BSPTree::build(const Progress& progress, const Objects& objects,
                    unsigned int& totalnodes, unsigned int& splitnodes, unsigned int& objectnodes, unsigned int& emptynodes,
                    unsigned int& maxobjects, float& averageobjects, unsigned int& maxdepth, float& averagedepth,
                    unsigned int& aborts, float& averageaborts, float& averageabortobjects, const UCS2String& inputFile)
{
    MinMaxBoundingBox bbox;

    lastProgressNodeCounter = 0;
    maxObjectsInNode = 0;
    maxTreeDepth = 0;
    maxTreeDepthNodes = 0;
    emptyNodeCounter = 0;
    objectNodeCounter = 0;
    objectsInTreeCounter = 0;
    objectsAtMaxDepthCounter = 0;
    treeDepthCounter = 0;

    progress(0);

    bbox.pmin[X] = BOUND_HUGE;
    bbox.pmin[Y] = BOUND_HUGE;
    bbox.pmin[Z] = BOUND_HUGE;

    bbox.pmax[X] = -BOUND_HUGE;
    bbox.pmax[Y] = -BOUND_HUGE;
    bbox.pmax[Z] = -BOUND_HUGE;

    // allocate memory that is going to be needed for building
    indices.reserve(objects.size() * 4); // can't tell what we need, but we'll start with object count * 4
    indices.resize(objects.size());
    splits[X].resize(objects.size() * 2);
    splits[Y].resize(objects.size() * 2);
    splits[Z].resize(objects.size() * 2);

#if BSP_WRITEBOUNDS || BSP_READNODES || BSP_WRITETREE
    string tempstr = UCS2toASCIIString(inputFile);
    if (tempstr.empty() == true)
        tempstr = "default";
    string::size_type pos = tempstr.find_last_of('.');
    if (pos != string::npos)
        tempstr.erase(pos);
#endif

#if BSP_WRITEBOUNDS
    FILE *bb = fopen(string(tempstr + ".bounds").c_str(), "w");
#else
    FILE *bb = NULL;
#endif

    if(bb != NULL)
        fprintf(bb, "%d\n", objects.size());

    // find bounding box containing all objects
    for(unsigned int i = 0; i < objects.size(); i++)
    {
        bbox.pmin[X] = min(bbox.pmin[X], objects.GetMin(X, i));
        bbox.pmin[Y] = min(bbox.pmin[Y], objects.GetMin(Y, i));
        bbox.pmin[Z] = min(bbox.pmin[Z], objects.GetMin(Z, i));

        bbox.pmax[X] = max(bbox.pmax[X], objects.GetMax(X, i));
        bbox.pmax[Y] = max(bbox.pmax[Y], objects.GetMax(Y, i));
        bbox.pmax[Z] = max(bbox.pmax[Z], objects.GetMax(Z, i));

        indices[i] = i;

        if(bb != NULL)
            fprintf(bb, "%f %f %f %f %f %f\n",
                    objects.GetMin(X, i), objects.GetMin(Y, i), objects.GetMin(Z, i),
                    objects.GetMax(X, i), objects.GetMax(Y, i), objects.GetMax(Z, i));
    }

    if(bb != NULL)
    {
        fprintf(bb, "%f %f %f %f %f %f\n",
                bbox.pmin[X], bbox.pmin[Y], bbox.pmin[Z], bbox.pmax[X], bbox.pmax[Y], bbox.pmax[Z]);
        fflush(bb);
        fclose(bb);
    }

    // remember bounding box for intersection testing
    bmin = Vector3d(bbox.pmin[X], bbox.pmin[Y], bbox.pmin[Z]);
    bmax = Vector3d(bbox.pmax[X], bbox.pmax[Y], bbox.pmax[Z]);

#if BSP_WRITETREE
    gFile = fopen(string(tempstr + ".tree").c_str(), "w");
#endif

    if(gFile != NULL)
    {
        fprintf(gFile, "> %f %f %f %f %f %f\n", bbox.pmin[X], bbox.pmin[Y], bbox.pmin[Z], bbox.pmax[X], bbox.pmax[Y], bbox.pmax[Z]);
        fprintf(gFile, "T %d\n", objects.size());
    }

    // recursively build BSP tree
    nodes.push_back(Node());

#if BSP_READNODES
    FILE *infile = fopen(string(tempstr + ".nodes").c_str(), "r");
    if(infile == NULL)
        throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot open BSP nodes file (BSP_READNODES == true, tree generation disabled)");
    try
    {
        ValidateBounds(infile, objects);
    }
    catch(pov_base::Exception& e)
    {
        if (gFile != NULL)
            fclose (gFile);
        if ((e.codevalid() != false) && (e.code() == kFileDataErr))
        {
            int line = 0;
            char str[1024];
            long pos = ftell(infile);
            fseek(infile, 0, SEEK_SET);
            while (fgets(str, sizeof(str) - 1, infile) != NULL)
            {
                line++;
                if (ftell(infile) >= pos)
                {
                    fclose (infile);
                    sprintf (str, "%s.nodes line %d: %s", tempstr.c_str(), line, e.what());
                    throw POV_EXCEPTION(e.code(), str);
                }
            }
        }
        fclose (infile);
        throw;
    }
    ReadRecursive(progress, infile, 0, 0, objects.size() - 1);
    fclose(infile);
#else
    BuildRecursive(progress, objects, 0, 0, (unsigned int) indices.size(), bbox, maxDepth);
#endif

    if(gFile != NULL)
    {
        fflush(gFile);
        fclose(gFile);
    }

    // memory was only needed for building
    splits[X].clear();
    splits[Y].clear();
    splits[Z].clear();

    progress((unsigned int) nodes.size());

    unsigned int nodesoftypeobject = emptyNodeCounter + objectNodeCounter; // number of terminal nodes

    totalnodes = (unsigned int) nodes.size();
    splitnodes = totalnodes - nodesoftypeobject;
    objectnodes = objectNodeCounter;
    emptynodes = emptyNodeCounter;
    maxobjects = maxObjectsInNode;
    averageobjects = float(double(objectsInTreeCounter) / double(nodesoftypeobject));
    maxdepth = maxTreeDepth;
    averagedepth = float(double(treeDepthCounter) / double(nodesoftypeobject));
    aborts = maxTreeDepthNodes;
    if(aborts > 0)
    {
        averageaborts = float(double(aborts) / double(nodesoftypeobject));
        averageabortobjects = float(double(objectsAtMaxDepthCounter) / double(aborts));
    }
    else
    {
        averageaborts = 0.0f;
        averageabortobjects = 0.0f;
    }

    // free up unused allocation in lists and nodes
    vector<unsigned int> tmplists;
    tmplists.swap(lists);
    lists = tmplists;

    vector<Node> tmpnodes;
    tmpnodes.swap(nodes);
    nodes = tmpnodes;

    indices.clear();
}

void BSPTree::clear()
{
    nodes.clear();
    lists.clear();
}

void BSPTree::BuildRecursive(const Progress& progress, const Objects& objects, unsigned int inode, unsigned int indexbegin, unsigned int indexend, MinMaxBoundingBox& cell, unsigned int maxlevel)
{
    maxTreeDepth = max(maxTreeDepth, maxDepth - maxlevel);

    if((nodes.size() - lastProgressNodeCounter) > NODE_PROGRESS_INTERVAL)
    {
        lastProgressNodeCounter = (unsigned int) nodes.size();
        progress(lastProgressNodeCounter);
    }

    if(gFile != NULL)
        fprintf(gFile, "%*s", (maxDepth - maxlevel) * 2, "");

    unsigned int cnt = indexend - indexbegin; // number of objects

    // stop if there are no more objects
    if(cnt == 0)
    {
        if(gFile != NULL)
            fprintf(gFile, "*\n");

        nodes[inode].type = Node::Object;
        nodes[inode].data = Node::Empty;
        nodes[inode].index = 0;

        emptyNodeCounter++;
        treeDepthCounter += (maxDepth - maxlevel);
        return;
    }

    // stop if maximum split recursion level reached or we only have one object
    if((maxlevel == 0) || (cnt == 1))
    {
        SetObjectNode(inode, indexbegin, indexend);

        if(maxlevel == 0)
        {
            maxTreeDepthNodes++;
            objectsAtMaxDepthCounter += cnt;
        }
        treeDepthCounter += (maxDepth - maxlevel);
        return;
    }

    unsigned int bestscnt = 0;
    unsigned int bestaxis = Node::NoAxis;
    unsigned int bestsplit = 0;

    // baseAccessCost is TK1 in Eric's article
    // objectIsectCost is TP in Eric's article
    // childAccessCost is TK3 in Eric's article

    // set bestcost to estimated time for processing unsplit node
    float bestcost = baseAccessCost + (cnt * objectIsectCost);

    // find best split axis and plane
    {
        float cellsize[5];

        cellsize[X] = cellsize[X + 3] = cell.pmax[X] - cell.pmin[X];
        cellsize[Y] = cellsize[Y + 3] = cell.pmax[Y] - cell.pmin[Y];
        cellsize[Z] =                   cell.pmax[Z] - cell.pmin[Z];

        // enh is node hit expectance
        float enh = cellsize[X] * cellsize[Y] + cellsize[X] * cellsize[Z] + cellsize[Y] * cellsize[Z];
        float enhinv = 1.0f / enh;

        // try every axis
        for(unsigned int axis = 0; axis < 3; axis++)
        {
            unsigned int pa = 0; // objects only in left side
            unsigned int pb = cnt; // objects only in right side
            unsigned int pab = 0; // objects in both
            float bmin = cell.pmin[axis];
            float bmax = cell.pmax[axis];

            // eph is plane hit expectance
            float eph = cellsize[axis + 1] * cellsize[axis + 2];

            // cph is plane hit relative chance (eph / enh)
            float cph = eph * enhinv;

            // relmul is used to calculate 'r' given the offset into the node
            float relmul = 1.0f / cellsize[axis];

            // chmul is used to calculate cah and cbh once we know r
            float chmul = cellsize[axis] * (cellsize[axis + 1] + cellsize[axis + 2]) * enhinv;

            // constcost is TK1 + TK2 + (1 + CPH) * TK3 in Eric's article
            float constcost = baseAccessCost + ((1.0f + cph) * childAccessCost);

            // since cph/2 is used in the main cost calculation we do the division here to avoid
            // doing it multiple times in the below loop.
            cph *= 0.5;

            unsigned int scnt = 0;
            for(unsigned int i = indexbegin; i < indexend; i++)
            {
                float smin = objects.GetMin(axis, indices[i]) - BSP_TOLERANCE;
                float smax = objects.GetMax(axis, indices[i]) + BSP_TOLERANCE;

                // (if they are equal for our purpose we consider it outside)
                if((smin >= bmax) || (smax <= bmin))
                    continue ;

                if(smin < bmin)
                {
                    // definitely intersects a, may not intersect b
                    // if it does intersect b then as it also intersects a, we need to add
                    // one to the common count (pab) and decrement the right-only count (pb).
                    pab++;
                    pb--;
                }
                splits[axis][scnt++] = Split(Split::Min, indices[i], smin);
                splits[axis][scnt++] = Split(Split::Max, indices[i], smax);
            }

            sort(splits[axis].begin(), splits[axis].begin() + scnt);

            for(unsigned int i = 0; i < scnt; i++)
            {
                float plane = splits[axis][i].plane;

                if(splits[axis][i].se == Split::Max) // leaving object
                {
                    pa++;
                    pab--;
                }

                if((plane > bmin) && (plane < bmax))
                {
                    float r = (plane - cell.pmin[axis]) * relmul; // range 0.0 (close boundary) to 1.0 (far boundary)
                    float cah = r * chmul;                        // chance of 'a' hit
                    float cbh = (1.0f - r) * chmul;               // chance of 'b' hit

                    // cost function as presented in Ray Tracing News Vol. 17 No. 1 by Eric Haines [trf]
                    // NB cph has been pre-divided by 2 and missChance has had 1.0 added to it.
                    float cost = constcost + (objectIsectCost * (pab + (cph  * ((missChance * pa) + (missChance * pb))) + (cah * pa) + (cbh * pb)));

                    if(cost < bestcost)
                    {
                        bestcost = cost;
                        bestsplit = i;
                        bestaxis = axis;
                        bestscnt = scnt;
                    }
                }

                if(splits[axis][i].se == Split::Min) // entering object
                {
                    pab++;
                    pb--;
                }
            }
        }
    }

    if(bestaxis == Node::NoAxis) // no better split found, so stop at this node
    {
        SetObjectNode(inode, indexbegin, indexend);

        treeDepthCounter += (maxDepth - maxlevel);
    }
    else // better split found, so create child nodes
    {
        unsigned int ichild = (unsigned int) nodes.size(); // child node position
        float bestplane = splits[bestaxis][bestsplit].plane;
        float ptemp = 0.0f;

        // create child nodes
        nodes.push_back(Node());
        nodes.push_back(Node());

        // set current node
        nodes[inode].type = Node::Split;
        nodes[inode].data = bestaxis;
        nodes[inode].index = ichild;
        nodes[inode].plane = bestplane;

        // if the best split is at the maximum side, the split goes
        // into the left child, otherwise it goes into the right side
        // and thus the mid-point for sorting has to be moved [trf]
        if(splits[bestaxis][bestsplit].se == Split::Max)
            bestsplit++;

        // reorder indices to find objects completely in one child
        Split::CompareIndex ci;
        sort(splits[bestaxis].begin(), splits[bestaxis].begin() + bestsplit, ci);
        sort(splits[bestaxis].begin() + bestsplit, splits[bestaxis].begin() + bestscnt, ci);

        if(gFile != NULL)
        {
            fprintf(gFile, "| %c = %g ", (int)('x' + bestaxis), bestplane);
            fprintf(gFile, "[%g,%g,%g -> %g,%g,%g]\n",
                    cell.pmin[0], cell.pmin[1], cell.pmin[2],
                    cell.pmax[0], cell.pmax[1], cell.pmax[2]);
        }

        unsigned int begin = (unsigned int) indices.size();
        for (vector<Split>::iterator it = splits[bestaxis].begin(), en = it + bestsplit; it != en; )
        {
            unsigned int index = it++->index;
            indices.push_back(index);
            if((it != en) && (it->index == index)) // keep only once if completely in child
                it++;
        }
        unsigned int middle = (unsigned int) indices.size();
        for (vector<Split>::iterator it = splits[bestaxis].begin() + bestsplit, en = splits[bestaxis].begin() + bestscnt; it != en; )
        {
            unsigned int index = it++->index;
            indices.push_back(index);
            if((it != en) && (it->index == index)) // keep only once if completely in child
                it++;
        }
        unsigned int end = (unsigned int) indices.size();

        // split left cell
        ptemp = cell.pmax[bestaxis];
        cell.pmax[bestaxis] = bestplane;
        BuildRecursive(progress, objects, ichild, begin, middle, cell, maxlevel - 1);
        cell.pmax[bestaxis] = ptemp;

        // split right cell
        ptemp = cell.pmin[bestaxis];
        cell.pmin[bestaxis] = bestplane;
        BuildRecursive(progress, objects, ichild + 1, middle, end, cell, maxlevel - 1);
        cell.pmin[bestaxis] = ptemp;

        // the efficiency of this code depends on the assumption that resize() does not
        // de-allocate memory when truncating a vector.
        indices.resize(begin);
    }
}

void BSPTree::SetObjectNode(unsigned int inode, unsigned int indexbegin, unsigned int indexend)
{
    unsigned int count = indexend - indexbegin;

    if(gFile != NULL)
    {
        fprintf(gFile, "# (%d) ", count);
        for(unsigned int i = indexbegin; i < indexend; i++)
            fprintf(gFile, " %d", indices[i]);
        fprintf(gFile, "\n");
    }

    objectNodeCounter++;

    // single object
    if(count == 1)
    {
        nodes[inode].type = Node::Object;
        nodes[inode].data = Node::SingleObject;
        nodes[inode].index = indices[indexbegin];

        maxObjectsInNode = max(maxObjectsInNode, (unsigned int)1);
        objectsInTreeCounter += 1;
    }
    // double object
    else if(count == 2)
    {
        nodes[inode].type = Node::Object;
        nodes[inode].data = Node::DoubleObject;
        nodes[inode].index = indices[indexbegin];
        nodes[inode].index2 = indices[indexbegin + 1];

        maxObjectsInNode = max(maxObjectsInNode, (unsigned int)2);
        objectsInTreeCounter += 2;
    }
    // object list
    else
    {
        unsigned int s((unsigned int) lists.size());

        nodes[inode].type = Node::Object;
        nodes[inode].data = Node::ObjectList;
        nodes[inode].index = count; // length of list
        nodes[inode].index2 = s; // list offset

        // Note: It is actually *much* faster with the Microsoft STL for large trees to not call lists.reserve() here! [cjc] Need to check this for other STL implementations... [trf]
        // lists.reserve(s + c);

        // Note: This could first search for an already existing sequence of the same objects
        // and then adjust the value of index2 accordingly, which would reduce memory consumption [trf]
        lists.insert(lists.end(), indices.begin() + indexbegin, indices.begin() + indexend);

        maxObjectsInNode = max(maxObjectsInNode, count);
        objectsInTreeCounter += count;
    }
}

char *BSPTree::GetLine(char *str, int len, FILE *infile)
{
    char *s;

    while((s = fgets(str, len, infile)) != NULL)
    {
        while(isspace(*s))
            s++;
        if(*s == '\0')
            continue;
        if(*s != '/')
            break;
        if(*++s != '/')
            throw POV_EXCEPTION(kFileDataErr, "Invalid character in node file");
    }
    if(s == NULL)
        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in node file");
    return s;
}

void BSPTree::ValidateBounds(FILE *infile, const Objects& objects)
{
    int count;
    char str[1024];

    if(sscanf(GetLine(str, sizeof(str), infile), "%d\n", &count) != 1)
        throw POV_EXCEPTION(kFileDataErr, "Expected count of objects at start of node file");
    if(count != objects.size())
        throw POV_EXCEPTION(kFileDataErr, "Object count in node file does not match parsed file");
    for(unsigned int i = 0; i < objects.size(); i++)
    {
        // since runtime libraries use double internally when dealing with float
        // types, we scan in and compare as double rather than float to avoid
        // additional float->double->float conversions, which make comparison
        // less precise.
        double llx1, lly1, llz1, urx1, ury1, urz1;
        if (sscanf(GetLine(str, sizeof(str), infile), "%lf %lf %lf %lf %lf %lf\n", &llx1, &lly1, &llz1, &urx1, &ury1, &urz1) != 6)
            throw POV_EXCEPTION(kFileDataErr, "Failed to parse bounds line in node file");

        double llx2, lly2, llz2, urx2, ury2, urz2;
        llx2 = objects.GetMin(X, i);
        lly2 = objects.GetMin(Y, i);
        llz2 = objects.GetMin(Z, i);
        urx2 = objects.GetMax(X, i);
        ury2 = objects.GetMax(Y, i);
        urz2 = objects.GetMax(Z, i);

        if ((fabs (llx1 - llx2) > 0.00001) ||
            (fabs (lly1 - lly2) > 0.00001) ||
            (fabs (llz1 - llz2) > 0.00001) ||
            (fabs (urx1 - urx2) > 0.00001) ||
            (fabs (ury1 - ury2) > 0.00001) ||
            (fabs (urz1 - urz2) > 0.00001))
            throw POV_EXCEPTION(kFileDataErr, "Node file bounds do not match that of parsed file");
    }
    GetLine(str, sizeof(str), infile);
    if (*GetLine(str, sizeof(str), infile) != '-')
        throw POV_EXCEPTION(kFileDataErr, "Invalid separator line in node file");
}

void BSPTree::ReadRecursive(const Progress& progress, FILE *infile, unsigned int inode, unsigned int level, unsigned int maxIndex)
{
    if (level == MAX_BSP_TREE_LEVEL)
        throw POV_EXCEPTION(kFileDataErr, "Depth in node file exceeded MAX_BSP_TREE_LEVEL");

    if((nodes.size() - lastProgressNodeCounter) > NODE_PROGRESS_INTERVAL)
    {
        lastProgressNodeCounter = (unsigned int) nodes.size();
        progress(lastProgressNodeCounter);
    }

    maxTreeDepth = max(maxTreeDepth, level);
    int c = fgetc(infile); // read node type

    if(c == '|') // split node
    {
        unsigned int ichild = (unsigned int) nodes.size(); // child node position
        unsigned int bestaxis = 0;
        float bestplane = 0.0f;

        if(fscanf(infile, " %u %f\n", &bestaxis, &bestplane) != 2) // read axis and plane
            throw POV_EXCEPTION(kFileDataErr, "Expected axis and plane whilst reading node file");

        if(gFile != NULL)
            fprintf(gFile, "%*s| %c = %g\n", level * 2, "", 'x' + bestaxis, bestplane);

        // create child nodes
        nodes.push_back(Node());
        nodes.push_back(Node());

        // set current node
        nodes[inode].type = Node::Split;
        nodes[inode].data = bestaxis;
        nodes[inode].index = ichild;
        nodes[inode].plane = bestplane;

        // left cell
        ReadRecursive(progress, infile, ichild, level + 1, maxIndex);

        // right cell
        ReadRecursive(progress, infile, ichild + 1, level + 1, maxIndex);
    }
    else if(c == '#') // object node
    {
        unsigned int cnt = 0;

        if(fscanf(infile, " %u", &cnt) != 1) // read number of objects
            throw POV_EXCEPTION(kFileDataErr, "Expected number of objects whilst reading node file");

        vector<unsigned int> ind(cnt);

        treeDepthCounter += level;

        if (cnt == 0)
        {
            if(gFile != NULL)
                fprintf(gFile, "%*s*\n", level * 2, "");
            nodes[inode].type = Node::Object;
            nodes[inode].data = Node::Empty;
            nodes[inode].index = 0;
            emptyNodeCounter++;
            fscanf(infile, "\n");
            return;
        }

        if(level == MAX_BSP_TREE_LEVEL - 1)
        {
            maxTreeDepthNodes++;
            objectsAtMaxDepthCounter += cnt;
        }

        for(unsigned int i = 0; i < cnt; i++)
        {
            if(fscanf(infile, " %u", &ind[i]) != 1) // read object index
                throw POV_EXCEPTION(kFileDataErr, "Expected object index whilst reading node file");
            if(ind[i] > maxIndex)
                throw POV_EXCEPTION(kFileDataErr, "Invalid object index in node file");
        }

        fscanf(infile, "\n");

        if(gFile != NULL)
            fprintf(gFile, "%*s", level * 2, "");

        SetObjectNode(inode, 0, (unsigned int) ind.size());
    }
    else
    {
        throw POV_EXCEPTION(kFileDataErr, "Unexpected character in node file");
    }
}


BSPIntersectFunctor::BSPIntersectFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t) :
    found(false),
    bestisect(bi),
    ray(r),
    objects(objs),
    traceThreadData(t)
{
    Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
    origin = BBoxVector3d(ray.Origin);
    invdir = BBoxVector3d(tmp);
    variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));
}

bool BSPIntersectFunctor::operator()(unsigned int index, double& maxdist)
{
    ObjectPtr object = objects[index];
    Intersection isect;

    if(Find_Intersection(&isect, object, ray, variant, origin, invdir, traceThreadData) && (isect.Depth <= maxdist))
    {
        if(isect.Depth < bestisect.Depth)
        {
            bestisect = isect;
            found = true;
            maxdist = bestisect.Depth;
        }
    }

    return found;
}

bool BSPIntersectFunctor::operator()() const
{
    return found;
}


BSPIntersectCondFunctor::BSPIntersectCondFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t,
                                                 const RayObjectCondition& prec, const RayObjectCondition& postc) :
    found(false),
    bestisect(bi),
    ray(r),
    objects(objs),
    traceThreadData(t),
    precondition(prec),
    postcondition(postc)
{
    Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
    origin = BBoxVector3d(ray.Origin);
    invdir = BBoxVector3d(tmp);
    variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));
}

bool BSPIntersectCondFunctor::operator()(unsigned int index, double& maxdist)
{
    ObjectPtr object = objects[index];

    if(precondition(ray, object, 0.0) == true)
    {
        Intersection isect;

        if(Find_Intersection(&isect, object, ray, variant, origin, invdir, postcondition, traceThreadData) && (isect.Depth <= maxdist))
        {
            if(isect.Depth < bestisect.Depth)
            {
                bestisect = isect;
                found = true;
                maxdist = bestisect.Depth;
            }
        }
    }

    return found;
}

bool BSPIntersectCondFunctor::operator()() const
{
    return found;
}


BSPInsideCondFunctor::BSPInsideCondFunctor(Vector3d o, vector<ObjectPtr>& objs, TraceThreadData *t,
                                           const PointObjectCondition& prec, const PointObjectCondition& postc) :
    found(false),
    origin(o),
    objects(objs),
    precondition(prec),
    postcondition(postc),
    threadData(t)
{
}

bool BSPInsideCondFunctor::operator()(unsigned int index)
{
    ObjectPtr object = objects[index];
    if(precondition(origin, object))
        if(Inside_BBox(origin, object->BBox) && object->Inside(origin, threadData))
            if(postcondition(origin, object))
                found = true;
    return found;
}

bool BSPInsideCondFunctor::operator()() const
{
    return found;
}

}
