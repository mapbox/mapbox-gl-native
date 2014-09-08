#ifndef MBGL_GEOMETRY_POINT
#define MBGL_GEOMETRY_POINT

#include <cmath>

namespace mbgl {

class Point {
public:
    typedef std::array<double, 4> mat2;

    double x = 0;
    double y = 0;

    Point() {}
    Point(double x, double y) : x(x), y(y) {}

    Point clone() const { return Point(x, y); }

    Point add(const Point& p)     const { return clone()._add(p);     }
    Point sub(const Point& p)     const { return clone()._sub(p);     }
    Point mult   (double k)       const { return clone()._mult(k);    }
    Point div    (double k)       const { return clone()._div(k);     }
    Point rotate (double a)       const { return clone()._rotate(a);  }
    Point matMult(const mat2& m)  const { return clone()._matMult(m); }
    Point unit()                  const { return clone()._unit();     }
    Point perp()                  const { return clone()._perp();     }
    Point round()                 const { return clone()._round();    }

    double mag() const {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    bool operator==(const Point& p) const {
        return this->x == p.x &&
               this->y == p.y;
    }

    double dist(const Point& p) const {
        return std::sqrt(distSqr(p));
    }

    double distSqr(const Point& p) const {
        double dx = p.x - this->x,
               dy = p.y - this->y;
        return dx * dx + dy * dy;
    }

    double angle() const {
        return std::atan2(this->y, this->x);
    }

    double angleTo(const Point& p) const {
        return std::atan2(this->y - p.y, this->x - p.x);
    }

    double angleWith(const Point& p) const {
        return angleWithSep(p.x, p.y);
    }

    // Find the angle of the two vectors, solving the formula for the cross product a x b = |a||b|sin(θ) for θ.
    double angleWithSep(double x, double y) const {
        return std::atan2(
            this->x * y - this->y * x,
            this->x * x + this->y * y);
    }

    Point& _matMult(const mat2& m) {
        double x = m[0] * this->x + m[1] * this->y,
               y = m[2] * this->x + m[3] * this->y;
        this->x = x;
        this->y = y;
        return *this;
    }

    Point& _add(const Point& p) {
        this->x += p.x;
        this->y += p.y;
        return *this;
    }

    Point& _sub(const Point& p) {
        this->x -= p.x;
        this->y -= p.y;
        return *this;
    }

    Point& _mult(double k) {
        this->x *= k;
        this->y *= k;
        return *this;
    }

    Point& _div(double k) {
        this->x /= k;
        this->y /= k;
        return *this;
    }

    Point& _unit() {
        _div(mag());
        return *this;
    }

    Point& _perp() {
        double y = this->y;
        this->y = this->x;
        this->x = -y;
        return *this;
    }

    Point& _rotate(double angle) {
        double cos = std::cos(angle),
               sin = std::sin(angle),
               x = cos * this->x - sin * this->y,
               y = sin * this->x + cos * this->y;
        this->x = x;
        this->y = y;
        return *this;
    }

    Point& _round() {
        this->x = std::round(this->x);
        this->y = std::round(this->y);
        return *this;
    }
};

}

#endif
