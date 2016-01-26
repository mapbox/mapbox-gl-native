CREATE TABLE resources (                   -- Generic table for style, source, sprite, and glyph resources.
  url TEXT NOT NULL PRIMARY KEY,           -- Same schema as http_cache table.
  kind INTEGER NOT NULL,
  expires INTEGER,
  modified INTEGER,
  accessed INTEGER,
  etag TEXT,
  data BLOB,                               -- NULL if the response was a 404
  compressed INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE tilesets (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  url_template TEXT NOT NULL,           -- As it would appear in TileJSON (but no support for host sharding).
  pixel_ratio INTEGER,                  -- If NULL, 1 is assumed for raster sources.
  UNIQUE (url_template, pixel_ratio)    -- Capable of caching the same tileset at multiple resolutions.
);

CREATE TABLE tiles (
  tileset_id INTEGER NOT NULL REFERENCES tilesets(id),
  z INTEGER NOT NULL,                   -- Fully abandon TMS coordinates in favor of ZXY.
  x INTEGER NOT NULL,
  y INTEGER NOT NULL,
  expires INTEGER,
  modified INTEGER,
  accessed INTEGER,
  etag TEXT,
  data BLOB,                            -- NULL if the response was a 404
  compressed INTEGER NOT NULL DEFAULT 0,
  PRIMARY KEY (tileset_id, z, x, y)
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
  resource_url TEXT NOT NULL REFERENCES resources(url),
  PRIMARY KEY (region_id, resource_url)
);

CREATE TABLE region_tiles (
  region_id INTEGER NOT NULL REFERENCES regions(id),
  tileset_id INTEGER NOT NULL REFERENCES tilesets(id),
  z INTEGER NOT NULL,
  x INTEGER NOT NULL,
  y INTEGER NOT NULL,
  PRIMARY KEY (region_id, tileset_id, z, x, y),
  FOREIGN KEY (tileset_id, z, x, y) REFERENCES tiles (tileset_id, z, x, y)
);

-- `region_resources` and `region_tiles` are used for deduplication and deletion logic.
-- A row in `tiles` exists IFF one or more corresponding rows exist in `region_tiles`. If
-- more than one corresponding row exists, it indicates multiple regions contain the tile, and
-- storage for the tile is being deduplicated. When a region is deleted, corresponding rows in
-- `region_tiles` must also be deleted, and then rows in `tiles` and `tilesets` without a
-- corresponding `region_tiles` row must be deleted. Similarly for `resources` / `region_resources`.
