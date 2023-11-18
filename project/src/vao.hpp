/*! \file vao.hpp
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _VAO_HPP_
#define _VAO_HPP_

#include "cs237.hpp"
#include "map-cell.hpp"

//! A vertex-array object is a container for the information
//! required to render a chunk of the mesh.
struct VAO {
    using VBuffer_t = cs237::VertexBuffer<HFVertex>;
    using IBuffer_t = cs237::IndexBuffer<uint16_t>;

    VBuffer_t *_vBuf;           //!< the vertex buffer
    IBuffer_t *_iBuf;           //!< the index buffer

    VAO (cs237::Application *_app, struct Chunk const &chunk);
    ~VAO ();

    uint32_t nIndices () const { return this->_iBuf->nIndices(); }

    //! emit commands to render the contents of the VAO.
    void render (vk::CommandBuffer cmdBuf)
    {
        vk::Buffer vertBuffers[] = {this->_vBuf->vkBuffer()};
        vk::DeviceSize offsets[] = {0};
        cmdBuf.bindVertexBuffers(0, 1, vertBuffers, offsets);

        cmdBuf.bindIndexBuffer(this->_iBuf->vkBuffer(), 0, vk::IndexType::eUint16);

        cmdBuf.drawIndexed(this->_iBuf->nIndices(), 1, 0, 0, 0);
    }

};

#endif //! _VAO_HPP_
