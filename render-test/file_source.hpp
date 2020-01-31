#pragma once

#include <mbgl/storage/default_file_source.hpp>

namespace mbgl {

class ProxyFileSource : public DefaultFileSource {
public:
    ProxyFileSource(const std::string& cachePath, const std::string& assetPath);
    ~ProxyFileSource();

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback);

    /**
     * @brief Flag to change the networking mode of the file source.
     */
    static void setOffline(bool);

    /**
     * @brief Starts/stops metrics tracking.
     */
    static void setTrackingActive(bool);

    /**
     * @brief Returns metrics tracking status.
     */
    static bool isTrackingActive();

    /**
     * @brief Returns the total amount of requests.
     *
     * @return size_t
     */
    static size_t getRequestCount();

    /**
     * @brief Returns the size of transferred data (in bytes).
     *
     * @return size_t
     */
    static size_t getTransferredSize();
};

} // namespace mbgl
