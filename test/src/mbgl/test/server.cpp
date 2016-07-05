#include <mbgl/test/server.hpp>
#include <mbgl/platform/log.hpp>
#include <iostream>
#include <future>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wformat"
#include "httplib.h"
#pragma GCC diagnostic pop

#include <memory>

namespace mbgl {
namespace test {

std::string dumpHeaders(const httplib::MultiMap& headers) {
    std::string s;
    char buf[BUFSIZ];

    for (auto it = headers.begin(); it != headers.end(); ++it) {
        const auto& x = *it;
        snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
        s += buf;
    }

    return s;
}

Server::Server(bool) {
    svr = std::make_unique<httplib::Server>();

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-parameter" 
    
    svr->get("/test", [](const auto& req, auto& res) {
        std::cout << "test\n";
        if(req.has_param("modified")) {
            //res.setHeader('Last-Modified', (new Date(req.query.modified * 1000)).toUTCString());
            std::cout << "Modified "  << req.params.find("modified")->second;   
        }
        if (req.has_param("expires")) {
                //res.setHeader('Expires', (new Date(req.query.expires * 1000)).toUTCString());
                std::cout << "Exipres "  << req.params.find("expires")->second; 
        }
            if (req.has_param("etag")) {
                std::cout << "etag "  << req.params.find("etag")->second; 
            res.set_header("ETag", req.params.find("etag")->second.c_str());
        }
        if (req.has_param("cachecontrol")) {
                std::cout << "cachecontrol "  << req.params.find("cachecontrol")->second; 
            res.set_header("Cache-Control", req.params.find("cachecontrol")->second.c_str());
        }
        res.set_content("Hello World!", "text/plain");
    });

    svr->get("/stale", [] (const auto& req, auto& res) {
        std::cout << "stale " << req.url << "\n";
        //TODO: Don't respond
    }); 

    svr->get("/cache", [](const auto& req, auto& res) {
        static int cacheCounter;
        res.set_header("Cache-Control", "max-age=30");
        res.set_content("Response " + std::to_string(++cacheCounter), "text/plain");    
    });

    svr->get("/revalidate-same", [](const auto& req, auto& res) {
        std::cout << "revalidate same\n";
        std::cout << dumpHeaders(req.headers) << "\n\n";
        if (req.has_header("if-non-match") && req.headers.find("if-none-match")->second == "snowfall") {
            // Second request can be cached for 30 seconds.
            res.set_header("Cache-Control", "max-age=30"); 
            res.status = 304;
        } else {
            // First request must always be revalidated.
            res.set_header("ETag", "snowfall"); 
            res.set_header("Cache-Control", "must-revalidate");
            res.status = 200;
            res.set_content("Response", "text/plain");
        }
    });
#pragma GCC diagnostic pop
}

Server::~Server() {
    svr->stop();
}

void Server::start() {
    const int port = 3000;
    std::cout << "Starting server on port: " << port << "\n";
    f_ = std::async(std::launch::async, [&](){
        up_ = true;
        svr->listen("127.0.0.1", port);
    });

    while (!up_) {
        usleep(1000);
    }
    std::cout << "Server should be up\n";
}

} // namespace test
} // namespace mbgl
