#include "../fixtures/util.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <cstdlib>

using namespace mbgl;

TEST(Transform, InvalidScale) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    const double invalid = std::nan("");
    transform.setScale(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.scaleBy(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setZoom(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setLatLngZoom({ 0, 0 }, invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
}

TEST(Transform, InvalidLatLng) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);
    transform.setLatLng({ 8, 10 });

    ASSERT_DOUBLE_EQ(8, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(10, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setLatLngZoom({ 10, 8 }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    const double invalid = std::nan("");
    transform.setLatLngZoom({ invalid, 8 }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    transform.setLatLngZoom({ 10, invalid }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());
}


TEST(Transform, InvalidBearing) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);
    transform.setAngle(2);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
    ASSERT_DOUBLE_EQ(2, transform.getAngle());

    const double invalid = std::nan("");
    transform.setAngle(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
    ASSERT_DOUBLE_EQ(2, transform.getAngle());
}
