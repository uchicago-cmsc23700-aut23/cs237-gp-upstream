/*! \file app.cpp
 *
 * CS23700 Autumn 2023 Sample Code for Group Project
 *
 * The main application class for Project 2.
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "app.hpp"
#include "window.hpp"
#include <cstdlib>
#include <unistd.h>

constexpr uint32_t kWindowWidth = 1280;
constexpr uint32_t kWindowHeight = 1024;

static void usage (int sts)
{
    std::cerr << "usage: part1 [options] <scene>\n";
    exit (sts);
}

Project::Project (std::vector<std::string> const &args)
  : cs237::Application (args, "CS237 Group Project"), _map(this)
{
    // the last argument is the name of the map that we should render
    if (args.size() < 2) {
        usage(EXIT_FAILURE);
    }
    std::string mapName = args.back();

    // verify that the scene path exists
    if (access(mapName.c_str(), F_OK) < 0) {
        std::cerr << "map '" << mapName
            << "' is not accessible or does not exist\n";
        exit(EXIT_FAILURE);
    }

    // load the scene
    std::clog << "loading " << mapName << std::endl;
    if (! this->_map.load(mapName)) {
        std::cerr << "cannot load map from '" << mapName << "'\n";
        exit(EXIT_FAILURE);
    }

}

Project::~Project () { }

void Project::run ()
{
    std::clog << "running ...\n";

    // create the application window
    cs237::CreateWindowInfo cwInfo(
        kWindowWidth, kWindowHeight,
        this->_map.name(),
        false, true, false);
    Window *win = new Window (this, cwInfo, &this->_map);

    try {
        this->_runLoop (win);
    } catch (const std::exception& e) {
        // cleanup and then re-throw the exception
        this->_device.waitIdle();
        this->_cleanup (win);
        throw e;
    }

    // wait until any in-flight rendering is complete
    this->_device.waitIdle();

    // cleanup
    this->_cleanup (win);

}


//! the actual interactive loop
void Project::_runLoop (Window *win)
{
    // we keep track of the time between frames for morphing and for
    // any time-based animation
    double lastFrameTime = glfwGetTime();

    // wait until the window is closed
    while(! win->windowShouldClose()) {
        // how long since the last frame?
        double now = glfwGetTime();
        float dt = float(now - lastFrameTime);
        lastFrameTime = now;

        /** HINT: Update camera if necessary */

        win->render (dt);

        // update animation state as necessary
        win->animate (now);

        glfwPollEvents();
    }
}

//! cleanup code
void Project::_cleanup (Window *win)
{
    /** HINT: any additional cleanup */
    delete win;
}
