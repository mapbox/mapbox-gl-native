#include "test_runner_common.hpp"

#include <cmath>
#include <regex>

using namespace mbgl;

// Strip precision for numbers, so that we can compare evaluated results with fixtures.
// Copied from JS expression harness.
Value stripPrecision(const Value& value) {
    const double decimalSigFigs = 6;
    if (auto num = numericValue<double>(value)) {
        if (*num == 0) {
            return *num;
        }

        const double multiplier = std::pow(10, std::max(0.0, decimalSigFigs - std::ceil(std::log10(std::fabs(*num)))));

        // We strip precision twice in a row here to avoid cases where
        // stripping an already stripped number will modify its value
        // due to bad floating point precision luck
        // eg `Math.floor(8.16598 * 100000) / 100000` -> 8.16597
        const double firstStrip = std::floor(*num * multiplier) / multiplier;
        return std::floor(firstStrip * multiplier) / multiplier;
    }

    if (value.is<std::vector<Value>>()) {
        std::vector<Value> stripped;
        const auto& vec = value.get<std::vector<Value>>();
        stripped.reserve(vec.size());
        for (const auto& val : vec) {
            stripped.emplace_back(stripPrecision(val));
        }
        return stripped;
    } else if (value.is<std::unordered_map<std::string, Value>>()) {
        std::unordered_map<std::string, Value> stripped;
        const auto& map = value.get<std::unordered_map<std::string, Value>>();
        for (const auto& pair : map) {
            stripped.emplace(pair.first, stripPrecision(pair.second));
        }
        return stripped;
    }

    return value;
}

std::vector<std::string> tokenize(std::string str) {
    std::vector<std::string> tokens;
    std::regex re("\n");
    std::copy(std::regex_token_iterator<std::string::iterator>(str.begin(), str.end(), re, -1),
              std::regex_token_iterator<std::string::iterator>(),
              std::back_inserter(tokens));
    return tokens;
}

bool deepEqual(const Value& a, const Value& b) {
    const auto& anum = numericValue<double>(a);
    const auto& bnum = numericValue<double>(b);
    if (anum && bnum) {
        return stripPrecision(*anum) == stripPrecision(*bnum);
    }

    if (a.which() != b.which()) {
        return false;
    }

    if (a.getArray() && b.getArray()) {
        const auto& avec = *a.getArray();
        const auto& bvec = *b.getArray();
        if (avec.size() != bvec.size()) {
            return false;
        }
        for (std::size_t i = 0; i < avec.size(); ++i) {
            if (!deepEqual(avec[i], bvec[i])) {
                return false;
            }
        }
        return true;
    }

    if (a.getObject() && b.getObject()) {
        const auto& amap = *a.getObject();
        const auto& bmap = *b.getObject();
        if (amap.size() != bmap.size()) {
            return false;
        }
        for (const auto& pair : amap) {
            auto it = bmap.find(pair.first);
            if (it == bmap.end()) {
                return false;
            }
            if (!deepEqual(pair.second, it->second)) {
                return false;
            }
        }
        return true;
    }

    if (a == b) {
        return true;
    }

    if (a.getString() && b.getString()) {
        const auto& strA = *a.getString();
        const auto& strB = *b.getString();
        if (strA == strB) {
            return true;
        }

        try {
            double numA = std::stod(strA);
            double numB = std::stod(strB);
            return stripPrecision(numA) == stripPrecision(numB);
        } catch (...) {
        }
    }

    return false;
}

bool deepEqual(const optional<Value>& a, const optional<Value>& b) {
    if ((a && !b) || (!a && b)) {
        return false;
    }

    if (a && b) {
        return deepEqual(*a, *b);
    }

    return true;
}
