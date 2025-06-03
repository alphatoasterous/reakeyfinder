#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>


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
}
