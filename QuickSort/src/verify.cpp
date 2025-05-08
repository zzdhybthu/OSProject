#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

const double EPSILON = 1e-9;


bool vectors_equal(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::fabs(a[i] - b[i]) > EPSILON) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <original_file> <sorted_file>" << std::endl;
        return 1;
    }
    std::string original_file = argv[1];
    std::string sorted_file = argv[2];
    std::ifstream fin_original(original_file);
    std::ifstream fin_sorted(sorted_file);

    std::vector<double> original_data, sorted_data;
    double v;
    while (fin_original >> v) {
        original_data.push_back(v);
    }
    while (fin_sorted >> v) {
        sorted_data.push_back(v);
    }

    if (original_data.size() != sorted_data.size()) {
        std::cout << "Failure: Mismatch in data size." << std::endl;
        return 0;
    }

    std::vector<double> expected_sorted = original_data;
    std::sort(expected_sorted.begin(), expected_sorted.end());
    if (!vectors_equal(sorted_data, expected_sorted)) {
        std::cout << "Failure: Not a correct sort." << std::endl;
        return 0;
    }

    std::cout << "Success: Correct sort." << std::endl;
    return 0;
}