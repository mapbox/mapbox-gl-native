#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fake_file_source.hpp>

#include <mbgl/storage/offline.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>
#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/string.hpp>

#include <gtest/gtest.h>
#include <iostream>

using namespace mbgl;
using namespace std::literals::string_literals;

class MockObserver : public OfflineRegionObserver {
public:
    void statusChanged(OfflineRegionStatus status) override {
        if (statusChangedFn) statusChangedFn(status);
    }

    void responseError(Response::Error error) override {
        if (responseErrorFn) responseErrorFn(error);
    }

    void mapboxTileCountLimitExceeded(uint64_t limit) override {
        if (mapboxTileCountLimitExceededFn) mapboxTileCountLimitExceededFn(limit);
    }

    std::function<void (OfflineRegionStatus)> statusChangedFn;
    std::function<void (Response::Error)> responseErrorFn;
    std::function<void (uint64_t)> mapboxTileCountLimitExceededFn;
};

class OfflineTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    OfflineDatabase db { ":memory:" };
    std::size_t size = 0;

    OfflineRegion createRegion() {
        OfflineRegionDefinition definition { "", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 1.0 };
        OfflineRegionMetadata metadata;
        return db.createRegion(definition, metadata);
    }

    Response response(const std::string& path) {
        Response result;
        result.data = std::make_shared<std::string>(util::read_file("test/fixtures/offline_download/"s + path));
        size_t uncompressed = result.data->size();
        size_t compressed = util::compress(*result.data).size();
        size += std::min(uncompressed, compressed);
        return result;
    }
};

TEST(OfflineDownload, NoSubresources) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("empty.style.json");
    };

    auto observer = std::make_unique<MockObserver>();

    bool expectsInactiveStatus = false;
    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            if (!expectsInactiveStatus) {
                expectsInactiveStatus = true;
                EXPECT_EQ(OfflineRegionDownloadState::Active, status.downloadState);
                EXPECT_EQ(1u, status.completedResourceCount);
                EXPECT_EQ(test.size, status.completedResourceSize);
                EXPECT_TRUE(status.requiredResourceCountIsPrecise);
            } else {
                EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
                EXPECT_EQ(1u, status.completedResourceCount);
                EXPECT_EQ(test.size, status.completedResourceSize);
                EXPECT_TRUE(status.requiredResourceCountIsPrecise);
                test.loop.stop();
            }
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, InlineSource) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("inline_source.style.json");
    };

    test.fileSource.tileResponse = [&] (const Resource& resource) {
        const Resource::TileData& tile = *resource.tileData;
        EXPECT_EQ("http://127.0.0.1:3000/{z}-{x}-{y}.vector.pbf", tile.urlTemplate);
        EXPECT_EQ(1, tile.pixelRatio);
        EXPECT_EQ(0, tile.x);
        EXPECT_EQ(0, tile.y);
        EXPECT_EQ(0, tile.z);
        return test.response("0-0-0.vector.pbf");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(2u, status.completedResourceCount);
            EXPECT_EQ(test.size, status.completedResourceSize);
            EXPECT_TRUE(status.requiredResourceCountIsPrecise);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, GeoJSONSource) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("geojson_source.style.json");
    };

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/geojson.json", resource.url);
        return test.response("geojson.json");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(2u, status.completedResourceCount);
            EXPECT_EQ(test.size, status.completedResourceSize);
            EXPECT_TRUE(status.requiredResourceCountIsPrecise);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, Activate) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("style.json");
    };

    test.fileSource.spriteImageResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/sprite.png", resource.url);
        return test.response("sprite.png");
    };

    test.fileSource.imageResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/radar.gif", resource.url);
        return test.response("radar.gif");
    };

    test.fileSource.spriteJSONResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/sprite.json", resource.url);
        return test.response("sprite.json");
    };

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        return test.response("glyph.pbf");
    };

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/streets.json", resource.url);
        return test.response("streets.json");
    };

    test.fileSource.tileResponse = [&] (const Resource& resource) {
        const Resource::TileData& tile = *resource.tileData;
        EXPECT_EQ("http://127.0.0.1:3000/{z}-{x}-{y}.vector.pbf", tile.urlTemplate);
        EXPECT_EQ(1, tile.pixelRatio);
        EXPECT_EQ(0, tile.x);
        EXPECT_EQ(0, tile.y);
        EXPECT_EQ(0, tile.z);
        return test.response("0-0-0.vector.pbf");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(262u, status.completedResourceCount); // 256 glyphs, 1 tile, 1 style, source, image, sprite image, and sprite json
            EXPECT_EQ(test.size, status.completedResourceSize);

            download.setState(OfflineRegionDownloadState::Inactive);
            OfflineRegionStatus computedStatus = download.getStatus();
            EXPECT_EQ(OfflineRegionDownloadState::Inactive, computedStatus.downloadState);
            EXPECT_EQ(status.requiredResourceCount, computedStatus.requiredResourceCount);
            EXPECT_EQ(status.completedResourceCount, computedStatus.completedResourceCount);
            EXPECT_EQ(status.completedResourceSize, computedStatus.completedResourceSize);
            EXPECT_EQ(status.completedTileCount, computedStatus.completedTileCount);
            EXPECT_EQ(status.completedTileSize, computedStatus.completedTileSize);
            EXPECT_TRUE(status.requiredResourceCountIsPrecise);

            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, DoesNotFloodTheFileSourceWithRequests) {
    FakeFileSource fileSource;
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, fileSource);

    auto observer = std::make_unique<MockObserver>();

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);
    test.loop.runOnce();

    EXPECT_EQ(1u, fileSource.requests.size());

    fileSource.respond(Resource::Kind::Style, test.response("style.json"));
    test.loop.runOnce();

    EXPECT_EQ(HTTPFileSource::maximumConcurrentRequests(), fileSource.requests.size());
}

