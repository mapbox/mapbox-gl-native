
#ifndef mbgl_mbtiles_source_impl_hpp
#define mbgl_mbtiles_source_impl_hpp

#include <mbgl/storage/mbtiles_source.hpp>

typedef struct uv_loop_s uv_loop_t;

namespace mapbox {
  namespace sqlite {
    class Database;
    class Statement;
  }
}

namespace mbgl {

  class MBTilesSource::Impl {
  public:
    Impl(uv_loop_t*, const std::string &path = ":memory:");
    ~Impl();

    std::unique_ptr<Response> get(const Resource&);
    void put(const Resource& resource, std::shared_ptr<const Response> response);
    void refresh(const Resource& resource, int64_t expires);

  private:
    void openDatabase();

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
    std::unique_ptr<::mapbox::sqlite::Statement> getStmt;
  };
  
}


#endif
