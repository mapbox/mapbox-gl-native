#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_impl.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/test/map_adapter.hpp>
#include <unordered_map>

#include <gtest/gtest.h>

using namespace mbgl;

class SyncFileSource : public FileSource {
public:
    std::unique_ptr<AsyncRequest> request(const Resource& resource, FileSource::Callback callback) {
        Response response;
        auto it = assets.find(resource.url);
        if (it == assets.end()) {
            response.error = std::make_unique<Response::Error>(
                Response::Error::Reason::NotFound, std::string{ "Not Found: " } + resource.url);
        } else {
            response.data = it->second;
        }
        callback(response);
        return nullptr;
    }

    void add(std::string const& key, std::string const& data) {
        assets.emplace(key, std::make_shared<std::string>(data));
    };

private:
    std::unordered_map<std::string, std::shared_ptr<std::string>> assets;
};

TEST(SyncFileSource, LoadSyncRender) {
    util::RunLoop loop;
    auto fs = std::make_shared<SyncFileSource>();
    fs->add("mapbox://mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v7",
            util::read_file("test/fixtures/resources/source_vector.json"));
    fs->add("mapbox://sprites/mapbox/streets-v9.png",
            util::read_file("test/fixtures/resources/sprite.png"));
    fs->add("mapbox://sprites/mapbox/streets-v9.json",
            util::read_file("test/fixtures/resources/sprite.json"));
    HeadlessFrontend frontend{ { 512, 512 }, 1.0 };
    MapAdapter map{ frontend, MapObserver::nullObserver(), fs, MapOptions() };
    map.getStyle().loadJSON(util::read_file("test/fixtures/resources/style_vector.json"));
}
