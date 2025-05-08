#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <semaphore.h>
#include <mutex>
#include <atomic>

#ifdef DEBUG
std::mutex mutex_debug;
#define DEBUG_PRINT(x) \
    do { std::lock_guard<std::mutex> lk(mutex_debug); std::cout << x << std::endl; } while(0)
#else
#define DEBUG_PRINT(x) do {} while(0)
#endif


std::size_t CUTOFF;
int MAX_THREADS;
double* shared_arr = nullptr;
sem_t mutex_thread;
std::atomic<int> thread_count(1);


std::size_t partition(std::size_t low, std::size_t high) {
    double pivot = shared_arr[high];
    std::size_t i = low;
    for (std::size_t j = low; j < high; ++j) {
        if (shared_arr[j] < pivot) {
            std::swap(shared_arr[i], shared_arr[j]);
            ++i;
        }
    }
    std::swap(shared_arr[i], shared_arr[high]);
    return i;
}

void insertion_sort(std::size_t low, std::size_t high) {
    for (std::size_t i = low + 1; i <= high; ++i) {
        double key = shared_arr[i];
        std::size_t j = i;
        while (j > low && shared_arr[j - 1] > key) {
            shared_arr[j] = shared_arr[j - 1];
            --j;
        }
        shared_arr[j] = key;
    }
}

void quick_sort_thread(std::size_t, std::size_t, int);
void quick_sort(std::size_t low, std::size_t high) {
    if (low >= high) return;
    std::size_t size = high - low + 1;
    if (size < CUTOFF) {
        insertion_sort(low, high);
        return;
    }
    std::size_t pivot_index = partition(low, high);
    if (thread_count.load() >= MAX_THREADS) {
        quick_sort(low, pivot_index == 0 ? 0 : pivot_index - 1);
        quick_sort(pivot_index + 1, high);
    } else {
        std::thread left_thread(quick_sort_thread, low, pivot_index == 0 ? 0 : pivot_index - 1, thread_count.fetch_add(1) + 1);
        std::thread right_thread(quick_sort_thread, pivot_index + 1, high, thread_count.fetch_add(1) + 1);
        left_thread.detach();
        right_thread.detach();
    }
}

void quick_sort_thread(std::size_t low, std::size_t high, int n_threads = 1) {
    DEBUG_PRINT("[Create] Thread ID: " << std::this_thread::get_id() << "\tthread count     : " << n_threads << "\tsorting range [" << low << ", " << high << "]");
    quick_sort(low, high);
    const int past_thread_count = thread_count.fetch_sub(1);
    DEBUG_PRINT("[Exit  ] Thread ID: " << std::this_thread::get_id() << "\tremaining threads: " << past_thread_count - 1);
    if (past_thread_count == 1) {
        DEBUG_PRINT("Thread ID: " << std::this_thread::get_id() << "\tall threads finished, notifying main thread");
        sem_post(&mutex_thread);
    }
}


int main(int argc, char *argv[]) {
    DEBUG_PRINT("Quick Sort Simulation");
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <cutoff> <max_threads>" << std::endl;
        return 1;
    }
    std::string infile = argv[1];
    CUTOFF = std::stoul(argv[2]);
    MAX_THREADS = std::stoi(argv[3]);
    std::ifstream fin(infile);

    std::vector<double> data;
    double v;
    while (fin >> v) {
        data.push_back(v);
    }
    fin.close();
    std::size_t total_size = data.size();
    DEBUG_PRINT("Number of elements: " << total_size);

    const char* shm_name = "/quick_sort_shm";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        std::cerr << "Failed to open shared memory" << std::endl;
        return 1;
    }
    DEBUG_PRINT("Shared memory created with fd: " << shm_fd);

    std::size_t shm_bytes = total_size * sizeof(double);
    if (ftruncate(shm_fd, shm_bytes) != 0) {
        std::cerr << "Failed to set size of shared memory" << std::endl;
        return 1;
    }
    DEBUG_PRINT("Shared memory size set to: " << shm_bytes);

    shared_arr = static_cast<double*>(mmap(nullptr, shm_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shared_arr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return 1;
    }
    DEBUG_PRINT("Shared memory mapped at: " << shared_arr);

    std::memcpy(shared_arr, data.data(), shm_bytes);
    DEBUG_PRINT("Data copied to shared memory");

    sem_init(&mutex_thread, 0, 0);
    DEBUG_PRINT("Starting quick sort");
    quick_sort_thread(0, total_size - 1);
    DEBUG_PRINT("Waiting for threads to finish");
    sem_wait(&mutex_thread);
    DEBUG_PRINT("Quick sort completed");

    for (std::size_t i = 0; i < total_size; ++i) {
        std::cout << shared_arr[i] << "\n";
    }

    munmap(shared_arr, shm_bytes);
    close(shm_fd);
    shm_unlink(shm_name);
    DEBUG_PRINT("Shared memory unlinked and cleaned up");
    
    return 0;
}