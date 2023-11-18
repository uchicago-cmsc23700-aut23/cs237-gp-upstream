/*! \file texture-cache.hpp
 *
 * \author John Reppy
 *
 * The interface to a caching mechanism for textures.
 */

/* CMSC23700 Final Project sample code (Autumn 2023)
 *
 * COPYRIGHT (c) 2023 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _TEXTURE_CACHE_HPP_
#define _TEXTURE_CACHE_HPP_

#include "cs237.hpp"
#include "tqt.hpp"
#include <unordered_map>
#include <vector>

class TextureCache;

//! A texture for a tile in the chunk quad treexs
class TileTexture {
  public:
    ~TileTexture ();

    //! is this texture active?
    bool isActive () const { return this->_active; }

    //! activate the texture; this operation is a hint to the texture
    //! cache that the texture is going to be used soon.
    void activate ();

    //! hint to the texture cache that this texture is not needed.
    void release ();

    //! initialize the descriptor-info needed to update a descriptor for this
    //! texture
    vk::DescriptorImageInfo getDescriptorInfo ()
    {
        if (! this->isActive()) {
            this->activate();
        }
        return vk::DescriptorImageInfo(
            this->_sampler,
            this->_txt->view(),
            vk::ImageLayout::eShaderReadOnlyOptimal);
    }

  private:
    cs237::Texture2D *_txt;     //!< the Vulkan texture (or nullptr, if not resident)
    vk::Sampler _sampler;         //!< the sampler for accessing the texture from the
                                //!< shaders (when active)
    TextureCache *_cache;       //!< the cache that this texture belongs to
    tqt::TextureQTree *_tree;   //!< the texture quadtree from which this texture comes
    uint32_t _level;            //!< the TQT level of this texture
    uint32_t _row;              //!< the TQT row of this texture
    uint32_t _col;              //!< the TQT column of this texture
    uint32_t _lastUsed;         //!< the last frame that this texture was used
    int _activeIdx;             //!< index of this texture in the cache's _active vector
    bool _active;               //!< true when this texture is in use
    bool _mipmaps;              //!< should we generate mipmaps for the texture?

    TileTexture (
        TextureCache *cache,
        tqt::TextureQTree *tree,
        int level, int row, int col,
        bool mipmaps);

    friend class TextureCache;
    friend struct TxtCompare;
};

//! A cache of Vulkan textures that is backed by texture-quad-trees
class TextureCache {
  public:

    //! TextureCache constructor
    //! \param app     the application
    //! \param mipmap  optional flag to request mipmaps for the textures when they are created.
    TextureCache (cs237::Application *app, bool mipmap = false);
    ~TextureCache ();

  //! \brief make a texture handle for the specified quad in the texture quad tree
  //! \param tree    the TQT to get the source image data from
  //! \param level   the TQT level of the texture
  //! \param row     the TQT row of the texture
  //! \param col     the TQT column of the texture
  //! \return the texture for the tile
    TileTexture *make (tqt::TextureQTree *tree, int level, int row, int col);

  //! mark the beginning of a new frame; the texture cache uses this information to
  //! track LRU information
    void newFrame () { this->_clock++; }

  private:
    cs237::Application *_app;   //!< application pointer
    uint64_t _numActive;        //!< number of GPU resident textures
    uint64_t _clock;            //!< counts number of frames

    //! keys for hashing texture specifications
    struct Key {
        tqt::TextureQTree       *_tree; //!< the texture quadtree from which this texture comes
        int                     _level; //!< the TQT level of this texture
        int                     _row;   //!< the TQT row of this texture
        int                     _col;   //!< the TQT column of this texture

        Key (tqt::TextureQTree *t, int l, int r, int c)
            : _tree(t), _level(l), _row(r), _col(c)
        { }
    };
    //! hashing keys
    struct Hash {
        std::size_t operator() (Key const &k) const
        {
            return static_cast<std::size_t>(
                reinterpret_cast<std::size_t>(k._tree) +
                (static_cast<int>(k._level) << 5) * 101 +
                (static_cast<int>(k._row) << 10) * 101 +
                static_cast<int>(k._col));
        }
    };
    //! equality test on keys
    struct Equal {
        bool operator()(Key const &k1, Key const &k2) const
        {
            return (k1._tree == k2._tree) && (k1._level == k2._level)
                && (k1._row == k2._row) && (k1._col == k2._col);
        }
    };

    // for ordering textures by timestamp
    struct TxtCompare {
        bool operator() (const TileTexture &lhs, const TileTexture &rhs) const
        {
            return (lhs._lastUsed > rhs._lastUsed);
        }
    };

    typedef std::unordered_map<Key,TileTexture *,Hash,Equal> TextureTbl;

    TextureTbl _textureTbl;             //!< mapping from TQT spec to TileTexture
    std::vector<TileTexture *> _active; //!< active textures
    std::vector<TileTexture *> _inactive; //!< inactive textures that are loaded, but may be reused.

    //! record that the given texture is now active
    void _makeActive (TileTexture *txt);

    //! record that the given texture is now inactive
    void _release (TileTexture *txt);

    friend class TileTexture;
};

#endif // !_TEXTURE_CACHE_HPP_
