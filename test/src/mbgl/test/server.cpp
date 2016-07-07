#include <mbgl/test/server.hpp>
#include <mbgl/platform/log.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <mbgl/util/chrono.hpp>

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

Server::Server(bool) {
    svr = std::make_unique<httplib::Server>();

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-parameter" 
    
    svr->get("/test", [](const auto& req, auto& res) {
        if(req.has_param("modified")) {
            res.set_header("Last-Modified", timestampToISO(std::stoi(req.params.at("modified"))).c_str());
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
        //Don't respond
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

    svr->get("/clockskew", [](const auto& req, auto& res) {
        static int expiresCounter = 0;
        char dateString[25];
        sprintf(dateString, "Feb 01 2010 10:0%d:00", ++expiresCounter); 
        res.set_header("Expires", dateString);
        res.status = 200;
        res.set_content("Response", "text/plain");
    });
    
    svr->get("/revalidate-modified", [](const auto& req, auto& res) { 
        Timestamp jan1 = util::parseTimestamp("Jan 01 2015");
            
        if (req.has_header("If-Modified-Since")) {
            Timestamp modified_since = util::parseTimestamp(req.headers.find("If-Modified-Since")->second.c_str());
            if (modified_since >= jan1) {
                res.set_header("Cache-Control", "max-age=30");
                res.status = 304;
                return;
            } 
        }
            
        // First request must always be revalidated.
        res.set_header("Last-Modified", timestampToISO(std::chrono::system_clock::to_time_t(jan1)).c_str());
        res.set_header("Cache-Control", "must-revalidate");
        res.status = 200;
        res.set_content("Response", "text/plain");
    });


    svr->get("/revalidate-etag", [](const auto& req, auto& res) { 
        static int revalidateEtagCounter = 1;
        res.set_header("ETag", std::string("response-").append(std::to_string(revalidateEtagCounter)).c_str());
        res.set_header("Cache-Control", "must-revalidate");

        res.status = 200;
        res.set_content(std::string("Response ").append(std::to_string(revalidateEtagCounter)), "text/plain");
        revalidateEtagCounter++;
    });


    svr->get("/empty-data", [](const auto& req, auto& res) { 
        res.status = 200;
    });

    svr->get("/no-content", [](const auto& req, auto& res) { 
        res.status = 204;
    });

    svr->get("/not-found", [](const auto& req, auto& res) { 
        res.status = 404;
        res.set_content("Not Found", "text/plain");
    });

    svr->get("/permanent-error", [](const auto& req, auto& res) { 
        res.status = 500;
        res.set_content("Server Error", "text/plain");
    });

    svr->get("/temporary-error", [](const auto& req, auto& res) { 
        static int temporaryErrorCounter = 0;
        if (temporaryErrorCounter == 0) {
            res.status = 500;
        } else {
            res.status = 200;
            res.set_content("Hello World!", "text/plain");
        }

        temporaryErrorCounter++;
    });

    svr->get("/delayed", [](const auto& req, auto& res) { 
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        res.status = 200;
        res.set_content("Response", "text/plain");
    });
    
    svr->get(R"(/load/(\d+))", [](const auto& req, auto& res) {
        std::string number = req.matches.str(1); 
        res.set_content(std::string("Request ").append(number), "text/plain");       
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
