module.exports = Protobuf;
function Protobuf(buf, pos) {
    if (Buffer.isBuffer(buf)) {
        this.buf = buf;
    } else {
        this.buf = new Buffer(buf || 128);
    }
    this.pos = pos || 0;
    this.length = this.buf.length;
}

// === READING =================================================================

Protobuf.prototype.readUInt32 = function() {
    var val = this.readUInt32LE(this.pos);
    this.pos += 4;
    return val;
};

Protobuf.prototype.readUInt64 = function() {
    var val = this.readUInt64LE(this.pos);
    this.pos += 4;
    return val;
};

Protobuf.prototype.readVarint = function() {
    // TODO: bounds checking
    var pos = this.pos;
    if (this.buf[pos] <= 0x7f) {
        this.pos++;
        return this.buf[pos];
    } else if (this.buf[pos + 1] <= 0x7f) {
        this.pos += 2;
        return (this.buf[pos] & 0x7f) | (this.buf[pos + 1] << 7);
    } else if (this.buf[pos + 2] <= 0x7f) {
        this.pos += 3;
        return (this.buf[pos] & 0x7f) | (this.buf[pos + 1] & 0x7f) << 7 | (this.buf[pos + 2]) << 14;
    } else if (this.buf[pos + 3] <= 0x7f) {
        this.pos += 4;
        return (this.buf[pos] & 0x7f) | (this.buf[pos + 1] & 0x7f) << 7 | (this.buf[pos + 2] & 0x7f) << 14 | (this.buf[pos + 3]) << 21;
    } else if (this.buf[pos + 4] <= 0x7f) {
        this.pos += 5;
        return ((this.buf[pos] & 0x7f) | (this.buf[pos + 1] & 0x7f) << 7 | (this.buf[pos + 2] & 0x7f) << 14 | (this.buf[pos + 3]) << 21) + (this.buf[pos + 4] * 268435456);
    } else {
        throw new Error("TODO: Handle 6+ byte varints");
    }
};

Protobuf.prototype.readSVarint = function() {
    var num = this.readVarint();
    if (num > 2147483647) throw new Error('TODO: Handle numbers >= 2^30');
    // zigzag encoding
    return ((num >> 1) ^ -(num & 1));
};

Protobuf.prototype.readString = function() {
    var bytes = this.readVarint();
    // Node.js
    var str = this.buf.toString('utf8', this.pos, this.pos + bytes);
    // Client side JS code:
    // // TODO: bounds checking
    // var chr = String.fromCharCode;
    // var b = this.buf;
    // var p = this.pos;
    // var end = this.pos;
    // var str = '';
    // while (p < end) {
    //     if (b[p] <= 0x7F) str += chr(b[p++]);
    //     else if (b[p] <= 0xBF) throw new Error('Invalid UTF-8 codepoint: ' + b[p]);
    //     else if (b[p] <= 0xDF) str += chr((b[p++] & 0x1F) << 6 | (b[p++] & 0x3F));
    //     else if (b[p] <= 0xEF) str += chr((b[p++] & 0x1F) << 12 | (b[p++] & 0x3F) << 6 | (b[p++] & 0x3F));
    //     else if (b[p] <= 0xF7) p += 4; // We can't handle these codepoints in JS, so skip.
    //     else if (b[p] <= 0xFB) p += 5;
    //     else if (b[p] <= 0xFD) p += 6;
    //     else throw new Error('Invalid UTF-8 codepoint: ' + b[p]);
    // }
    this.pos += bytes;
    return str;
};

Protobuf.prototype.readPacked = function(type) {
    // TODO: bounds checking
    var bytes = this.readVarint();
    var end = this.pos + bytes;
    var array = [];
    while (this.pos < end) {
        array.push(this['read' + type]());
    }
    return array;
};

Protobuf.prototype.skip = function(val) {
    // TODO: bounds checking
    var type = val & 0x7;
    switch (type) {
        /* varint */ case 0: while (this.buf[this.pos++] > 0x7f); break;
        /* 64 bit */ case 1: this.pos += 8; break;
        /* length */ case 2: var bytes = this.readVarint(); this.pos += bytes; break;
        /* 32 bit */ case 5: this.pos += 4; break;
        default: throw new Error('Unimplemented type: ' + type);
    }
};

// Generic read function
Protobuf.prototype.read = function(spec, end) {
    var ref, tag;
    var obj = {};
    for (tag in spec) {
        ref = spec[tag];
        if ('default' in ref) {
            obj[ref.name] = ref['default'];
        }

        if ('repeated' in ref) {
            obj[ref.name] = [];
        }
    }

    var begin = this.pos;
    if (typeof end == 'undefined') {
        var bytes = this.readVarint();
        end = this.pos + bytes;
    }

    while (this.pos < end) {
        var val = this.readVarint();
        tag = val >> 3;
        ref = spec[tag];
        if (ref) {
            if (ref.packed) {
                obj[ref.name] = this.readPacked(ref.type);
            }
            else if (ref.repeated) {
                if (!obj[ref.name]) obj[ref.name] = [];
                obj[ref.name].push(this['read' + ref.type]());
            } else {
                obj[ref.name] = this['read' + ref.type]();
            }
        } else {
            this.skip(val);
        }
    }

    if (this._debug) {
        obj._begin = begin;
        obj._end = end;
        obj._bytes = obj._end - obj._begin;
    }

    for (tag in spec) {
        ref = spec[tag];
        if (!(ref.name in obj) && ('required' in ref)) {
            throw new Error('Field ' + ref.name + ' is required');
        }
    }

    return obj;
};

