#!/usr/bin/env node

'use strict';

var style = require('./style.js');
var Protobuf = require('./protobuf.js');

// var fs = require('fs');

var pbf = new Protobuf();

// enum
var bucket_type = {
    fill: 1,
    line: 2,
    point: 3
};

// enum
var cap_type = {
    round: 1,
};

// enum
var join_type = {
    butt: 1,
    bevel: 2
};


function createValue(value) {
    var pbf = new Protobuf();

    if (typeof value === 'string') {
        pbf.writeTaggedString(1 /* string_value */, value);
    } else if (typeof value === 'boolean') {
        pbf.writeTaggedBoolean(7 /* bool_value */, value);
    } else {
        // TODO:
        console.warn('encode value: %s', value);
        process.exit();
    }

    return pbf;
}

function createBucket(bucket, name) {
    var pbf = new Protobuf();
    pbf.writeTaggedString(1 /* name */, name);
    pbf.writeTaggedVarint(2 /* type */, bucket_type[bucket.type]);

    pbf.writeTaggedString(3 /* source_name */, bucket.datasource);
    pbf.writeTaggedString(4 /* source_layer */, bucket.layer);
    if (bucket.field) {
        pbf.writeTaggedString(5 /* source_field */, bucket.field);
        var values = Array.isArray(bucket.value) ? bucket.value : [bucket.value];
        for (var i = 0; i < values.length; i++) {
            pbf.writeMessage(6 /* source_value */, createValue(values[i]));
        }
    }
    if (bucket.cap) {
        pbf.writeTaggedVarint(7 /* cap */, cap_type[bucket.cap]);
    }
    if (bucket.join) {
        pbf.writeTaggedVarint(8 /* join */, join_type[bucket.join]);
    }

    // TODO: font stuff

    return pbf;
}

function createStructure(structure) {
    var pbf = new Protobuf();
    pbf.writeTaggedString(1 /* name */, structure.name);
    if (structure.bucket) {
        pbf.writeTaggedString(2 /* bucket_name */, structure.bucket);
    } else if (structure.layers) {
        for (var i = 0; i < structure.layers.length; i++) {
            pbf.writeMessage(3 /* child_layer */, createStructure(structure.layers[i]));
        }
    }
    return pbf;
}

function createWidth(width) {
    var pbf = new Protobuf();
    var values = [];
    if (Array.isArray(width)) {
        pbf.writeTaggedString(1 /* scaling */, width[0]);
        for (var i = 1; i < width.length; i++) {
            if (width[0] === 'stops') {
                values.push(width[i].z, width[i].val);
            } else {
                values.push(width[i]);
            }
        }
    } else {
        values.push(width);
    }
    pbf.writePackedFloats(2 /* value */, values);

    return pbf;
}

function createLayer(layer, name) {
    var pbf = new Protobuf();
    pbf.writeTaggedString(1 /* layer_name */, name);

    if ('color' in layer) {
        var color = layer.color.match(/^#([0-9a-f]{6})$/i);
        if (!color) {
            console.warn('invalid color');
        } else {
            pbf.writeTaggedUInt32(2 /* color */, parseInt(color[1] + 'ff', 16));
        }
    }

    if ('antialias' in layer) {
        pbf.writeTaggedBoolean(3 /* antialias */, layer.antialias);
    }

    if ('width' in layer) {
        pbf.writeMessage(4 /* width */, createWidth(layer.width));
    }

    return pbf;
}


function createClass(klass) {
    var pbf = new Protobuf();
    pbf.writeTaggedString(1 /* name */, klass.name);
    for (var name in klass.layers) {
        pbf.writeMessage(2 /* layer */, createLayer(klass.layers[name], name));
    }
    return pbf;
}





for (var name in style.buckets) {
    var bucket = style.buckets[name];
    pbf.writeMessage(1 /* bucket */, createBucket(bucket, name));
}

for (var i = 0; i < style.structure.length; i++) {
    var structure = style.structure[i];
    pbf.writeMessage(2 /* structure */, createStructure(structure));
}

for (var i = 0; i < style.classes.length; i++) {
    var klass = style.classes[i];
    pbf.writeMessage(3 /* class */, createClass(klass));
}

process.stdout.write(pbf.finish());
