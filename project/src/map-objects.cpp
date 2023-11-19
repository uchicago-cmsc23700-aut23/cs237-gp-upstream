/*! \file map-objects.cpp
 *
 * Implementation of the MapObjects class.
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "map-objects.hpp"
#include "map.hpp"
#include "map-cell.hpp"
#include "json.hpp"
#include <unistd.h>

/* helper functions to make extracting values from the JSON easier */

//! load a vec3f from a JSON object.
//! \return false if okay, true if there is an error.
bool loadVec3 (json::Object const *jv, glm::vec3 &vec)
{
    if (jv == nullptr) {
        return true;
    }

    const json::Number *x = jv->fieldAsNumber("x");
    const json::Number *y = jv->fieldAsNumber("y");
    const json::Number *z = jv->fieldAsNumber("z");
    if ((x == nullptr) || (y == nullptr) || (z == nullptr)) {
        return true;
    }

    vec = glm::vec3 (
        static_cast<float>(x->realVal()),
        static_cast<float>(y->realVal()),
        static_cast<float>(z->realVal()));

    return false;
}

//! load a color3f from a JSON object.
//! \return false if okay, true if there is an error.
bool loadColor (json::Object const *jv, glm::vec3 &color)
{
    if (jv == nullptr) {
        return true;
    }

    const json::Number *r = jv->fieldAsNumber("r");
    const json::Number *g = jv->fieldAsNumber("g");
    const json::Number *b = jv->fieldAsNumber("b");
    if ((r == nullptr) || (g == nullptr) || (b == nullptr)) {
        return true;
    }

    color = glm::vec3 (
        static_cast<float>(r->realVal()),
        static_cast<float>(g->realVal()),
        static_cast<float>(b->realVal()));

    return false;
}

/***** class MapObjects member functions *****/

MapObjects::~MapObjects ()
{
    for (auto it : this->_objs) {
        delete it.second;
    }
    for (auto it : this->_texs) {
        delete it.second;
    }
}

void MapObjects::loadObjects (Cell *cell, std::vector<Instance *> &objs)
{
    // check that the vector is empty
    if (! objs.empty()) {
        ERROR("loadObjects called with non-empty object vector");
    }

    std::string objsFile = cell->datafile("/objects.json");

    // if the objects.json file does not exist, then we return the empty vector
    if (access(objsFile.c_str(), F_OK) != 0) {
        return;
    }

    // load the objects list
    json::Value *root = json::parseFile(objsFile);

    // check for errors
    if (root == nullptr) {
        ERROR("Unable to load objects list \"" + objsFile + "\"");
    } else if (! root->isArray()) {
        ERROR("Invalid object list in \"" + objsFile + "\"; root is not an array");
    }
    const json::Array *rootObj = root->asArray();

    if (rootObj->length() != 0) {
        // allocate space for the objects in the scene
        objs.reserve(rootObj->length());

        // load the object instances in the cell
        for (int i = 0;  i < rootObj->length();  i++) {
            json::Object const *object = (*rootObj)[i]->asObject();
            if (object == nullptr) {
                ERROR("Expected array of JSON objects in \"" + objsFile + "\"");
            }
            json::String const *file = object->fieldAsString("file");
            json::Object const *frame = object->fieldAsObject("frame");
            glm::vec3 pos, xAxis, yAxis, zAxis;
            glm::vec3 color;
            if ((file == nullptr) || (frame == nullptr)
            ||  loadVec3 (object->fieldAsObject("pos"), pos)
            ||  loadVec3 (frame->fieldAsObject("x-axis"), xAxis)
            ||  loadVec3 (frame->fieldAsObject("y-axis"), yAxis)
            ||  loadVec3 (frame->fieldAsObject("z-axis"), zAxis)
            ||  loadColor (object->fieldAsObject("color"), color)) {
                ERROR("Invalid object description in \"" + objsFile + "\"");
            }
            const json::Bool *transparent = object->fieldAsBool("transparent");
            Instance *inst = this->_makeInstance(
                file->value(),
                cell,
                glm::mat4 (
                    glm::vec4 (xAxis, 0.0f),
                    glm::vec4 (yAxis, 0.0f),
                    glm::vec4 (zAxis, 0.0f),
                    glm::vec4 (pos, 1.0f)),
                color,
                (transparent != nullptr) && transparent->value());
          // add to objs vector
            objs.push_back (inst);
        }

    }

}

Instance *MapObjects::_makeInstance (
    std::string const &file,
    Cell *cell,
    glm::mat4 const &toCell,
    glm::vec3 const &color,
    bool isTransparent)
{
  // first we need to get the model
    OBJ::Model *model = this->_loadModel (file);

    assert (model != nullptr);

  // create the instance
    Instance *inst = new Instance;
    inst->model = model;
    inst->toCell = toCell;
    inst->color = color;
    inst->transparent = isTransparent;

    return inst;

}

OBJ::Model *MapObjects::_loadModel (std::string const &file)
{
    auto it = this->_objs.find (file);
    if (it == this->_objs.end()) {
      // load the model from the OBJ file
        OBJ::Model *model = new OBJ::Model (this->_map->assetsDir() + file);
      // preload any textures in the materials of the model
        for (auto grpIt = model->beginGroups();  grpIt != model->endGroups();  grpIt++) {
            const OBJ::Material *mat = &model->material((*grpIt).material);
            /* we ignore the ambient map */
            this->_loadTexture (mat->emissiveMap, true);
            this->_loadTexture (mat->diffuseMap, true);
            this->_loadTexture (mat->specularMap, false);
            this->_loadTexture (mat->normalMap, false);
        }
      // cache the model
        this->_objs[file] = model;
        return model;
    }
    else {
        return it->second;
    }
}

void MapObjects::_loadTexture (std::string name, bool sRGB)
{
    if (name.empty()) {
        return;
    }
    // have we already loaded this texture?
    if (this->_texs.find(name) != this->_texs.end()) {
        return;
    }
    // load the image data;
    cs237::Image2D *img;
    if (sRGB) {
        img = new cs237::Image2D(this->_map->assetsDir() + name);
    } else {
        img = new cs237::DataImage2D(this->_map->assetsDir() + name);
    }
    if (img == nullptr) {
        ERROR("Unable to find texture-image file \"" + name + "\"");
    }
    // add to _texs map
    this->_texs.insert (std::pair<std::string, cs237::Image2D *>(name, img));

}
