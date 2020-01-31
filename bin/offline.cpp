#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/geojson.hpp>

#include <mbgl/storage/default_file_source.hpp>

#include <args.hxx>

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <atomic>
#include <sstream>
#include <fstream>

using namespace std::literals::chrono_literals;

std::string readFile(const std::string& fileName) {
    std::ifstream stream(fileName.c_str());
    if (!stream.good()) {
        throw std::runtime_error("Cannot read file: " + fileName);
    }
    
    std::stringstream buffer;
    buffer << stream.rdbuf();
    stream.close();
    
    return buffer.str();
}

mapbox::geometry::geometry<double> parseGeometry(const std::string& json) {
    using namespace mapbox::geojson;
    auto geojson = parse(json);
    return geojson.match(
        [](const geometry& geom) {
            return geom;
        },
        [](const feature& feature) {
            return feature.geometry;
        },
        [](const feature_collection& featureCollection) {
            if (featureCollection.size() < 1) {
                throw std::runtime_error("No features in feature collection");
            }
            geometry_collection geometries;
            
            for (auto feature : featureCollection) {
                geometries.push_back(feature.geometry);
            }
            
            return geometries;
        });
}

std::ostream& operator<<(std::ostream& os, mbgl::Response::Error::Reason r) {
    switch (r) {
    case mbgl::Response::Error::Reason::Success:
        return os << "Response::Error::Reason::Success";
    case mbgl::Response::Error::Reason::NotFound:
        return os << "Response::Error::Reason::NotFound";
    case mbgl::Response::Error::Reason::Server:
        return os << "Response::Error::Reason::Server";
    case mbgl::Response::Error::Reason::Connection:
        return os << "Response::Error::Reason::Connection";
    case mbgl::Response::Error::Reason::RateLimit:
        return os << "Response::Error::Reason::RateLimit";
    case mbgl::Response::Error::Reason::Other:
        return os << "Response::Error::Reason::Other";
    }

    // The above switch is exhaustive, but placate GCC nonetheless:
    assert(false);
    return os;
}

