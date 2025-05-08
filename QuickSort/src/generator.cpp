#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <array_size>" << std::endl;
        return 1;
    }
    const size_t N = std::stoul(argv[1]);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (size_t i = 0; i < N; ++i) {
        double x = dist(gen);
        std::cout << x;
        if (i + 1 < N) std::cout << "\n";
    }
    return 0;
}
