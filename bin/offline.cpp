#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/storage/default_file_source.hpp>

#include <cstdlib>
#include <iostream>

using namespace std::literals::chrono_literals;

int main(int, char * []) {
    using namespace mbgl;

    unlink("offline.db");

    util::RunLoop loop;
    DefaultFileSource fileSource("offline.db", ".");

    fileSource.setAccessToken(getenv("MAPBOX_ACCESS_TOKEN"));

    LatLngBounds bayArea = LatLngBounds::hull(LatLng(37.2, -122.8), LatLng(38.1, -121.7));
    OfflineTilePyramidRegionDefinition definition("mapbox://styles/mapbox/streets-v8", bayArea, 0, 15, 1.0);
    OfflineRegionMetadata metadata;

    class Observer : public OfflineRegionObserver {
    public:
        Observer(util::RunLoop& loop_)
            : loop(loop_),
              start(SystemClock::now()) {
        }

        void statusChanged(OfflineRegionStatus status) override {
            std::string bytesPerSecond = "-";

            auto elapsedSeconds = (SystemClock::now() - start) / 1s;
            if (elapsedSeconds != 0) {
                bytesPerSecond = util::toString(status.completedResourceSize / elapsedSeconds);
            }

            std::cout << status.completedResourceCount << " / " << status.requiredResourceCount
                      << " resources"
                      << (status.requiredResourceCountIsIndeterminate ? " (indeterminate); " : "; ")
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

        util::RunLoop& loop;
        SystemTimePoint start;
    };

    fileSource.createOfflineRegion(definition, metadata, [&] (std::exception_ptr error, optional<OfflineRegion> region) {
        if (error) {
            std::cerr << "Error creating region: " << util::toString(error) << std::endl;
            loop.stop();
            exit(1);
        } else {
            assert(region);
            fileSource.setOfflineRegionObserver(*region, std::make_unique<Observer>(loop));
            fileSource.setOfflineRegionDownloadState(*region, OfflineRegionDownloadState::Active);
        }
    });

    loop.run();
    return 0;
}
