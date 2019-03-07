#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace mbgl {

/**
 * @brief Holds values for FileSource options.
 */

class FileSourceOptions {
public:
    /**
     * @brief Constructs a FileSourceOptions object with default values.
     */
    FileSourceOptions();
    ~FileSourceOptions();

    /**
     * @brief Sets the Mapbox API access token.
     * Details: https://docs.mapbox.com/help/glossary/access-token/ 
     * 
     * @param accessToken Mapbox API access token, as string.
     * @return reference to FileSourceOptions for chaining options together.
     */
    FileSourceOptions& withAccessToken(std::string accessToken);

    /**
     * @brief Gets the previously set Mapbox API access token.
     * Details: https://docs.mapbox.com/help/glossary/access-token/ 
     *
     * @return Mapbox API access token, as string.
     */
    const std::string& accessToken() const;

    /**
     * @brief Sets the cache path.
     *
     * @param path Cache path.
     * @return reference to FileSourceOptions for chaining options together.
     */
    FileSourceOptions& withCachePath(std::string path);

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
     * @return reference to FileSourceOptions for chaining options together.
     */
    FileSourceOptions& withAssetRoot(std::string path);

    /**
     * @brief Gets the previously set (or default) asset path.
     *
     * @return asset path
     */
    const std::string& assetRoot() const;

    /**
     * @brief Sets the maximum cache size.
     *
     * @param size Cache maximum size in bytes.
     * @return reference to FileSourceOptions for chaining options together.
     */
    FileSourceOptions& withMaximumCacheSize(uint64_t size);

    /**
     * @brief Gets the previously set (or default) maximum allowed cache size.
     *
     * @return maximum allowed cache database size in bytes.
     */
    uint64_t maximumCacheSize() const;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace mbgl