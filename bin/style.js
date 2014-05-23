module.exports = {
    "version": "1",
    "buckets": {
        "landcover_snow": {
            "filter": {"source": "outdoors", "layer": "landcover", "class": "snow"},
            "fill": true
        },
        "landcover_crop": {
            "filter": {"source": "outdoors", "layer": "landcover", "class": "crop"},
            "fill": true
        },
        "landcover_grass": {
            "filter": {"source": "outdoors", "layer": "landcover", "class": "grass"},
            "fill": true
        },
        "landcover_scrub": {
            "filter": {"source": "outdoors", "layer": "landcover", "class": "scrub"},
            "fill": true
        },
        "landcover_wood": {
            "filter": {"source": "outdoors", "layer": "landcover", "class": "wood"},
            "fill": true
        },
        "landuse_wood": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "wood"},
            "fill": true
        },
        "landuse_school": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "school"},
            "fill": true
        },
        "landuse_sand": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "sand"},
            "fill": true
        },
        "landuse_pitch": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "pitch"},
            "fill": true
        },
        "landuse_park": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "park"},
            "fill": true
        },
        "landuse_industrial": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "industrial"},
            "fill": true
        },
        "landuse_scrub": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "scrub"},
            "fill": true
        },
        "landuse_grass": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "grass"},
            "fill": true
        },
        "landuse_crop": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "crop"},
            "fill": true
        },
        "landuse_rock": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "rock"},
            "fill": true
        },
        "landuse_snow": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "snow"},
            "fill": true
        },
        "landuse_hospital": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "hospital"},
            "fill": true
        },
        "landuse_cemetery": {
            "filter": {"source": "outdoors", "layer": "landuse", "class": "cemetery"},
            "fill": true
        },
        "overlay_wetland": {
            "filter": {"source": "outdoors", "layer": "landuse_overlay", "class": ["wetland", "wetland_noveg"]},
            "fill": true
        },
        "overlay_breakwater_pier": {
            "filter": {"source": "outdoors", "layer": "landuse_overlay", "class": ["breakwater", "pier"]},
            "fill": true
        },
        "waterway_river_canal": {
            "filter": {"source": "outdoors", "layer": "waterway", "type": ["river", "canal"]},
            "line": true,
            "line-cap": "round"
        },
        "waterway_stream": {
            "filter": {"source": "outdoors", "layer": "waterway", "type": "stream"},
            "line": true,
            "line-cap": "round"
        },
        "hillshade_full_highlight": {
            "filter": {"source": "outdoors", "layer": "hillshade", "class": "full_highlight"},
            "fill": true
        },
        "hillshade_medium_highlight": {
            "filter": {"source": "outdoors", "layer": "hillshade", "class": "medium_highlight"},
            "fill": true
        },
        "hillshade_medium_shadow": {
            "filter": {"source": "outdoors", "layer": "hillshade", "class": "medium_shadow"},
            "fill": true
        },
        "hillshade_full_shadow": {
            "filter": {"source": "outdoors", "layer": "hillshade", "class": "full_shadow"},
            "fill": true
        },
        "contour_line_loud": {
            "filter": {"source": "outdoors", "layer": "contour", "index": 5},
            "line": true,
            "line-join": "round"
        },
        "contour_line_regular": {
            "filter": {"source": "outdoors", "layer": "contour"},
            "line": true,
            "line-join": "round"
        },
        "barrier_line_gate": {
            "filter": {"source": "outdoors", "layer": "barrier_line", "class": "gate"},
            "line": true
        },
        "barrier_line_fence": {
            "filter": {"source": "outdoors", "layer": "barrier_line", "class": "fence"},
            "line": true
        },
        "barrier_line_hedge": {
            "filter": {"source": "outdoors", "layer": "barrier_line", "class": "hedge"},
            "line": true
        },
        "barrier_line_land": {
            "filter": {"source": "outdoors", "layer": "barrier_line", "class": "land"},
            "fill": true
        },
        "barrier_line_cliff": {
            "filter": {"source": "outdoors", "layer": "barrier_line", "class": "cliff"},
            "line": true
        },
        "water": {
            "filter": {"source": "outdoors", "layer": "water"},
            "fill": true
        },
        "aeroway_fill": {
            "filter": {"source": "outdoors", "layer": "aeroway"},
            "fill": true,
            "min-zoom": 12
        },
        "aeroway_runway": {
            "filter": {"source": "outdoors", "layer": "aeroway", "type": "runway"},
            "line": true,
            "min-zoom": 12
        },
        "aeroway_taxiway": {
            "filter": {"source": "outdoors", "layer": "aeroway", "type": "taxiway"},
            "line": true,
            "min-zoom": 12
        },
        "building": {
            "filter": {"source": "outdoors", "layer": "building"},
            "fill": true
        },
        "tunnel_motorway_link": {
            "filter": {"source": "outdoors", "layer": "tunnel", "class": "motorway_link"},
            "line": true
        },
        "tunnel_service": {
            "filter": {"source": "outdoors", "layer": "tunnel", "class": "service"},
            "line": true,
            "min-zoom": 15
        },
        "tunnel_main": {
            "filter": {"source": "outdoors", "layer": "tunnel", "class": "main"},
            "line": true
        },
        "tunnel_street": {
            "filter": {"source": "outdoors", "layer": "tunnel", "class": ["street", "street_limited"]},
            "line": true,
            "min-zoom": 12
        },
        "tunnel_motorway": {
            "filter": {"source": "outdoors", "layer": "tunnel", "class": "motorway"},
            "line": true
        },
        "road_path_case": {
            "filter": {"source": "outdoors", "layer": "road", "class": "path"},
            "line": true
        },
        "road_path_footway": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "footway"},
            "line": true
        },
        "road_path_path": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "path"},
            "line": true
        },
        "road_path_cycleway": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "cycleway"},
            "line": true
        },
        "road_path_mtb": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "mtb"},
            "line": true
        },
        "road_path_piste": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "piste"},
            "line": true
        },
        "road_path_steps": {
            "filter": {"source": "outdoors", "layer": "bridge", "type": "steps"},
            "line": true
        },
        "road_major_rail": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "major_rail"},
            "line": true
        },
        "road_motorway_link": {
            "filter": {"source": "outdoors", "layer": "road", "class": "motorway_link"},
            "line": true,
            "line-cap": "round",
            "line-join": "round"
        },
        "road_service": {
            "filter": {"source": "outdoors", "layer": "road", "class": "service"},
            "line": true,
            "min-zoom": 15,
            "line-cap": "round",
            "line-join": "round"
        },
        "road_main": {
            "filter": {"source": "outdoors", "layer": "road", "class": "main"},
            "line": true,
            "line-cap": "round",
            "line-join": "round"
        },
        "road_street": {
            "filter": {"source": "outdoors", "layer": "road", "class": ["street", "street_limited"]},
            "line": true,
            "min-zoom": 12,
            "line-cap": "round",
            "line-join": "round"
        },
        "road_motorway": {
            "filter": {"source": "outdoors", "layer": "road", "class": "motorway"},
            "line": true,
            "line-cap": "round",
            "line-join": "round"
        },
        "bridge_motorway_link": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "motorway_link"},
            "line": true
        },
        "bridge_service": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "service"},
            "line": true,
            "min-zoom": 15
        },
        "bridge_main": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "main"},
            "line": true
        },
        "bridge_street": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": ["street", "street_limited"]},
            "line": true,
            "min-zoom": 12
        },
        "bridge_motorway": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "motorway"},
            "line": true
        },
        "bridge_aerialway": {
            "filter": {"source": "outdoors", "layer": "bridge", "class": "aerialway"},
            "line": true
        },
        "admin_l3": {
            "filter": {"source": "outdoors", "layer": "admin", "admin_level": [3, 4, 5]},
            "line": true,
            "line-join": "round"
        },
        "admin_l2": {
            "filter": {"source": "outdoors", "layer": "admin", "admin_level": 2},
            "line": true,
            "line-cap": "round",
            "line-join": "round"
        },
        "admin_maritime": {
            "filter": {"source": "outdoors", "layer": "admin", "maritime": 1},
            "line": true,
            "line-cap": "round",
            "line-join": "round"
        },
        "country_label_line": {
            "filter": {"source": "outdoors", "layer": "country_label_line"},
            "line": true
        },
        "country_label": {
            "filter": {"source": "outdoors", "layer": "country_label", "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 24,
            "text-path": "horizontal"
        },
        "marine_label_line": {
            "filter": {"source": "outdoors", "layer": "marine_label", "feature_type": "line"},
            "text": true,
            "text-field": "name",
            "text-max-size": 16,
            "text-path": "curve"
        },
        "marine_label_point_1": {
            "filter": {"source": "outdoors", "layer": "marine_label", "labelrank": 1, "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 30,
            "text-path": "horizontal"
        },
        "marine_label_point_2": {
            "filter": {"source": "outdoors", "layer": "marine_label", "labelrank": 2, "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 24,
            "text-path": "horizontal"
        },
        "marine_label_point_other": {
            "filter": {
                "source": "outdoors",
                "layer": "marine_label",
                "labelrank": [3, 4, 5, 6],
                "feature_type": "point"
            },
            "text": true,
            "text-field": "name",
            "text-max-size": 18,
            "text-path": "horizontal"
        },
        "state_label": {
            "filter": {"source": "outdoors", "layer": "state_label", "feature_type": "point"},
            "text": true,
            "min-zoom": 4,
            "text-field": "name",
            "text-max-size": 16,
            "text-path": "horizontal"
        },
        "place_label_city_point": {
            "filter": {"source": "outdoors", "layer": "place_label", "type": "city"},
            "point": true,
            "point-size": [null, null]
        },
        "place_label_city": {
            "filter": {"source": "outdoors", "layer": "place_label", "type": "city", "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 20,
            "text-path": "horizontal"
        },
        "place_label_town": {
            "filter": {"source": "outdoors", "layer": "place_label", "type": "town", "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 24,
            "text-path": "horizontal"
        },
        "place_label_village": {
            "filter": {
                "source": "outdoors",
                "layer": "place_label",
                "type": "village",
                "feature_type": "point"
            },
            "text": true,
            "text-field": "name",
            "text-max-size": 22,
            "text-path": "horizontal"
        },
        "place_label_other": {
            "filter": {
                "source": "outdoors",
                "layer": "place_label",
                "type": ["hamlet", "suburb", "neighbourhood"],
                "feature_type": "point"
            },
            "text": true,
            "text-field": "name",
            "text-max-size": 14,
            "text-path": "horizontal"
        },
        "poi_label": {
            "filter": {
                "source": "outdoors",
                "layer": "poi_label",
                "scalerank": [1, 2],
                "feature_type": "point"
            },
            "text": true,
            "text-field": "name",
            "text-max-size": 12,
            "text-path": "horizontal",
            "text-padding": 2,
            "text-always-visible": true
        },
        "road_label": {
            "filter": {"source": "outdoors", "layer": "road_label", "feature_type": "line"},
            "text": true,
            "text-field": "name",
            "text-max-size": 13,
            "text-path": "curve",
            "text-padding": 2,
            "text-max-angle": 0.5
        },
        "contour_label": {
            "filter": {"source": "outdoors", "layer": "contour", "index": 10, "feature_type": "line"},
            "line": true
        },
        "water_label": {
            "filter": {"source": "outdoors", "layer": "water_label", "feature_type": "point"},
            "text": true,
            "text-field": "name",
            "text-max-size": 12,
            "text-path": "horizontal"
        },
        "waterway_label": {
            "filter": {"source": "outdoors", "layer": "waterway_label", "feature_type": "line"},
            "text": true,
            "text-field": "name",
            "text-max-size": 12,
            "text-path": "curve",
            "text-min-dist": 15
        },
        "poi": {
            "filter": {"source": "outdoors", "layer": "poi_label", "scalerank": [1, 2, 3]},
            "point": true,
            "point-size": [null, null]
        },
        "poi_aerodrome": {
            "filter": {"source": "outdoors", "layer": "poi_label", "maki": "airport"},
            "point": true,
            "point-size": [null, null]
        }
    },
    "layers": [{
               "id": "background"
               }, {
               "id": "landcover_snow",
               "bucket": "landcover_snow"
               }, {
               "id": "landcover_crop",
               "bucket": "landcover_crop"
               }, {
               "id": "landcover_grass",
               "bucket": "landcover_grass"
               }, {
               "id": "landcover_scrub",
               "bucket": "landcover_scrub"
               }, {
               "id": "landcover_wood",
               "bucket": "landcover_wood"
               }, {
               "id": "landuse_wood",
               "bucket": "landuse_wood"
               }, {
               "id": "landuse_school",
               "bucket": "landuse_school"
               }, {
               "id": "landuse_sand",
               "bucket": "landuse_sand"
               }, {
               "id": "landuse_pitch",
               "bucket": "landuse_pitch"
               }, {
               "id": "landuse_park",
               "bucket": "landuse_park"
               }, {
               "id": "landuse_industrial",
               "bucket": "landuse_industrial"
               }, {
               "id": "landuse_scrub",
               "bucket": "landuse_scrub"
               }, {
               "id": "landuse_grass",
               "bucket": "landuse_grass"
               }, {
               "id": "landuse_crop",
               "bucket": "landuse_crop"
               }, {
               "id": "landuse_rock",
               "bucket": "landuse_rock"
               }, {
               "id": "landuse_snow",
               "bucket": "landuse_snow"
               }, {
               "id": "landuse_hospital",
               "bucket": "landuse_hospital"
               }, {
               "id": "landuse_cemetery",
               "bucket": "landuse_cemetery"
               }, {
               "id": "overlay_wetland",
               "bucket": "overlay_wetland"
               }, {
               "id": "overlay_breakwater_pier",
               "bucket": "overlay_breakwater_pier"
               }, {
               "id": "waterway_river_canal",
               "bucket": "waterway_river_canal"
               }, {
               "id": "waterway_stream",
               "bucket": "waterway_stream"
               }, {
               "id": "hillshade_full_highlight",
               "bucket": "hillshade_full_highlight"
               }, {
               "id": "hillshade_medium_highlight",
               "bucket": "hillshade_medium_highlight"
               }, {
               "id": "hillshade_medium_shadow",
               "bucket": "hillshade_medium_shadow"
               }, {
               "id": "hillshade_full_shadow",
               "bucket": "hillshade_full_shadow"
               }, {
               "id": "contour_line_loud",
               "bucket": "contour_line_loud"
               }, {
               "id": "contour_line_loud",
               "bucket": "contour_line_loud"
               }, {
               "id": "contour_line_regular",
               "bucket": "contour_line_regular"
               }, {
               "id": "barrier_line_gate",
               "bucket": "barrier_line_gate"
               }, {
               "id": "barrier_line_fence",
               "bucket": "barrier_line_fence"
               }, {
               "id": "barrier_line_hedge",
               "bucket": "barrier_line_hedge"
               }, {
               "id": "barrier_line_land",
               "bucket": "barrier_line_land"
               }, {
               "id": "barrier_line_land_fill",
               "bucket": "barrier_line_land"
               }, {
               "id": "barrier_line_cliff",
               "bucket": "barrier_line_cliff"
               }, {
               "id": "water",
               "bucket": "water"
               }, {
               "id": "aeroway_fill",
               "bucket": "aeroway_fill"
               }, {
               "id": "aeroway_runway",
               "bucket": "aeroway_runway"
               }, {
               "id": "aeroway_taxiway",
               "bucket": "aeroway_taxiway"
               }, {
               "id": "building_shadow",
               "bucket": "building"
               }, {
               "id": "building",
               "bucket": "building"
               }, {
               "id": "building_wall",
               "bucket": "building"
               }, {
               "id": "tunnel_motorway_link_casing",
               "bucket": "tunnel_motorway_link"
               }, {
               "id": "tunnel_service_casing",
               "bucket": "tunnel_service"
               }, {
               "id": "tunnel_main_casing",
               "bucket": "tunnel_main"
               }, {
               "id": "tunnel_street_casing",
               "bucket": "tunnel_street"
               }, {
               "id": "tunnel_motorway_link",
               "bucket": "tunnel_motorway_link"
               }, {
               "id": "tunnel_service",
               "bucket": "tunnel_service"
               }, {
               "id": "tunnel_street",
               "bucket": "tunnel_street"
               }, {
               "id": "tunnel_main",
               "bucket": "tunnel_main"
               }, {
               "id": "tunnel_motorway_casing",
               "bucket": "tunnel_motorway"
               }, {
               "id": "tunnel_motorway",
               "bucket": "tunnel_motorway"
               }, {
               "id": "road_path_case",
               "bucket": "road_path_case"
               }, {
               "id": "road_path_footway",
               "bucket": "road_path_footway"
               }, {
               "id": "road_path_path",
               "bucket": "road_path_path"
               }, {
               "id": "road_path_cycleway",
               "bucket": "road_path_cycleway"
               }, {
               "id": "road_path_mtb",
               "bucket": "road_path_mtb"
               }, {
               "id": "road_path_piste",
               "bucket": "road_path_piste"
               }, {
               "id": "road_path_steps",
               "bucket": "road_path_steps"
               }, {
               "id": "road_major_rail",
               "bucket": "road_major_rail"
               }, {
               "id": "road_major_rail_hatching",
               "bucket": "road_major_rail"
               }, {
               "id": "road_motorway_link_casing",
               "bucket": "road_motorway_link"
               }, {
               "id": "road_service_casing",
               "bucket": "road_service"
               }, {
               "id": "road_main_casing",
               "bucket": "road_main"
               }, {
               "id": "road_street_casing",
               "bucket": "road_street"
               }, {
               "id": "road_motorway_link",
               "bucket": "road_motorway_link"
               }, {
               "id": "road_service",
               "bucket": "road_service"
               }, {
               "id": "road_street",
               "bucket": "road_street"
               }, {
               "id": "road_main",
               "bucket": "road_main"
               }, {
               "id": "road_motorway_casing",
               "bucket": "road_motorway"
               }, {
               "id": "road_motorway",
               "bucket": "road_motorway"
               }, {
               "id": "road_path_case",
               "bucket": "road_path_case"
               }, {
               "id": "road_path_footway",
               "bucket": "road_path_footway"
               }, {
               "id": "road_path_path",
               "bucket": "road_path_path"
               }, {
               "id": "road_path_cycleway",
               "bucket": "road_path_cycleway"
               }, {
               "id": "road_path_mtb",
               "bucket": "road_path_mtb"
               }, {
               "id": "road_path_piste",
               "bucket": "road_path_piste"
               }, {
               "id": "road_path_steps",
               "bucket": "road_path_steps"
               }, {
               "id": "road_major_rail",
               "bucket": "road_major_rail"
               }, {
               "id": "road_major_rail_hatching",
               "bucket": "road_major_rail"
               }, {
               "id": "bridge_motorway_link_casing",
               "bucket": "bridge_motorway_link"
               }, {
               "id": "bridge_service_casing",
               "bucket": "bridge_service"
               }, {
               "id": "bridge_main_casing",
               "bucket": "bridge_main"
               }, {
               "id": "bridge_street_casing",
               "bucket": "bridge_street"
               }, {
               "id": "bridge_motorway_link",
               "bucket": "bridge_motorway_link"
               }, {
               "id": "bridge_service",
               "bucket": "bridge_service"
               }, {
               "id": "bridge_street",
               "bucket": "bridge_street"
               }, {
               "id": "bridge_main",
               "bucket": "bridge_main"
               }, {
               "id": "bridge_motorway_casing",
               "bucket": "bridge_motorway"
               }, {
               "id": "bridge_motorway",
               "bucket": "bridge_motorway"
               }, {
               "id": "road_path_footway",
               "bucket": "road_path_footway"
               }, {
               "id": "road_path_path",
               "bucket": "road_path_path"
               }, {
               "id": "road_path_cycleway",
               "bucket": "road_path_cycleway"
               }, {
               "id": "road_path_mtb",
               "bucket": "road_path_mtb"
               }, {
               "id": "road_path_piste",
               "bucket": "road_path_piste"
               }, {
               "id": "road_path_steps",
               "bucket": "road_path_steps"
               }, {
               "id": "bridge_aerialway_casing",
               "bucket": "bridge_aerialway"
               }, {
               "id": "bridge_aerialway",
               "bucket": "bridge_aerialway"
               }, {
               "id": "road_major_rail",
               "bucket": "road_major_rail"
               }, {
               "id": "road_major_rail_hatching",
               "bucket": "road_major_rail"
               }, {
               "id": "admin_l3",
               "bucket": "admin_l3"
               }, {
               "id": "admin_l2",
               "bucket": "admin_l2"
               }, {
               "id": "admin_maritime_cover",
               "bucket": "admin_maritime"
               }, {
               "id": "admin_maritime",
               "bucket": "admin_maritime"
               }, {
               "id": "country_label_line",
               "bucket": "country_label_line"
               }, {
               "id": "country_label",
               "bucket": "country_label"
               }, {
               "id": "marine_label_line",
               "bucket": "marine_label_line"
               }, {
               "id": "marine_label_point_1",
               "bucket": "marine_label_point_1"
               }, {
               "id": "marine_label_point_2",
               "bucket": "marine_label_point_2"
               }, {
               "id": "marine_label_point_other",
               "bucket": "marine_label_point_other"
               }, {
               "id": "state_label",
               "bucket": "state_label"
               }, {
               "id": "place_label_city_point",
               "bucket": "place_label_city_point"
               }, {
               "id": "place_label_city",
               "bucket": "place_label_city"
               }, {
               "id": "place_label_town",
               "bucket": "place_label_town"
               }, {
               "id": "place_label_village",
               "bucket": "place_label_village"
               }, {
               "id": "place_label_other",
               "bucket": "place_label_other"
               }, {
               "id": "poi_label",
               "bucket": "poi_label"
               }, {
               "id": "road_label",
               "bucket": "road_label"
               }, {
               "id": "contour_label",
               "bucket": "contour_label"
               }, {
               "id": "water_label",
               "bucket": "water_label"
               }, {
               "id": "waterway_label",
               "bucket": "waterway_label"
               }, {
               "id": "poi",
               "bucket": "poi"
               }, {
               "id": "poi_aerodrome",
               "bucket": "poi_aerodrome"
               }],
    "constants": {
        "land": "rgb(244,239,225)",
        "water": "#cdd",
        "water_dark": "#185869",
        "crop": "#eeeed4",
        "grass": "#e6e6cc",
        "scrub": "#dfe5c8",
        "wood": "#cee2bd",
        "snow": "#f4f8ff",
        "rock": "#ddd",
        "sand": "#ffd",
        "cemetery": "#edf4ed",
        "pitch": "#fff",
        "park": "#d4e4bc",
        "piste": "blue",
        "school": "#e8dfe0",
        "hospital": "#f8eee0",
        "builtup": "#f6faff",
        "case": "#fff",
        "motorway": "#cda0a0",
        "main": "#ddc0b9",
        "street": "#fff",
        "text": "#666",
        "text_stroke": "rgba(255,255,255,0.8)",
        "country_text": "#333",
        "marine_text": "#a0bdc0",
        "water_text": "#185869",
        "land_night": "#017293",
        "water_night": "#103",
        "water_dark_night": "#003366",
        "crop_night": "#178d96",
        "grass_night": "#23948a",
        "scrub_night": "#31a186",
        "wood_night": "#45b581",
        "park_night": "#51bd8b",
        "snow_night": "#5ad9fe",
        "rock_night": "#999",
        "sand_night": "#437162",
        "cemetery_night": "#218c96",
        "pitch_night": "rgba(255,255,255,0.2)",
        "school_night": "#01536a",
        "hospital_night": "#015e7a",
        "builtup_night": "#014b60",
        "admin_night": "#ffb680",
        "text_night": "#fff",
        "text_stroke_night": "#103",
        "case_night": "#015e7a",
        "street_case_night": "#015b76",
        "motorway_night": "#eee",
        "main_night": "#64b2c9",
        "street_night": "#0186ac",
        "contour_night": "#ffff80",
        "river_canal_width": ["stops", {
                              "z": 11,
                              "val": 0.5
                              }, {
                              "z": 12,
                              "val": 1
                              }, {
                              "z": 14,
                              "val": 2
                              }, {
                              "z": 16,
                              "val": 3
                              }],
        "stream_width": ["stops", {
                         "z": 13,
                         "val": 0.25
                         }, {
                         "z": 14,
                         "val": 0.5
                         }, {
                         "z": 16,
                         "val": 1.5
                         }, {
                         "z": 18,
                         "val": 2
                         }],
        "motorway_width": ["stops", {
                           "z": 5,
                           "val": 0
                           }, {
                           "z": 6,
                           "val": 0.5
                           }, {
                           "z": 8,
                           "val": 0.8
                           }, {
                           "z": 10,
                           "val": 1
                           }, {
                           "z": 11,
                           "val": 1.2
                           }, {
                           "z": 12,
                           "val": 2
                           }, {
                           "z": 13,
                           "val": 3
                           }, {
                           "z": 14,
                           "val": 4
                           }, {
                           "z": 15,
                           "val": 6
                           }, {
                           "z": 16,
                           "val": 9
                           }, {
                           "z": 17,
                           "val": 12
                           }, {
                           "z": 18,
                           "val": 14
                           }],
        "motorway_casing_width": ["stops", {
                                  "z": 7.5,
                                  "val": 0.6
                                  }, {
                                  "z": 8,
                                  "val": 0.8
                                  }, {
                                  "z": 10,
                                  "val": 2.8
                                  }, {
                                  "z": 11,
                                  "val": 3
                                  }, {
                                  "z": 12,
                                  "val": 4
                                  }, {
                                  "z": 13,
                                  "val": 5
                                  }, {
                                  "z": 14,
                                  "val": 6.5
                                  }, {
                                  "z": 15,
                                  "val": 9
                                  }, {
                                  "z": 16,
                                  "val": 12
                                  }, {
                                  "z": 17,
                                  "val": 15
                                  }, {
                                  "z": 18,
                                  "val": 17
                                  }],
        "motorway_link_width": ["stops", {
                                "z": 12,
                                "val": 1.2
                                }, {
                                "z": 14,
                                "val": 2
                                }, {
                                "z": 16,
                                "val": 3
                                }, {
                                "z": 18,
                                "val": 4
                                }],
        "motorway_link_casing_width": ["stops", {
                                       "z": 12,
                                       "val": 2.8
                                       }, {
                                       "z": 14,
                                       "val": 3.5
                                       }, {
                                       "z": 16,
                                       "val": 5
                                       }, {
                                       "z": 18,
                                       "val": 6
                                       }],
        "main_width": ["stops", {
                       "z": 5,
                       "val": 1
                       }, {
                       "z": 12,
                       "val": 1
                       }, {
                       "z": 13,
                       "val": 1.5
                       }, {
                       "z": 14,
                       "val": 2
                       }, {
                       "z": 15,
                       "val": 3
                       }, {
                       "z": 16,
                       "val": 6
                       }, {
                       "z": 17,
                       "val": 10
                       }, {
                       "z": 18,
                       "val": 12
                       }],
        "main_casing_width": ["stops", {
                              "z": 9,
                              "val": 2.9
                              }, {
                              "z": 12,
                              "val": 2.9
                              }, {
                              "z": 13,
                              "val": 3.5
                              }, {
                              "z": 14,
                              "val": 4
                              }, {
                              "z": 15,
                              "val": 5.5
                              }, {
                              "z": 16,
                              "val": 9
                              }, {
                              "z": 17,
                              "val": 12
                              }, {
                              "z": 18,
                              "val": 14
                              }],
        "street_width": ["stops", {
                         "z": 14.5,
                         "val": 0
                         }, {
                         "z": 15,
                         "val": 1.5
                         }, {
                         "z": 16,
                         "val": 3
                         }, {
                         "z": 17,
                         "val": 8
                         }],
        "street_casing_width": ["stops", {
                                "z": 13,
                                "val": 0.4
                                }, {
                                "z": 14,
                                "val": 1
                                }, {
                                "z": 15,
                                "val": 2.5
                                }, {
                                "z": 16,
                                "val": 4
                                }, {
                                "z": 17,
                                "val": 10
                                }],
        "service_casing_width": ["stops", {
                                 "z": 14,
                                 "val": 0.5
                                 }, {
                                 "z": 15,
                                 "val": 3
                                 }, {
                                 "z": 16,
                                 "val": 3.5
                                 }, {
                                 "z": 17,
                                 "val": 4
                                 }, {
                                 "z": 18,
                                 "val": 5
                                 }, {
                                 "z": 19,
                                 "val": 6
                                 }],
        "runway_width": ["stops", {
                         "z": 10,
                         "val": 1
                         }, {
                         "z": 11,
                         "val": 2
                         }, {
                         "z": 12,
                         "val": 3
                         }, {
                         "z": 13,
                         "val": 5
                         }, {
                         "z": 14,
                         "val": 7
                         }, {
                         "z": 15,
                         "val": 11
                         }, {
                         "z": 16,
                         "val": 15
                         }, {
                         "z": 17,
                         "val": 19
                         }, {
                         "z": 18,
                         "val": 23
                         }],
        "taxiway_width": ["stops", {
                          "z": 10,
                          "val": 0.2
                          }, {
                          "z": 12,
                          "val": 0.2
                          }, {
                          "z": 13,
                          "val": 1
                          }, {
                          "z": 14,
                          "val": 1.5
                          }, {
                          "z": 15,
                          "val": 2
                          }, {
                          "z": 16,
                          "val": 3
                          }, {
                          "z": 17,
                          "val": 4
                          }, {
                          "z": 18,
                          "val": 5
                          }],
        "aerialway_width": ["stops", {
                            "z": 13.5,
                            "val": 0.8
                            }, {
                            "z": 14,
                            "val": 1.4
                            }, {
                            "z": 15,
                            "val": 1.6
                            }, {
                            "z": 16,
                            "val": 2
                            }, {
                            "z": 17,
                            "val": 2.4
                            }, {
                            "z": 18,
                            "val": 3
                            }],
        "aerialway_casing_width": ["stops", {
                                   "z": 13.5,
                                   "val": 2
                                   }, {
                                   "z": 14,
                                   "val": 2.5
                                   }, {
                                   "z": 15,
                                   "val": 3
                                   }, {
                                   "z": 16,
                                   "val": 3.5
                                   }, {
                                   "z": 17,
                                   "val": 4
                                   }, {
                                   "z": 22,
                                   "val": 5
                                   }],
        "path_width": ["stops", {
                       "z": 14,
                       "val": 1.2
                       }, {
                       "z": 15,
                       "val": 1.5
                       }, {
                       "z": 16,
                       "val": 1.8
                       }],
        "admin_l2_width": ["stops", {
                           "z": 2,
                           "val": 0.5
                           }, {
                           "z": 3,
                           "val": 0.7
                           }, {
                           "z": 4,
                           "val": 0.7
                           }, {
                           "z": 5,
                           "val": 0.8
                           }, {
                           "z": 6,
                           "val": 1
                           }, {
                           "z": 8,
                           "val": 2
                           }, {
                           "z": 10,
                           "val": 3
                           }],
        "admin_l3_width": ["stops", {
                           "z": 6,
                           "val": 0.6
                           }, {
                           "z": 8,
                           "val": 1
                           }, {
                           "z": 12,
                           "val": 2
                           }],
        "road_label_size": ["stops", {
                            "z": 0,
                            "val": 12
                            }, {
                            "z": 14,
                            "val": 12
                            }, {
                            "z": 15,
                            "val": 13
                            }, {
                            "z": 22,
                            "val": 13
                            }],
        "fence_width": ["stops", {
                        "z": 17,
                        "val": 0.6
                        }, {
                        "z": 19,
                        "val": 1
                        }],
        "hedge_width": ["stops", {
                        "z": 16,
                        "val": 0.6
                        }, {
                        "z": 17,
                        "val": 1.2
                        }, {
                        "z": 19,
                        "val": 1.6
                        }],
        "barrier_line_land_width": ["stops", {
                                    "z": 14,
                                    "val": 0.4
                                    }, {
                                    "z": 15,
                                    "val": 0.75
                                    }, {
                                    "z": 16,
                                    "val": 1.5
                                    }, {
                                    "z": 17,
                                    "val": 3
                                    }, {
                                    "z": 18,
                                    "val": 6
                                    }, {
                                    "z": 19,
                                    "val": 12
                                    }, {
                                    "z": 20,
                                    "val": 24
                                    }, {
                                    "z": 21,
                                    "val": 48
                                    }]
    },
    "styles": {
        "default": {
            "background": {
                "fill-color": "land"
            },
            "admin_maritime_cover": {
                "line-color": "water",
                "line-width": 5
            },
            "admin_maritime": {
                "line-color": "#c0d6d6",
                "line-width": {
                    "fn": "stops",
                    "stops": [[6, 1], [8, 2], [12, 3]]
                }
            },
            "admin_l2": {
                "line-color": "#88a",
                "line-width": "admin_l2_width"
            },
            "admin_l3": {
                "line-color": "#88a",
                "line-dasharray": [60, 20],
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[4, 0], [6, 1]]
                },
                "line-width": "admin_l3_width"
            },
            "waterway_river_canal": {
                "line-color": "#87abaf",
                "line-width": "river_canal_width"
            },
            "waterway_stream": {
                "line-color": "#87abaf",
                "line-width": "stream_width"
            },
            "barrier_line_gate": {
                "line-width": 2.5,
                "line-color": "#aab"
            },
            "barrier_line_fence": {
                "line-color": "#aeada3",
                "line-width": "fence_width"
            },
            "barrier_line_hedge": {
                "line-color": "#8de99b",
                "line-width": "hedge_width"
            },
            "barrier_line_land": {
                "line-color": "land",
                "line-width": "barrier_line_land_width"
            },
            "barrier_line_land_fill": {
                "fill-color": "land"
            },
            "barrier_line_cliff": {
                "line-color": "#987",
                "line-width": 4
            },
            "landcover_wood": {
                "fill-color": "wood",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_scrub": {
                "fill-color": "scrub",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_grass": {
                "fill-color": "grass",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_crop": {
                "fill-color": "crop"
            },
            "landcover_snow": {
                "fill-color": "snow"
            },
            "landuse_wood": {
                "fill-color": "wood"
            },
            "landuse_scrub": {
                "fill-color": "scrub"
            },
            "landuse_grass": {
                "fill-color": "grass"
            },
            "landuse_crop": {
                "fill-color": "crop"
            },
            "landuse_snow": {
                "fill-color": "snow"
            },
            "landuse_rock": {
                "fill-color": "rock"
            },
            "landuse_sand": {
                "fill-color": "sand"
            },
            "landuse_park": {
                "fill-color": "park"
            },
            "landuse_cemetery": {
                "fill-color": "cemetery"
            },
            "landuse_hospital": {
                "fill-color": "hospital"
            },
            "landuse_school": {
                "fill-color": "school"
            },
            "landuse_pitch": {
                "fill-color": "rgba(255,255,255,0.5)",
                "stroke-color": "pitch"
            },
            "landuse_industrial": {
                "fill-color": "rgba(246,250,255,0.5)"
            },
            "overlay_wetland": {
                "fill-color": "rgba(210,225,225,0.2)",
                "fill-image": "wetland_noveg_64"
            },
            "overlay_breakwater_pier": {
                "fill-color": "land"
            },
            "hillshade_full_shadow": {
                "fill-color": "#103",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[15, 0.075], [17, 0.05], [18, 0.025]]
                }
            },
            "hillshade_medium_shadow": {
                "fill-color": "#206",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[15, 0.075], [17, 0.05], [18, 0.025]]
                }
            },
            "hillshade_full_highlight": {
                "fill-color": "#fffff3",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[15, 0.3], [17, 0.2], [18, 0.1]]
                }
            },
            "hillshade_medium_highlight": {
                "fill-color": "#ffd",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[15, 0.3], [17, 0.2], [18, 0.1]]
                }
            },
            "contour_line_loud": {
                "line-color": "#008",
                "line-width": 0.9,
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[12, 0.05], [13, 0.11]]
                }
            },
            "contour_line_regular": {
                "line-color": "#008",
                "line-width": 0.5,
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[12, 0.05], [13, 0.11]]
                }
            },
            "water": {
                "fill-color": "water",
                "stroke-color": "#a2bdc0"
            },
            "aeroway_fill": {
                "fill-color": "#ddd"
            },
            "aeroway_runway": {
                "line-color": "#ddd",
                "line-width": "runway_width"
            },
            "aeroway_taxiway": {
                "line-color": "#ddd",
                "line-width": "taxiway_width"
            },
            "building": {
                "fill-color": "#ebe7db"
            },
            "building_wall": {
                "fill-color": "#ebe7db",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[16.5, 0], [17, 0.7]]
                },
                "stroke-color": "#d5d1c6"
            },
            "building_shadow": {
                "fill-color": "#d5d1c6",
                "fill-translate": [1, 1],
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[16.5, 0], [17, 1]]
                },
                "stroke-color": "#d5d1c6"
            },
            "tunnel_motorway_casing": {
                "line-color": "case",
                "line-dasharray": [6, 6],
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "tunnel_motorway": {
                "line-color": "#e6cec7",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "tunnel_main_casing": {
                "line-color": "case",
                "line-dasharray": [6, 6],
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "tunnel_main": {
                "line-color": "#e6cec7",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "tunnel_motorway_link_casing": {
                "line-color": "case",
                "line-dasharray": [6, 6],
                "line-width": "motorway_link_casing_width"
            },
            "tunnel_motorway_link": {
                "line-color": "#e6cec7",
                "line-width": "motorway_link_width"
            },
            "tunnel_street_casing": {
                "line-color": "#d9d5c6",
                "line-width": "street_casing_width"
            },
            "tunnel_street": {
                "line-color": "#d9d5c6",
                "line-width": "street_width"
            },
            "tunnel_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-dasharray": [6, 6],
                "line-width": "service_casing_width"
            },
            "tunnel_service": {
                "line-color": "#e6cec7",
                "line-width": 2
            },
            "road_motorway_casing": {
                "line-color": "case",
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "road_motorway": {
                "line-color": "motorway",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "road_main_casing": {
                "line-color": "case",
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "road_main": {
                "line-color": "main",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "road_motorway_link_casing": {
                "line-color": "case",
                "line-width": "motorway_link_casing_width"
            },
            "road_motorway_link": {
                "line-color": "motorway",
                "line-width": "motorway_link_width"
            },
            "road_street_casing": {
                "line-color": "#d9d5c6",
                "line-width": "street_casing_width"
            },
            "road_street": {
                "line-color": "street",
                "line-width": "street_width"
            },
            "road_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-width": "service_casing_width"
            },
            "road_service": {
                "line-color": "street",
                "line-width": 2
            },
            "road_path_case": {
                "line-color": "#ffd",
                "line-opacity": 0.4,
                "line-width": {
                    "fn": "stops",
                    "stops": [[15, 3], [16, 4]]
                }
            },
            "road_path_footway": {
                "line-color": "#bba",
                "line-dasharray": [10, 4],
                "line-width": "path_width"
            },
            "road_path_path": {
                "line-color": "#987",
                "line-dasharray": [10, 4],
                "line-opacity": 0.8,
                "line-width": {
                    "fn": "stops",
                    "stops": [[14, 0.8], [15, 0.9], [16, 1.2]]
                }
            },
            "road_path_cycleway": {
                "line-color": "#488",
                "line-dasharray": [10, 4],
                "line-width": "path_width"
            },
            "road_path_mtb": {
                "line-color": "#488",
                "line-dasharray": [12, 4],
                "line-width": "path_width"
            },
            "road_path_piste": {
                "line-color": "#87b",
                "line-dasharray": [8, 4],
                "line-width": "path_width"
            },
            "road_path_steps": {
                "line-color": "#bba",
                "line-dasharray": [10, 4],
                "line-width": 4
            },
            "road_major_rail": {
                "line-color": "#c8c4c0",
                "line-width": 0.8
            },
            "road_major_rail_hatching": {
                "line-color": "#c8c4c0",
                "line-dasharray": [2, 31],
                "line-width": 5
            },
            "bridge_motorway_casing": {
                "line-color": "case",
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "bridge_motorway": {
                "line-color": "motorway",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "bridge_main_casing": {
                "line-color": "case",
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "bridge_main": {
                "line-color": "main",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "bridge_motorway_link_casing": {
                "line-color": "case",
                "line-width": "motorway_link_casing_width"
            },
            "bridge_motorway_link": {
                "line-color": "motorway",
                "line-width": "motorway_link_width"
            },
            "bridge_street_casing": {
                "line-color": "#d9d5c6",
                "line-width": "street_casing_width"
            },
            "bridge_street": {
                "line-color": "street",
                "line-width": "street_width"
            },
            "bridge_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-width": "service_casing_width"
            },
            "bridge_service": {
                "line-color": "street",
                "line-width": 2
            },
            "bridge_aerialway_casing": {
                "line-color": "white",
                "line-opacity": 0.5,
                "line-width": "aerialway_casing_width"
            },
            "bridge_aerialway": {
                "line-color": "#876",
                "line-opacity": 0.5,
                "line-width": "aerialway_width"
            },
            "country_label": {
                "text-color": "country_text",
                "text-halo-color": "rgba(255,255,255,0.5)",
                "text-size": {
                    "fn": "stops",
                    "stops": [[6, 14], [12, 24]]
                }
            },
            "country_label_line": {
                "line-color": "country_text",
                "line-width": 0.5,
                "line-opacity": 0.5
            },
            "marine_label_line": {
                "text-color": "marine_text",
                "text-halo-color": "water"
            },
            "marine_label_point_1": {
                "text-color": "#a9c4c7",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 20], [3, 20], [4, 25], [5, 30], [22, 30]]
                },
                "text-halo-color": "water"
            },
            "marine_label_point_2": {
                "text-color": "#a9c4c7",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 13], [3, 13], [4, 14], [5, 20], [6, 24], [22, 24]]
                },
                "text-halo-color": "water"
            },
            "marine_label_point_other": {
                "text-color": "#a9c4c7",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 12], [3, 12], [4, 13], [5, 15], [6, 18], [22, 18]]
                },
                "text-halo-color": "water"
            },
            "state_label": {
                "text-color": "#333",
                "text-halo-width": 0.4,
                "text-halo-color": "rgba(244,239,225,0.8)",
                "text-size": {
                    "fn": "stops",
                    "stops": [[3.99, 0], [4, 10], [9.99, 16], [10, 0]]
                }
            },
            "place_label_city_point": {
                "point-color": "#4a4032"
            },
            "place_label_city": {
                "text-color": "#444",
                "text-halo-width": 0.4,
                "text-halo-color": "text_stroke",
                "text-size": {
                    "fn": "stops",
                    "stops": [[3.99, 0], [4, 10], [7, 14], [14.99, 20], [15, 0]]
                },
                "text-translate": [0, ["stops", {
                                       "z": 4,
                                       "val": 10
                                       }, {
                                       "z": 6,
                                       "val": 30
                                       }, {
                                       "z": 7,
                                       "val": 0
                                       }]]
            },
            "place_label_town": {
                "text-color": "#716656",
                "text-halo-width": 0.5,
                "text-halo-color": "text_stroke",
                "text-size": {
                    "fn": "stops",
                    "stops": [[9, 10], [12, 13], [14, 17], [16, 22]]
                }
            },
            "place_label_village": {
                "text-color": "#635644",
                "text-halo-width": 0.5,
                "text-halo-color": "text_stroke",
                "text-size": {
                    "fn": "stops",
                    "stops": [[9, 8], [12, 10], [14, 14], [16, 16], [17, 20]]
                }
            },
            "place_label_other": {
                "text-color": "#7d6c55",
                "text-halo-color": "text_stroke",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 10], [14, 11], [15, 12], [16, 14]]
                }
            },
            "road_label": {
                "text-color": "#585042",
                "text-halo-color": "land",
                "text-halo-width": 0.6,
                "text-size": "road_label_size"
            },
            "water_label": {
                "text-color": "water_dark",
                "text-halo-color": "rgba(255,255,255,0.75)"
            },
            "waterway_label": {
                "text-color": "water_dark",
                "text-halo-width": 0.4,
                "text-halo-color": "text_stroke"
            },
            "poi": {
                "point-color": "rgba(50,50,50,0.9)",
                "point-antialias": false
            },
            "poi_label": {
                "text-color": "#444",
                "text-size": 10,
                "text-halo-color": "land",
                "text-halo-width": 0.6,
                "text-translate": [0, 14]
            },
            "poi_aerodrome": {
                "point-opacity": {
                    "fn": "stops",
                    "stops": [[13, 0], [13.25, 1]]
                },
                "point-antialias": false
            }
        },
        "satellite": {
            "background": {
                "fill-color": "land_night"
            },
            "admin_maritime_cover": {
                "line-color": "#0a1347",
                "line-width": 5
            },
            "admin_maritime": {
                "line-color": "#0a1347",
                "line-width": {
                    "fn": "stops",
                    "stops": [[6, 1], [8, 2], [12, 3]]
                }
            },
            "admin_l2": {
                "line-color": "admin_night",
                "line-width": "admin_l2_width"
            },
            "admin_l3": {
                "line-color": "admin_night",
                "line-dasharray": [60, 20],
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[4, 0], [6, 1]]
                },
                "line-width": "admin_l3_width"
            },
            "waterway_river_canal": {
                "line-color": "rgb(10,20,71)",
                "line-width": "river_canal_width"
            },
            "waterway_stream": {
                "line-color": "rgb(10,20,71)",
                "line-width": "stream_width"
            },
            "barrier_line_gate": {
                "line-width": 2.5,
                "line-color": "#59596f"
            },
            "barrier_line_fence": {
                "line-color": "#014b61",
                "line-width": "fence_width"
            },
            "barrier_line_hedge": {
                "line-color": "#2e7a57",
                "line-width": "hedge_width"
            },
            "barrier_line_land": {
                "line-color": "land_night",
                "line-width": "barrier_line_land_width"
            },
            "barrier_line_land_fill": {
                "fill-color": "land_night"
            },
            "barrier_line_cliff": {
                "line-color": "#63574b",
                "line-width": 4
            },
            "landcover_wood": {
                "fill-color": "wood_night",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_scrub": {
                "fill-color": "scrub_night",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_grass": {
                "fill-color": "grass_night",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[13, 1], [14, 0.8], [17, 0.2]]
                }
            },
            "landcover_crop": {
                "fill-color": "crop_night"
            },
            "landcover_snow": {
                "fill-color": "snow_night"
            },
            "landuse_wood": {
                "fill-color": "wood_night",
                "fill-opacity": 0.8
            },
            "landuse_scrub": {
                "fill-color": "scrub_night",
                "fill-opacity": 0.8
            },
            "landuse_grass": {
                "fill-color": "grass_night",
                "fill-opacity": 0.8
            },
            "landuse_crop": {
                "fill-color": "crop_night",
                "fill-opacity": 0.8
            },
            "landuse_snow": {
                "fill-color": "snow_night",
                "fill-opacity": 0.8
            },
            "landuse_rock": {
                "fill-color": "rock_night",
                "fill-opacity": 0.8
            },
            "landuse_sand": {
                "fill-color": "sand_night",
                "fill-opacity": 0.8
            },
            "landuse_park": {
                "fill-color": "park_night"
            },
            "landuse_cemetery": {
                "fill-color": "cemetery_night"
            },
            "landuse_hospital": {
                "fill-color": "hospital_night"
            },
            "landuse_school": {
                "fill-color": "school_night"
            },
            "landuse_pitch": {
                "fill-color": "pitch_night",
                "stroke-color": "pitch"
            },
            "landuse_industrial": {
                "fill-color": "builtup_night"
            },
            "overlay_wetland": {
                "fill-color": "rgba(210,225,225,0.2)",
                "fill-image": "wetland_noveg_64"
            },
            "overlay_breakwater_pier": {
                "fill-color": "land_night"
            },
            "hillshade_full_shadow": {
                "fill-color": "#103",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[0, 0.3], [17, 0.2], [18, 0.1]]
                }
            },
            "hillshade_medium_shadow": {
                "fill-color": "#206",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[0, 0.3], [17, 0.2], [18, 0.1]]
                }
            },
            "hillshade_full_highlight": {
                "fill-color": "#fdfdad",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[14, 0.4], [15, 0.3], [17, 0.2], [18, 0.1]]
                }
            },
            "hillshade_medium_highlight": {
                "fill-color": "#ffe1b7",
                "fill-antialias": false,
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[15, 0.3], [17, 0.2], [18, 0.15]]
                }
            },
            "contour_line_loud": {
                "line-color": "contour_night",
                "line-width": 0.9,
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[12, 0.1], [13, 0.2]]
                }
            },
            "contour_line_regular": {
                "line-color": "contour_night",
                "line-width": 0.5,
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[12, 0.1], [13, 0.4]]
                }
            },
            "water": {
                "fill-color": "water_night",
                "stroke-color": "water_dark_night"
            },
            "aeroway_fill": {
                "fill-color": "#367"
            },
            "aeroway_runway": {
                "line-color": "#367",
                "line-width": "runway_width"
            },
            "aeroway_taxiway": {
                "line-color": "#367",
                "line-width": "taxiway_width"
            },
            "building": {
                "fill-color": "#027797"
            },
            "building_wall": {
                "fill-color": "#027797",
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[16.5, 0], [17, 0.7]]
                },
                "stroke-color": "#026688"
            },
            "building_shadow": {
                "fill-color": "#026688",
                "fill-translate": [1, 1],
                "fill-opacity": {
                    "fn": "stops",
                    "stops": [[16.5, 0], [17, 1]]
                },
                "stroke-color": "#026688"
            },
            "tunnel_motorway_casing": {
                "line-color": "case_night",
                "line-dasharray": [6, 6],
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "tunnel_motorway": {
                "line-color": "#78b0c1",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "tunnel_main_casing": {
                "line-color": "case_night",
                "line-dasharray": [6, 6],
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "tunnel_main": {
                "line-color": "#78b0c1",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "tunnel_motorway_link_casing": {
                "line-color": "case_night",
                "line-dasharray": [6, 6],
                "line-width": "motorway_link_casing_width"
            },
            "tunnel_motorway_link": {
                "line-color": "#78b0c1",
                "line-width": "motorway_link_width"
            },
            "tunnel_street_casing": {
                "line-color": "street_case_night",
                "line-width": "street_casing_width"
            },
            "tunnel_street": {
                "line-color": "street_night",
                "line-width": "street_width"
            },
            "tunnel_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-dasharray": [6, 6],
                "line-width": "service_casing_width"
            },
            "tunnel_service": {
                "line-color": "#017ca0",
                "line-width": 2
            },
            "road_motorway_casing": {
                "line-color": "case_night",
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "road_motorway": {
                "line-color": "motorway_night",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "road_main_casing": {
                "line-color": "case_night",
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "road_main": {
                "line-color": "main_night",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "road_motorway_link_casing": {
                "line-color": "case_night",
                "line-width": "motorway_link_casing_width"
            },
            "road_motorway_link": {
                "line-color": "motorway_night",
                "line-width": "motorway_link_width"
            },
            "road_street_casing": {
                "line-color": "street_case_night",
                "line-width": "street_casing_width"
            },
            "road_street": {
                "line-color": "street_night",
                "line-width": "street_width"
            },
            "road_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-width": "service_casing_width"
            },
            "road_service": {
                "line-color": "street_night",
                "line-width": 2
            },
            "road_path_case": {
                "line-color": "land_night",
                "line-opacity": 0.2
            },
            "road_path_footway": {
                "line-color": "#fff",
                "line-dasharray": [10, 4],
                "line-width": "path_width"
            },
            "road_path_path": {
                "line-color": "#fff",
                "line-dasharray": [10, 4],
                "line-opacity": 0.8,
                "line-width": {
                    "fn": "stops",
                    "stops": [[14, 0.8], [15, 0.9], [16, 1.2]]
                }
            },
            "road_path_cycleway": {
                "line-color": "#94e6ff",
                "line-dasharray": [10, 4],
                "line-width": "path_width"
            },
            "road_path_mtb": {
                "line-color": "#94e6ff",
                "line-dasharray": [12, 4],
                "line-width": "path_width"
            },
            "road_path_piste": {
                "line-color": "#715dae",
                "line-dasharray": [8, 4],
                "line-width": "path_width"
            },
            "road_path_steps": {
                "line-color": "#016684",
                "line-dasharray": [10, 4],
                "line-opacity": 0.3,
                "line-width": 6
            },
            "road_major_rail": {
                "line-color": "#c8c4c0",
                "line-width": 0.8
            },
            "road_major_rail_hatching": {
                "line-color": "#c8c4c0",
                "line-dasharray": [2, 31],
                "line-width": 5
            },
            "bridge_motorway_casing": {
                "line-color": "case_night",
                "line-width": "motorway_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9.5, 0], [10, 1]]
                }
            },
            "bridge_motorway": {
                "line-color": "motorway_night",
                "line-width": "motorway_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "bridge_main_casing": {
                "line-color": "case_night",
                "line-width": "main_casing_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[9, 0], [10, 1]]
                }
            },
            "bridge_main": {
                "line-color": "main_night",
                "line-width": "main_width",
                "line-opacity": {
                    "fn": "stops",
                    "stops": [[6.5, 0], [7, 1]]
                }
            },
            "bridge_motorway_link_casing": {
                "line-color": "case_night",
                "line-width": "motorway_link_casing_width"
            },
            "bridge_motorway_link": {
                "line-color": "motorway_night",
                "line-width": "motorway_link_width"
            },
            "bridge_street_casing": {
                "line-color": "street_case_night",
                "line-width": "street_casing_width"
            },
            "bridge_street": {
                "line-color": "street_night",
                "line-width": "street_width"
            },
            "bridge_service_casing": {
                "line-color": "#000",
                "line-opacity": 0.04,
                "line-width": "service_casing_width"
            },
            "bridge_service": {
                "line-color": "street_night",
                "line-width": 2
            },
            "bridge_aerialway_casing": {
                "line-color": "white",
                "line-opacity": 0.5,
                "line-width": "aerialway_casing_width"
            },
            "bridge_aerialway": {
                "line-color": "#876",
                "line-opacity": 0.5,
                "line-width": "aerialway_width"
            },
            "country_label": {
                "text-color": "text_night",
                "text-halo-color": "text_stroke_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[6, 14], [12, 24]]
                }
            },
            "country_label_line": {
                "line-color": "text_night",
                "line-width": 0.5,
                "line-opacity": 0.5
            },
            "marine_label_line": {
                "text-color": "water_dark_night",
                "text-halo-color": "water_night"
            },
            "marine_label_point_1": {
                "text-color": "water_dark_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 20], [3, 20], [4, 25], [5, 30], [22, 30]]
                },
                "text-halo-color": "water_night"
            },
            "marine_label_point_2": {
                "text-color": "water_dark_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 13], [3, 13], [4, 14], [5, 20], [6, 24], [22, 24]]
                },
                "text-halo-color": "water_night"
            },
            "marine_label_point_other": {
                "text-color": "#a9c4c7",
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 12], [3, 12], [4, 13], [5, 15], [6, 18], [22, 18]]
                },
                "text-halo-color": "water_night"
            },
            "state_label": {
                "text-color": "#fff",
                "text-halo-width": 0.4,
                "text-halo-color": "land_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[3.99, 0], [4, 10], [9.99, 16], [10, 0]]
                }
            },
            "place_label_city_point": {
                "point-color": "#ddd"
            },
            "place_label_city": {
                "text-color": "#fff",
                "text-halo-width": 0.4,
                "text-halo-color": "text_stroke_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[3.99, 0], [4, 10], [7, 14], [14.99, 20], [15, 0]]
                },
                "text-translate": [0, ["stops", {
                                       "z": 4,
                                       "val": 10
                                       }, {
                                       "z": 6,
                                       "val": 30
                                       }, {
                                       "z": 7,
                                       "val": 0
                                       }]]
            },
            "place_label_town": {
                "text-color": "text_night",
                "text-halo-width": 0.5,
                "text-halo-color": "text_stroke_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[9, 10], [12, 13], [14, 17], [16, 22]]
                }
            },
            "place_label_village": {
                "text-color": "text_night",
                "text-halo-width": 0.5,
                "text-halo-color": "text_stroke_night",
                "text-size": {
                    "fn": "stops",
                    "stops": [[9, 8], [12, 10], [14, 14], [16, 16], [17, 20]]
                }
            },
            "place_label_other": {
                "text-color": "text_night",
                "text-halo-color": "text_stroke_night",
                "text-halo-width": 0.5,
                "text-size": {
                    "fn": "stops",
                    "stops": [[0, 10], [14, 11], [15, 12], [16, 14]]
                }
            },
            "road_label": {
                "text-color": "text_night",
                "text-halo-color": "text_stroke_night",
                "text-halo-width": 0.5,
                "text-size": "road_label_size"
            },
            "water_label": {
                "text-color": "water_dark_night",
                "text-halo-color": "text_stroke_night"
            },
            "waterway_label": {
                "text-color": "land_night",
                "text-halo-width": 0.8,
                "text-halo-color": "water_night"
            },
            "poi": {
                "point-color": "white",
                "point-antialias": false
            },
            "poi_label": {
                "text-color": "#fff",
                "text-size": 10,
                "text-halo-color": "land_night",
                "text-halo-width": 0.4,
                "text-translate": [0, 14]
            },
            "poi_aerodrome": {
                "point-opacity": {
                    "fn": "stops",
                    "stops": [[13, 0], [13.25, 1]]
                },
                "point-antialias": false
            }
        }
    },
    "sprite": "/img/maki-sprite"
}
