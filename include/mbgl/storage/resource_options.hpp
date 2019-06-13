#pragma once

#include <memory>
#include <string>

namespace mbgl {

/**
 * @brief Holds values for resource options.
 */
class ResourceOptions final {
public:
    /**
     * @brief Constructs a ResourceOptions object with default values.
     */
    ResourceOptions();
    ~ResourceOptions();

    ResourceOptions(ResourceOptions&&) noexcept;

    ResourceOptions clone() const;

    /**
     * @brief Sets the Mapbox access token - see https://docs.mapbox.com/help/how-mapbox-works/access-tokens/ for details.
     *
     * @param token Mapbox access token.
     * @return ResourceOptions for chaining options together.
     */
    ResourceOptions& withAccessToken(std::string token);

    /**
     * @brief Gets the previously set (or default) Mapbox access token.
     *
     * @return const std::string& Mapbox access token.
     */
    const std::string& accessToken() const;

    /**
     * @brief Sets the API base URL. Default is https://api.mapbox.com for Mapbox.
     *
     * @param baseURL API base URL.
     * @return ResourceOptions for chaining options together.
     */
    ResourceOptions& withBaseURL(std::string baseURL);

    /**
     * @brief Gets the previously set (or default) API base URL.
     *
     * @return const std::string& API base URL.
     */
    const std::string& baseURL() const;

    /**
     * @brief Sets the cache path.
     *
     * @param path Cache path.
     * @return ResourceOptions for chaining options together.
     */
    ResourceOptions& withCachePath(std::string path);

    /**
     * @brief Gets the previously set (or default) cache path.
     *
     * @return cache path
     */
    const std::string& cachePath() const;

    /**
     * @brief Sets the asset path, which is the root directory from where
     * the asset:// scheme gets resolved in a style.
     *
     * @param path Asset path.
     * @return ResourceOptions for chaining options together.
     */
    ResourceOptions& withAssetPath(std::string path);

    /**
     * @brief Gets the previously set (or default) asset path.
     *
     * @return asset path
     */
    const std::string& assetPath() const;

    /**
     * @brief Sets the maximum cache size.
     *
     * @param size Cache maximum size in bytes.
     * @return reference to ResourceOptions for chaining options together.
     */
    ResourceOptions& withMaximumCacheSize(uint64_t size);

    /**
     * @brief Gets the previously set (or default) maximum allowed cache size.
     *
     * @return maximum allowed cache database size in bytes.
     */
    uint64_t maximumCacheSize() const;

    /**
     * @brief Sets whether to support cache-only requests.
     *
     * @return Whether or not cache-only requests are supported.
     */
    bool supportsCacheOnlyRequests() const;

    /**
     * @brief Gets the previously set (or default) support for cache-only requests.
     *
     * @param Whether or not cache-only requests are supported.
     * @return reference to ResourceOptions for chaining options together.
     */
    ResourceOptions& withCacheOnlyRequestsSupport(bool);

    /**
     * @brief Sets the platform context. A platform context is usually an object
     * that assists the creation of a file source.
     *
     * @param context Platform context.
     * @return reference to ResourceOptions for chaining options together.
     */
    ResourceOptions& withPlatformContext(void* context);

    /**
     * @brief Gets the previously set (or default) platform context.
     *
     * @return Platform context.
     */
    void* platformContext() const;

private:
    ResourceOptions(const ResourceOptions&);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mbgl
