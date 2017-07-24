#include "dtoa.hpp"

// Clang/C2 on Windows 64-bits can't parse rapidjson's dtoa
// and it was causing the compiler to crash.
#if !defined(_WINDOWS)
#include <rapidjson/internal/dtoa.h>
#endif

#include <mbgl/util/string.hpp>

namespace mbgl {
namespace util {

#if !defined(_WINDOWS)

namespace {

// From https://github.com/miloyip/rapidjson/blob/master/include/rapidjson/internal/dtoa.h

char* Prettify(char* buffer, int length, int k) {
    constexpr int maxDecimalPlaces = 324;
    const int kk = length + k;  // 10^(kk-1) <= v < 10^kk

    if (0 <= k && kk <= 21) {
        // 1234e7 -> 12340000000
        for (int i = length; i < kk; i++)
            buffer[i] = '0';
        return &buffer[kk];
    }
    else if (0 < kk && kk <= 21) {
        // 1234e-2 -> 12.34
        std::memmove(&buffer[kk + 1], &buffer[kk], static_cast<size_t>(length - kk));
        buffer[kk] = '.';
        if (0 > k + maxDecimalPlaces) {
            // When maxDecimalPlaces = 2, 1.2345 -> 1.23, 1.102 -> 1.1
            // Remove extra trailing zeros (at least one) after truncation.
            for (int i = kk + maxDecimalPlaces; i > kk + 1; i--)
                if (buffer[i] != '0')
                    return &buffer[i + 1];
            return &buffer[kk + 2]; // Reserve one zero
        }
        else
            return &buffer[length + 1];
    }
    else if (-6 < kk && kk <= 0) {
        // 1234e-6 -> 0.001234
        const int offset = 2 - kk;
        std::memmove(&buffer[offset], &buffer[0], static_cast<size_t>(length));
        buffer[0] = '0';
        buffer[1] = '.';
        for (int i = 2; i < offset; i++)
            buffer[i] = '0';
        if (length - kk > maxDecimalPlaces) {
            // When maxDecimalPlaces = 2, 0.123 -> 0.12, 0.102 -> 0.1
            // Remove extra trailing zeros (at least one) after truncation.
            for (int i = maxDecimalPlaces + 1; i > 2; i--)
                if (buffer[i] != '0')
                    return &buffer[i + 1];
            return &buffer[3]; // Reserve one zero
        }
        else
            return &buffer[length + offset];
    }
    else if (kk < -maxDecimalPlaces) {
        // Truncate to zero
        buffer[0] = '0';
        return &buffer[1];
    }
    else if (length == 1) {
        // 1e30
        buffer[1] = 'e';
        return rapidjson::internal::WriteExponent(kk - 1, &buffer[2]);
    }
    else {
        // 1234e30 -> 1.234e33
        std::memmove(&buffer[2], &buffer[1], static_cast<size_t>(length - 1));
        buffer[1] = '.';
        buffer[length + 1] = 'e';
        return rapidjson::internal::WriteExponent(kk - 1, &buffer[0 + length + 2]);
    }
}

} // namespace

char* dtoa(double value, char* buffer) {
    rapidjson::internal::Double d(value);
    if (d.IsZero()) {
        if (d.Sign())
            *buffer++ = '-';     // -0.0, Issue #289
        buffer[0] = '0';
        return &buffer[1];
    }
    else {
        if (value < 0) {
            *buffer++ = '-';
            value = -value;
        }
        int length, K;
        rapidjson::internal::Grisu2(value, buffer, &length, &K);
        return Prettify(buffer, length, K);
    }
}

std::string dtoa(double value) {
    std::string data;
    data.resize(25);
    auto end = dtoa(value, const_cast<char*>(data.data()));
    data.resize(end - data.data());
    return data;
}

#else

std::string dtoa(double value) {
    return std::to_string(value);
}

#endif

} // namespace util
} // namespace mbgl
