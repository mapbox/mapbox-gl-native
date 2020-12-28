--
-- Table containing the style, source, sprite, and glyph
-- resources. Essentially everything that is not a tile.
--
CREATE TABLE resources (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,   -- Primary key.

  url TEXT NOT NULL,                               -- The URL of the resource without the access token. If a Mapbox
                                                   -- resource, will be stored using the mapbox:// schema. Must be
                                                   -- unique and that is enforced by the database schema.

  kind INTEGER NOT NULL,                           -- Type of the resource, taken from Resource::Kind enumeration:
                                                   -- style        = 1
                                                   -- source       = 2
                                                   -- tile         = 3
                                                   -- glyphs       = 4
                                                   -- sprite image = 5
                                                   -- sprite JSON  = 6
                                                   -- image        = 7

  expires INTEGER,                                 -- Expiration time. The resource will be refreshed after this
                                                   -- expiration is reached.

  modified INTEGER,                                -- Last time the resource was modified.

  etag TEXT,                                       -- Checksum used for cache optimization. If, when refreshing the
                                                   -- resource, it matches the etag on the server, the resource will
                                                   -- not get re-downloaded. See:
                                                   -- https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/ETag

  data BLOB,                                       -- Contents of the resource.

  compressed INTEGER NOT NULL DEFAULT 0,           -- If the resource is compressed with Deflate or not. Compression is
                                                   -- optional and should be used when the compression ratio is
                                                   -- significant. Using compression will make decoding time slower
                                                   -- because it will add an extra decompression step.

  accessed INTEGER NOT NULL,                       -- Last time the resource was used by GL Native. Useful for when
                                                   -- evicting the least used resources from the cache.

  must_revalidate INTEGER NOT NULL DEFAULT 0,      -- When set to true, the resource will not be used unless it gets
                                                   -- first revalidated by the server.
  UNIQUE (url)
);

--
-- Table containing all tiles, both vector and raster.
--
CREATE TABLE tiles (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,   -- Primary key.

  url_template TEXT NOT NULL,                      -- The URL of the resource without the access token and without
                                                   -- the tiles id substituted. Mapbox tiles will be stored using
                                                   -- the mapbox:// schema. Example:
                                                   -- mapbox://tiles/user.dataset/{z}/{x}/{y}.vector.pbf

  pixel_ratio INTEGER NOT NULL,                    -- The tile pixel ratio, typically 1 for vector tiles.

  z INTEGER NOT NULL,                              -- The zoom level of the tile.

  x INTEGER NOT NULL,                              -- The x position of the tile on the tile grid.

  y INTEGER NOT NULL,                              -- The y position of the tile on the tile grid.

  expires INTEGER,                                 -- Expiration time. The tile will be refreshed after this
                                                   -- expiration is reached. Expired tiles can still be rendered,
                                                   -- unless must_revalidate is set to true. If an expired tile
                                                   -- gets rendered, it will be replaced by a newer version as soon
                                                   -- as the network request with a new tile arrives.

  modified INTEGER,                                -- Last time the tile was modified.

  etag TEXT,                                       -- Checksum used for cache optimization. If, when refreshing the
                                                   -- tile, it matches the etag on the server, the tile will not
                                                   -- get re-downloaded. See:
                                                   -- https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/ETag

  data BLOB,                                       -- Contents of the tile.

  compressed INTEGER NOT NULL DEFAULT 0,           -- If the tile is compressed with Deflate or not. Compression is
                                                   -- optional and should be used when the compression ratio is
                                                   -- significant. Using compression will make decoding time slower
                                                   -- because it will add an extra decompression step.

  accessed INTEGER NOT NULL,                       -- Last time the tile was used by GL Native. Useful for when
                                                   -- evicting the least used tiles from the cache.

  must_revalidate INTEGER NOT NULL DEFAULT 0,      -- When set to true, the tile will not be used unless it gets
                                                   -- first revalidated by the server.
  UNIQUE (url_template, pixel_ratio, z, x, y)
);

--
-- Regions define the offline regions, which could be a GeoJSON geometry,
-- or a bounding box like this example:
--
-- {
--     "bounds": [
--         37.2,
--         -122.8,
--         38.1,
--         -121.7
--     ],
--     "include_ideographs": false,
--     "max_zoom": 15.0,
--     "min_zoom": 0.0,
--     "pixel_ratio": 1.0,
--     "style_url": "mapbox://styles/mapbox/streets-v11"
-- }
--
-- The semantic definition of the region is up to the user and
-- it could be a city, a country or an arbitrary bounding box.
--
-- Regions can overlap, which will cause them to share resources
-- when it is the case.
--
-- "include_ideographs" is set to true when CJK characters are
-- include on the offline package. By default, CJK is rendered
-- by GL Native client side using local fonts. Downloading CJK
-- will increase the size of the database considerably.
--
CREATE TABLE regions (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,    -- Primary key.

  definition TEXT NOT NULL,                         -- JSON formatted definition of a region, a bounding box
                                                    -- or a GeoJSON geometry. See https://geojson.org.

  description BLOB                                  -- User provided data in user-defined format.
);

--
-- Table mapping resources to regions. A resource
-- might be part of many regions. Resources without
-- regions are part of the ambient cache.
--
CREATE TABLE region_resources (
  region_id INTEGER NOT NULL REFERENCES regions(id) ON DELETE CASCADE,
  resource_id INTEGER NOT NULL REFERENCES resources(id),
  UNIQUE (region_id, resource_id)
);

--
-- Table mapping tiles to regions. A tile might
-- be part of many regions, meaning that regions might
-- overlap efficiently. Tiles without regions are part
-- of the ambient cache.
--
CREATE TABLE region_tiles (
  region_id INTEGER NOT NULL REFERENCES regions(id) ON DELETE CASCADE,
  tile_id INTEGER NOT NULL REFERENCES tiles(id),
  UNIQUE (region_id, tile_id)
);

--
-- Indexes for efficient eviction queries.
--

CREATE INDEX resources_accessed
ON resources (accessed);

CREATE INDEX tiles_accessed
ON tiles (accessed);

CREATE INDEX region_resources_resource_id
ON region_resources (resource_id);

CREATE INDEX region_tiles_tile_id
ON region_tiles (tile_id);
