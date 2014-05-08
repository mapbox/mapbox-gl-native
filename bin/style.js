"use strict";

module.exports = {
    "buckets": {
        "admin_maritime": {
            "source": "mapbox streets",
            "layer": "admin",
            "field": "maritime",
            "value": "1",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "admin_level_2": {
            "source": "mapbox streets",
            "layer": "admin",
            "field": "admin_level",
            "value": 2,
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "admin_level_3": {
            "source": "mapbox streets",
            "layer": "admin",
            "field": "admin_level",
            "value": [3, 4, 5],
            "join": "round",
            "type": "line"
        },
        "landcover_wood": {
            "source": "mapbox streets",
            "layer": "landcover",
            "field": "class",
            "value": "wood",
            "type": "fill"
        },
        "landcover_scrub": {
            "source": "mapbox streets",
            "layer": "landcover",
            "field": "class",
            "value": "scrub",
            "type": "fill"
        },
        "landcover_grass": {
            "source": "mapbox streets",
            "layer": "landcover",
            "field": "class",
            "value": "grass",
            "type": "fill"
        },
        "landcover_crop": {
            "source": "mapbox streets",
            "layer": "landcover",
            "field": "class",
            "value": "crop",
            "type": "fill"
        },
        "landcover_snow": {
            "source": "mapbox streets",
            "layer": "landcover",
            "field": "class",
            "value": "snow",
            "type": "fill"
        },
        "water": {
            "source": "mapbox streets",
            "layer": "water",
            "type": "fill"
        },
        "waterway_other": {
            "source": "mapbox streets",
            "layer": "waterway",
            "field": "type",
            "value": [
                      "ditch",
                      "drain"
                      ],
            "cap": "round",
            "type": "line"
        },
        "waterway_river_canal": {
            "source": "mapbox streets",
            "layer": "waterway",
            "field": "type",
            "value": [
                      "river",
                      "canal"
                      ],
            "cap": "round",
            "type": "line"
        },
        "waterway_stream": {
            "source": "mapbox streets",
            "layer": "waterway",
            "field": "type",
            "value": "stream",
            "cap": "round",
            "type": "line"
        },
        "landuse_park": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "park",
            "type": "fill"
        },
        "landuse_pitch": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "pitch",
            "type": "fill"
        },
        "landuse_cemetary": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "cemetary",
            "type": "fill"
        },
        "landuse_hospital": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "hospital",
            "type": "fill"
        },
        "landuse_industrial": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "industrial",
            "type": "fill"
        },
        "landuse_school": {
            "source": "mapbox streets",
            "layer": "landuse",
            "field": "class",
            "value": "school",
            "type": "fill"
        },
        "overlay_wetland": {
            "source": "mapbox streets",
            "layer": "landuse_overlay",
            "field": "class",
            "value": "wetland",
            "type": "fill"
        },
        "overlay_wetland_noveg": {
            "source": "mapbox streets",
            "layer": "landuse_overlay",
            "field": "class",
            "value": "wetland_noveg",
            "type": "fill"
        },
        "overlay_breakwater_pier": {
            "source": "mapbox streets",
            "layer": "landuse_overlay",
            "field": "class",
            "value": [
                      "breakwater",
                      "pier"
                      ],
            "type": "fill"
        },
        "hillshade_full_shadow": {
            "source": "mapbox streets",
            "layer": "hillshade",
            "field": "class",
            "value": "full_shadow",
            "type": "fill"
        },
        "hillshade_medium_shadow": {
            "source": "mapbox streets",
            "layer": "hillshade",
            "field": "class",
            "value": "medium_shadow",
            "type": "fill"
        },
        "hillshade_medium_highlight": {
            "source": "mapbox streets",
            "layer": "hillshade",
            "field": "class",
            "value": "medium_highlight",
            "type": "fill"
        },
        "hillshade_full_highlight": {
            "source": "mapbox streets",
            "layer": "hillshade",
            "field": "class",
            "value": "full_highlight",
            "type": "fill"
        },
        "contour_line_10": {
            "source": "mapbox streets",
            "layer": "contour",
            "field": "index",
            "value": 10,
            "type": "line"
        },
        "contour_line_other": {
            "source": "mapbox streets",
            "layer": "contour",
            "type": "line"
        },
        "contour_label": {
            "source": "mapbox streets",
            "layer": "label",
            "type": "label"
        },
        "building": {
            "source": "mapbox streets",
            "layer": "building",
            "type": "fill"
        },
        "barrier_line_gate": {
            "source": "mapbox streets",
            "layer": "barrier_line",
            "field": "class",
            "value": "gate",
            "type": "line"
        },
        "barrier_line_fence": {
            "source": "mapbox streets",
            "layer": "barrier_line",
            "field": "class",
            "value": "fence",
            "type": "line"
        },
        "barrier_line_hedge": {
            "source": "mapbox streets",
            "layer": "barrier_line",
            "field": "class",
            "value": "hedge",
            "type": "line"
        },
        "barrier_line_land": {
            "source": "mapbox streets",
            "layer": "barrier_line",
            "field": "class",
            "value": "land",
            "type": "line"
        },
        "barrier_line_cliff": {
            "source": "mapbox streets",
            "layer": "barrier_line",
            "field": "class",
            "value": "cliff",
            "type": "line"
        },
        "aeroway_fill": {
            "source": "mapbox streets",
            "layer": "aeroway",
            "type": "fill",
            "enabled": 12
        },
        "aeroway_runway": {
            "source": "mapbox streets",
            "layer": "aeroway",
            "field": "type",
            "value": "runway",
            "type": "line",
            "enabled": 12
        },
        "aeroway_taxiway": {
            "source": "mapbox streets",
            "layer": "aeroway",
            "field": "type",
            "value": "taxiway",
            "type": "line",
            "enabled": 12
        },
        "motorway": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "motorway",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "motorway_link": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "motorway_link",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "main": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "main",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "street": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": [
                      "street",
                      "street_limited"
                      ],
            "join": "round",
            "cap": "round",
            "type": "line",
            "enabled": 12
        },
        "service": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "service",
            "join": "round",
            "cap": "round",
            "type": "line",
            "enabled": 15
        },
        "path": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "path",
            "type": "line"
        },
        "path_footway": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "path_path": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "path_cycleway": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "path_mtb": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "path_steps": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "path_piste": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "major_rail": {
            "source": "mapbox streets",
            "layer": "road",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "tunnel_motorway": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "motorway",
            "type": "line"
        },
        "tunnel_motorway_link": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "motorway_link",
            "type": "line"
        },
        "tunnel_main": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "main",
            "type": "line"
        },
        "tunnel_street": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": [
                      "street",
                      "street_limited"
                      ],
            "type": "line",
            "enabled": 12
        },
        "tunnel_service": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "service",
            "type": "line",
            "enabled": 15
        },
        "tunnel_path": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "path",
            "type": "line"
        },
        "tunnel_path_footway": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "tunnel_path_path": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "tunnel_path_cycleway": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "tunnel_path_mtb": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "tunnel_path_steps": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "tunnel_path_piste": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "tunnel_major_rail": {
            "source": "mapbox streets",
            "layer": "tunnel",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "bridge_motorway": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "motorway",
            "type": "line"
        },
        "bridge_motorway_link": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "motorway_link",
            "type": "line"
        },
        "bridge_main": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "main",
            "type": "line"
        },
        "bridge_street": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": [
                      "street",
                      "street_limited"
                      ],
            "type": "line",
            "enabled": 12
        },
        "bridge_service": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "service",
            "type": "line",
            "enabled": 15
        },
        "bridge_path": {
            "source": "mapbox streets",
            "layer": "bridge",
            "type": "line"
        },
        "bridge_path_footway": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "bridge_path_path": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "bridge_path_cycleway": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "bridge_path_mtb": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "bridge_path_steps": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "bridge_path_piste": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "bridge_major_rail": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "bridge_aerialway": {
            "source": "mapbox streets",
            "layer": "bridge",
            "field": "class",
            "value": "aerialway",
            "type": "line"
        },
        "country_label": {
            "source": "mapbox streets",
            "layer": "country_label",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 13,
            "feature_type": "point",
            "type": "text",
            "enabled": 3
        },
        "country_label_line": {
            "source": "mapbox streets",
            "layer": "country_label_line",
            "type": "line",
            "enabled": 3
        },
        "marine_label_1": {
            "source": "mapbox streets",
            "layer": "marine_label",
            "field": "labelrank",
            "value": 1,
            "feature_type": "line",
            "type": "text",
            "text_field": "name",
            "path": "curve",
            "fontSize": 22
        },
        "marine_label_2": {
            "source": "mapbox streets",
            "layer": "marine_label",
            "field": "labelrank",
            "value": 2,
            "feature_type": "line",
            "type": "text",
            "text_field": "name",
            "path": "curve",
            "fontSize": 16
        },
        "marine_label_3": {
            "source": "mapbox streets",
            "layer": "marine_label",
            "field": "labelrank",
            "value": 3,
            "feature_type": "line",
            "type": "text",
            "text_field": "name",
            "path": "curve",
            "fontSize": 14
        },
        "place_label_city_point": {
            "source": "mapbox streets",
            "layer": "place_label",
            "field": "type",
            "value": "city",
            "type": "point"
        },
        "place_label_city": {
            "source": "mapbox streets",
            "layer": "place_label",
            "field": "type",
            "value": "city",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 24,
            "feature_type": "point",
            "type": "text"
        },
        "place_label_town": {
            "source": "mapbox streets",
            "layer": "place_label",
            "field": "type",
            "value": "town",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 24,
            "feature_type": "point",
            "type": "text"
        },
        "place_label_village": {
            "source": "mapbox streets",
            "layer": "place_label",
            "field": "type",
            "value": "village",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 22,
            "feature_type": "point",
            "type": "text"
        },
        "place_label_other": {
            "source": "mapbox streets",
            "layer": "place_label",
            "field": "type",
            "value": [
                      "hamlet",
                      "suburb",
                      "neighbourhood"
                      ],
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 14,
            "feature_type": "point",
            "type": "text"
        },
        "poi_label": {
            "source": "mapbox streets",
            "layer": "poi_label",
            "text_field": "name",
            "path": "horizontal",
            "padding": 2,
            "fontSize": 12,
            "feature_type": "point",
            "type": "text"
        },
        "road_label": {
            "source": "mapbox streets",
            "layer": "road_label",
            "text_field": "name",
            "path": "curve",
            "padding": 2,
            "fontSize": 13,
            "feature_type": "line",
            "type": "text"
        },
        "water_label": {
            "source": "mapbox streets",
            "layer": "water_label",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 12,
            "feature_type": "point",
            "type": "text"
        },
        "poi_airport": {
            "source": "mapbox streets",
            "layer": "poi_label",
            "field": "maki",
            "value": "airport",
            "size": {
                "x": 12,
                "y": 12
            },
            "type": "point"
        },
        "poi_park": {
            "source": "mapbox streets",
            "layer": "poi_label",
            "field": "maki",
            "value": "park",
            "size": {
                "x": 12,
                "y": 12
            },
            "type": "point"
        }
    },
    "constants": {
        "land": "#f4efe1",
        "water": "#cdd",
        "water_dark": "#185869",
        "crop": "#eeeed4",
        "grass": "#e7ebd1",
        "scrub": "#e0e8cd",
        "wood": "#d4e2c6",
        "snow": "#f4f8ff",
        "rock": "#ddd",
        "sand": "#ffd",
        "cemetary": "#edf4ed",
        "pitch": "#fff",
        "park": "#d4e4bc",
        "piste": "blue",
        "school": "#e8dfe0",
        "hospital": "#f8eee0",
        "builtup": "#f6faff",
        "case": "#fff",
        "motorway": "#d7a8a8",
        "main": "#ddc0b9",
        "street": "#fff",
        "text": "#666"
    },
    "structure": [
                  {
                  "name": "background",
                  "bucket": "background"
                  },
                  {
                  "name": "waterway_other",
                  "bucket": "waterway_other"
                  },
                  {
                  "name": "waterway_river_canal",
                  "bucket": "waterway_river_canal"
                  },
                  {
                  "name": "waterway_stream",
                  "bucket": "waterway_stream"
                  },
                  {
                  "name": "landcover_wood",
                  "bucket": "landcover_wood"
                  },
                  {
                  "name": "landcover_scrub",
                  "bucket": "landcover_scrub"
                  },
                  {
                  "name": "landcover_grass",
                  "bucket": "landcover_grass"
                  },
                  {
                  "name": "landcover_crop",
                  "bucket": "landcover_crop"
                  },
                  {
                  "name": "landcover_snow",
                  "bucket": "landcover_snow"
                  },
                  {
                  "name": "landuse_park",
                  "bucket": "landuse_park"
                  },
                  {
                  "name": "landuse_pitch",
                  "bucket": "landuse_pitch"
                  },
                  {
                  "name": "landuse_cemetary",
                  "bucket": "landuse_cemetary"
                  },
                  {
                  "name": "landuse_hospital",
                  "bucket": "landuse_hospital"
                  },
                  {
                  "name": "landuse_industrial",
                  "bucket": "landuse_indusrial"
                  },
                  {
                  "name": "landuse_school",
                  "bucket": "landuse_school"
                  },
                  {
                  "name": "overlay_wetland",
                  "bucket": "overlay_wetland"
                  },
                  {
                  "name": "overlay_wetland_noveg",
                  "bucket": "overlay_wetland_noveg"
                  },
                  {
                  "name": "overlay_breakwater_pier",
                  "bucket": "overlay_breakwater_pier"
                  },
                  {
                  "name": "hillshade_full_shadow",
                  "bucket": "hillshade_full_shadow"
                  },
                  {
                  "name": "hillshade_medium_shadow",
                  "bucket": "hillshade_medium_shadow"
                  },
                  {
                  "name": "hillshade_medium_highlight",
                  "bucket": "hillshade_medium_highlight"
                  },
                  {
                  "name": "hillshade_full_highlight",
                  "bucket": "hillshade_full_highlight"
                  },
                  {
                  "name": "contour_line_10",
                  "bucket": "contour_line_10"
                  },
                  {
                  "name": "contour_line_other",
                  "bucket": "contour_line_other"
                  },
                  {
                  "name": "contour_label",
                  "bucket": "contour_label"
                  },
                  {
                  "name": "barrier_line_gate",
                  "bucket": "barrier_line_gate"
                  },
                  {
                  "name": "barrier_line_fence",
                  "bucket": "barrier_line_fence"
                  },
                  {
                  "name": "barrier_line_hedge",
                  "bucket": "barrier_line_hedge"
                  },
                  {
                  "name": "barrier_line_land",
                  "bucket": "barrier_line_land"
                  },
                  {
                  "name": "barrier_line_cliff",
                  "bucket": "barrier_line_cliff"
                  },
                  {
                  "name": "water",
                  "bucket": "water"
                  },
                  {
                  "name": "aeroway_fill",
                  "bucket": "aeroway_fill"
                  },
                  {
                  "name": "aeroway_runway",
                  "bucket": "aeroway_runway"
                  },
                  {
                  "name": "aeroway_taxiway",
                  "bucket": "aeroway_taxiway"
                  },
                  {
                  "name": "building_shadow",
                  "bucket": "building"
                  },
                  {
                  "name": "building",
                  "bucket": "building"
                  },
                  {
                  "name": "building_wall",
                  "bucket": "building"
                  },
                  {
                  "name": "tunnel_motorway_link_casing",
                  "bucket": "tunnel_motorway_link"
                  },
                  {
                  "name": "tunnel_service_casing",
                  "bucket": "tunnel_service"
                  },
                  {
                  "name": "tunnel_main_casing",
                  "bucket": "tunnel_main"
                  },
                  {
                  "name": "tunnel_street_casing",
                  "bucket": "tunnel_street"
                  },
                  {
                  "name": "tunnel_motorway_link",
                  "bucket": "tunnel_motorway_link"
                  },
                  {
                  "name": "tunnel_service",
                  "bucket": "tunnel_service"
                  },
                  {
                  "name": "tunnel_street",
                  "bucket": "tunnel_street"
                  },
                  {
                  "name": "tunnel_main",
                  "bucket": "tunnel_main"
                  },
                  {
                  "name": "tunnel_motorway_casing",
                  "bucket": "tunnel_motorway"
                  },
                  {
                  "name": "tunnel_motorway",
                  "bucket": "tunnel_motorway"
                  },
                  {
                  "name": "road_path_case",
                  "bucket": "tunnel_path"
                  },
                  {
                  "name": "road_path_footway",
                  "bucket": "tunnel_path_footway"
                  },
                  {
                  "name": "road_path_path",
                  "bucket": "tunnel_path_path"
                  },
                  {
                  "name": "road_path_cycleway",
                  "bucket": "tunnel_path_cycleway"
                  },
                  {
                  "name": "road_path_mtb",
                  "bucket": "tunnel_path_mtb"
                  },
                  {
                  "name": "road_path_piste",
                  "bucket": "tunnel_path_piste"
                  },
                  {
                  "name": "road_path_steps",
                  "bucket": "tunnel_path_steps"
                  },
                  {
                  "name": "road_major_rail",
                  "bucket": "tunnel_major_rail"
                  },
                  {
                  "name": "road_major_rail_hatching",
                  "bucket": "tunnel_major_rail"
                  },
                  {
                  "name": "road_motorway_link_casing",
                  "bucket": "motorway_link"
                  },
                  {
                  "name": "road_service_casing",
                  "bucket": "service"
                  },
                  {
                  "name": "road_main_casing",
                  "bucket": "main"
                  },
                  {
                  "name": "road_street_casing",
                  "bucket": "street"
                  },
                  {
                  "name": "road_motorway_link",
                  "bucket": "motorway_link"
                  },
                  {
                  "name": "road_service",
                  "bucket": "service"
                  },
                  {
                  "name": "road_street",
                  "bucket": "street"
                  },
                  {
                  "name": "road_main",
                  "bucket": "main"
                  },
                  {
                  "name": "road_motorway_casing",
                  "bucket": "motorway"
                  },
                  {
                  "name": "road_motorway",
                  "bucket": "motorway"
                  },
                  {
                  "name": "road_path_case",
                  "bucket": "path"
                  },
                  {
                  "name": "road_path_footway",
                  "bucket": "path_footway"
                  },
                  {
                  "name": "road_path_path",
                  "bucket": "path_path"
                  },
                  {
                  "name": "road_path_cycleway",
                  "bucket": "path_cycleway"
                  },
                  {
                  "name": "road_path_mtb",
                  "bucket": "path_mtb"
                  },
                  {
                  "name": "road_path_piste",
                  "bucket": "path_piste"
                  },
                  {
                  "name": "road_path_steps",
                  "bucket": "path_steps"
                  },
                  {
                  "name": "road_major_rail",
                  "bucket": "major_rail"
                  },
                  {
                  "name": "road_major_rail_hatching",
                  "bucket": "major_rail"
                  },
                  {
                  "name": "bridge_motorway_link_casing",
                  "bucket": "bridge_motorway_link"
                  },
                  {
                  "name": "bridge_service_casing",
                  "bucket": "bridge_service"
                  },
                  {
                  "name": "bridge_main_casing",
                  "bucket": "bridge_main"
                  },
                  {
                  "name": "bridge_street_casing",
                  "bucket": "bridge_street"
                  },
                  {
                  "name": "bridge_motorway_link",
                  "bucket": "bridge_motorway_link"
                  },
                  {
                  "name": "bridge_service",
                  "bucket": "bridge_service"
                  },
                  {
                  "name": "bridge_street",
                  "bucket": "bridge_street"
                  },
                  {
                  "name": "bridge_main",
                  "bucket": "bridge_main"
                  },
                  {
                  "name": "bridge_motorway_casing",
                  "bucket": "bridge_motorway"
                  },
                  {
                  "name": "bridge_motorway",
                  "bucket": "bridge_motorway"
                  },
                  {
                  "name": "road_path_footway",
                  "bucket": "bridge_path_footway"
                  },
                  {
                  "name": "road_path_path",
                  "bucket": "bridge_path_path"
                  },
                  {
                  "name": "road_path_cycleway",
                  "bucket": "bridge_path_cycleway"
                  },
                  {
                  "name": "road_path_mtb",
                  "bucket": "bridge_path_mtb"
                  },
                  {
                  "name": "road_path_piste",
                  "bucket": "bridge_path_piste"
                  },
                  {
                  "name": "road_path_steps",
                  "bucket": "bridge_path_steps"
                  },
                  {
                  "name": "bridge_aerialway_casing",
                  "bucket": "bridge_aerialway"
                  },
                  {
                  "name": "bridge_aerialway",
                  "bucket": "bridge_aerialway"
                  },
                  {
                  "name": "road_major_rail",
                  "bucket": "bridge_major_rail"
                  },
                  {
                  "name": "road_major_rail_hatching",
                  "bucket": "bridge_major_rail"
                  },
                  {
                  "name": "admin_level_3",
                  "bucket": "admin_level_3"
                  },
                  {
                  "name": "admin_level_2",
                  "bucket": "admin_level_2"
                  },
                  {
                  "name": "admin_maritime",
                  "bucket": "admin_maritime"
                  }
                  ],
    "classes": [
                {
                "name": "default",
                "layers": {
                "background": {
                "color": "land"
                },
                "admin_maritime": {
                "color": "#c0d6d6",
                "width": 4.5
                },
                "admin_level_2": {
                "color": "#88a",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.4
                          },
                          {
                          "z": 2,
                          "val": 0.4
                          },
                          {
                          "z": 3,
                          "val": 0.8
                          },
                          {
                          "z": 4,
                          "val": 1
                          },
                          {
                          "z": 5,
                          "val": 1.5
                          },
                          {
                          "z": 6,
                          "val": 2
                          },
                          {
                          "z": 8,
                          "val": 3
                          },
                          {
                          "z": 10,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 4
                          }
                          ]
                },
                "admin_level_3": {
                "color": "#88a",
                "dasharray": [
                              30,
                              5
                              ],
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.4
                          },
                          {
                          "z": 6,
                          "val": 1
                          },
                          {
                          "z": 8,
                          "val": 2
                          },
                          {
                          "z": 12,
                          "val": 3
                          },
                          {
                          "z": 22,
                          "val": 3
                          }
                          ]
                },
                "waterway_other": {
                "color": "water",
                "width": 0.5
                },
                "waterway_river_canal": {
                "color": "#87abaf",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.5
                          },
                          {
                          "z": 12,
                          "val": 1
                          },
                          {
                          "z": 14,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 22,
                          "val": 3
                          }
                          ]
                },
                "waterway_stream": {
                "color": "#87abaf",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.25
                          },
                          {
                          "z": 13,
                          "val": 0.25
                          },
                          {
                          "z": 14,
                          "val": 0.5
                          },
                          {
                          "z": 16,
                          "val": 1.5
                          },
                          {
                          "z": 18,
                          "val": 2
                          },
                          {
                          "z": 22,
                          "val": 2
                          }
                          ]
                },
                "landcover_wood": {
                "color": "wood"
                },
                "landcover_scrub": {
                "color": "scrub"
                },
                "landcover_grass": {
                "color": "grass"
                },
                "landcover_crop": {
                "color": "crop"
                },
                "landcover_snow": {
                "color": "snow"
                },
                "landuse_park": {
                "color": "park"
                },
                "landuse_cemetary": {
                "color": "cemetary"
                },
                "landuse_hospital": {
                "color": "hospital"
                },
                "landuse_school": {
                "color": "school"
                },
                "landuse_pitch": {
                "color": "rgba(255,255,255,0.5)",
                "stroke": "pitch"
                },
                "hillshade_full_shadow": {
                "color": "#103",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.08
                            },
                            {
                            "z": 15,
                            "val": 0.075
                            },
                            {
                            "z": 17,
                            "val": 0.05
                            },
                            {
                            "z": 18,
                            "val": 0.025
                            },
                            {
                            "z": 22,
                            "val": 0.025
                            }
                            ]
                },
                "hillshade_medium_shadow": {
                "color": "#206",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.08
                            },
                            {
                            "z": 15,
                            "val": 0.075
                            },
                            {
                            "z": 17,
                            "val": 0.05
                            },
                            {
                            "z": 18,
                            "val": 0.025
                            },
                            {
                            "z": 22,
                            "val": 0.025
                            }
                            ]
                },
                "hillshade_full_highlight": {
                "color": "#ffd",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.25
                            },
                            {
                            "z": 15,
                            "val": 0.3
                            },
                            {
                            "z": 17,
                            "val": 0.2
                            },
                            {
                            "z": 18,
                            "val": 0.1
                            },
                            {
                            "z": 22,
                            "val": 0.1
                            }
                            ]
                },
                "hillshade_medium_highlight": {
                "color": "#ffb",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.2
                            },
                            {
                            "z": 15,
                            "val": 0.3
                            },
                            {
                            "z": 17,
                            "val": 0.2
                            },
                            {
                            "z": 18,
                            "val": 0.1
                            },
                            {
                            "z": 22,
                            "val": 0.1
                            }
                            ]
                },
                "contour_line_10": {
                "color": "#008",
                "width": 1.2,
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.06
                            },
                            {
                            "z": 12,
                            "val": 0.06
                            },
                            {
                            "z": 13,
                            "val": 0.12
                            },
                            {
                            "z": 22,
                            "val": 0.12
                            }
                            ]
                },
                "contour_line_other": {
                "color": "#008",
                "width": 0.5,
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0.06
                            },
                            {
                            "z": 12,
                            "val": 0.06
                            },
                            {
                            "z": 13,
                            "val": 0.12
                            },
                            {
                            "z": 22,
                            "val": 0.12
                            }
                            ]
                },
                "water": {
                "color": "water",
                "stroke": "#a2bdc0"
                },
                "aeroway_fill": {
                "color": "#ddd"
                },
                "aeroway_runway": {
                "color": "#ddd",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 1
                          },
                          {
                          "z": 11,
                          "val": 2
                          },
                          {
                          "z": 12,
                          "val": 3
                          },
                          {
                          "z": 13,
                          "val": 5
                          },
                          {
                          "z": 14,
                          "val": 7
                          },
                          {
                          "z": 15,
                          "val": 11
                          },
                          {
                          "z": 16,
                          "val": 15
                          },
                          {
                          "z": 17,
                          "val": 19
                          },
                          {
                          "z": 18,
                          "val": 23
                          },
                          {
                          "z": 22,
                          "val": 23
                          }
                          ]
                },
                "aeroway_taxiway": {
                "color": "#ddd",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 0.2
                          },
                          {
                          "z": 12,
                          "val": 0.2
                          },
                          {
                          "z": 13,
                          "val": 1
                          },
                          {
                          "z": 14,
                          "val": 1.5
                          },
                          {
                          "z": 15,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 18,
                          "val": 5
                          },
                          {
                          "z": 22,
                          "val": 5
                          }
                          ]
                },
                "building": {
                "color": "#ebe7db"
                },
                "building_wall": {
                "color": "#ebe7db",
                "stroke": "#d5d1c6",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 16.5,
                            "val": 0
                            },
                            {
                            "z": 17,
                            "val": 0.7
                            },
                            {
                            "z": 22,
                            "val": 0.7
                            }
                            ]
                },
                "building_shadow": {
                "color": "#d5d1c6",
                "stroke": "#d5d1c6",
                "translate": [
                              2,
                              2
                              ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 16.5,
                            "val": 0
                            },
                            {
                            "z": 17,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "tunnel_motorway_casing": {
                "color": "case",
                "dasharray": [
                              6,
                              6
                              ],
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.6
                          },
                          {
                          "z": 7.5,
                          "val": 0.6
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 2.8
                          },
                          {
                          "z": 11,
                          "val": 3
                          },
                          {
                          "z": 12,
                          "val": 4
                          },
                          {
                          "z": 13,
                          "val": 5
                          },
                          {
                          "z": 14,
                          "val": 6.5
                          },
                          {
                          "z": 15,
                          "val": 9
                          },
                          {
                          "z": 16,
                          "val": 12
                          },
                          {
                          "z": 17,
                          "val": 15
                          },
                          {
                          "z": 18,
                          "val": 17
                          },
                          {
                          "z": 22,
                          "val": 17
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 9.5,
                            "val": 0
                            },
                            {
                            "z": 10,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "tunnel_motorway": {
                "color": "#e6cec7",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0
                          },
                          {
                          "z": 5,
                          "val": 0
                          },
                          {
                          "z": 6,
                          "val": 0.5
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 1
                          },
                          {
                          "z": 11,
                          "val": 1.2
                          },
                          {
                          "z": 12,
                          "val": 2
                          },
                          {
                          "z": 13,
                          "val": 3
                          },
                          {
                          "z": 14,
                          "val": 4
                          },
                          {
                          "z": 15,
                          "val": 6
                          },
                          {
                          "z": 16,
                          "val": 9
                          },
                          {
                          "z": 17,
                          "val": 12
                          },
                          {
                          "z": 18,
                          "val": 14
                          },
                          {
                          "z": 22,
                          "val": 14
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "tunnel_main_casing": {
                "color": "case",
                "dasharray": [
                              6,
                              6
                              ],
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 2.9
                          },
                          {
                          "z": 13,
                          "val": 2.9
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 15,
                          "val": 4
                          },
                          {
                          "z": 16,
                          "val": 5.5
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 15
                          },
                          {
                          "z": 22,
                          "val": 15
                          }
                          ]
                },
                "tunnel_main": {
                "color": "#e6cec7",
                "width": [
                          "stops",
                          {
                          "z": 6,
                          "val": 1
                          },
                          {
                          "z": 12,
                          "val": 1
                          },
                          {
                          "z": 13,
                          "val": 1
                          },
                          {
                          "z": 14,
                          "val": 1.5
                          },
                          {
                          "z": 15,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 6
                          },
                          {
                          "z": 18,
                          "val": 10
                          },
                          {
                          "z": 19,
                          "val": 12
                          },
                          {
                          "z": 22,
                          "val": 12
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "tunnel_motorway_link_casing": {
                "color": "case",
                "dasharray": [
                              6,
                              6
                              ],
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 2.8
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 16,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "tunnel_motorway_link": {
                "color": "#e6cec7",
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 1.2
                          },
                          {
                          "z": 14,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 18,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 4
                          }
                          ]
                },
                "tunnel_street_casing_group": {
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 12.5,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0.6
                            },
                            {
                            "z": 22,
                            "val": 0.6
                            }
                            ]
                },
                "tunnel_street_casing": {
                "color": "#d9d5c6",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 0.5
                          },
                          {
                          "z": 12.5,
                          "val": 0.5
                          },
                          {
                          "z": 13,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 2.25
                          },
                          {
                          "z": 15,
                          "val": 5
                          },
                          {
                          "z": 16,
                          "val": 7
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 11
                          },
                          {
                          "z": 22,
                          "val": 11
                          }
                          ]
                },
                "tunnel_street_group": {
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0
                            },
                            {
                            "z": 14.25,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "tunnel_street": {
                "color": "#d9d5c6",
                "width": [
                          "stops",
                          {
                          "z": 11,
                          "val": 0
                          },
                          {
                          "z": 12.5,
                          "val": 0
                          },
                          {
                          "z": 13,
                          "val": 0.6
                          },
                          {
                          "z": 14,
                          "val": 1.2
                          },
                          {
                          "z": 15,
                          "val": 1.8
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 8
                          },
                          {
                          "z": 22,
                          "val": 8
                          }
                          ]
                },
                "tunnel_service_casing": {
                "color": "#000",
                "opacity": 0.04,
                "dasharray": [
                              6,
                              6
                              ],
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 0.5
                          },
                          {
                          "z": 15,
                          "val": 3
                          },
                          {
                          "z": 16,
                          "val": 3.5
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 18,
                          "val": 5
                          },
                          {
                          "z": 19,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "tunnel_service": {
                "color": "#e6cec7",
                "width": 2
                },
                "road_motorway_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.6
                          },
                          {
                          "z": 7.5,
                          "val": 0.6
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 2.8
                          },
                          {
                          "z": 11,
                          "val": 3
                          },
                          {
                          "z": 12,
                          "val": 4
                          },
                          {
                          "z": 13,
                          "val": 5
                          },
                          {
                          "z": 14,
                          "val": 6.5
                          },
                          {
                          "z": 15,
                          "val": 9
                          },
                          {
                          "z": 16,
                          "val": 12
                          },
                          {
                          "z": 17,
                          "val": 15
                          },
                          {
                          "z": 18,
                          "val": 17
                          },
                          {
                          "z": 22,
                          "val": 17
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 9.5,
                            "val": 0
                            },
                            {
                            "z": 10,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "road_motorway": {
                "color": "motorway",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0
                          },
                          {
                          "z": 5,
                          "val": 0
                          },
                          {
                          "z": 6,
                          "val": 0.5
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 1
                          },
                          {
                          "z": 11,
                          "val": 1.2
                          },
                          {
                          "z": 12,
                          "val": 2
                          },
                          {
                          "z": 13,
                          "val": 3
                          },
                          {
                          "z": 14,
                          "val": 4
                          },
                          {
                          "z": 15,
                          "val": 6
                          },
                          {
                          "z": 16,
                          "val": 9
                          },
                          {
                          "z": 17,
                          "val": 12
                          },
                          {
                          "z": 18,
                          "val": 14
                          },
                          {
                          "z": 22,
                          "val": 14
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "road_main_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 2.9
                          },
                          {
                          "z": 13,
                          "val": 2.9
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 15,
                          "val": 4
                          },
                          {
                          "z": 16,
                          "val": 5.5
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 15
                          },
                          {
                          "z": 22,
                          "val": 15
                          }
                          ]
                },
                "road_main": {
                "color": "main",
                "width": [
                          "stops",
                          {
                          "z": 6,
                          "val": 1
                          },
                          {
                          "z": 12,
                          "val": 1
                          },
                          {
                          "z": 13,
                          "val": 1
                          },
                          {
                          "z": 14,
                          "val": 1.5
                          },
                          {
                          "z": 15,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 6
                          },
                          {
                          "z": 18,
                          "val": 10
                          },
                          {
                          "z": 19,
                          "val": 12
                          },
                          {
                          "z": 22,
                          "val": 12
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "road_motorway_link_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 2.8
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 16,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "road_motorway_link": {
                "color": "motorway",
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 1.2
                          },
                          {
                          "z": 14,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 18,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 4
                          }
                          ]
                },
                "road_street_casing": {
                "color": "#d9d5c6",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 0.5
                          },
                          {
                          "z": 12.5,
                          "val": 0.5
                          },
                          {
                          "z": 13,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 2.25
                          },
                          {
                          "z": 15,
                          "val": 5
                          },
                          {
                          "z": 16,
                          "val": 7
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 11
                          },
                          {
                          "z": 22,
                          "val": 11
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 12.5,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0.6
                            },
                            {
                            "z": 22,
                            "val": 0.6
                            }
                            ]
                },
                "road_street": {
                "color": "street",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0
                            },
                            {
                            "z": 14.25,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ],
                "width": [
                          "stops",
                          {
                          "z": 11,
                          "val": 0
                          },
                          {
                          "z": 12.5,
                          "val": 0
                          },
                          {
                          "z": 13,
                          "val": 0.6
                          },
                          {
                          "z": 14,
                          "val": 1.2
                          },
                          {
                          "z": 15,
                          "val": 1.8
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 8
                          },
                          {
                          "z": 22,
                          "val": 8
                          }
                          ]
                },
                "road_service_casing": {
                "color": "#000",
                "opacity": 0.04,
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 0.5
                          },
                          {
                          "z": 15,
                          "val": 3
                          },
                          {
                          "z": 16,
                          "val": 3.5
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 18,
                          "val": 5
                          },
                          {
                          "z": 19,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "road_service": {
                "color": "street",
                "width": 2
                },
                "road_path_case": {
                "color": "#ffd",
                "opacity": 0.4,
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 3
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 4
                          }
                          ]
                },
                "road_path_footway": {
                "color": "#bba",
                "dasharray": [
                              4,
                              2
                              ],
                "width": [
                          "stops",
                          {
                          "z": 15,
                          "val": 1.2
                          },
                          {
                          "z": 16,
                          "val": 1.5
                          },
                          {
                          "z": 17,
                          "val": 1.8
                          },
                          {
                          "z": 22,
                          "val": 1.8
                          }
                          ]
                },
                "road_path_path": {
                "color": "#987",
                "dasharray": [
                              6,
                              2
                              ],
                "opacity": 0.8,
                "width": [
                          "stops",
                          {
                          "z": 15,
                          "val": 0.8
                          },
                          {
                          "z": 16,
                          "val": 0.9
                          },
                          {
                          "z": 17,
                          "val": 1.2
                          },
                          {
                          "z": 22,
                          "val": 1.2
                          }
                          ]
                },
                "road_path_cycleway": {
                "color": "#488",
                "dasharray": [
                              4,
                              2
                              ],
                "width": [
                          "stops",
                          {
                          "z": 15,
                          "val": 1.2
                          },
                          {
                          "z": 16,
                          "val": 1.5
                          },
                          {
                          "z": 17,
                          "val": 1.8
                          },
                          {
                          "z": 22,
                          "val": 1.8
                          }
                          ]
                },
                "road_path_mtb": {
                "color": "#488",
                "dasharray": [
                              12,
                              4
                              ],
                "width": [
                          "stops",
                          {
                          "z": 15,
                          "val": 1.2
                          },
                          {
                          "z": 16,
                          "val": 1.5
                          },
                          {
                          "z": 17,
                          "val": 1.8
                          },
                          {
                          "z": 22,
                          "val": 1.8
                          }
                          ]
                },
                "road_path_piste": {
                "color": "#87b",
                "dasharray": [
                              8,
                              4
                              ],
                "width": [
                          "stops",
                          {
                          "z": 15,
                          "val": 1.2
                          },
                          {
                          "z": 16,
                          "val": 1.5
                          },
                          {
                          "z": 17,
                          "val": 1.8
                          },
                          {
                          "z": 22,
                          "val": 1.8
                          }
                          ]
                },
                "road_path_steps": {
                "color": "#bba",
                "dasharray": [
                              4,
                              2
                              ],
                "width": 4
                },
                "road_major_rail": {
                "color": "#c8c4c0",
                "width": 0.8
                },
                "road_major_rail_hatching": {
                "color": "#c8c4c0",
                "dasharray": [
                              2,
                              31
                              ],
                "width": 5
                },
                "bridge_motorway_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.6
                          },
                          {
                          "z": 7.5,
                          "val": 0.6
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 2.8
                          },
                          {
                          "z": 11,
                          "val": 3
                          },
                          {
                          "z": 12,
                          "val": 4
                          },
                          {
                          "z": 13,
                          "val": 5
                          },
                          {
                          "z": 14,
                          "val": 6.5
                          },
                          {
                          "z": 15,
                          "val": 9
                          },
                          {
                          "z": 16,
                          "val": 12
                          },
                          {
                          "z": 17,
                          "val": 15
                          },
                          {
                          "z": 18,
                          "val": 17
                          },
                          {
                          "z": 22,
                          "val": 17
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 9.5,
                            "val": 0
                            },
                            {
                            "z": 10,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "bridge_motorway": {
                "color": "motorway",
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0
                          },
                          {
                          "z": 5,
                          "val": 0
                          },
                          {
                          "z": 6,
                          "val": 0.5
                          },
                          {
                          "z": 8,
                          "val": 0.8
                          },
                          {
                          "z": 10,
                          "val": 1
                          },
                          {
                          "z": 11,
                          "val": 1.2
                          },
                          {
                          "z": 12,
                          "val": 2
                          },
                          {
                          "z": 13,
                          "val": 3
                          },
                          {
                          "z": 14,
                          "val": 4
                          },
                          {
                          "z": 15,
                          "val": 6
                          },
                          {
                          "z": 16,
                          "val": 9
                          },
                          {
                          "z": 17,
                          "val": 12
                          },
                          {
                          "z": 18,
                          "val": 14
                          },
                          {
                          "z": 22,
                          "val": 14
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "bridge_main_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 2.9
                          },
                          {
                          "z": 13,
                          "val": 2.9
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 15,
                          "val": 4
                          },
                          {
                          "z": 16,
                          "val": 5.5
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 15
                          },
                          {
                          "z": 22,
                          "val": 15
                          }
                          ]
                },
                "bridge_main": {
                "color": "main",
                "width": [
                          "stops",
                          {
                          "z": 6,
                          "val": 1
                          },
                          {
                          "z": 12,
                          "val": 1
                          },
                          {
                          "z": 13,
                          "val": 1
                          },
                          {
                          "z": 14,
                          "val": 1.5
                          },
                          {
                          "z": 15,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 6
                          },
                          {
                          "z": 18,
                          "val": 10
                          },
                          {
                          "z": 19,
                          "val": 12
                          },
                          {
                          "z": 22,
                          "val": 12
                          }
                          ],
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 6.5,
                            "val": 0
                            },
                            {
                            "z": 7,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ]
                },
                "bridge_motorway_link_casing": {
                "color": "case",
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 2.8
                          },
                          {
                          "z": 14,
                          "val": 3.5
                          },
                          {
                          "z": 16,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "bridge_motorway_link": {
                "color": "motorway",
                "width": [
                          "stops",
                          {
                          "z": 12,
                          "val": 1.2
                          },
                          {
                          "z": 14,
                          "val": 2
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 18,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 4
                          }
                          ]
                },
                "bridge_street_casing": {
                "color": "#d9d5c6",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 12.5,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0.6
                            },
                            {
                            "z": 22,
                            "val": 0.6
                            }
                            ],
                "width": [
                          "stops",
                          {
                          "z": 10,
                          "val": 0.5
                          },
                          {
                          "z": 12.5,
                          "val": 0.5
                          },
                          {
                          "z": 13,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 2.25
                          },
                          {
                          "z": 15,
                          "val": 5
                          },
                          {
                          "z": 16,
                          "val": 7
                          },
                          {
                          "z": 17,
                          "val": 9
                          },
                          {
                          "z": 18,
                          "val": 11
                          },
                          {
                          "z": 22,
                          "val": 11
                          }
                          ]
                },
                "bridge_street": {
                "color": "street",
                "opacity": [
                            "stops",
                            {
                            "z": 0,
                            "val": 0
                            },
                            {
                            "z": 13.5,
                            "val": 0
                            },
                            {
                            "z": 14.25,
                            "val": 1
                            },
                            {
                            "z": 22,
                            "val": 1
                            }
                            ],
                "width": [
                          "stops",
                          {
                          "z": 11,
                          "val": 0
                          },
                          {
                          "z": 12.5,
                          "val": 0
                          },
                          {
                          "z": 13,
                          "val": 0.6
                          },
                          {
                          "z": 14,
                          "val": 1.2
                          },
                          {
                          "z": 15,
                          "val": 1.8
                          },
                          {
                          "z": 16,
                          "val": 3
                          },
                          {
                          "z": 17,
                          "val": 5
                          },
                          {
                          "z": 18,
                          "val": 8
                          },
                          {
                          "z": 22,
                          "val": 8
                          }
                          ]
                },
                "bridge_service_casing": {
                "color": "#000",
                "opacity": 0.04,
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.5
                          },
                          {
                          "z": 14,
                          "val": 0.5
                          },
                          {
                          "z": 15,
                          "val": 3
                          },
                          {
                          "z": 16,
                          "val": 3.5
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 18,
                          "val": 5
                          },
                          {
                          "z": 19,
                          "val": 6
                          },
                          {
                          "z": 22,
                          "val": 6
                          }
                          ]
                },
                "bridge_service": {
                "color": "street",
                "width": 2
                },
                "bridge_aerialway_casing": {
                "color": "white",
                "opacity": 0.5,
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 2
                          },
                          {
                          "z": 13.5,
                          "val": 2
                          },
                          {
                          "z": 14,
                          "val": 2.5
                          },
                          {
                          "z": 15,
                          "val": 3
                          },
                          {
                          "z": 16,
                          "val": 3.5
                          },
                          {
                          "z": 17,
                          "val": 4
                          },
                          {
                          "z": 22,
                          "val": 5
                          }
                          ]
                },
                "bridge_aerialway": {
                "color": "#876",
                "opacity": 0.5,
                "width": [
                          "stops",
                          {
                          "z": 0,
                          "val": 0.8
                          },
                          {
                          "z": 13.5,
                          "val": 0.8
                          },
                          {
                          "z": 14,
                          "val": 1.4
                          },
                          {
                          "z": 15,
                          "val": 1.6
                          },
                          {
                          "z": 16,
                          "val": 2
                          },
                          {
                          "z": 17,
                          "val": 2.4
                          },
                          {
                          "z": 18,
                          "val": 3
                          },
                          {
                          "z": 22,
                          "val": 3
                          }
                          ]
                },
                "country_label": {
                "color": "text",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ]
                },
                "country_label_line": {
                "color": "text",
                "width": 0.5,
                "opacity": 0.5
                },
                "marine_label_1": {
                "color": "text",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ]
                },
                "marine_label_2": {
                "color": "text",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ]
                },
                "marine_label_3": {
                "color": "text",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ]
                },
                "place_label_city_point": {
                "color": "#333",
                "radius": 2
                },
                "place_label_city": {
                "color": "#333",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ],
                "size": [
                         "stops",
                         {
                         "z": 0,
                         "val": 10
                         },
                         {
                         "z": 10,
                         "val": 18
                         },
                         {
                         "z": 12,
                         "val": 24
                         }
                         ],
                "translate": [
                              0,
                              30
                              ]
                },
                "place_label_town": {
                "color": "#333",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ],
                "size": [
                         "stops",
                         {
                         "z": 0,
                         "val": 14
                         },
                         {
                         "z": 12,
                         "val": 16
                         },
                         {
                         "z": 14,
                         "val": 20
                         },
                         {
                         "z": 16,
                         "val": 24
                         }
                         ]
                },
                "place_label_village": {
                "color": "#333",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ],
                "size": [
                         "stops",
                         {
                         "z": 0,
                         "val": 12
                         },
                         {
                         "z": 12,
                         "val": 14
                         },
                         {
                         "z": 14,
                         "val": 28
                         },
                         {
                         "z": 16,
                         "val": 22
                         }
                         ]
                },
                "place_label_other": {
                "color": "#633",
                "stroke": [
                           1,
                           1,
                           1,
                           0.8
                           ],
                "size": [
                         "stops",
                         {
                         "z": 0,
                         "val": 10
                         },
                         {
                         "z": 14,
                         "val": 11
                         },
                         {
                         "z": 15,
                         "val": 12
                         },
                         {
                         "z": 16,
                         "val": 14
                         }
                         ]
                },
                "poi_label": {
                "color": "#666",
                "stroke": [
                           1,
                           1,
                           1,
                           0.5
                           ]
                },
                "road_label": {
                "color": "#765",
                "stroke": [
                           1,
                           1,
                           1,
                           0.5
                           ],
                "size": [
                         "stops",
                         {
                         "z": 0,
                         "val": 12
                         },
                         {
                         "z": 14,
                         "val": 12
                         },
                         {
                         "z": 15,
                         "val": 13
                         }
                         ]
                },
                "water_label": {
                "color": "text",
                "stroke": [
                           1,
                           1,
                           1,
                           0.75
                           ]
                },
                "poi_airport": {
                "color": "text",
                "image": "airport-12",
                "translate": [
                              0,
                              1
                              ]
                },
                "poi_park": {
                "color": "text",
                "image": "park-12",
                "translate": [
                              0,
                              1
                              ]
                }
                }
                }
                ],
    "sprite": "/img/maki-sprite"
};
