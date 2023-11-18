/*! \file window.cpp
 *
 * CS23700 Autumn 2023 Sample Code for Group Project
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "window.hpp"
#include "map-cell.hpp"
#include "vao.hpp"
#include "texture-cache.hpp"

constexpr double kTimeStep = 0.001;     //! animation/physics timestep

Window::Window (Project *app, cs237::CreateWindowInfo const &info, Map *map)
  : cs237::Window (app, info), _map(map), _syncObjs(this), _nFrames(0)
{
    // Compute the bounding box for the entire map
    this->_mapBBox = cs237::AABBd_t(
        glm::dvec3(0.0, double(map->minElevation()), 0.0),
        glm::dvec3(
            double(map->hScale()) * double(map->width()),
            double(map->maxElevation()),
            double(map->hScale()) * double(map->height())));

    // Place the viewer in the center of cell(0,0), just above the
    // cell's bounding box.
    cs237::AABBd_t bb = map->cell(0,0)->tile(0).bBox();
    glm::dvec3 pos = bb.center();
    pos.y = bb.maxY() + 0.01 * (bb.maxX() - bb.minX());

    // The camera's direction is toward the bulk of the terrain
    glm::dvec3 at;
    if ((map->nRows() == 1) && (map->nCols() == 1)) {
        at = pos + glm::dvec3(1.0, -0.25, 1.0);
    }
    else {
        at = pos + glm::dvec3(double(map->nCols()-1), 0.0, double(map->nRows()-1));
    }
    this->_cam.move(pos, at, glm::dvec3(0.0, 1.0, 0.0));

    // set the FOV and near/far planes
    this->_cam.setFOV (60.0);
    double diagonal = 1.02 * std::sqrt(
        double(map->nRows() * map->nRows())
        + double(map->nCols() * map->nCols()));
    this->_cam.setNearFar (
        10.0,
        diagonal * double(map->cellWidth()) * double(map->hScale()));

    // default error limit is 1%
    this->_errorLimit = float(info.ht) / 100.0f;

    // initialize the Vulkan resources for the map cells
    std::clog << "initializing textures" << std::endl;
    this->_tCache = new TextureCache(app);
    for (int r = 0;  r < map->nRows(); r++) {
        for (int c = 0;  c < map->nCols();  c++) {
            Cell *cell = map->cell(r, c);
            if (map->hasAssets()) {
                cell->loadObjects();
            }
            cell->initTextures (this);
        }
    }

    /***** Vulkan initialization *****/

    std::clog << "vulkan state initialization" << std::endl;
    this->_initRenderPass ();

    /** HINT: add additional initialization for render modes */

    // create framebuffers for the swap chain
    this->_swap.initFramebuffers (this->_renderPass);

    // create the command buffer
    this->_cmdBuf = this->_app->newCommandBuf();

    // enable handling of keyboard events
    this->enableKeyEvent (true);

    // initialize animation state
    this->_lastStep = glfwGetTime();
}

Window::~Window ()
{
    auto device = this->device();

    /* delete the command buffer */
    this->_app->freeCommandBuf (this->_cmdBuf);

    vkDestroyRenderPass(device, this->_renderPass, nullptr);

    /** HINT: release other allocated objects */

}

void Window::_initRenderPass ()
{
    std::vector<vk::AttachmentDescription> attachDescs;
    std::vector<vk::AttachmentReference> attachRefs;
    this->_initAttachments(attachDescs, attachRefs);
    assert (attachRefs.size() == 2); /* we expect a depth buffer */

    vk::SubpassDescription subpass(
        {}, /* flags */
        vk::PipelineBindPoint::eGraphics, /* pipeline bind point */
        0, nullptr, /* input attachments */
        1, &(attachRefs[0]), /* color attachments */
        nullptr, /* resolve attachments */
        &(attachRefs[1]), /* depth-stencil attachment */
        0, nullptr); /* preserve attachments */

    vk::SubpassDependency dependency(
        VK_SUBPASS_EXTERNAL, /* src subpass */
        0, /* dst subpass */
        vk::PipelineStageFlagBits::eColorAttachmentOutput, /* src stage mask */
        vk::PipelineStageFlagBits::eColorAttachmentOutput, /* dst stage mask */
        {}, /* src access mask */
        { vk::AccessFlagBits::eColorAttachmentWrite }, /* dst access mask */
        {}); /* dependency flags */

    vk::RenderPassCreateInfo renderPassInfo(
        {}, /* flags */
        attachDescs, /* attachments */
        subpass, /* subpasses_ */
        dependency); /* dependencies */

    this->_renderPass = this->device().createRenderPass(renderPassInfo);

}

void Window::animate (double now)
{
    double dt = now - this->_lastStep;
    if (dt >= kTimeStep) {
        this->_lastStep = now;

        /* PUT ANIMATION CODE HERE */

    }

}

void Window::draw ()
{
    /* required function but we use `render` instead */
    ERROR("unexpected call to Window::draw()");
}

void Window::key (int key, int scancode, int action, int mods)
{
    // ignore releases, control keys, command keys, etc.
    if ((action != GLFW_RELEASE)
    && ((mods & (GLFW_MOD_CONTROL|GLFW_MOD_ALT|GLFW_MOD_SUPER)) == 0))
    {

        switch (key) {
        case GLFW_KEY_F: // toggle fog mode
            this->toggleFog();
            break;

        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:  // 'esc', 'q' or 'Q' ==> quit
            glfwSetWindowShouldClose (this->_win, true);
            break;

        case GLFW_KEY_W:  // 'w' or 'W' ==> toggle wireframe mode
            this->_wireframe = !this->_wireframe;
            break;

        case GLFW_KEY_LEFT:
            /** HINT: rotate the camera left around the vertical axis */
            break;

        case GLFW_KEY_RIGHT:
            /** HINT: rotate the camera right around the vertical axis */
            break;

        case GLFW_KEY_UP:
            /** HINT: rotate the camera up around the horizontal axis */
            break;

        case GLFW_KEY_DOWN:
            /** HINT: rotate the camera down around the horizontal axis */
            break;

        case GLFW_KEY_KP_ADD:
        case GLFW_KEY_EQUAL:
            if ((key == GLFW_KEY_KP_ADD)
            || ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)) // shifted '=' is '+'
            {
                // increase the error limit
                this->_errorLimit *= kSqrt2;
            }
            break;

        case GLFW_KEY_KP_SUBTRACT:
        case GLFW_KEY_MINUS:
            // reduce the error limit
            this->_errorLimit *= kOneOverSqrt2;
            break;

        default: // ignore all other keys
            return;
        }
    }

}
