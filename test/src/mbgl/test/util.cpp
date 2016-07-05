#include <mbgl/test/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mapbox/pixelmatch.hpp>

#include <csignal>
#include <future>

#include <unistd.h>

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

Server::Server() {
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

	int cacheCounter = 0;
	svr->get("/cache", [&cacheCounter](const auto& req, auto& res) {
		std::cout << "cache\n";
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
	std::cout << "Starting server on port 3000\n";
	//svr->listen("localhost", 3000);
	//std::cout << "Stopped!!\n";
	f_ = std::async([&](){
		up_ = true;
		svr->listen("localhost", 3000);
	});

	while (!up_) {
		usleep(1000);
	}
	std::cout << "Server should be up\n";
}

PremultipliedImage render(Map& map) {
    PremultipliedImage result;
    map.renderStill([&result](std::exception_ptr, PremultipliedImage&& image) {
        result = std::move(image);
    });

    while (!result.size()) {
        util::RunLoop::Get()->runOnce();
    }

    return result;
}

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold,
                double pixelThreshold) {
#if !TEST_READ_ONLY
    if (getenv("UPDATE")) {
        util::write_file(base + "/expected.png", encodePNG(actual));
        return;
    }
#endif

    PremultipliedImage expected = decodeImage(util::read_file(base + "/expected.png"));
    PremultipliedImage diff { expected.width, expected.height };

    ASSERT_EQ(expected.width, actual.width);
    ASSERT_EQ(expected.height, actual.height);

    double pixels = mapbox::pixelmatch(actual.data.get(),
                                       expected.data.get(),
                                       expected.width,
                                       expected.height,
                                       diff.data.get(),
                                       pixelThreshold);

    EXPECT_LE(pixels / (expected.width * expected.height), imageThreshold);

#if !TEST_READ_ONLY
    util::write_file(base + "/actual.png", encodePNG(actual));
    util::write_file(base + "/diff.png", encodePNG(diff));
#endif
}

} // namespace test
} // namespace mbgl
