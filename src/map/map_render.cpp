#include <llmr/map/map.hpp>
// #include <llmr/map/settings.hpp>
// #include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/map/coverage.hpp>
// #include <llmr/util/animation.hpp>

using namespace llmr;

// NOTE: All functions in this file run in the render thread.

// This is the entry point for the render_thread
void Map::loop(void *ptr) {
    Map *map = static_cast<Map *>(ptr);
    map->context->make_active();
    map->setup();
    map->rerender();
    uv_run(map->render_loop, UV_RUN_DEFAULT);
}

void Map::setup() {
    painter.setup();

    style.sprite = std::make_shared<Sprite>();
    style.sprite->load(kSpriteURL, transform.getPixelRatio());

    style.loadJSON(resources::style, resources::style_size);
}

void Map::async_transform_cb(uv_async_t *async, int status) {
    Map *map = static_cast<Map *>(async->data);
    map->processTransforms();
    map->cascadeStyle();
    map->rerender();
}

void Map::async_render_cb(uv_async_t *async, int status) {
    Map *map = static_cast<Map *>(async->data);

    if (map->updateTiles()) {
        map->dirty = true;
    }

    if (map->transform.needsAnimation()) {
        map->dirty = true;
    }

    if (map->dirty == true) {
        if (map->swap == false) {
            // The framebuffer has been swapped previously, so we're good to paint
            // on the new framebuffer if we need to.
            map->dirty = false;
            map->render();
            map->swap = true;
            map->context->swap();
        } else {
            map->rerender();
        }
    }

    if (map->transform.needsAnimation()) {
        map->rerender();
    }
}

void Map::async_terminate_cb(uv_async_t *async, int status) {
    Map *map = static_cast<Map *>(async->data);
    uv_stop(map->render_loop);
}

void Map::processTransforms() {
    std::lock_guard<std::mutex> lock(transform_commands_lock);

    while (!transform_commands.empty()) {
        const TransformCommand &cmd = transform_commands.front();
        switch (cmd.type) {
            case TransformCommand::Null: /* null */ break;
            case TransformCommand::MoveBy: transform(cmd.moveBy, cmd.duration); break;
            case TransformCommand::ScaleBy: transform(cmd.scaleBy, cmd.duration); break;
            case TransformCommand::RotateBy: transform(cmd.rotateBy, cmd.duration); break;
            case TransformCommand::LonLat: transform(cmd.lonLat, cmd.duration); break;
            case TransformCommand::Scale: transform(cmd.scale, cmd.duration); break;
            case TransformCommand::Angle: transform(cmd.angle, cmd.duration); break;
            case TransformCommand::Resize:
                transform(cmd.resize);
                painter.resize(cmd.resize.fb_width, cmd.resize.fb_height);
                break;
            default: fprintf(stderr, "unhandled transform type\n"); break;
        }
        transform_commands.pop();
        dirty = true;
    }
}

void Map::cascadeStyle() {
    style.cascade(transform.getNormalizedZoom());
}

void Map::render() {
    transform.updateAnimations();

    if (*style.sprite->raster && !style.sprite->raster->textured) {
        style.sprite->raster->setTexturepool(&texturepool);
    }

    painter.clear();

    painter.changeMatrix();

    painter.setDebug(debug);

    // First, update all tile matrices with the new transform and render into
    // the stencil buffer.
    uint8_t i = 1;
    painter.prepareClippingMask();
    for (Tile& tile : tiles) {
        if (tile.data && tile.data->state == TileData::State::parsed) {
            // The position matrix.
            transform.matrixFor(tile.matrix, tile.id);
            matrix::multiply(tile.matrix, painter.projMatrix, tile.matrix);
            tile.clip_id = i++;
            painter.drawClippingMask(tile.matrix, tile.clip_id, !tile.data->use_raster);
        }
    }
    painter.finishClippingMask();

    for (const Tile& tile : tiles) {
        if (tile.data && tile.data->state == TileData::State::parsed) {
            if (tile.data->use_raster && *tile.data->raster && !tile.data->raster->textured) {
                tile.data->raster->setTexturepool(&texturepool);
                tile.data->raster->beginFadeInAnimation();
            }
            if (tile.data->use_raster && tile.data->raster->needsAnimation()) {
                tile.data->raster->updateAnimations();
            }
            painter.render(tile);
        }
    }

    painter.renderMatte();
}


