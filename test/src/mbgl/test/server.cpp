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

std::string timestampToISO(const time_t rawtime){
    struct tm * dt;
    char buffer [30];
    dt = localtime(&rawtime);
    //Tue, 05 Jul 2016 19:52:50 GMT
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", dt);
    return std::string(buffer);
}

std::string dumpHeaders(const httplib::MultiMap& headers) {
    std::string s;
    char buf[BUFSIZ];

    for (auto it = headers.begin(); it != headers.end(); ++it) {
        const auto& x = *it;
        snprintf(buf, sizeof(buf), "|%s|: |%s|\n", x.first.c_str(), x.second.c_str());
        s += buf;
    }

    return s;
}

Server::Server(bool) {
    svr = std::make_unique<httplib::Server>();

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-parameter" 
    
    svr->get("/test", [](const auto& req, auto& res) {
        //XXX Remove
        std::cout << "test " << req.url << "\n";
        for (auto it = req.params.begin(); it != req.params.end(); ++it) { 
            const auto& x = *it;  
            std::cout << "Param: " << x.first << " - " << x.second << " / " << req.has_param(x.first.c_str()) << "\n";
        } 

        if(req.has_param("modified")) {
            res.set_header("Last-Modified", timestampToISO(std::stoi(req.params.at("modified"))).c_str());
            std::cout << "Modified "  << timestampToISO(std::stoi(req.params.at("modified")));   
        }
        if (req.has_param("expires")) {
            res.set_header("Expires", timestampToISO(std::stoi(req.params.at("expires"))).c_str());
        }
        if (req.has_param("etag")) {
            res.set_header("ETag", req.params.find("etag")->second.c_str());
        }
        if (req.has_param("cachecontrol")) {
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
        if (req.has_header("If-None-Match") && req.headers.find("If-None-Match")->second == "snowfall") {
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