// === WRITING =================================================================

Protobuf.Varint = 0;
Protobuf.Int64 = 1;
Protobuf.Message = 2;
Protobuf.String = 2;
Protobuf.Packed = 2;
Protobuf.Int32 = 5;

Protobuf.prototype.writeTag = function(tag, type) {
    this.writeVarint((tag << 3) | type);
};

Protobuf.prototype.writeUInt32 = function(val) {
    while (this.length < this.pos + 4) this.realloc();
    this.buf.writeUInt32LE(val, this.pos);
    this.pos += 4;
};

Protobuf.prototype.writeTaggedUInt32 = function(tag, val) {
    this.writeTag(tag, Protobuf.Int32);
    this.writeUInt32(val);
};

Protobuf.prototype.writeVarint = function(val) {
    val = Number(val);
    if (isNaN(val)) {
        val = 0;
    }

    if (val <= 0x7f) {
        while (this.length < this.pos + 1) this.realloc();
        this.buf[this.pos++] = val;
    } else if (val <= 0x3fff) {
        while (this.length < this.pos + 2) this.realloc();
        this.buf[this.pos++] = 0x80 | ((val >>> 0) & 0x7f);
        this.buf[this.pos++] = 0x00 | ((val >>> 7) & 0x7f);
    } else if (val <= 0x1ffffff) {
        while (this.length < this.pos + 3) this.realloc();
        this.buf[this.pos++] = 0x80 | ((val >>> 0) & 0x7f);
        this.buf[this.pos++] = 0x80 | ((val >>> 7) & 0x7f);
        this.buf[this.pos++] = 0x00 | ((val >>> 14) & 0x7f);
    } else if (val <= 0xfffffff) {
        while (this.length < this.pos + 4) this.realloc();
        this.buf[this.pos++] = 0x80 | ((val >>> 0) & 0x7f);
        this.buf[this.pos++] = 0x80 | ((val >>> 7) & 0x7f);
        this.buf[this.pos++] = 0x80 | ((val >>> 14) & 0x7f);
        this.buf[this.pos++] = 0x00 | ((val >>> 21) & 0x7f);
    } else {
        throw new Error("TODO: Handle 5+ byte varints (" + val + ")");
    }
};

Protobuf.prototype.writeTaggedVarint = function(tag, val) {
    this.writeTag(tag, Protobuf.Varint);
    this.writeVarint(val);
};

Protobuf.prototype.writeBoolean = function(val) {
    this.writeVarint(Boolean(val));
};

Protobuf.prototype.writeTaggedBoolean = function(tag, val) {
    this.writeTaggedVarint(tag, Boolean(val));
};

Protobuf.prototype.writeString = function(str) {
    str = String(str);
    var bytes = Buffer.byteLength(str);
    this.writeVarint(bytes);
    while (this.length < this.pos + bytes) this.realloc();
    this.buf.write(str, this.pos);
    this.pos += bytes;
};

Protobuf.prototype.writeTaggedString = function(tag, str) {
    this.writeTag(tag, Protobuf.String);
    this.writeString(str);
};

Protobuf.prototype.writeFloat = function(val) {
    while (this.length < this.pos + 4) this.realloc();
    this.buf.writeFloatLE(val, this.pos);
    this.pos += 4;
};

Protobuf.prototype.writeTaggedFloat = function(tag, val) {
    this.writeTag(tag, Protobuf.Int32);
    this.writeFloat(val);
};

Protobuf.prototype.writeDouble = function(val) {
    while (this.length < this.pos + 8) this.realloc();
    this.buf.writeDoubleLE(val, this.pos);
    this.pos += 8;
};

Protobuf.prototype.writeTaggedDouble = function(tag, val) {
    this.writeTag(tag, Protobuf.Int64);
    this.writeDouble(val);
};

Protobuf.prototype.writeBuffer = function(buffer) {
    var bytes = buffer.length;
    this.writeVarint(bytes);
    while (this.length < this.pos + bytes) this.realloc();
    buffer.copy(this.buf, this.pos);
    this.pos += bytes;
};

Protobuf.prototype.writeMessage = function(tag, protobuf) {
    var buffer = protobuf.finish();
    this.writeTag(tag, Protobuf.Message);
    this.writeBuffer(buffer);
};

Protobuf.prototype.writeRepeated = function(type, tag, items) {
    for (var i = 0; i < items.length; i++) {
        this['write' + type](tag, items[i]);
    }
};

Protobuf.prototype.writePacked = function(type, tag, items) {
    if (!items.length) return;

    var message = new Protobuf();
    for (var i = 0; i < items.length; i++) {
        message['write' + type](items[i]);
    }
    var data = message.finish();

    this.writeTag(tag, Protobuf.Packed);
    this.writeBuffer(data);
};

Protobuf.prototype.writePackedVarints = function(tag, items) {
    this.writePacked('Varint', tag, items);
};

Protobuf.prototype.writePackedFloats = function(tag, items) {
    this.writePacked('Float', tag, items);
};

Protobuf.prototype.realloc = function() {
    var buf = new Buffer(this.buf.length * 2);
    this.buf.copy(buf);
    this.buf = buf;
    this.length = this.buf.length;
};

Protobuf.prototype.finish = function() {
    return this.buf.slice(0, this.pos);
};
