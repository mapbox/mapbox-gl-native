#ifndef mbgl_mbtiles_source_hpp
#define mbgl_mbtiles_source_hpp

#include <mbgl/storage/file_cache.hpp>

#include <string>

namespace mbgl {

  namespace util {
    template <typename T> class Thread;
  }

  class MBTilesSource : public FileCache {
  public:
    MBTilesSource(const std::string &path = ":memory:");
    ~MBTilesSource() override;

    // FileCache API
    void get(const Resource &resource, Callback callback) override;
    void put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) override;

    class Impl;

  private:
    const std::unique_ptr<util::Thread<Impl>> thread;
  };
  
}

#endif
