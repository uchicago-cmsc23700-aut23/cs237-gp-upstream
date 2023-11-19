/*! \file map-objects.hpp
 *
 * Support for loading OBJ objects with supporting textures.  The MapObjects class
 * is a singleton class that is embedded in the Map object.  It is meant to support
 * both loading renderer-specific objects from the 'data' directory as well as
 * map-specific objects from the map's 'objects' directory.
 *
 * Note that names used for objects and textures need to be globally unique.
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _MAP_OBJECTS_HPP_
#define _MAP_OBJECTS_HPP_

#include "cs237.hpp"
#include "obj.hpp"
#include <map>

class Map;
class Cell;

//! an instance of a model, which has its own position and color.
struct Instance {
    Cell *cell;                 //!< the cell where this instance is located
    OBJ::Model *model;          //!< pointer to the OBJ model
    glm::mat4 toCell;           //!< affine transform from model space to the cell's
                                //!  coordinate system
    glm::vec3 color;            //!< the color of the object
    bool transparent;           //!< is the object transparent?

    /// return the matrix for converting normal vectors from the model's
    /// coordinate system to the cell's coordinate system
    glm::mat3 normToCell () const
    {
        return glm::inverseTranspose(glm::mat3(this->toCell));
    }

};

//! a container for the objects in a map
class MapObjects {
  public:

    //! constructor
    //! \param map the map that contains the objects
    MapObjects (Map *map) : _map(map), _objs(), _texs() { }

    ~MapObjects ();

    //! load the objects instances for a map cell
    //! \param cell[in]   the path to the cell's subdirectory
    //! \param objs[out]  the vector to load with the objects
    void loadObjects (Cell *cell, std::vector<Instance *> &objs);

    //! lookup a texture image by name
    //! \returns a pointer to the image object or nullptr if the image is not found
    cs237::Image2D *textureByName (std::string name) const;

    Map *map () { return this->_map; }

  private:
    Map *_map;                                          //!< the map
    std::map<std::string, OBJ::Model *> _objs;          //!< object-mesh cache
    std::map<std::string, cs237::Image2D *> _texs;      //!< texture-image cache

    //! helper function for creating instances
    Instance *_makeInstance (
        std::string const &file,
        Cell *cell,
        glm::mat4 const &toCell,
        glm::vec3 const &color,
        bool isTransparent);

    //! helper function for loading OBJ models from files
    //! \param dir  the map's asset directory
    //! \param file  the file name of the model
    //! \return a pointer to the model; the model data is cached so that it is only
    //!         loaded once.
    OBJ::Model *_loadModel (std::string const &file);

    //! helper function for pre-loading texture images into the _texs map
    //! \param name  the name of the file
    //! \param sRGB  argument specifying if the image is a SRGB image
    void _loadTexture (std::string name, bool sRGB);

};

#endif //! _MAP_OBJECTS_HPP_
