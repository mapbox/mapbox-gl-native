CREATE TABLE resources (                   -- Generic table for style, source, sprite, and glyph resources.
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  url TEXT NOT NULL,
  kind INTEGER NOT NULL,
  expires INTEGER,
  modified INTEGER,
  etag TEXT,
  data BLOB,
  compressed INTEGER NOT NULL DEFAULT 0,
  accessed INTEGER NOT NULL,
  UNIQUE (url)
);

CREATE TABLE tilesets (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  url_template TEXT NOT NULL,           -- As it would appear in TileJSON (but no support for host sharding).
  pixel_ratio INTEGER,                  -- If NULL, 1 is assumed for raster sources.
  UNIQUE (url_template, pixel_ratio)    -- Capable of caching the same tileset at multiple resolutions.
);

CREATE TABLE tiles (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  tileset_id INTEGER NOT NULL REFERENCES tilesets(id),
  z INTEGER NOT NULL,
  x INTEGER NOT NULL,
  y INTEGER NOT NULL,
  expires INTEGER,
  modified INTEGER,
  etag TEXT,
  data BLOB,
  compressed INTEGER NOT NULL DEFAULT 0,
  accessed INTEGER NOT NULL,
  UNIQUE (tileset_id, z, x, y)
);

CREATE TABLE regions (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  definition TEXT NOT NULL,   -- JSON formatted definition of region. Regions may be of variant types:
                              -- e.g. bbox and zoom range, route path, flyTo parameters, etc. Note that
                              -- the set of tiles required for a region may span multiple sources.
  description BLOB            -- User provided data in user-defined format
);

CREATE TABLE region_resources (
  region_id INTEGER NOT NULL REFERENCES regions(id),
  resource_id INTEGER NOT NULL REFERENCES resources(id),
  UNIQUE (region_id, resource_id)
);

CREATE TABLE region_tiles (
  region_id INTEGER NOT NULL REFERENCES regions(id),
  tile_id INTEGER NOT NULL REFERENCES tiles(id),
  UNIQUE (region_id, tile_id)
);

-- Indexes for efficient eviction queries

CREATE INDEX resources_accessed
ON resources (accessed);

CREATE INDEX tiles_accessed
ON tiles (accessed);

CREATE INDEX region_resources_resource_id
ON region_resources (resource_id);

CREATE INDEX region_tiles_tile_id
ON region_tiles (tile_id);
