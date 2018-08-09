import mbgl from '../index';
import request from 'request';
import {PNG} from 'pngjs';
import * as fs from 'fs';
import * as path from 'path';

mbgl.on('message', function(msg) {
    console.log('%s (%s): %s', msg.severity, msg.class, msg.text);
});

// Map of map objects by pixel ratio
const maps = new Map();

export default function (style, options, callback) {
    const tileMode = options.mapMode === 'tile';
    let map;

    if (options.recycleMap) {
        const key = options.pixelRatio + '/' + tileMode;
        if (maps.has(key)) {
            map = maps.get(key);
            map.request = mapRequest;
        } else {
            maps.set(key, new mbgl.Map({
                ratio: options.pixelRatio,
                request: mapRequest,
                mode: options.mapMode
            }));
            map = maps.get(key);
        }
    } else {
        map = new mbgl.Map({
            ratio: options.pixelRatio,
            request: mapRequest,
            mode: options.mapMode
        });
    }

    let timedOut = false;
    const watchdog = setTimeout(function () {
        timedOut = true;
        map.dumpDebugLogs();
        callback(new Error('timed out after 20 seconds'));
    }, 20000);

    options.debug = {
        tileBorders: options.debug,
        collision: options.collisionDebug,
        overdraw: options.showOverdrawInspector,
    };

    options.center = style.center || [0, 0];
    options.zoom = style.zoom || 0;
    options.bearing = style.bearing || 0;
    options.pitch = style.pitch || 0;
    options.light = style.light || {};

    map.load(style, { defaultStyleCamera: true });

    function mapRequest(req, callback) {
        request(req.url, {encoding: null}, function (err, response, body) {
            if (err) {
                callback(err);
            } else if (response.statusCode === 404) {
                callback();
            } else if (response.statusCode !== 200) {
                callback(new Error(response.statusMessage));
            } else {
                callback(null, {data: body});
            }
        });
    }

    applyOperations(options.operations, function() {
        map.render(options, function (err, pixels) {
            const results = options.queryGeometry ?
              map.queryRenderedFeatures(options.queryGeometry, options.queryOptions || {}) :
              [];
            if (!options.recycleMap) {
                map.release();
            }
            if (timedOut) return;
            clearTimeout(watchdog);
            callback(err, pixels, results.map(prepareFeatures));
        });
    });

    function applyOperations(operations, callback) {
        const operation = operations && operations[0];
        if (!operations || operations.length === 0) {
            callback();

        } else if (operation[0] === 'wait') {
            map.render(options, function () {
                applyOperations(operations.slice(1), callback);
            });

        } else if (operation[0] === 'sleep') {
            // Prefer "wait", which renders until the map is loaded
            // Use "sleep" when you need to test something that sidesteps the "loaded" logic
            setTimeout(() => {
                applyOperations(operations.slice(1), callback);
            }, operation[1]);
        } else if (operation[0] === 'addImage' || operation[0] === 'updateImage') {
            const img = PNG.sync.read(fs.readFileSync(path.join(__dirname, '../../../mapbox-gl-js/test/integration', operation[2])));
            const testOpts = (operation.length > 3) ? operation[3] : {};

            const options = {
                height: img.height,
                width: img.width,
                pixelRatio: testOpts.pixelRatio || 1,
                sdf: testOpts.sdf || false
            };

            map.addImage(operation[1], img.data, options);

            applyOperations(operations.slice(1), callback);

        } else if (operation[0] === 'setStyle') {
            map.load(operation[1]);
            applyOperations(operations.slice(1), callback);

        } else {
            // Ensure that the next `map.render(options)` does not overwrite this change.
            if (operation[0] === 'setCenter') {
                options.center = operation[1];
            } else if (operation[0] === 'setZoom') {
                options.zoom = operation[1];
            } else if (operation[0] === 'setBearing') {
                options.bearing = operation[1];
            } else if (operation[0] === 'setPitch') {
                options.pitch = operation[1];
            } else if (operation[0] === 'setLight') {
                options.light = operation[1];
            }

            map[operation[0]].apply(map, operation.slice(1));
            applyOperations(operations.slice(1), callback);
        }
    }

    function prepareFeatures(r) {
        delete r.layer;
        return r;
    }
};
