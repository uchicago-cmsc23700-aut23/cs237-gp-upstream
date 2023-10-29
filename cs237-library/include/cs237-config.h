/*! \file cs237-config.h
 *
 * Generated configuration file
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _CS237_CONFIG_H_
#define _CS237_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif // C++

//! the path to the root of the build directory
#define CS237_BINARY_DIR "/Users/jhr/Classes/23700/f2023/cs237-gp-upstream/build"

//! the path to the root of the source directory
#define CS237_SOURCE_DIR "/Users/jhr/Classes/23700/f2023/cs237-gp-upstream"

//! if <strings.h> is available and needed for strncasecmp
#define INCLUDE_STRINGS_H <strings.h>

//! is strncasecmp available?
#define HAVE_STRNCASECMP

//! flag for windows build
/* #undef CS237_WINDOWS */

#ifdef __cplusplus
}
#endif // C++

#endif // !_CS237_CONFIG_H_
