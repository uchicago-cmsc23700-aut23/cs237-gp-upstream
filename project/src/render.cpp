/*! \file render.cpp
 *
 * This file contains the rendering methods for the Window, Cell, Tile, and Chunk classes.
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "cs237.hpp"
#include "texture-cache.hpp"
#include "map.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "vao.hpp"

/***** Window methods *****/

void Window::render (float dt)
{
    if (! this->_isVis)
        return;

    // count the number of frames rendered
    this->_nFrames++;

    // next buffer from the swap chain
    auto imageIndex = this->_syncObjs.acquireNextImage ();
    if (imageIndex.result != vk::Result::eSuccess) {
        ERROR("inable to acquire next image");
    }
    int idx = imageIndex.value;

    this->_syncObjs.reset();

    /** HINT: draw the objects in the scene using the current rendering mode.
     ** For the terrain mesh, you will need to iterate over the cells in
     ** the map and for each cell you will need to walk the quad tree and
     ** render the tiles that comprise the frontier of the mesh refinement.
     */

    // set up submission for the graphics queue
    this->_syncObjs.submitCommands (this->graphicsQ(), this->_cmdBuf);

    // set up submission for the presentation queue
    this->_syncObjs.present (this->presentationQ(), idx);

    // record the time of the frame
    this->_lastFrameTime = glfwGetTime();
}

/***** Cell methods *****/

/** HINT: any Cell rendering methods should go here */

/***** Tile methods *****/

/** HINT: any Tile rendering methods should go here */