TEST(OfflineDownload, GetStatusNoResources) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);
    OfflineRegionStatus status = download.getStatus();

    EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
    EXPECT_EQ(0u, status.completedResourceCount);
    EXPECT_EQ(0u, status.completedResourceSize);
    EXPECT_EQ(1u, status.requiredResourceCount);
    EXPECT_FALSE(status.requiredResourceCountIsPrecise);
    EXPECT_FALSE(status.complete());
}

TEST(OfflineDownload, GetStatusStyleComplete) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.db.putRegionResource(1,
        Resource::style("http://127.0.0.1:3000/style.json"),
        test.response("style.json"));

    OfflineRegionStatus status = download.getStatus();

    EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
    EXPECT_EQ(1u, status.completedResourceCount);
    EXPECT_EQ(test.size, status.completedResourceSize);
    EXPECT_EQ(261u, status.requiredResourceCount);
    EXPECT_FALSE(status.requiredResourceCountIsPrecise);
    EXPECT_FALSE(status.complete());
}

TEST(OfflineDownload, GetStatusStyleAndSourceComplete) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.db.putRegionResource(1,
        Resource::style("http://127.0.0.1:3000/style.json"),
        test.response("style.json"));

    test.db.putRegionResource(1,
        Resource::source("http://127.0.0.1:3000/streets.json"),
        test.response("streets.json"));

    OfflineRegionStatus status = download.getStatus();

    EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
    EXPECT_EQ(2u, status.completedResourceCount);
    EXPECT_EQ(test.size, status.completedResourceSize);
    EXPECT_EQ(262u, status.requiredResourceCount);
    EXPECT_TRUE(status.requiredResourceCountIsPrecise);
    EXPECT_FALSE(status.complete());
}

