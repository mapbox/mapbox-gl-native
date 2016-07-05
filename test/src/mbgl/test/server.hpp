#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wformat"
#include "httplib.h"
#pragma GCC diagnostic pop

#include <memory>
#include <future>

namespace mbgl {

class Map;

namespace test {

class Server {
public:
    Server(bool);
    ~Server();
    void start();
private:
    std::unique_ptr<httplib::Server> svr;
    std::future<void> f_;
    bool up_ = false;
};
} // namespace test
} // namespace mbgl
