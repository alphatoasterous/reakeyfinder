#pragma once
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
}