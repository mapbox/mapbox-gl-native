INSERT INTO regions
   SELECT DISTINCT NULL, sr.definition, sr.description -- Merge duplicate regions
    FROM side.regions sr 
    LEFT JOIN regions r ON sr.definition = r.definition AND sr.description IS r.description
      WHERE r.definition IS NULL;

CREATE TEMPORARY TABLE region_mapping AS
    SELECT sr.id AS side_region_id,
      r.id AS main_region_id
    FROM side.regions sr
    JOIN regions r ON sr.definition = r.definition  AND sr.description IS r.description;

--Insert /Update tiles
REPLACE INTO tiles
    SELECT t.id, -- use the old ID in case we run a REPLACE. If it doesn't exist yet, it'll be NULL which will auto-assign a new ID.
        st.url_template, st.pixel_ratio, st.z, st.x, st.y,
        st.expires, st.modified, st.etag, st.data, st.compressed, st.accessed, st.must_revalidate
    FROM (SELECT DISTINCT sti.* FROM side.region_tiles srt JOIN side.tiles sti ON srt.tile_id = sti.id)   -- ensure that we're only considering region tiles, and not ambient tiles.
    AS st
    LEFT JOIN tiles t ON st.url_template = t.url_template AND st.pixel_ratio = t.pixel_ratio AND st.z = t.z AND st.x = t.x AND st.y = t.y
        WHERE t.id IS NULL -- only consider tiles that don't exist yet in the original database.
        OR st.modified > t.modified; -- ...or tiles that are newer in the side loaded DB.

-- Update region_tiles usage
INSERT OR IGNORE INTO region_tiles
    SELECT rm.main_region_id, sti.id
    FROM side.region_tiles srt
    JOIN region_mapping rm ON srt.region_id = rm.side_region_id
    JOIN (SELECT t.id, st.id AS side_tile_id FROM side.tiles st
            JOIN tiles t ON st.url_template = t.url_template AND st.pixel_ratio = t.pixel_ratio AND st.z = t.z AND st.x = t.x AND st.y = t.y
    ) AS sti ON srt.tile_id = sti.side_tile_id;

-- copy over resources
REPLACE INTO resources
    SELECT r.id, 
        sr.url, sr.kind, sr.expires, sr.modified, sr.etag,
        sr.data, sr.compressed, sr.accessed, sr.must_revalidate
    FROM side.region_resources srr JOIN side.resources sr ON srr.resource_id = sr.id   --only consider region resources, and not ambient resources.
    LEFT JOIN resources r ON sr.url = r.url
        WHERE r.id IS NULL -- only consider resources that don't exist yet in the main database
        OR sr.modified > r.modified; -- ...or resources that are newer in the side loaded DB.

-- Update region_resources usage
INSERT OR IGNORE INTO region_resources
  SELECT rm.main_region_id, sri.id
  FROM side.region_resources srr
  JOIN region_mapping rm ON srr.region_id = rm.side_region_id
  JOIN (SELECT r.id, sr.id AS side_resource_id FROM side.resources sr
          JOIN resources r ON sr.url = r.url) AS sri  ON srr.resource_id = sri.side_resource_id;
 
DROP TABLE region_mapping;