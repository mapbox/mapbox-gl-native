"use strict";

module.exports = {
    "buckets": {
        "admin_maritime": {
            "source": "outdoors",
            "layer": "admin",
            "field": "maritime",
            "value": 1,
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "admin_l2": {
            "source": "outdoors",
            "layer": "admin",
            "field": "admin_level",
            "value": 2,
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "admin_l3": {
            "source": "outdoors",
            "layer": "admin",
            "field": "admin_level",
            "value": [
                3,
                4,
                5
            ],
            "join": "round",
            "type": "line"
        },
        "landcover_wood": {
            "source": "outdoors",
            "layer": "landcover",
            "field": "class",
            "value": "wood",
            "type": "fill"
        },
        "landcover_scrub": {
            "source": "outdoors",
            "layer": "landcover",
            "field": "class",
            "value": "scrub",
            "type": "fill"
        },
        "landcover_grass": {
            "source": "outdoors",
            "layer": "landcover",
            "field": "class",
            "value": "grass",
            "type": "fill"
        },
        "landcover_crop": {
            "source": "outdoors",
            "layer": "landcover",
            "field": "class",
            "value": "crop",
            "type": "fill"
        },
        "landcover_snow": {
            "source": "outdoors",
            "layer": "landcover",
            "field": "class",
            "value": "snow",
            "type": "fill"
        },
        "water": {
            "source": "outdoors",
            "layer": "water",
            "type": "fill"
        },
        "waterway_other": {
            "source": "outdoors",
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
            "source": "outdoors",
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
            "source": "outdoors",
            "layer": "waterway",
            "field": "type",
            "value": "stream",
            "cap": "round",
            "type": "line"
        },
        "landuse_park": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "park",
            "type": "fill"
        },
        "landuse_pitch": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "pitch",
            "type": "fill"
        },
        "landuse_cemetery": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "cemetery",
            "type": "fill"
        },
        "landuse_hospital": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "hospital",
            "type": "fill"
        },
        "landuse_industrial": {
            "source": "outdoors",
            "layer": "landuse",
            "type": "fill",
            "field": "class",
            "value": "industrial"
        },
        "landuse_school": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "school",
            "type": "fill"
        },
        "landuse_wood": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "wood",
            "type": "fill"
        },
        "landuse_scrub": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "scrub",
            "type": "fill"
        },
        "landuse_grass": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "grass",
            "type": "fill"
        },
        "landuse_crop": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "crop",
            "type": "fill"
        },
        "landuse_sand": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "sand",
            "type": "fill"
        },
        "landuse_rock": {
            "source": "outdoors",
            "layer": "landuse",
            "field": "class",
            "value": "rock",
            "type": "fill"
        },
        "overlay_wetland": {
            "source": "outdoors",
            "layer": "landuse_overlay",
            "field": "class",
            "value": "wetland",
            "type": "fill"
        },
        "overlay_breakwater_pier": {
            "source": "outdoors",
            "layer": "landuse_overlay",
            "field": "class",
            "value": [
                "breakwater",
                "pier"
            ],
            "type": "fill"
        },
        "hillshade_full_shadow": {
            "source": "outdoors",
            "layer": "hillshade",
            "field": "class",
            "value": "full_shadow",
            "type": "fill"
        },
        "hillshade_medium_shadow": {
            "source": "outdoors",
            "layer": "hillshade",
            "field": "class",
            "value": "medium_shadow",
            "type": "fill"
        },
        "hillshade_medium_highlight": {
            "source": "outdoors",
            "layer": "hillshade",
            "field": "class",
            "value": "medium_highlight",
            "type": "fill"
        },
        "hillshade_full_highlight": {
            "source": "outdoors",
            "layer": "hillshade",
            "field": "class",
            "value": "full_highlight",
            "type": "fill"
        },
        "contour_line_5": {
            "source": "outdoors",
            "layer": "contour",
            "field": "index",
            "join": "round",
            "value": 5,
            "type": "line"
        },
        "contour_line_10": {
            "source": "outdoors",
            "layer": "contour",
            "field": "index",
            "join": "round",
            "value": 10,
            "type": "line"
        },
        "contour_line_other": {
            "source": "outdoors",
            "layer": "contour",
            "join": "round",
            "type": "line"
        },
        "contour_label": {
            "source": "outdoors",
            "layer": "contour",
            "field": "index",
            "value": 10,
            "path": "curve",
            "text_field": "ele",
            "fontSize": 10,
            "feature_type": "line",
            "type": "text"
        },
        "building": {
            "source": "outdoors",
            "layer": "building",
            "type": "fill"
        },
        "barrier_line_gate": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "gate",
            "type": "line"
        },
        "barrier_line_fence": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "fence",
            "type": "line"
        },
        "barrier_line_hedge": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "hedge",
            "type": "line"
        },
        "barrier_line_land": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "land",
            "type": "line"
        },
        "barrier_line_land_fill": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "land",
            "type": "fill"
        },
        "barrier_line_cliff": {
            "source": "outdoors",
            "layer": "barrier_line",
            "field": "class",
            "value": "cliff",
            "type": "line"
        },
        "aeroway_fill": {
            "source": "outdoors",
            "layer": "aeroway",
            "type": "fill",
            "enabled": 12
        },
        "aeroway_runway": {
            "source": "outdoors",
            "layer": "aeroway",
            "field": "type",
            "value": "runway",
            "type": "line",
            "enabled": 12
        },
        "aeroway_taxiway": {
            "source": "outdoors",
            "layer": "aeroway",
            "field": "type",
            "value": "taxiway",
            "type": "line",
            "enabled": 12
        },
        "motorway": {
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "motorway",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "motorway_link": {
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "motorway_link",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "main": {
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "main",
            "join": "round",
            "cap": "round",
            "type": "line"
        },
        "street": {
            "source": "outdoors",
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
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "service",
            "join": "round",
            "cap": "round",
            "type": "line",
            "enabled": 15
        },
        "path": {
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "path",
            "type": "line"
        },
        "path_footway": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "path_path": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "path_cycleway": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "path_mtb": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "path_steps": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "path_piste": {
            "source": "outdoors",
            "layer": "road",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "major_rail": {
            "source": "outdoors",
            "layer": "road",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "tunnel_motorway": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "motorway",
            "type": "line"
        },
        "tunnel_motorway_link": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "motorway_link",
            "type": "line"
        },
        "tunnel_main": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "main",
            "type": "line"
        },
        "tunnel_street": {
            "source": "outdoors",
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
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "service",
            "type": "line",
            "enabled": 15
        },
        "tunnel_path": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "path",
            "type": "line"
        },
        "tunnel_path_footway": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "tunnel_path_path": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "tunnel_path_cycleway": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "tunnel_path_mtb": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "tunnel_path_steps": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "tunnel_path_piste": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "tunnel_major_rail": {
            "source": "outdoors",
            "layer": "tunnel",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "bridge_motorway": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "motorway",
            "type": "line"
        },
        "bridge_motorway_link": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "motorway_link",
            "type": "line"
        },
        "bridge_main": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "main",
            "type": "line"
        },
        "bridge_street": {
            "source": "outdoors",
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
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "service",
            "type": "line",
            "enabled": 15
        },
        "bridge_path": {
            "source": "outdoors",
            "layer": "bridge",
            "type": "line"
        },
        "bridge_path_footway": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "footway",
            "type": "line"
        },
        "bridge_path_path": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "path",
            "type": "line"
        },
        "bridge_path_cycleway": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "cycleway",
            "type": "line"
        },
        "bridge_path_mtb": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "mtb",
            "type": "line"
        },
        "bridge_path_steps": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "steps",
            "type": "line"
        },
        "bridge_path_piste": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "type",
            "value": "piste",
            "type": "line"
        },
        "bridge_major_rail": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "major_rail",
            "type": "line"
        },
        "bridge_aerialway": {
            "source": "outdoors",
            "layer": "bridge",
            "field": "class",
            "value": "aerialway",
            "type": "line"
        },
        "country_label": {
            "source": "outdoors",
            "layer": "country_label",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 24,
            "feature_type": "point",
            "type": "text"
        },
        "country_label_line": {
            "source": "outdoors",
            "layer": "country_label_line",
            "type": "line"
        },
        "marine_label_line": {
            "source": "outdoors",
            "layer": "marine_label",
            "feature_type": "line",
            "type": "text",
            "text_field": "name",
            "path": "curve",
            "fontSize": 16
        },
        "marine_label_point_1": {
            "source": "outdoors",
            "layer": "marine_label",
            "feature_type": "point",
            "type": "text",
            "text_field": "name",
            "path": "horizontal",
            "field": "labelrank",
            "value": 1,
            "fontSize": 30
        },
        "marine_label_point_2": {
            "source": "outdoors",
            "layer": "marine_label",
            "feature_type": "point",
            "type": "text",
            "text_field": "name",
            "path": "horizontal",
            "field": "labelrank",
            "value": 2,
            "fontSize": 24
        },
        "marine_label_point_other": {
            "source": "outdoors",
            "layer": "marine_label",
            "feature_type": "point",
            "type": "text",
            "text_field": "name",
            "path": "horizontal",
            "field": "labelrank",
            "value": [3,4,5,6],
            "fontSize": 18
        },
        "state_label": {
            "source": "outdoors",
            "layer": "state_label",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 16,
            "feature_type": "point",
            "type": "text",
            "enabled": 4
        },
        "place_label_city_point": {
            "source": "outdoors",
            "layer": "place_label",
            "field": "type",
            "value": "city",
            "type": "point",
            "size": 4
        },
        "place_label_city": {
            "source": "outdoors",
            "layer": "place_label",
            "field": "type",
            "value": "city",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 20,
            "feature_type": "point",
            "type": "text"
        },
        "place_label_town": {
            "source": "outdoors",
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
            "source": "outdoors",
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
            "source": "outdoors",
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
        "road_label": {
            "source": "outdoors",
            "layer": "road_label",
            "text_field": "name",
            "path": "curve",
            "padding": 2,
            "fontSize": 13,
            "feature_type": "line",
            "type": "text",
            "maxAngleDelta": 0.5
        },
        "water_label": {
            "source": "outdoors",
            "layer": "water_label",
            "text_field": "name",
            "path": "horizontal",
            "fontSize": 12,
            "feature_type": "point",
            "type": "text"
        },
        "waterway_label": {
            "source": "outdoors",
            "layer": "waterway_label",
            "text_field": "name",
            "path": "curve",
            "fontSize": 12,
            "textMinDistance": 15,
            "feature_type": "line",
            "type": "text"
        },
        "poi": {
            "source": "outdoors",
            "layer": "poi_label",
            "icon": "maki",
            "field": "scalerank",
            "value": [1, 2, 3],
            "size": 12,
            "type": "point"
        },
        "poi_aerodrome": {
            "source": "outdoors",
            "layer": "poi_label",
            "icon": "maki",
            "field": "maki",
            "value": "airport",
            "size": 24,
            "type": "point"
        },
        "poi_label": {
            "source": "outdoors",
            "layer": "poi_label",
            "field": "scalerank",
            "value": [
                1,
                2
            ],
            "text_field": "name",
            "path": "horizontal",
            "padding": 2,
            "fontSize": 12,
            "feature_type": "point",
            "type": "text",
            "alwaysVisible": true
        }
    },
    "structure": [
        {
            "name": "background",
            "bucket": "background"
        },
        {
            "name": "landcover_snow",
            "bucket": "landcover_snow"
        },
        {
            "name": "landcover_crop",
            "bucket": "landcover_crop"
        },
        {
            "name": "landcover_grass",
            "bucket": "landcover_grass"
        },
        {
            "name": "landcover_scrub",
            "bucket": "landcover_scrub"
        },
        {
            "name": "landcover_wood",
            "bucket": "landcover_wood"
        },
        {
            "name": "landuse_wood",
            "bucket": "landuse_wood"
        },
        {
            "name": "landuse_school",
            "bucket": "landuse_school"
        },
        {
            "name": "landuse_sand",
            "bucket": "landuse_sand"
        },
        {
            "name": "landuse_pitch",
            "bucket": "landuse_pitch"
        },
        {
            "name": "landuse_park",
            "bucket": "landuse_park"
        },
        {
            "name": "landuse_industrial",
            "bucket": "landuse_industrial"
        },
        {
            "name": "landuse_scrub",
            "bucket": "landuse_scrub"
        },
        {
            "name": "landuse_grass",
            "bucket": "landuse_grass"
        },
        {
            "name": "landuse_crop",
            "bucket": "landuse_crop"
        },
        {
            "name": "landuse_rock",
            "bucket": "landuse_rock"
        },
        {
            "name": "landuse_hospital",
            "bucket": "landuse_hospital"
        },
        {
            "name": "landuse_cemetery",
            "bucket": "landuse_cemetery"
        },
        {
            "name": "overlay_wetland",
            "bucket": "overlay_wetland"
        },
        {
            "name": "overlay_breakwater_pier",
            "bucket": "overlay_breakwater_pier"
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
            "name": "hillshade_full_highlight",
            "bucket": "hillshade_full_highlight"
        },
        {
            "name": "hillshade_medium_highlight",
            "bucket": "hillshade_medium_highlight"
        },
        {
            "name": "hillshade_medium_shadow",
            "bucket": "hillshade_medium_shadow"
        },
        {
            "name": "hillshade_full_shadow",
            "bucket": "hillshade_full_shadow"
        },
        {
            "name": "contour_line_loud",
            "bucket": "contour_line_10"
        },
        {
            "name": "contour_line_loud",
            "bucket": "contour_line_5"
        },
        {
            "name": "contour_line_regular",
            "bucket": "contour_line_other"
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
            "name": "barrier_line_land_fill",
            "bucket": "barrier_line_land_fill"
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
            "name": "admin_l3",
            "bucket": "admin_l3"
        },
        {
            "name": "admin_l2",
            "bucket": "admin_l2"
        },
        {
            "name": "admin_maritime_cover",
            "bucket": "admin_maritime"
        },
        {
            "name": "admin_maritime",
            "bucket": "admin_maritime"
        },
        {
            "name": "country_label_line",
            "bucket": "country_label_line"
        },
        {
            "name": "country_label",
            "bucket": "country_label"
        },
        {
            "name": "marine_label_line",
            "bucket": "marine_label_line"
        },
        {
            "name": "marine_label_point_1",
            "bucket": "marine_label_point_1"
        },
        {
            "name": "marine_label_point_2",
            "bucket": "marine_label_point_2"
        },
        {
            "name": "marine_label_point_other",
            "bucket": "marine_label_point_other"
        },
        {
            "name": "state_label",
            "bucket": "state_label"
        },
        {
            "name": "place_label_city_point",
            "bucket": "place_label_city_point"
        },
        {
            "name": "place_label_city",
            "bucket": "place_label_city"
        },
        {
            "name": "place_label_town",
            "bucket": "place_label_town"
        },
        {
            "name": "place_label_village",
            "bucket": "place_label_village"
        },
        {
            "name": "place_label_other",
            "bucket": "place_label_other"
        },
        {
            "name": "poi_label",
            "bucket": "poi_label"
        },
        {
            "name": "road_label",
            "bucket": "road_label"
        },
        {
            "name": "contour_label",
            "bucket": "contour_label"
        },
        {
            "name": "water_label",
            "bucket": "water_label"
        },
        {
            "name": "waterway_label",
            "bucket": "waterway_label"
        },
        {
            "name": "poi",
            "bucket": "poi"
        },
        {
            "name": "poi_aerodrome",
            "bucket": "poi_aerodrome"
        }
    ],
    "constants": {
        "land": "rgb(244,239,225)",
        "water": "#cdd",
        "water_dark": "#185869",
        "crop": "#eeeed4",
        "grass": "#e7ebd1",
        "scrub": "#e0e8cd",
        "wood": "#d4e2c6",
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
        "park_night": "#6ad0a1",
        "snow_night": "#5ad9fe",
        "rock_night": "#999",
        "sand_night": "#437162",
        "cemetery_night": "#218c96",
        "pitch_night": "rgba(255,255,255,0.2)",
        "school_night": "#01536a",
        "hospital_night": "#015e7a",
        "builtup_night": "#014b60",

        "text_night": "#fff",
        "text_stroke_night": "#103",

        "case_night": "#026986",
        "street_case_night": "#056585",
        "motorway_night": "#eee",
        "main_night": "#95ccdb",
        "street_night": "#0291b5",
        "contour_night": "#ffff80",

        "river_canal_width": [
            "stops",
            {"z": 11, "val": 0.5},
            {"z": 12, "val": 1},
            {"z": 14, "val": 2},
            {"z": 16, "val": 3}
        ],
        "stream_width": [
            "stops",
            {"z": 13, "val": 0.25},
            {"z": 14, "val": 0.5},
            {"z": 16, "val": 1.5},
            {"z": 18, "val": 2}
        ],
        "motorway_width": [
            "stops",
            {"z": 5, "val": 0},
            {"z": 6, "val": 0.5},
            {"z": 8, "val": 0.8},
            {"z": 10, "val": 1},
            {"z": 11, "val": 1.2},
            {"z": 12, "val": 2},
            {"z": 13, "val": 3},
            {"z": 14, "val": 4},
            {"z": 15, "val": 6},
            {"z": 16, "val": 9},
            {"z": 17, "val": 12},
            {"z": 18, "val": 14}
        ],
        "motorway_casing_width": [
            "stops",
            {"z": 7.5, "val": 0.6},
            {"z": 8, "val": 0.8},
            {"z": 10, "val": 2.8},
            {"z": 11, "val": 3},
            {"z": 12, "val": 4},
            {"z": 13, "val": 5},
            {"z": 14, "val": 6.5},
            {"z": 15, "val": 9},
            {"z": 16, "val": 12},
            {"z": 17, "val": 15},
            {"z": 18, "val": 17}
        ],
        "motorway_link_width": [
            "stops",
            {"z": 12, "val": 1.2},
            {"z": 14, "val": 2},
            {"z": 16, "val": 3},
            {"z": 18, "val": 4}
        ],
        "motorway_link_casing_width": [
            "stops",
            {"z": 12, "val": 2.8},
            {"z": 14, "val": 3.5},
            {"z": 16, "val": 5},
            {"z": 18, "val": 6}
        ],
        "main_width": [
            "stops",
            {"z": 5, "val": 1},
            {"z": 12, "val": 1},
            {"z": 13, "val": 1.5},
            {"z": 14, "val": 2},
            {"z": 15, "val": 3},
            {"z": 16, "val": 6},
            {"z": 17, "val": 10},
            {"z": 18, "val": 12}
        ],
        "main_casing_width": [
            "stops",
            {"z": 9, "val": 2.9},
            {"z": 12, "val": 2.9},
            {"z": 13, "val": 3.5},
            {"z": 14, "val": 4},
            {"z": 15, "val": 5.5},
            {"z": 16, "val": 9},
            {"z": 17, "val": 12},
            {"z": 18, "val": 14}
        ],
        "street_width": [
            "stops",
            {"z": 14.5, "val": 0},
            {"z": 15, "val": 1.5},
            {"z": 16, "val": 3},
            {"z": 17, "val": 8}
        ],
        "street_casing_width": [
            "stops",
            {"z": 13,"val": 0.4},
            {"z": 14, "val": 1},
            {"z": 15, "val": 2.5},
            {"z": 16, "val": 4},
            {"z": 17, "val": 10}
        ],
        "service_casing_width": [
            "stops",
            {"z": 14, "val": 0.5},
            {"z": 15, "val": 3},
            {"z": 16, "val": 3.5},
            {"z": 17, "val": 4},
            {"z": 18, "val": 5},
            {"z": 19, "val": 6}
        ],
        "runway_width": [
            "stops",
            {"z": 10, "val": 1},
            {"z": 11, "val": 2},
            {"z": 12, "val": 3},
            {"z": 13, "val": 5},
            {"z": 14, "val": 7},
            {"z": 15, "val": 11},
            {"z": 16, "val": 15},
            {"z": 17, "val": 19},
            {"z": 18, "val": 23}
        ],
        "taxiway_width": [
            "stops",
            {"z": 10, "val": 0.2},
            {"z": 12, "val": 0.2},
            {"z": 13, "val": 1},
            {"z": 14, "val": 1.5},
            {"z": 15, "val": 2},
            {"z": 16, "val": 3},
            {"z": 17, "val": 4},
            {"z": 18,"val": 5}
        ],
        "aerialway_width": [
            "stops",
            {"z": 13.5, "val": 0.8},
            {"z": 14, "val": 1.4},
            {"z": 15, "val": 1.6},
            {"z": 16, "val": 2},
            {"z": 17, "val": 2.4},
            {"z": 18, "val": 3}
        ],
        "aerialway_casing_width": [
            "stops",
            {"z": 13.5, "val": 2},
            {"z": 14, "val": 2.5},
            {"z": 15, "val": 3},
            {"z": 16, "val": 3.5},
            {"z": 17, "val": 4},
            {"z": 22, "val": 5}
        ],
        "path_width": [
            "stops",
            {"z": 14, "val": 1.2},
            {"z": 15, "val": 1.5},
            {"z": 16, "val": 1.8}
        ],
        "admin_l2_width": [
            "stops",
            {"z": 2, "val": 0.5},
            {"z": 3, "val": 0.7},
            {"z": 4, "val": 0.7},
            {"z": 5, "val": 0.8},
            {"z": 6, "val": 1},
            {"z": 8, "val": 2},
            {"z": 10, "val": 3}
        ],
        "admin_l3_width": [
            "stops",
            {"z": 6, "val": 0.6},
            {"z": 8, "val": 1},
            {"z": 12, "val": 2}
        ],
        "road_label_size": [
            "stops",
            {"z": 0, "val": 12},
            {"z": 14, "val": 12},
            {"z": 15, "val": 13},
            {"z": 22, "val": 13}
        ],
        "fence_width": [
            "stops",
            {"z": 17, "val": 0.6},
            {"z": 19, "val": 1}
        ],
        "hedge_width": [
            "stops",
            {"z": 16, "val": 0.6},
            {"z": 17, "val": 1.2},
            {"z": 19, "val": 1.6}
        ],
        "barrier_line_land_width": [
            "stops",
            {"z": 14, "val": 0.4},
            {"z": 15, "val": 0.75},
            {"z": 16, "val": 1.5},
            {"z": 17, "val": 3},
            {"z": 18, "val": 6},
            {"z": 19, "val": 12},
            {"z": 20, "val": 24},
            {"z": 21, "val": 48}
        ]
    },
    "classes": [
        {
            "name": "default",
            "layers": {
                "background": {
                    "transition-color": {"duration": 300},
                    "color": "land"
                },
                "admin_maritime_cover": {
                    "transition-color": {"duration": 300},
                    "color": "water",
                    "width": 5
                },
                "admin_maritime": {
                    "transition-color": {"duration": 300},
                    "color": "#c0d6d6",
                    "width": [
                        "stops",
                        {"z": 6, "val": 1},
                        {"z": 8, "val": 2},
                        {"z": 12, "val": 3}
                    ]
                },
                "admin_l2": {
                    "transition-color": {"duration": 300},
                    "color": "#88a",
                    "width": "admin_l2_width"
                },
                "admin_l3": {
                    "transition-color": {"duration": 300},
                    "color": "#88a",
                    "dasharray": [
                        60,
                        20
                    ],
                    "opacity": [
                        "stops",
                        {"z": 4, "val": 0},
                        {"z": 6, "val": 1}
                    ],
                    "width": "admin_l3_width"
                },
                "waterway_river_canal": {
                    "transition-color": {"duration": 300},
                    "color": "#87abaf",
                    "width": "river_canal_width"
                },
                "waterway_stream": {
                    "transition-color": {"duration": 300},
                    "color": "#87abaf",
                    "width": "stream_width"
                },
                "barrier_line_gate": {
                    "width": 2.5,
                    "color": "#aab"
                },
                "barrier_line_fence": {
                    "color": "#aeada3",
                    "width": "fence_width"
                },
                "barrier_line_hedge": {
                    "color": "#8de99b",
                    "width": "hedge_width"
                },
                "barrier_line_land": {
                    "color": "land",
                    "width": "barrier_line_land_width"
                },
                "barrier_line_land_fill": {
                    "color": "land"
                },
                "barrier_line_cliff": {
                    "color": "#987",
                    "width": 4
                },
                "landcover_wood": {
                    "transition-color": {"duration": 300},
                    "color": "wood",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_scrub": {
                    "transition-color": {"duration": 300},
                    "color": "scrub",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_grass": {
                    "transition-color": {"duration": 300},
                    "color": "grass",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_crop": {
                    "transition-color": {"duration": 300},
                    "color": "crop"
                },
                "landcover_snow": {
                    "transition-color": {"duration": 300},
                    "color": "snow"
                },
                "landuse_wood": {
                    "transition-color": {"duration": 300},
                    "color": "wood"
                },
                "landuse_scrub": {
                    "transition-color": {"duration": 300},
                    "color": "scrub"
                },
                "landuse_grass": {
                    "transition-color": {"duration": 300},
                    "color": "grass"
                },
                "landuse_crop": {
                    "transition-color": {"duration": 300},
                    "color": "crop"
                },
                "landuse_snow": {
                    "transition-color": {"duration": 300},
                    "color": "snow"
                },
                "landuse_rock": {
                    "transition-color": {"duration": 300},
                    "color": "rock"
                },
                "landuse_sand": {
                    "transition-color": {"duration": 300},
                    "color": "sand"
                },
                "landuse_park": {
                    "transition-color": {"duration": 300},
                    "color": "park"
                },
                "landuse_cemetery": {
                    "transition-color": {"duration": 300},
                    "color": "cemetery"
                },
                "landuse_hospital": {
                    "transition-color": {"duration": 300},
                    "color": "hospital"
                },
                "landuse_school": {
                    "transition-color": {"duration": 300},
                    "color": "school"
                },
                "landuse_pitch": {
                    "transition-color": {"duration": 300},
                    "color": "rgba(255,255,255,0.5)",
                    "stroke": "pitch"
                },
                "landuse_industrial": {
                    "transition-color": {"duration": 300},
                    "color": "rgba(246,250,255,0.5)"
                },
                "overlay_wetland": {
                    "transition-color": {"duration": 300},
                    "color": "rgba(210,225,225,0.2)",
                    "image": "wetland_noveg_64"
                },
                "overlay_breakwater_pier": {
                    "transition-color": {"duration": 300},
                    "color": "land"
                },
                "hillshade_full_shadow": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#103",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 15, "val": 0.075},
                        {"z": 17, "val": 0.05},
                        {"z": 18, "val": 0.025}
                    ]
                },
                "hillshade_medium_shadow": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#206",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 15, "val": 0.075},
                        {"z": 17, "val": 0.05},
                        {"z": 18, "val": 0.025}
                    ]
                },
                "hillshade_full_highlight": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#fffff3",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 15, "val": 0.3},
                        {"z": 17, "val": 0.2},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "hillshade_medium_highlight": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#ffd",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 15, "val": 0.3},
                        {"z": 17, "val": 0.2},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "contour_line_loud": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#008",
                    "width": 0.9,
                    "opacity": [
                        "stops",
                        {"z": 12, "val": 0.05},
                        {"z": 13, "val": 0.11}
                    ]
                },
                "contour_line_regular": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#008",
                    "width": 0.5,
                    "opacity": [
                        "stops",
                        {"z": 12, "val": 0.05},
                        {"z": 13, "val": 0.11}
                    ]
                },
                "water": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water",
                    "stroke": "#a2bdc0"
                },
                "aeroway_fill": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd"
                },
                "aeroway_runway": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd",
                    "width": "runway_width"
                },
                "aeroway_taxiway": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd",
                    "width": "taxiway_width"
                },
                "building": {
                    "transition-color": {"duration": 300},
                    "color": "#ebe7db"
                },
                "building_wall": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#ebe7db",
                    "stroke": "#d5d1c6",
                    "opacity": [
                        "stops",
                        {"z": 16.5, "val": 0},
                        {"z": 17, "val": 0.7}
                    ]
                },
                "building_shadow": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#d5d1c6",
                    "stroke": "#d5d1c6",
                    "translate": [
                        1,
                        1
                    ],
                    "opacity": [
                        "stops",
                        {"z": 16.5, "val": 0},
                        {"z": 17, "val": 1}
                    ]
                },
                "tunnel_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "tunnel_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "tunnel_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "tunnel_main": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "tunnel_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "motorway_link_casing_width"
                },
                "tunnel_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": "motorway_link_width"
                },
                "tunnel_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#d9d5c6",
                    "width": "street_casing_width"
                },
                "tunnel_street": {
                    "transition-color": {"duration": 300},
                    "color": "#d9d5c6",
                    "width": "street_width"
                },
                "tunnel_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "service_casing_width"
                },
                "tunnel_service": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": 2
                },
                "road_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "road_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "motorway",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "road_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "road_main": {
                    "transition-color": {"duration": 300},
                    "color": "main",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "road_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "motorway_link_casing_width"
                },
                "road_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "motorway",
                    "width": "motorway_link_width"
                },
                "road_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#d9d5c6",
                    "width": "street_casing_width"
                },
                "road_street": {
                    "transition-color": {"duration": 300},
                    "color": "street",
                    "width": "street_width"
                },
                "road_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "width": "service_casing_width"
                },
                "road_service": {
                    "transition-color": {"duration": 300},
                    "color": "street",
                    "width": 2
                },
                "road_path_case": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#ffd",
                    "opacity": 0.4,
                    "width": [
                        "stops",
                        {"z": 15, "val": 3},
                        {"z": 16, "val": 4}
                    ]
                },
                "road_path_footway": {
                    "transition-color": {"duration": 300},
                    "color": "#bba",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_path": {
                    "transition-color": {"duration": 300},
                    "color": "#987",
                    "dasharray": [
                        10,
                        4
                    ],
                    "opacity": 0.8,
                    "width": [
                        "stops",
                        {"z": 14, "val": 0.8},
                        {"z": 15, "val": 0.9},
                        {"z": 16, "val": 1.2}
                    ]
                },
                "road_path_cycleway": {
                    "transition-color": {"duration": 300},
                    "color": "#488",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_mtb": {
                    "transition-color": {"duration": 300},
                    "color": "#488",
                    "dasharray": [
                        12,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_piste": {
                    "transition-color": {"duration": 300},
                    "color": "#87b",
                    "dasharray": [
                        8,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_steps": {
                    "transition-color": {"duration": 300},
                    "color": "#bba",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": 4
                },
                "road_major_rail": {
                    "transition-color": {"duration": 300},
                    "color": "#c8c4c0",
                    "width": 0.8
                },
                "road_major_rail_hatching": {
                    "transition-color": {"duration": 300},
                    "color": "#c8c4c0",
                    "dasharray": [
                        2,
                        31
                    ],
                    "width": 5
                },
                "bridge_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "bridge_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "motorway",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "bridge_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "bridge_main": {
                    "transition-color": {"duration": 300},
                    "color": "main",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "bridge_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case",
                    "width": "motorway_link_casing_width"
                },
                "bridge_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "motorway",
                    "width": "motorway_link_width"
                },
                "bridge_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#d9d5c6",
                    "width": "street_casing_width"
                },
                "bridge_street": {
                    "transition-color": {"duration": 300},
                    "color": "street",
                    "width": "street_width"
                },
                "bridge_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "width": "service_casing_width"
                },
                "bridge_service": {
                    "transition-color": {"duration": 300},
                    "color": "street",
                    "width": 2
                },
                "bridge_aerialway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "white",
                    "opacity": 0.5,
                    "width": "aerialway_casing_width"
                },
                "bridge_aerialway": {
                    "transition-color": {"duration": 300},
                    "color": "#876",
                    "opacity": 0.5,
                    "width": "aerialway_width"
                },
                "country_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "country_text",
                    "stroke": "rgba(255,255,255,0.5)",
                    "size": [
                        "stops",
                        {"z": 6, "val": 14},
                        {"z": 12, "val": 24}
                    ]
                },
                "country_label_line": {
                    "transition-color": {"duration": 300},
                    "color": "country_text",
                    "width": 0.5,
                    "opacity": 0.5
                },
                "marine_label_line": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "marine_text",
                    "stroke": "water"
                },
                "marine_label_point_1": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#a9c4c7",
                    "size": ["stops",
                        {"z": 0, "val": 20},
                        {"z": 3, "val": 20},
                        {"z": 4, "val": 25},
                        {"z": 5, "val": 30},
                        {"z": 22, "val": 30}
                    ],
                    "stroke": "water"
                },
                "marine_label_point_2": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#a9c4c7",
                    "size": ["stops",
                        {"z": 0, "val": 13},
                        {"z": 3, "val": 13},
                        {"z": 4, "val": 14},
                        {"z": 5, "val": 20},
                        {"z": 6, "val": 24},
                        {"z": 22, "val": 24}
                    ],
                    "stroke": "water"
                },
                "marine_label_point_other": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#a9c4c7",
                    "size": ["stops",
                        {"z": 0, "val": 12},
                        {"z": 3, "val": 12},
                        {"z": 4, "val": 13},
                        {"z": 5, "val": 15},
                        {"z": 6, "val": 18},
                        {"z": 22, "val": 18}
                    ],
                    "stroke": "water"
                },
                "state_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#333",
                    "strokeWidth": 0.4,
                    "strokeBlur": 1,
                    "stroke": "rgba(244,239,225,0.8)",
                    "size": [
                        "stops",
                        {"z": 3.99, "val": 0},
                        {"z": 4, "val": 10},
                        {"z": 9.99, "val": 16},
                        {"z": 10, "val": 0}
                    ]
                },
                "place_label_city_point": {
                    "transition-color": {"duration": 300},
                    "color": "#4a4032"
                },
                "place_label_city": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#444",
                    "strokeWidth": 0.4,
                    "stroke": "text_stroke",
                    "size": [
                        "stops",
                        {"z": 3.99, "val": 0},
                        {"z": 4, "val": 10},
                        {"z": 7, "val": 14},
                        {"z": 14.99, "val": 20},
                        {"z": 15, "val": 0}
                    ],
                    "translate": [
                        0,
                        [
                            "stops",
                            {"z": 4, "val": 10},
                            {"z": 6, "val": 30},
                            {"z": 7, "val": 0}
                        ]
                    ]
                },
                "place_label_town": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#716656",
                    "strokeWidth": 0.5,
                    "stroke": "text_stroke",
                    "size": [
                        "stops",
                        {"z": 9, "val": 10},
                        {"z": 12, "val": 13},
                        {"z": 14, "val": 17},
                        {"z": 16, "val": 22}
                    ]
                },
                "place_label_village": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#635644",
                    "strokeWidth": 0.5,
                    "stroke": "text_stroke",
                    "size": [
                        "stops",
                        {"z": 9, "val": 8},
                        {"z": 12, "val": 10},
                        {"z": 14, "val": 14},
                        {"z": 16, "val": 16},
                        {"z": 17, "val": 20}
                    ]
                },
                "place_label_other": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#7d6c55",
                    "stroke": "text_stroke",
                    "size": [
                        "stops",
                        {"z": 0, "val": 10},
                        {"z": 14, "val": 11},
                        {"z": 15, "val": 12},
                        {"z": 16, "val": 14}
                    ]
                },
                "road_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#585042",
                    "stroke": "land",
                    "strokeWidth": 0.6,
                    "strokeBlur": 2,
                    "size": "road_label_size"
                },
                "water_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark",
                    "stroke": "rgba(255,255,255,0.75)"
                },
                "waterway_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark",
                    "strokeWidth": 0.4,
                    "strokeBlur": 2,
                    "stroke": "text_stroke"
                },
                "poi": {
                    "transition-color": {"duration": 300},
                    "color": "rgba(50,50,50,0.9)",
                    "antialias": false
                },
                "poi_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#444",
                    "size": 10,
                    "stroke": "land",
                    "strokeWidth": 0.6,
                    "strokeBlur": 2,
                    "translate": [0, 14]
                },
                "poi_aerodrome": {
                    "antialias": false
                }
            }
        },
        {
            "name": "satellite",
            "layers": {
                "background": {
                    "transition-color": {"duration": 300},
                    "color": "land_night"
                },
                "admin_maritime_cover": {
                    "transition-color": {"duration": 300},
                    "color": "water_night",
                    "width": 5
                },
                "admin_maritime": {
                    "transition-color": {"duration": 300},
                    "color": "water_night",
                    "width": [
                        "stops",
                        {"z": 6, "val": 1},
                        {"z": 8, "val": 2},
                        {"z": 12, "val": 3}
                    ]
                },
                "admin_l2": {
                    "transition-color": {"duration": 300},
                    "color": "#ccc",
                    "width": "admin_l2_width"
                },
                "admin_l3": {
                    "transition-color": {"duration": 300},
                    "color": "#ccc",
                    "dasharray": [
                        60,
                        20
                    ],
                    "opacity": [
                        "stops",
                        {"z": 4, "val": 0},
                        {"z": 6, "val": 1}
                    ],
                    "width": "admin_l3_width"
                },
                "waterway_river_canal": {
                    "transition-color": {"duration": 300},
                    "color": "rgb(10,20,71)",
                    "width": "river_canal_width"
                },
                "waterway_stream": {
                    "transition-color": {"duration": 300},
                    "color": "rgb(10,20,71)",
                    "width": "stream_width"
                },
                "barrier_line_gate": {
                    "width": 2.5,
                    "color": "#aab"
                },
                "barrier_line_fence": {
                    "color": "#024e64",
                    "width": "fence_width"
                },
                "barrier_line_hedge": {
                    "color": "#3aab79",
                    "width": "hedge_width"
                },
                "barrier_line_land": {
                    "color": "land_night",
                    "width": "barrier_line_land_width"
                },
                "barrier_line_land_fill": {
                    "color": "land_night"
                },
                "barrier_line_cliff": {
                    "color": "#63574b",
                    "width": 4
                },
                "landcover_wood": {
                    "transition-color": {"duration": 300},
                    "color": "wood_night",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_scrub": {
                    "transition-color": {"duration": 300},
                    "color": "scrub_night",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_grass": {
                    "transition-color": {"duration": 300},
                    "color": "grass_night",
                    "opacity": [
                        "stops",
                        {"z": 13, "val": 1},
                        {"z": 14, "val": 0.8},
                        {"z": 17, "val": 0.2}
                    ]
                },
                "landcover_crop": {
                    "transition-color": {"duration": 300},
                    "color": "crop_night"
                },
                "landcover_snow": {
                    "transition-color": {"duration": 300},
                    "color": "snow_night"
                },
                "landuse_wood": {
                    "transition-color": {"duration": 300},
                    "color": "wood_night",
                    "opacity": 0.8
                },
                "landuse_scrub": {
                    "transition-color": {"duration": 300},
                    "color": "scrub_night",
                    "opacity": 0.8
                },
                "landuse_grass": {
                    "transition-color": {"duration": 300},
                    "color": "grass_night",
                    "opacity": 0.8
                },
                "landuse_crop": {
                    "transition-color": {"duration": 300},
                    "color": "crop_night",
                    "opacity": 0.8
                },
                "landuse_snow": {
                    "transition-color": {"duration": 300},
                    "color": "snow_night",
                    "opacity": 0.8
                },
                "landuse_rock": {
                    "transition-color": {"duration": 300},
                    "color": "rock_night",
                    "opacity": 0.8
                },
                "landuse_sand": {
                    "transition-color": {"duration": 300},
                    "color": "sand_night",
                    "opacity": 0.8
                },
                "landuse_park": {
                    "transition-color": {"duration": 300},
                    "color": "park_night"
                },
                "landuse_cemetery": {
                    "transition-color": {"duration": 300},
                    "color": "cemetery_night"
                },
                "landuse_hospital": {
                    "transition-color": {"duration": 300},
                    "color": "hospital_night"
                },
                "landuse_school": {
                    "transition-color": {"duration": 300},
                    "color": "school_night"
                },
                "landuse_pitch": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "pitch_night",
                    "stroke": "pitch"
                },
                "landuse_industrial": {
                    "transition-color": {"duration": 300},
                    "color": "builtup_night"
                },
                "overlay_wetland": {
                    "transition-color": {"duration": 300},
                    "color": "rgba(210,225,225,0.2)",
                    "image": "wetland_noveg_64"
                },
                "overlay_breakwater_pier": {
                    "transition-color": {"duration": 300},
                    "color": "land_night"
                },
                "hillshade_full_shadow": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#103",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 0, "val": 0.3},
                        {"z": 17, "val": 0.2},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "hillshade_medium_shadow": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#206",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 0, "val": 0.3},
                        {"z": 17, "val": 0.2},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "hillshade_full_highlight": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#fdfdad",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 0, "val": 0.7},
                        {"z": 15, "val": 0.5},
                        {"z": 17, "val": 0.3},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "hillshade_medium_highlight": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "#ffb680",
                    "antialias": false,
                    "opacity": [
                        "stops",
                        {"z": 0, "val": 0.6},
                        {"z": 15, "val": 0.5},
                        {"z": 17, "val": 0.3},
                        {"z": 18, "val": 0.1}
                    ]
                },
                "contour_line_loud": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "contour_night",
                    "width": 0.9,
                    "opacity": [
                        "stops",
                        {"z": 12, "val": 0.1},
                        {"z": 13, "val": 0.4}
                    ]
                },
                "contour_line_regular": {
                    "transition-color": {"duration": 300},
                    "transition-opacity": {"duration": 300},
                    "color": "contour_night",
                    "width": 0.5,
                    "opacity": [
                        "stops",
                        {"z": 12, "val": 0.1},
                        {"z": 13, "val": 0.4}
                    ]
                },
                "water": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_night",
                    "stroke": "water_dark_night"
                },
                "aeroway_fill": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd"
                },
                "aeroway_runway": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd",
                    "width": "runway_width"
                },
                "aeroway_taxiway": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd",
                    "width": "taxiway_width"
                },
                "building": {
                    "transition-color": {"duration": 300},
                    "color": "#027797"
                },
                "building_wall": {
                    "transition-color": {"duration": 300},
                    "color": "#027797",
                    "stroke": "#026688",
                    "opacity": [
                        "stops",
                        {"z": 16.5, "val": 0},
                        {"z": 17, "val": 0.7}
                    ]
                },
                "building_shadow": {
                    "transition-color": {"duration": 300},
                    "color": "#026688",
                    "stroke": "#026688",
                    "translate": [
                        1,
                        1
                    ],
                    "opacity": [
                        "stops",
                        {"z": 16.5, "val": 0},
                        {"z": 17, "val": 1}
                    ]
                },
                "tunnel_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "tunnel_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "#83b9c8",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "tunnel_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "tunnel_main": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "tunnel_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "motorway_link_casing_width"
                },
                "tunnel_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": "motorway_link_width"
                },
                "tunnel_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "street_case_night",
                    "width": "street_casing_width"
                },
                "tunnel_street": {
                    "transition-color": {"duration": 300},
                    "color": "street_case_night",
                    "width": "street_width"
                },
                "tunnel_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "dasharray": [
                        6,
                        6
                    ],
                    "width": "service_casing_width"
                },
                "tunnel_service": {
                    "transition-color": {"duration": 300},
                    "color": "#e6cec7",
                    "width": 2
                },
                "road_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "road_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "motorway_night",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "road_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "road_main": {
                    "transition-color": {"duration": 300},
                    "color": "main_night",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "road_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "motorway_link_casing_width"
                },
                "road_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "motorway_night",
                    "width": "motorway_link_width"
                },
                "road_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "street_case_night",
                    "width": "street_casing_width"
                },
                "road_street": {
                    "transition-color": {"duration": 300},
                    "color": "street_night",
                    "width": "street_width"
                },
                "road_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "width": "service_casing_width"
                },
                "road_service": {
                    "transition-color": {"duration": 300},
                    "color": "street_night",
                    "width": 2
                },
                "road_path_case": {
                    "transition-opacity": {"duration": 300},
                    "opacity": 0
                },
                "road_path_footway": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_path": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        10,
                        4
                    ],
                    "opacity": 0.8,
                    "width": [
                        "stops",
                        {"z": 14, "val": 0.8},
                        {"z": 15, "val": 0.9},
                        {"z": 16, "val": 1.2}
                    ]
                },
                "road_path_cycleway": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_mtb": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        12,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_piste": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        8,
                        4
                    ],
                    "width": "path_width"
                },
                "road_path_steps": {
                    "transition-color": {"duration": 300},
                    "color": "#fff",
                    "dasharray": [
                        10,
                        4
                    ],
                    "width": 4
                },
                "road_major_rail": {
                    "transition-color": {"duration": 300},
                    "color": "#c8c4c0",
                    "width": 0.8
                },
                "road_major_rail_hatching": {
                    "transition-color": {"duration": 300},
                    "color": "#c8c4c0",
                    "dasharray": [
                        2,
                        31
                    ],
                    "width": 5
                },
                "bridge_motorway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "motorway_casing_width",
                    "opacity": [
                        "stops",
                        {"z": 9.5, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "bridge_motorway": {
                    "transition-color": {"duration": 300},
                    "color": "motorway_night",
                    "width": "motorway_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "bridge_main_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "main_casing_width",
                    "opacity": ["stops",
                        {"z": 9, "val": 0},
                        {"z": 10, "val": 1}
                    ]
                },
                "bridge_main": {
                    "transition-color": {"duration": 300},
                    "color": "main_night",
                    "width": "main_width",
                    "opacity": [
                        "stops",
                        {"z": 6.5, "val": 0},
                        {"z": 7, "val": 1}
                    ]
                },
                "bridge_motorway_link_casing": {
                    "transition-color": {"duration": 300},
                    "color": "case_night",
                    "width": "motorway_link_casing_width"
                },
                "bridge_motorway_link": {
                    "transition-color": {"duration": 300},
                    "color": "motorway_night",
                    "width": "motorway_link_width"
                },
                "bridge_street_casing": {
                    "transition-color": {"duration": 300},
                    "color": "street_case_night",
                    "width": "street_casing_width"
                },
                "bridge_street": {
                    "transition-color": {"duration": 300},
                    "color": "street_night",
                    "width": "street_width"
                },
                "bridge_service_casing": {
                    "transition-color": {"duration": 300},
                    "color": "#000",
                    "opacity": 0.04,
                    "width": "service_casing_width"
                },
                "bridge_service": {
                    "transition-color": {"duration": 300},
                    "color": "street_night",
                    "width": 2
                },
                "bridge_aerialway_casing": {
                    "transition-color": {"duration": 300},
                    "color": "white",
                    "opacity": 0.5,
                    "width": "aerialway_casing_width"
                },
                "bridge_aerialway": {
                    "transition-color": {"duration": 300},
                    "color": "#876",
                    "opacity": 0.5,
                    "width": "aerialway_width"
                },
                "country_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "text_night",
                    "stroke": "text_stroke_night",
                    "size": [
                        "stops",
                        {"z": 6, "val": 14},
                        {"z": 12, "val": 24}
                    ]
                },
                "country_label_line": {
                    "transition-color": {"duration": 300},
                    "color": "text_night",
                    "width": 0.5,
                    "opacity": 0.5
                },
                "marine_label_line": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark_night",
                    "stroke": "water_night"
                },
                "marine_label_point_1": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark_night",
                    "size": ["stops",
                        {"z": 0, "val": 20},
                        {"z": 3, "val": 20},
                        {"z": 4, "val": 25},
                        {"z": 5, "val": 30},
                        {"z": 22, "val": 30}
                    ],
                    "stroke": "water_night"
                },
                "marine_label_point_2": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark_night",
                    "size": ["stops",
                        {"z": 0, "val": 13},
                        {"z": 3, "val": 13},
                        {"z": 4, "val": 14},
                        {"z": 5, "val": 20},
                        {"z": 6, "val": 24},
                        {"z": 22, "val": 24}
                    ],
                    "stroke": "water_night"
                },
                "marine_label_point_other": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#a9c4c7",
                    "size": ["stops",
                        {"z": 0, "val": 12},
                        {"z": 3, "val": 12},
                        {"z": 4, "val": 13},
                        {"z": 5, "val": 15},
                        {"z": 6, "val": 18},
                        {"z": 22, "val": 18}
                    ],
                    "stroke": "water_night"
                },
                "state_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#fff",
                    "strokeWidth": 0.4,
                    "strokeBlur": 1,
                    "stroke": "land_night",
                    "size": [
                        "stops",
                        {"z": 3.99, "val": 0},
                        {"z": 4, "val": 10},
                        {"z": 9.99, "val": 16},
                        {"z": 10, "val": 0}
                    ]
                },
                "place_label_city_point": {
                    "transition-color": {"duration": 300},
                    "color": "#ddd"
                },
                "place_label_city": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#fff",
                    "strokeWidth": 0.4,
                    "stroke": "text_stroke_night",
                    "size": [
                        "stops",
                        {"z": 3.99, "val": 0},
                        {"z": 4, "val": 10},
                        {"z": 7, "val": 14},
                        {"z": 14.99, "val": 20},
                        {"z": 15, "val": 0}
                    ],
                    "translate": [
                        0,
                        [
                            "stops",
                            {"z": 4, "val": 10},
                            {"z": 6, "val": 30},
                            {"z": 7, "val": 0}
                        ]
                    ]
                },
                "place_label_town": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "text_night",
                    "strokeWidth": 0.5,
                    "stroke": "text_stroke_night",
                    "size": [
                        "stops",
                        {"z": 9, "val": 10},
                        {"z": 12, "val": 13},
                        {"z": 14, "val": 17},
                        {"z": 16, "val": 22}
                    ]
                },
                "place_label_village": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "text_night",
                    "strokeWidth": 0.5,
                    "stroke": "text_stroke_night",
                    "size": [
                        "stops",
                        {"z": 9, "val": 8},
                        {"z": 12, "val": 10},
                        {"z": 14, "val": 14},
                        {"z": 16, "val": 16},
                        {"z": 17, "val": 20}
                    ]
                },
                "place_label_other": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "text_night",
                    "stroke": "text_stroke_night",
                    "strokeWidth": 0.5,
                    "strokeBlur": 3,
                    "size": [
                        "stops",
                        {"z": 0, "val": 10},
                        {"z": 14, "val": 11},
                        {"z": 15, "val": 12},
                        {"z": 16, "val": 14}
                    ]
                },
                "road_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "text_night",
                    "stroke": "text_stroke_night",
                    "strokeWidth": 0.5,
                    "strokeBlur": 3,
                    "size": "road_label_size"
                },
                "water_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark_night",
                    "stroke": "text_stroke_night"
                },
                "waterway_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "water_dark_night",
                    "strokeWidth": 0.4,
                    "strokeBlur": 2,
                    "stroke": "water_night"
                },
                "poi": {
                    "transition-color": {"duration": 300},
                    "color": "white",
                    "antialias": false
                },
                "poi_label": {
                    "transition-color": {"duration": 300},
                    "transition-stroke": {"duration": 300},
                    "color": "#fff",
                    "size": 11,
                    "stroke": "land_night",
                    "strokeWidth": 0.4,
                    "strokeBlur": 0.2,
                    "translate": [0, 14]
                },
                "poi_aerodrome": {
                    "antialias": false
                }
            }
        }
    ],
    "sprite": "/img/maki-sprite"
};
