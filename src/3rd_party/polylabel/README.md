## polylabel [![Build Status](https://travis-ci.org/mapbox/polylabel.svg?branch=master)](https://travis-ci.org/mapbox/polylabel)

A fast algorithm for finding polygon _pole of inaccessibility_,
the most distant internal point from the polygon outline (not to be confused with centroid),
implemented as a JavaScript library.
Useful for optimal placement of a text label on a polygon.

It's an iterative grid algorithm,
inspired by [paper by Garcia-Castellanos & Lombardo, 2007](https://sites.google.com/site/polesofinaccessibility/).
Unlike the one in the paper, this algorithm:

- guarantees finding **global optimum** within the given precision
- is many times faster (10-40x)

![](https://cloud.githubusercontent.com/assets/25395/16745865/864a0a30-47c0-11e6-87bc-58acac41a520.png)

### How the algorithm works

This is an iterative grid-based algorithm, which starts by covering the polygon with big square cells and then iteratively splitting them in the order of the most promising ones, while aggressively pruning uninteresting cells.

1. Generate initial square cells that fully cover the polygon (with cell size equal to either width or height, whichever is lower). Calculate distance from the center of each cell to the outer polygon, using negative value if the point is outside the polygon (detected by ray-casting).
2. Put the cells into a priority queue sorted by the maximum potential distance from a point inside a cell, defined as a sum of the distance from the center and the cell radius (equal to `cell_size * sqrt(2) / 2`).
3. Calculate the distance from the centroid of the polygon and pick it as the first "best so far".
4. Pull out cells from the priority queue one by one. If a cell's distance is better than the current best, save it as such.
Then, if the cell potentially contains a better solution that the current best (`cell_max - best_dist > precision`),
split it into 4 children cells and put them in the queue.
5. Stop the algorithm when we have exhausted the queue and return the best cell's center as the pole of inaccessibility.
It will be guaranteed to be a global optimum within the given precision.

![image](https://cloud.githubusercontent.com/assets/25395/16748630/e6b3336c-47cd-11e6-8059-0eeccf22cf6b.png)

### JavaScript Usage

Given polygon coordinates in
[GeoJSON-like format](http://geojson.org/geojson-spec.html#polygon)
and precision (`1.0` by default),
Polylabel returns the pole of inaccessibility coordinate in `[x, y]` format.

```js
var p = polylabel(polygon, 1.0);
```

### TypeScript

[TypeScript type definitions](https://github.com/DefinitelyTyped/DefinitelyTyped/tree/master/concaveman)
are available via `npm install --save @types/polylabel`.

### C++ Usage

It is recommended to install polylabel via [mason](https://github.com/mapbox/mason). You will also need to install its dependencies: [geometry.hpp](https://github.com/mapbox/geometry.hpp) and [variant](https://github.com/mapbox/variant).

```C++
#include <mapbox/polylabel.hpp>

int main() {
    mapbox::geometry::polygon<double> polygon = readPolygon(); // Get polygon data from somewhere.
    mapbox::geometry::point<double> p = mapbox::polylabel(polygon, 1.0);
    return 0;
}
```
