/*! \file frustum.cpp
 *
 * Support for view-frustum culling.
 *
 * \author John Reppy
 */

/* CMSC23700 Final Project sample code (Autumn 2023)
 *
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hpp"
#include "frustum.hpp"

Outcode Frustum::intersectBox (cs237::AABBd_t const &bb, Outcode const &parent) const
{
    assert ((! parent.culled()) && (! parent.allIn()));

    Outcode code = parent;

    glm::dvec3 corner[8] = {
	    bb.corner(0), bb.corner(1), bb.corner(2), bb.corner(3),
	    bb.corner(4), bb.corner(5), bb.corner(6), bb.corner(7)
	};
    for (int i = 0;  i < 6;  i++) {
	if (code.notCulledBy(i)) {
	    // the parent is wholly contained by this plane, so the children must be too!
	    continue;
	}
	int nCornersIn = 0;
	for (int j = 0;  j < 8;  j++) {
	    double d = this->_sides[i].distanceToPt(corner[j]);
	    if (d >= 0) {
	        // at least part of the box is inside this plane
		nCornersIn++;
	    }
	}
	if (nCornersIn == 0) {
	    // this plane culls the box
	    return Outcode(true, 0);
	}
	else if (nCornersIn == 8) {
	    // the whole box is contained in this plane
	    code.clearPlane (i);
	    if (code.allIn()) {
	        // the box is inside all of the planes, so we're done
		return code;
	    }
	}
	// else this plane is ambiguous so leave it active.
    }

    return code;

}
