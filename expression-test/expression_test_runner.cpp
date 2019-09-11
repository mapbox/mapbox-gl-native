#include "expression_test_runner.hpp"
#include "expression_test_parser.hpp"
#include "expression_test_logger.hpp"
#include "filesystem.hpp"

#include <mapbox/io.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <sstream>
#include <regex>

using namespace std::literals;

namespace {

// Strip precision for numbers, so that we can compare evaluated results with fixtures.
// Copied from JS expression harness.
Value stripPrecision(const Value& value) {
    const double decimalSigFigs = 6;
    if (auto num = numericValue<double>(value)) {
        if (*num == 0) {
            return *num;
        }

        const double multiplier = std::pow(10,
            std::max(0.0, decimalSigFigs - std::ceil(std::log10(std::fabs(*num)))));

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

bool deepEqual(const Value& a, const Value& b) {
    const auto& anum = numericValue<double>(a);
    const auto& bnum = numericValue<double>(b);
    if (anum && bnum) {
        return stripPrecision(*anum) == stripPrecision(*bnum);
    }

    if (a.which() != b.which()) {
        return false;
    }

    if (a.is<std::vector<Value>>()) {
        const auto& avec = a.get<std::vector<Value>>();
        const auto& bvec = b.get<std::vector<Value>>();
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

    if (a.is<std::unordered_map<std::string, Value>>()) {
        const auto& amap = a.get<std::unordered_map<std::string, Value>>();
        const auto& bmap = b.get<std::unordered_map<std::string, Value>>();
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

    return a == b;
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

std::vector<std::string> tokenize(std::string str) {
    std::vector<std::string> tokens;
    std::regex re("\n");
    std::copy(std::regex_token_iterator<std::string::iterator>(str.begin(), str.end(), re, -1),
              std::regex_token_iterator<std::string::iterator>(),
              std::back_inserter(tokens));
    return tokens;
}

std::string simpleDiff(const Value& result, const Value& expected) {
    std::vector<std::string> resultTokens {tokenize(toJSON(result, 2, true))};
    std::vector<std::string> expectedTokens {tokenize(toJSON(expected, 2, true))};
    std::size_t maxLength = std::max(resultTokens.size(), expectedTokens.size());
    std::ostringstream diff;
    const auto flush = [] (const std::vector<std::string>& vec, std::size_t pos, std::ostringstream& out, std::string separator) {
        for (std::size_t j = pos; j < vec.size(); ++j) {
            out << separator << vec[j] << std::endl;
        }
    };

    for (std::size_t i = 0; i < maxLength; ++i) {
        if (resultTokens.size() <= i) {
            flush(expectedTokens, i, diff, "+"s);
            break;
        }

        if (expectedTokens.size() <= i) {
            flush(expectedTokens, i, diff, "-"s);
            break;
        }

        if (resultTokens[i] != expectedTokens[i]) {
            diff << "-"s << expectedTokens[i] << std::endl;
            diff << "+"s << resultTokens[i] << std::endl;
        } else {
            diff << resultTokens[i] << std::endl;
        }
    }
    return diff.str();
}

void rewriteRoundtrippedType(const std::string& expected, std::string& actual) {
    if (!expected.rfind("array", 0) && !actual.rfind("array", 0)) {
        actual = expected;
    }
}

void updateTest(TestData& data) {
    assert(data.document["expected"].IsObject());
    auto& expected = data.document["expected"];
    auto compiled = toDocument(toValue(data.result.compiled));
    assert(!compiled.HasParseError());
    expected["compiled"].Swap(compiled);

    if (data.result.outputs) {
        auto outputs = toDocument(stripPrecision(data.result.outputs.value_or(Value{})));
        assert(!outputs.HasParseError());
        expected["outputs"].Swap(outputs);
    } else {
        expected.RemoveMember("outputs");
    }

    if (data.result.serialized) {
        auto serialized = toDocument(*data.result.serialized);
        assert(!serialized.HasParseError());
        expected["serialized"].Swap(serialized);
    } else {
        expected.RemoveMember("serialized");
    }
}

void writeTestData(const JSDocument& document, const std::string& rootPath, const std::string& id) {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    writer.SetIndent(' ', 2);
    document.Accept(writer);
    buffer.Put('\n');
    filesystem::path path = filesystem::path(rootPath) / id / "test.json"s;
    auto expected = mapbox::base::io::writeFile(path, {buffer.GetString(), buffer.GetSize()});
    if (!expected) {
        printf(ANSI_COLOR_RED "* ERROR: %s" ANSI_COLOR_RESET "\n", expected.error().c_str());
    }
}

} // namespace

TestRunOutput runExpressionTest(TestData& data, const std::string& rootPath, const std::string& id) {
    TestRunOutput output(id);
    const auto evaluateExpression = [&data](std::unique_ptr<style::expression::Expression>& expression,
                                            TestResult& result) {
        assert(expression);
        std::vector<Value> outputs;
        if (!data.inputs.empty()) {
            for (const auto& input : data.inputs) {
                auto evaluationResult = expression->evaluate(input.zoom, input.feature, input.heatmapDensity);
                if (!evaluationResult) {
                    std::unordered_map<std::string, Value> error{{"error", Value{evaluationResult.error().message}}};
                    outputs.emplace_back(Value{std::move(error)});
                } else {
                    auto value = toValue(*evaluationResult);
                    assert(value);
                    outputs.emplace_back(Value{*value});
                }
            }
        }
        result.outputs = {Value{std::move(outputs)}};
    };

    // Parse expression
    auto parsedExpression = parseExpression(data.document["expression"], data.spec, data.result);
    output.expression = toJSON(data.result.expression.value_or(Value{}), 2, true);

    // Evaluate expression
    if (parsedExpression) {
        evaluateExpression(parsedExpression, data.result);
        output.serialized = toJSON(data.result.serialized.value_or(Value{}), 2, true);

        // round trip
        auto recompiledExpression = parseExpression(data.result.serialized, data.spec, data.recompiled);
        if (recompiledExpression) {
            evaluateExpression(recompiledExpression, data.recompiled);
            rewriteRoundtrippedType(data.expected.compiled.serializedType,
                                    data.recompiled.compiled.serializedType);
        }
    }

    if (getenv("UPDATE")) {
        output.passed = true;
        updateTest(data);
        writeTestData(data.document, rootPath, id);
        return output;
    }

    bool compileOk = data.result.compiled == data.expected.compiled;
    bool evalOk = compileOk && deepEqual(data.result.outputs, data.expected.outputs);

    bool recompileOk = true;
    bool roundTripOk = true;
    bool serializationOk = true;

    if (data.expected.compiled.success) {
        serializationOk = compileOk && deepEqual(data.result.serialized, data.expected.serialized);
        recompileOk = compileOk &&  data.recompiled.compiled == data.expected.compiled;
        roundTripOk = recompileOk && deepEqual(data.recompiled.outputs, data.expected.outputs);
    }

    output.passed = compileOk && evalOk && recompileOk && roundTripOk && serializationOk;

    if (!compileOk) {
        auto resultValue = toValue(data.result.compiled);
        auto expectedValue = toValue(data.expected.compiled);
        output.text += "Compiled expression difference:\n"s +
                        simpleDiff(resultValue, expectedValue) +
                        "\n"s;
    }

    if (compileOk && !serializationOk) {
        auto diff = simpleDiff(data.expected.serialized.value_or(Value{}),
                               data.result.serialized.value_or(Value{}));
        output.text += "Serialized expression difference:\n"s + diff + "\n"s;
    }

    if (compileOk && !recompileOk) {
        auto recompiledValue = toValue(data.recompiled.compiled);
        auto expectedValue = toValue(data.expected.compiled);
        output.text += "Recompiled expression difference:\n"s +
                        simpleDiff(recompiledValue, expectedValue) +
                        "\n"s;
    }

    if (compileOk && !evalOk) {
        auto diff = simpleDiff(data.expected.outputs.value_or(Value{}),
                               data.result.outputs.value_or(Value{}));
        output.text += "Expression outputs difference:\n"s + diff + "\n"s;
    }

    if (recompileOk && !roundTripOk) {
        auto diff = simpleDiff(data.expected.outputs.value_or(Value{}),
                               data.recompiled.outputs.value_or(Value{}));
        output.text += "Roundtripped through serialize expression outputs difference:\n"s +
                        diff + "\n"s;
    }

    return output;
}
