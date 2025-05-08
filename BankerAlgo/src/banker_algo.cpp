#include "utils.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>

#ifdef DEBUG
std::mutex mutex_debug;
#define DEBUG_PRINT(x) \
    do { std::lock_guard<std::mutex> lk(mutex_debug); std::cout << x << std::endl; } while(0)
#else
#define DEBUG_PRINT(x) do {} while(0)
#endif


enum class result {
    FAIL    = 0,
    WAIT    = 1,
    SUCCESS = 2
};

bool is_safe(
    const std::vector<int> available,
    const std::vector<std::vector<int>> allocation,
    const std::vector<std::vector<int>> need
) {
    DEBUG_PRINT("Checking if the system is in a safe state...");

    std::vector<int> work = available;
    std::queue<std::pair<int, int>> unfinished;
    for (std::size_t i = 0; i < allocation.size(); ++i) {
        unfinished.push(std::make_pair(i, 0));
    }
    DEBUG_PRINT("\tAllocation: " << allocation);
    DEBUG_PRINT("\tNeed      : " << need);
    DEBUG_PRINT("\tWork      : " << available);

    int task_count = 0;
    while (!unfinished.empty()) {
        int id = unfinished.front().first;
        int count = unfinished.front().second;
        if (task_count < count) {
            DEBUG_PRINT("System is not in a safe state.");
            DEBUG_PRINT("\tRemaining : " << unfinished);
            return false;
        }
        unfinished.pop();

        if (need[id] <= work) {
            work += allocation[id];
            ++task_count;
            DEBUG_PRINT("\t\tTask " << id << " finished, work updated. \tTag: " << count << ",\ttasks finished: " << task_count);
            DEBUG_PRINT("\tWork      : " << work);
        } else {
            unfinished.push(std::make_pair(id, task_count + 1));
            DEBUG_PRINT("\t\tTask " << id << " cannot finish, skipped. \tTag: " << task_count + 1 << ",\ttasks finished: " << task_count);
        }
    }

    DEBUG_PRINT("System is in a safe state.");
    return true;
}


result banker_allocate(
    std::vector<int>& available,
    std::vector<std::vector<int>>& allocation,
    std::vector<std::vector<int>>& need,
    const std::vector<int>& request,
    int request_id
) {
    if (request.empty()) {
        DEBUG_PRINT("No resources requested.");
        return result::SUCCESS;
    }
    if (!(request <= need[request_id])) {
        DEBUG_PRINT("Request exceeds need.");
        return result::FAIL;
    }
    if (!(request <= available)) {
        DEBUG_PRINT("Request exceeds available resources.");
        return result::WAIT;
    }

    available -= request;
    allocation[request_id] += request;
    need[request_id] -= request;

    if (!is_safe(available, allocation, need)) {
        available += request;
        allocation[request_id] -= request;
        need[request_id] += request;
        DEBUG_PRINT("Request cannot be granted, system is not in a safe state.");
        return result::WAIT;
    }

    DEBUG_PRINT("Request granted.");
    return result::SUCCESS;
}


bool check_input(
    const std::vector<int>& available,
    const std::vector<std::vector<int>>& allocation,
    const std::vector<std::vector<int>>& need,
    const std::vector<int>& request,
    int request_id
) {
    if (allocation.size() != need.size()) {
        std::cerr << "Allocation and Need matrices must have the same number of rows!" << std::endl;
        return false;
    }
    const int num_resources = available.size();
    if (request.size() != num_resources) {
        std::cerr << "Request vector size must match the number of resources!" << std::endl;
        return false;
    }
    for (const auto& row : allocation) {
        if (row.size() != num_resources) {
            std::cerr << "Allocation matrix rows must have the same number of columns!" << std::endl;
            return false;
        }
    }
    for (const auto& row : need) {
        if (row.size() != num_resources) {
            std::cerr << "Need matrix rows must have the same number of columns!" << std::endl;
            return false;
        }
    }
    if (request_id < 0 || request_id >= allocation.size()) {
        std::cerr << "Request ID is out of range!" << std::endl;
        return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    DEBUG_PRINT("Banker Algorithm Simulation");
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <Available> <Allocation> <Need> <Request> <Id>" << std::endl;
        return 1;
    }
    std::vector<int> available = parseVector(argv[1]);
    std::vector<std::vector<int>> allocation = parseMatrix(argv[2]);
    std::vector<std::vector<int>> need = parseMatrix(argv[3]);
    std::vector<int> request = parseVector(argv[4]);
    int request_id = std::stoi(argv[5]);

    DEBUG_PRINT("Requesting resources...");
    DEBUG_PRINT("\tAvailable : " << available);
    DEBUG_PRINT("\tAllocation: " << allocation);
    DEBUG_PRINT("\tNeed      : " << need);
    DEBUG_PRINT("\tRequest   : " << request);
    DEBUG_PRINT("\tRequest ID: " << request_id);

    if (!check_input(available, allocation, need, request, request_id)) {
        std::cerr << "Input validation failed!" << std::endl;
        return 1;
    }

    result res = banker_allocate(available, allocation, need, request, request_id);


    DEBUG_PRINT("Resources after allocation:");
    DEBUG_PRINT("\tAvailable : " << available);
    DEBUG_PRINT("\tAllocation: " << allocation);
    DEBUG_PRINT("\tNeed      : " << need);

    std::cout << "Result: ";
    switch (res) {
        case result::FAIL:
            std::cout << "FAIL" << std::endl;
            break;
        case result::WAIT:
            std::cout << "WAIT" << std::endl;
            break;
        case result::SUCCESS:
            std::cout << "SUCCESS" << std::endl;
            break;
    }

    return 0;
}