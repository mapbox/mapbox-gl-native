#pragma once

// THIS IS A GENERATED FILE; EDIT merge_sideloaded.sql INSTEAD
// To regenerate, run `node platform/default/mbgl/storage/merge_sideloaded.js`

namespace mbgl {

static constexpr const char* mergeSideloadedDatabaseSQL =
"INSERT INTO regions\n"
"   SELECT DISTINCT NULL, sr.definition, sr.description\n"
"    FROM side.regions sr \n"
"    LEFT JOIN regions r ON sr.definition = r.definition AND sr.description IS r.description\n"
"      WHERE r.definition IS NULL;\n"
"CREATE TEMPORARY TABLE region_mapping AS\n"
"    SELECT sr.id AS side_region_id,\n"
"      r.id AS main_region_id\n"
"    FROM side.regions sr\n"
"    JOIN regions r ON sr.definition = r.definition  AND sr.description IS r.description;\n"
"REPLACE INTO tiles\n"
"    SELECT t.id,\n"
"        st.url_template, st.pixel_ratio, st.z, st.x, st.y,\n"
"        st.expires, st.modified, st.etag, st.data, st.compressed, st.accessed, st.must_revalidate\n"
"    FROM (SELECT DISTINCT sti.* FROM side.region_tiles srt JOIN side.tiles sti ON srt.tile_id = sti.id)\n"
"    AS st\n"
"    LEFT JOIN tiles t ON st.url_template = t.url_template AND st.pixel_ratio = t.pixel_ratio AND st.z = t.z AND st.x = t.x AND st.y = t.y\n"
"        WHERE t.id IS NULL\n"
"        OR st.modified > t.modified;\n"
"INSERT OR IGNORE INTO region_tiles\n"
"    SELECT rm.main_region_id, sti.id\n"
"    FROM side.region_tiles srt\n"
"    JOIN region_mapping rm ON srt.region_id = rm.side_region_id\n"
"    JOIN (SELECT t.id, st.id AS side_tile_id FROM side.tiles st\n"
"            JOIN tiles t ON st.url_template = t.url_template AND st.pixel_ratio = t.pixel_ratio AND st.z = t.z AND st.x = t.x AND st.y = t.y\n"
"    ) AS sti ON srt.tile_id = sti.side_tile_id;\n"
"REPLACE INTO resources\n"
"    SELECT r.id, \n"
"        sr.url, sr.kind, sr.expires, sr.modified, sr.etag,\n"
"        sr.data, sr.compressed, sr.accessed, sr.must_revalidate\n"
"    FROM side.region_resources srr JOIN side.resources sr ON srr.resource_id = sr.id\n"
"    LEFT JOIN resources r ON sr.url = r.url\n"
"        WHERE r.id IS NULL\n"
"        OR sr.modified > r.modified;\n"
"INSERT OR IGNORE INTO region_resources\n"
"  SELECT rm.main_region_id, sri.id\n"
"  FROM side.region_resources srr\n"
"  JOIN region_mapping rm ON srr.region_id = rm.side_region_id\n"
"  JOIN (SELECT r.id, sr.id AS side_resource_id FROM side.resources sr\n"
"          JOIN resources r ON sr.url = r.url) AS sri  ON srr.resource_id = sri.side_resource_id;\n"
" \n"
"DROP TABLE region_mapping;\n"
;

} // namespace mbgl
