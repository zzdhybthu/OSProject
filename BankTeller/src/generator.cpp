#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <num_cust> <min_arrival_time> <max_arrival_time> <min_service_time> <max_service_time>\n";
        return 1;
    }
    const int num_cust = std::stoi(argv[1]);
    const int min_arr = std::stoi(argv[2]);
    const int max_arr = std::stoi(argv[3]);
    const int min_svc = std::stoi(argv[4]);
    const int max_svc = std::stoi(argv[5]);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> arr(min_arr, max_arr);
    std::uniform_int_distribution<int> svc(min_svc, max_svc);
    for (std::size_t i = 1; i <= num_cust; ++i) {
        std::cout << i << " " << arr(rng) << " " << svc(rng) << "\n";
    }
    return 0;
}