/*! \file frustum.hpp
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

#ifndef _FRUSTUM_HPP_
#define _FRUSTUM_HPP_

#include "cs237.hpp"
#include "outcode.hpp"

/// A representation of a view frustum.  The planes of the frustum must be computed
/// from the camera.
//
struct Frustum {
    cs237::Planed_t _sides[6];  //!< the six frustum planes in world space

    //! The six frustum planes
    enum {
        LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
    };

    //! test an AABB against this frustum
    //! \param bb      the bounding box to test
    //! \param parent  the outcode for the parent of the box in a BVH
    Outcode intersectBox (cs237::AABBd_t const &bb, Outcode const &parent) const;

    Outcode intersectBox (cs237::AABBf_t const &bb, Outcode const &parent) const
    {
        return this->intersectBox (
            cs237::AABBd_t(glm::dvec3(bb.min()), glm::dvec3(bb.max())),
            parent);
    }

    //! test an AABB against this frustum
    Outcode intersectBox (cs237::AABBd_t const &bb) const
    {
        return this->intersectBox (bb, Outcode());
    }

};

#endif // !_FRUSTUM_HPP_
