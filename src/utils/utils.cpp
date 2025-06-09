#include "utils.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <regex>
#include <sstream>


namespace PROJECT_NAME {
    std::string StringizeSampleBuffer(double* buf, int size) {
        std::ostringstream oss;
        oss << "[";
        for (int i = 0; i < size; ++i) {
            oss << std::fixed << std::setprecision(6) << buf[i];
            if (i != size - 1) oss << ", ";
        }
        oss << "]" << std::endl;
        return oss.str();
    }
    std::string FloatToString(float value, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    std::string FloatToString(double value, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    bool isStringNotEmpty(const char* input) {
        return input != nullptr && std::strlen(input) > 0;
    }
    #include <string>

    // Wraps string into Lua [[...]] safely (handles embedded ]]
    std::string wrapLuaBracketString(const std::string& input) {
        std::string pad = "";
        while (input.find("]" + pad + "]") != std::string::npos) {
            pad += "=";
        }
        return "[[" + pad + "[" + input + "]" + pad + "]]";
    }

    std::string unwrapLuaBracketString(const char* input) {
        if (!input) {
            throw std::invalid_argument("Null input string");
        }

        std::string str(input);
        std::smatch match;

        // This pattern works for [[...]], [=[...]=], [==[...]==], etc.
        std::regex bracket_regex(R"(\[(=*)\[(.*?)\]\1\])", std::regex::ECMAScript);

        if (std::regex_search(str, match, bracket_regex)) {
            return match[2].str();  // Extract inner content
        } else {
            // throw std::runtime_error("Invalid Lua bracketed string format.");
        }
    }

}
