package com.mapbox.mapboxsdk.style.sources;

import java.util.HashMap;

public abstract class Source extends HashMap<String, Object> {
    private final String id;

    protected Source(String id, String type) {
        this.put("type", type);
        this.id = id;
    }

    public String getId() {
        return id;
    }
}
