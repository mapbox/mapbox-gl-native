#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/storage/default_file_source.hpp>

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <atomic>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

namespace po = boost::program_options;
using namespace std::literals::chrono_literals;

int main(int argc, char *argv[]) {
    std::string style = mbgl::util::default_styles::streets.url;
    double north = 37.2, west = -122.8, south = 38.1, east = -121.7; // Bay area
    double minZoom = 0.0, maxZoom = 15.0, pixelRatio = 1.0;
    std::string output = "offline.db";

    const char* tokenEnv = getenv("MAPBOX_ACCESS_TOKEN");
    std::string token = tokenEnv ? tokenEnv : std::string();

    po::options_description desc("Allowed options");
    desc.add_options()
        ("style,s", po::value(&style)->value_name("URL"), "Map stylesheet")
        ("north", po::value(&north)->value_name("degrees")->default_value(north), "North latitude")
        ("west", po::value(&west)->value_name("degrees")->default_value(west), "West longitude")
        ("south", po::value(&south)->value_name("degrees")->default_value(south), "South latitude")
        ("east", po::value(&east)->value_name("degrees")->default_value(east), "East longitude")
        ("minZoom", po::value(&minZoom)->value_name("number")->default_value(minZoom), "Min zoom level")
        ("maxZoom", po::value(&maxZoom)->value_name("number")->default_value(maxZoom), "Max zoom level")
        ("pixelRatio", po::value(&pixelRatio)->value_name("number")->default_value(pixelRatio), "Pixel ratio")
        ("token,t", po::value(&token)->value_name("key")->default_value(token), "Mapbox access token")
        ("output,o", po::value(&output)->value_name("file")->default_value(output), "Output database file name")
    ;

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl << desc;
        exit(1);
    }

    using namespace mbgl;

    util::RunLoop loop;
    DefaultFileSource fileSource(output, ".");
    std::unique_ptr<OfflineRegion> region;

    fileSource.setAccessToken(token);

    LatLngBounds boundingBox = LatLngBounds::hull(LatLng(north, west), LatLng(south, east));
    OfflineTilePyramidRegionDefinition definition(style, boundingBox, minZoom, maxZoom, pixelRatio);
    OfflineRegionMetadata metadata;

    class Observer : public OfflineRegionObserver {
    public:
        Observer(OfflineRegion& region_, DefaultFileSource& fileSource_, util::RunLoop& loop_)
            : region(region_),
              fileSource(fileSource_),
              loop(loop_),
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
                      << (status.requiredResourceCountIsPrecise ? "; " : " (indeterminate); ")
                      << status.completedResourceSize << " bytes downloaded"
                      << " (" << bytesPerSecond << " bytes/sec)"
                      << std::endl;

            if (status.complete()) {
                std::cout << "Finished" << std::endl;
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
        Timestamp start;
    };

    static auto stop = [&] {
        if (region) {
            std::cout << "Stopping download... ";
            fileSource.setOfflineRegionDownloadState(*region, OfflineRegionDownloadState::Inactive);
        }
    };

    std::signal(SIGINT, [] (int) { stop(); });

    fileSource.createOfflineRegion(definition, metadata, [&] (std::exception_ptr error, optional<OfflineRegion> region_) {
        if (error) {
            std::cerr << "Error creating region: " << util::toString(error) << std::endl;
            loop.stop();
            exit(1);
        } else {
            assert(region_);
            region = std::make_unique<OfflineRegion>(std::move(*region_));
            fileSource.setOfflineRegionObserver(*region, std::make_unique<Observer>(*region, fileSource, loop));
            fileSource.setOfflineRegionDownloadState(*region, OfflineRegionDownloadState::Active);
        }
    });

    loop.run();
    return 0;
}
