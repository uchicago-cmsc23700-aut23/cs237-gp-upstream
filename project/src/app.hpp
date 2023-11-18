/*! \file app.hpp
 *
 * CS23700 Autumn 2023 Sample Code for Group Project
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _APP_HPP_
#define _APP_HPP_

#include "cs237.hpp"
#include "map.hpp"

//! The main Application class
class Project : public cs237::Application {
public:
    Project (std::vector<std::string> const &args);
    ~Project ();

    //! run the application
    void run () override;

protected:
    Map _map;           //!< holds the map to be rendered

    //! the actual interactive loop
    void _runLoop (class Window *win);

    //! cleanup code
    void _cleanup (class Window *win);

};

#endif // !_APP_HPP_

