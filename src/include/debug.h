#pragma once
#include "common.h"

inline void print() {
    std::cout << std::endl;
}

template<typename T>
inline void print(std::vector<T> vector) {
    std::string output = "{";
    for (int i = 0; i < vector.size(); i++) {
        output += vector[i] + (i == vector.size() - 1 ? "}" : ", ");
    }
    
    if (!vector.size()) 
        output += "}";

    std::cout << output << std::endl;
}

template<typename First, typename ... Strings>
inline void print(First arg, const Strings&... rest) {
    std::cout << arg << " ";
    print(rest...);
}

inline std::ostream &operator << (std::ostream &os, Rectangle &rect)
{
    os << "Rectangle {" << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << "}";
    return os;
}

inline std::ostream &operator << (std::ostream &os, Vector2 &vec)
{
    os << "Vector2 {" << vec.x << ", " << vec.y << "}";
    return os;
}