int main(int argc, char *argv[]) {
    args::ArgumentParser argumentParser("Mapbox GL offline tool");
    args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {'h', "help"});

    args::ValueFlag<std::string> tokenValue(argumentParser, "key", "Mapbox access token", {'t', "token"});
    args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
    args::ValueFlag<std::string> outputValue(argumentParser, "file", "Output database file name", {'o', "output"});
    args::ValueFlag<std::string> apiBaseValue(argumentParser, "URL", "API Base URL", {'a', "apiBaseURL"});
    
    args::Group mergeGroup(argumentParser, "Merge databases:", args::Group::Validators::AllOrNone);
    args::ValueFlag<std::string> mergePathValue(mergeGroup, "merge", "Database to merge from", {'m', "merge"});
    args::ValueFlag<std::string> inputValue(mergeGroup, "input", "Database to merge into. Use with --merge option.", {'i', "input"});

    // LatLngBounds
    args::Group latLngBoundsGroup(argumentParser, "LatLng bounds:", args::Group::Validators::AllOrNone);
    args::ValueFlag<double> northValue(latLngBoundsGroup, "degrees", "North latitude", {"north"});
    args::ValueFlag<double> westValue(latLngBoundsGroup, "degrees", "West longitude", {"west"});
    args::ValueFlag<double> southValue(latLngBoundsGroup, "degrees", "South latitude", {"south"});
    args::ValueFlag<double> eastValue(latLngBoundsGroup, "degrees", "East longitude", {"east"});

    // Geometry
    args::Group geoJSONGroup(argumentParser, "GeoJson geometry:", args::Group::Validators::AllOrNone);
    args::ValueFlag<std::string> geometryValue(geoJSONGroup, "file", "GeoJSON Feature file containing the region geometry (can't be a FeatureCollection)", {"geojson"});

    args::ValueFlag<double> minZoomValue(argumentParser, "number", "Min zoom level", {"minZoom"});
    args::ValueFlag<double> maxZoomValue(argumentParser, "number", "Max zoom level", {"maxZoom"});
    args::ValueFlag<double> pixelRatioValue(argumentParser, "number", "Pixel ratio", {"pixelRatio"});
    args::ValueFlag<bool> includeIdeographsValue(argumentParser, "boolean", "Include CJK glyphs", {"includeIdeographs"});

    try {
        argumentParser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
        std::cout << argumentParser;
        exit(0);
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(1);
    } catch (const args::ValidationError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(2);
    }

    std::string style = styleValue ? args::get(styleValue) : mbgl::util::default_styles::streets.url;

    mbgl::optional<std::string> mergePath = {};
    if (mergePathValue) mergePath = args::get(mergePathValue);
    mbgl::optional<std::string> inputDb = {};
    if (inputValue) inputDb = args::get(inputValue);

    const double minZoom = minZoomValue ? args::get(minZoomValue) : 0.0;
    const double maxZoom = maxZoomValue ? args::get(maxZoomValue) : 15.0;
    const double pixelRatio = pixelRatioValue ? args::get(pixelRatioValue) : 1.0;
    const bool includeIdeographs = includeIdeographsValue ? args::get(includeIdeographsValue) : false;
    const std::string output = outputValue ? args::get(outputValue) : "offline.db";
    
    using namespace mbgl;
    
    OfflineRegionDefinition definition = [&]() {
        if (geometryValue) {
            try {
                std::string json = readFile(geometryValue.Get());
                auto geometry = parseGeometry(json);
                return OfflineRegionDefinition{ OfflineGeometryRegionDefinition(style, geometry, minZoom, maxZoom, pixelRatio, includeIdeographs) };
            } catch(const std::runtime_error& e) {
                std::cerr << "Could not parse geojson file " << geometryValue.Get() << ": " << e.what() << std::endl;
                exit(1);
            }
        } else {
            // Bay area
            const double north = northValue ? args::get(northValue) : 37.2;
            const double west = westValue ? args::get(westValue) : -122.8;
            const double south = southValue ? args::get(southValue) : 38.1;
            const double east = eastValue ? args::get(eastValue) : -121.7;
            LatLngBounds boundingBox = LatLngBounds::hull(LatLng(north, west), LatLng(south, east));
            return OfflineRegionDefinition{ OfflineTilePyramidRegionDefinition(style, boundingBox, minZoom, maxZoom, pixelRatio, includeIdeographs) };
        }
    }();

    const char* tokenEnv = getenv("MAPBOX_ACCESS_TOKEN");
    const std::string token = tokenValue ? args::get(tokenValue) : (tokenEnv ? tokenEnv : std::string());
    
    const std::string apiBaseURL = apiBaseValue ? args::get(apiBaseValue) : mbgl::util::API_BASE_URL;


    util::RunLoop loop;
    DefaultFileSource fileSource(output, ".");
    std::unique_ptr<OfflineRegion> region;

    fileSource.setAccessToken(token);
    fileSource.setAPIBaseURL(apiBaseURL);

    if (inputDb && mergePath) {
        DefaultFileSource inputSource(*inputDb, ".");
        inputSource.setAccessToken(token);
        inputSource.setAPIBaseURL(apiBaseURL);
        
        int retCode = 0;
        std::cout << "Start Merge" << std::endl;
        inputSource.mergeOfflineRegions(*mergePath,  [&] (mbgl::expected<std::vector<OfflineRegion>, std::exception_ptr> result) {

            if (!result) {
                std::cerr << "Error merging database: " << util::toString(result.error()) << std::endl;
                retCode = 1;
            } else {
                std::cout << " Added " << result->size() << " Regions" << std::endl;
                std::cout << "Finished Merge" << std::endl;
            }
            loop.stop();
        });
        loop.run();
        return retCode;
    }

    OfflineRegionMetadata metadata;

    class Observer : public OfflineRegionObserver {
    public:
        Observer(OfflineRegion& region_, DefaultFileSource& fileSource_, util::RunLoop& loop_, mbgl::optional<std::string> mergePath_)
            : region(region_),
              fileSource(fileSource_),
              loop(loop_),
              mergePath(std::move(mergePath_)),
              start(util::now()) {
        }

        void statusChanged(OfflineRegionStatus status) override {
            if (status.downloadState == OfflineRegionDownloadState::Inactive) {
                std::cout << "stopped" << std::endl;
                loop.stop();
                return;
            }

            std::string bytesPerSecond = "-";

            auto elapsedSeconds = (util::now() - start) / 1s;
            if (elapsedSeconds != 0) {
                bytesPerSecond = util::toString(status.completedResourceSize / elapsedSeconds);
            }

            std::cout << status.completedResourceCount << " / " << status.requiredResourceCount
                      << " resources"
                      << status.completedTileCount << " / " << status.requiredTileCount
                      << "tiles"
                      << (status.requiredResourceCountIsPrecise ? "; " : " (indeterminate); ")
                      << status.completedResourceSize << " bytes downloaded"
                      << " (" << bytesPerSecond << " bytes/sec)"
                      << std::endl;

            if (status.complete()) {
                std::cout << "Finished Download" << std::endl;
                loop.stop();
            }
        }

        void responseError(Response::Error error) override {
            std::cerr << error.reason << " downloading resource: " << error.message << std::endl;
        }

        void mapboxTileCountLimitExceeded(uint64_t limit) override {
            std::cerr << "Error: reached limit of " << limit << " offline tiles" << std::endl;
        }

        OfflineRegion& region;
        DefaultFileSource& fileSource;
        util::RunLoop& loop;
        mbgl::optional<std::string> mergePath;
        Timestamp start;
    };

    static auto stop = [&] {
        if (region) {
            std::cout << "Stopping download... ";
            fileSource.setOfflineRegionDownloadState(*region, OfflineRegionDownloadState::Inactive);
        }
    };

    std::signal(SIGINT, [] (int) { stop(); });

    fileSource.createOfflineRegion(definition, metadata, [&] (mbgl::expected<OfflineRegion, std::exception_ptr> region_) {
        if (!region_) {
            std::cerr << "Error creating region: " << util::toString(region_.error()) << std::endl;
            loop.stop();
            exit(1);
        } else {
            assert(region_);
            region = std::make_unique<OfflineRegion>(std::move(*region_));
            fileSource.setOfflineRegionObserver(*region, std::make_unique<Observer>(*region, fileSource, loop, mergePath));
            fileSource.setOfflineRegionDownloadState(*region, OfflineRegionDownloadState::Active);
        }
    });

    loop.run();
    return 0;
}