TEST(OfflineDownload, RequestError) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection, "connection error");
        return response;
    };

    auto observer = std::make_unique<MockObserver>();

    observer->responseErrorFn = [&] (Response::Error error) {
        EXPECT_EQ(Response::Error::Reason::Connection, error.reason);
        EXPECT_EQ("connection error", error.message);
        test.loop.stop();
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, RequestErrorsAreRetried) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource&) {
        test.fileSource.styleResponse = [&] (const Resource&) {
            return test.response("empty.style.json");
        };

        Response response;
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection, "connection error");
        return response;
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(1u, status.completedResourceCount);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, TileCountLimitExceededNoTileResponse) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    uint64_t tileLimit = 0;

    test.db.setOfflineMapboxTileCountLimit(tileLimit);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("mapbox_source.style.json");
    };

    auto observer = std::make_unique<MockObserver>();
    bool mapboxTileCountLimitExceededCalled = false;

    observer->mapboxTileCountLimitExceededFn = [&] (uint64_t limit) {
        EXPECT_FALSE(mapboxTileCountLimitExceededCalled);
        EXPECT_EQ(tileLimit, limit);
        mapboxTileCountLimitExceededCalled = true;
    };

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (!mapboxTileCountLimitExceededCalled) {
            EXPECT_FALSE(status.complete());
            EXPECT_EQ(OfflineRegionDownloadState::Active, status.downloadState);
        } else {
            EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, TileCountLimitExceededWithTileResponse) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    uint64_t tileLimit = 1;

    test.db.setOfflineMapboxTileCountLimit(tileLimit);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("mapbox_source.style.json");
    };

    test.fileSource.tileResponse = [&] (const Resource& resource) {
        const Resource::TileData& tile = *resource.tileData;
        EXPECT_EQ("mapbox://{z}-{x}-{y}.vector.pbf", tile.urlTemplate);
        EXPECT_EQ(1, tile.pixelRatio);
        EXPECT_EQ(0, tile.x);
        EXPECT_EQ(0, tile.y);
        EXPECT_EQ(0, tile.z);
        return test.response("0-0-0.vector.pbf");
    };

    auto observer = std::make_unique<MockObserver>();
    bool mapboxTileCountLimitExceededCalled = false;

    observer->mapboxTileCountLimitExceededFn = [&] (uint64_t limit) {
        EXPECT_FALSE(mapboxTileCountLimitExceededCalled);
        EXPECT_EQ(tileLimit, limit);
        mapboxTileCountLimitExceededCalled = true;
    };

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (!mapboxTileCountLimitExceededCalled) {
            EXPECT_EQ(OfflineRegionDownloadState::Active, status.downloadState);
        } else {
            EXPECT_EQ(OfflineRegionDownloadState::Inactive, status.downloadState);
            test.loop.stop();
        }
        if (status.completedResourceCount > tileLimit) {
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, WithPreviouslyExistingTile) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("inline_source.style.json");
    };

    test.db.put(
        Resource::tile("http://127.0.0.1:3000/{z}-{x}-{y}.vector.pbf", 1, 0, 0, 0, Tileset::Scheme::XYZ),
        test.response("0-0-0.vector.pbf"));

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(2u, status.completedResourceCount);
            EXPECT_EQ(test.size, status.completedResourceSize);
            EXPECT_TRUE(status.requiredResourceCountIsPrecise);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, ReactivatePreviouslyCompletedDownload) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("inline_source.style.json");
    };

    test.db.put(
        Resource::tile("http://127.0.0.1:3000/{z}-{x}-{y}.vector.pbf", 1, 0, 0, 0, Tileset::Scheme::XYZ),
        test.response("0-0-0.vector.pbf"));

    auto observer = std::make_unique<MockObserver>();
    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();

    OfflineDownload redownload(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    std::vector<OfflineRegionStatus> statusesAfterReactivate;

    observer = std::make_unique<MockObserver>();
    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        statusesAfterReactivate.push_back(status);
        if (status.complete()) {
            test.loop.stop();
        }
    };

    redownload.setObserver(std::move(observer));
    redownload.setState(OfflineRegionDownloadState::Active);

    test.loop.run();

    ASSERT_EQ(4u, statusesAfterReactivate.size());

    EXPECT_EQ(OfflineRegionDownloadState::Active, statusesAfterReactivate[0].downloadState);
    EXPECT_FALSE(statusesAfterReactivate[0].requiredResourceCountIsPrecise);
    EXPECT_EQ(1u, statusesAfterReactivate[0].requiredResourceCount);
    EXPECT_EQ(0u, statusesAfterReactivate[0].completedResourceCount);

    EXPECT_EQ(OfflineRegionDownloadState::Active, statusesAfterReactivate[1].downloadState);
    EXPECT_TRUE(statusesAfterReactivate[1].requiredResourceCountIsPrecise);
    EXPECT_EQ(2u, statusesAfterReactivate[1].requiredResourceCount);
    EXPECT_EQ(1u, statusesAfterReactivate[1].completedResourceCount);

    EXPECT_EQ(OfflineRegionDownloadState::Active, statusesAfterReactivate[2].downloadState);
    EXPECT_TRUE(statusesAfterReactivate[2].requiredResourceCountIsPrecise);
    EXPECT_EQ(2u, statusesAfterReactivate[2].requiredResourceCount);
    EXPECT_EQ(2u, statusesAfterReactivate[2].completedResourceCount);
}

TEST(OfflineDownload, Deactivate) {
    OfflineTest test;
    OfflineRegion region = test.createRegion();
    OfflineDownload download(
        region.getID(),
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/style.json", resource.url);
        return test.response("mapbox_source.style.json");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.downloadState == OfflineRegionDownloadState::Inactive) {
            test.loop.stop();
        } else {
            download.setState(OfflineRegionDownloadState::Inactive);
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}
