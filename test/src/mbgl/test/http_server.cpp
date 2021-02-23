#include <mbgl/test/util.hpp>

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>

// Limit to 4 threads (default is the nbr of CPU cores)
#define CPPHTTPLIB_THREAD_POOL_COUNT 4
#include <httplib.h>

#include <atomic>

using namespace std::literals::string_literals;

namespace mbgl {
namespace test {

void runServer(std::unique_ptr<httplib::Server>& server) {
    using namespace httplib;

    server->Get("/test", [](const Request& req, Response& res) {
        if (req.has_param("modified")) {
            std::string str = util::rfc1123(util::parseTimestamp(std::stoi(req.get_param_value("modified"))));
            res.set_header("Last-Modified", str);
        }
        if (req.has_param("expires")) {
            std::string str = util::rfc1123(util::parseTimestamp(std::stoi(req.get_param_value("expires"))));
            res.set_header("Expires", str);
        }
        if (req.has_param("etag")) {
            res.set_header("ETag", req.get_param_value("etag"));
        }
        if (req.has_param("cachecontrol")) {
            res.set_header("Cache-Control", "max-age=" + req.get_param_value("cachecontrol"));
        }
        res.set_content("Hello World!", "text/plain");
    });

    server->Get("/stale", [](const Request&, Response&) {
        // Never respond.
    });

    std::atomic_int cacheCounter(0);
    server->Get("/cache", [&](const Request&, Response& res) {
        res.set_header("Cache-Control", "max-age=30"); // Allow caching for 30 seconds
        res.set_content("Response " + std::to_string(++cacheCounter), "text/plain");
    });

    server->Get("/revalidate-same", [](const Request& req, Response& res) {
        if (req.get_header_value("if-none-match") == "snowfall") {
            // Second request can be cached for 1 second.
            res.set_header("Cache-Control", "max-age=1, must-revalidate");
            res.status = 304;
        } else {
            // First request must always be revalidated.
            res.set_header("ETag", "snowfall");
            res.set_header("Cache-Control", "must-revalidate");
            res.status = 200;
            res.set_content("Response", "text/plain");
        }
    });

    std::atomic_bool expiredOnce(true);
    server->Get("/clockskew", [&](const Request&, Response& res) {
        std::string dateStr;
        if (expiredOnce) {
            dateStr = "jan 1 10:00:00 2010 utc";
            expiredOnce = false;
        } else {
            dateStr = "jan 1 10:01:00 2010 utc";
        }
        res.set_header("Expires", util::rfc1123(util::parseTimestamp(dateStr.c_str())));
        res.status = 200;
        res.set_content("Response", "text/plain");
    });

    server->Get("/revalidate-modified", [&](const Request& req, Response& res) {
        auto jan1 = util::parseTimestamp("jan 1 2015 utc");

        if (req.has_header("if-modified-since")) {
            auto modified_since = util::parseTimestamp(req.get_header_value("if-modified-since").c_str());
            if (modified_since >= jan1) {
                res.set_header("Cache-Control", "max-age=1, must-revalidate");
                res.status = 304;
                return;
            }
        }

        // First request must always be revalidated.
        res.set_header("Last-Modified", util::rfc1123(jan1));
        res.set_header("Cache-Control", "must-revalidate");
        res.status = 200;
        res.set_content("Response", "text/plain");
    });

    std::atomic_int revalidateEtagCounter(1);
    server->Get("/revalidate-etag", [&](const Request&, Response& res) {
        res.set_header("ETag", "response-" + std::to_string(revalidateEtagCounter));
        res.set_header("Cache-Control", "must-revalidate");

        res.status = 200;
        res.set_content("Response " + std::to_string(revalidateEtagCounter), "text/plain");
        revalidateEtagCounter++;
    });

    server->Get("/empty-data", [](const Request&, Response& res) { res.status = 200; });

    server->Get("/no-content", [](const Request&, Response& res) { res.status = 204; });

    server->Get("/not-found", [](const Request&, Response& res) {
        res.status = 404;
        res.set_content("Not Found!", "text/plain");
    });

    server->Get("/permanent-error", [](const Request&, Response& res) {
        res.status = 500;
        res.set_content("Server Error!", "text/plain");
    });

    std::atomic_int temporaryErrorCounter(0);
    server->Get("/temporary-error", [&](const Request&, Response& res) {
        if (temporaryErrorCounter == 0) {
            res.status = 500;
        } else {
            res.status = 200;
            res.set_content("Hello World!", "text/plain");
        }
        temporaryErrorCounter++;
    });

    server->Get("/rate-limit", [](const Request& req, Response& res) {
        if (req.has_param("std")) {
            res.set_header("Retry-After", "1");
        } else if (req.has_param("mbx")) {
            res.set_header("x-rate-limit-reset", std::to_string(util::now().time_since_epoch().count() + 1));
        }
        res.status = 429;
    });

    std::atomic_bool styleFailOnce500(true);
    server->Get("/style-fail-once-500", [&](const Request&, Response& res) {
        if (styleFailOnce500) {
            res.status = 500;
            res.set_content("Not found!", "text/plain");
            styleFailOnce500 = false;
        } else {
            res.status = 200;
            res.set_content(R"({ "version": 8, "name": "Test Style" })", "text/plain");
        }
    });

    std::atomic_bool styleFailOnce404(true);
    server->Get("/style-fail-once-404", [&](const Request&, Response& res) {
        if (styleFailOnce404) {
            res.status = 404;
            res.set_content("Not found!", "text/plain");
            styleFailOnce404 = false;
        } else {
            res.status = 200;
            res.set_content(R"({ "version": 8, "name": "Test Style" })", "text/plain");
        }
    });

    std::atomic_bool styleFailOnce404Cache(true);
    server->Get("/style-fail-once-404-cache", [&](const Request&, Response& res) {
        if (styleFailOnce404Cache) {
            res.set_header("Cache-Control", "max-age=30");
            res.status = 404;
            res.set_content("Not found!", "text/plain");
            styleFailOnce404Cache = false;
        } else {
            res.status = 200;
            res.set_content(R"({ "version": 8, "name": "Teste Style" })", "text/plain");
        }
    });

    server->Get("/delayed", [](const Request&, Response& res) {
        usleep(200000);
        res.status = 200;
        res.set_content("Response", "text/plain");
    });

    server->Get(R"(/load/(\d+))", [](const Request req, Response& res) {
        auto numbers = req.matches[1];
        res.set_content("Request " + std::string(numbers), "text/plain");
    });

    server->Get(R"(/online/(.*))", [](const Request req, Response& res) {
        res.status = 200;
        auto file = "test/fixtures/map/online/"s + std::string(req.matches[1]);
        auto content = util::read_file(file);
        res.set_content(content, "text/plain");
    });

    server->listen("127.0.0.1", 3000);
}

HttpServer::HttpServer() {
    server = std::make_unique<httplib::Server>();
    serverThread = std::thread(runServer, std::ref(server));
}

HttpServer::~HttpServer() {
    server->stop();
    serverThread.join();
}

} // namespace test
} // namespace mbgl
