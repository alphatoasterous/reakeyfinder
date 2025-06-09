#include "utils.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
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
}
