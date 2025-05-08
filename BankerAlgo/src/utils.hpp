#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <queue>


template <typename T>
bool operator>=(const std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }
    for (std::size_t i = 0; i < v1.size(); ++i) {
        if (v1[i] < v2[i]) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool operator<=(const std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }
    for (std::size_t i = 0; i < v1.size(); ++i) {
        if (v1[i] > v2[i]) {
            return false;
        }
    }
    return true;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }
    for (std::size_t i = 0; i < v1.size(); ++i) {
        v1[i] += v2[i];
    }
    return v1;
}

template <typename T>
std::vector<T>& operator-=(std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }
    for (std::size_t i = 0; i < v1.size(); ++i) {
        v1[i] -= v2[i];
    }
    return v1;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<T>>& matrix) {
    os << "[";
    for (std::size_t i = 0; i < matrix.size(); ++i) {
        os << matrix[i];
        if (i != matrix.size() - 1) {
            os << "; ";
        }
    }
    os << "]";
    return os;
}

template<typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::queue<T>& q) {
    os << "[";
    std::queue<T> copy = q;
    while (!copy.empty()) {
        os << copy.front();
        copy.pop();
        if (!copy.empty()) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::vector<int> parseVector(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        result.push_back(std::stoi(token));
    }
    return result;
}

std::vector<std::vector<int>> parseMatrix(const std::string& str) {
    std::vector<std::vector<int>> matrix;
    std::stringstream ss_rows(str);
    std::string row;
    while (std::getline(ss_rows, row, ';')) {
        matrix.push_back(parseVector(row));
    }
    return matrix;
}


#endif // UTILS_HPP