bool Map::updateTiles() {
    bool changed = false;

    // Figure out what tiles we need to load
    int32_t zoom = floor(transform.getZoom());
    if (zoom > max_zoom) zoom = max_zoom;
    if (zoom < min_zoom) zoom = min_zoom;

    int32_t max_covering_zoom = zoom + 1;
    if (max_covering_zoom > max_zoom) max_covering_zoom = max_zoom;

    int32_t min_covering_zoom = zoom - 10;
    if (min_covering_zoom < min_zoom) min_covering_zoom = min_zoom;

    // Map four viewport corners to pixel coordinates
    box box = transform.mapCornersToBox(zoom);

    // Performs a scanline algorithm search that covers the rectangle of the box
    // and sorts them by proximity to the center.
    std::forward_list<Tile::ID> required = llmr::covering_tiles(zoom, box, use_raster, (transform.getPixelRatio() > 1.0));

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        const TileData::State state = addTile(id);

        if (state != TileData::State::parsed) {
            // if (use_raster && (transform.rotating || transform.scaling || transform.panning))
            //     break;

            // The tile we require is not yet loaded. Try to find a parent or
            // child tile that we already have.

            // First, try to find existing child tiles that completely cover the
            // missing tile.
            bool complete = findLoadedChildren(id, max_covering_zoom, retain);

            // Then, if there are no complete child tiles, try to find existing
            // parent tiles that completely cover the missing tile.
            if (!complete) {
                findLoadedParent(id, min_covering_zoom, retain);
            }
        }

        if (state == TileData::State::initial) {
            changed = true;
        }
    }

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    std::forward_list<Tile::ID> retain_data;
    tiles.remove_if([&retain, &retain_data, &changed](const Tile & tile) {
        bool obsolete = std::find(retain.begin(), retain.end(), tile.id) == retain.end();
        if (obsolete) {
            changed = true;
        } else {
            retain_data.push_front(tile.data->id);
        }
        return obsolete;
    });

    // Sort tiles by zoom level, front to back.
    // We're painting front-to-back, so we want to draw more detailed tiles first
    // before filling in other parts with lower zoom levels.
    tiles.sort([](const Tile & a, const Tile & b) {
        return a.id.z < b.id.z;
    });

    // Remove all the expired pointers from the list.
    tile_data.remove_if([&retain_data](const std::weak_ptr<TileData>& tile_data) {
        const std::shared_ptr<TileData> tile = tile_data.lock();
        if (!tile) {
            return true;
        }
        bool obsolete = std::find(retain_data.begin(), retain_data.end(), tile->id) == retain_data.end();
        if (obsolete) {
            tile->cancel();
            return true;
        } else {
            return false;
        }
    });

    return changed;
}

TileData::State Map::hasTile(const Tile::ID& id) {
    for (const Tile& tile : tiles) {
        if (tile.id == id && tile.data) {
            return tile.data->state;
        }
    }

    return TileData::State::invalid;
}

TileData::State Map::addTile(const Tile::ID& id) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    tiles.emplace_front(id);
    Tile& new_tile = tiles.front();

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const Tile::ID normalized_id = id.normalized();

    auto it = std::find_if(tile_data.begin(), tile_data.end(), [&normalized_id](const std::weak_ptr<TileData>& tile_data) {
        return !tile_data.expired() && tile_data.lock()->id == normalized_id;
    });

    if (it != tile_data.end()) {
        // Create a shared_ptr handle. Note that this might be empty!
        new_tile.data = it->lock();
    }

    if (new_tile.data && new_tile.data->state == TileData::State::obsolete) {
        // Do not consider the tile if it's already obsolete.
        new_tile.data.reset();
    }

    if (!new_tile.data) {
        // If we don't find working tile data, we're just going to load it.
        new_tile.data = std::make_shared<TileData>(normalized_id, style, glyphAtlas, use_raster, (transform.getPixelRatio() > 1.0));
        new_tile.data->request([this](){
            this->dirty = true;
            this->rerender();
        });
        tile_data.push_front(new_tile.data);
    }

    return new_tile.data->state;
}

/**
 * Recursively find children of the given tile that are already loaded.
 *
 * @param id The tile ID that we should find children for.
 * @param maxCoveringZoom The maximum zoom level of children to look for.
 * @param retain An object that we add the found tiles to.
 *
 * @return boolean Whether the children found completely cover the tile.
 */
bool Map::findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain) {
    bool complete = true;
    int32_t z = id.z;


    auto ids = id.children(z + 1);
    for (const Tile::ID& child_id : ids) {
        const TileData::State state = hasTile(child_id);
        if (state == TileData::State::parsed) {
            retain.emplace_front(child_id);
        } else {
            complete = false;
            if (z < maxCoveringZoom) {
                // Go further down the hierarchy to find more unloaded children.
                findLoadedChildren(child_id, maxCoveringZoom, retain);
            }
        }
    }
    return complete;
}

/**
 * Find a loaded parent of the given tile.
 *
 * @param id The tile ID that we should find children for.
 * @param minCoveringZoom The minimum zoom level of parents to look for.
 * @param retain An object that we add the found tiles to.
 *
 * @return boolean Whether a parent was found.
 */
bool Map::findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain) {
    for (int32_t z = id.z - 1; z >= minCoveringZoom; --z) {
        const Tile::ID parent_id = id.parent(z);
        const TileData::State state = hasTile(parent_id);
        if (state == TileData::State::parsed) {
            retain.emplace_front(parent_id);
            return true;
        }
    }
    return false;
}

