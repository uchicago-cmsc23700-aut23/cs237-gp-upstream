/*! \file vao.cpp
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "vao.hpp"

VAO::VAO (cs237::Application *app, struct Chunk const &chunk)
  : _vBuf(new VAO::VBuffer_t(app, chunk.vertices)),
    _iBuf(new VAO::IBuffer_t(app, chunk.indices))
{ }

VAO::~VAO ()
{
    delete this->_vBuf;
    delete this->_iBuf;
}
