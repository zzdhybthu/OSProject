#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <climits>

struct Result {
    int id;
    int arrive;
    int start;
    int leave;
    int teller_id;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    std::string infile = argv[1];
    std::ifstream fin(infile);
    if (!fin) {
        std::cerr << "Error opening " << infile << "\n";
        return 1;
    }

    std::vector<Result> results;
    std::string line;
    Result r;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        if (iss >> r.id >> r.arrive >> r.start >> r.leave >> r.teller_id) {
            results.push_back(r);
        } else {
            std::cout << line << std::endl;
        }
    }
    fin.close();
    

    // 1. 检查是否有顾客被多个柜台服务
    {
        std::map<int, int> cust_to_teller;
        for (auto &x : results) {
            auto it = cust_to_teller.find(x.id);
            if (it == cust_to_teller.end()) {
                cust_to_teller[x.id] = x.teller_id;
            } else if (it->second != x.teller_id) {
                std::cout << "ERROR: Customer " << x.id
                          << " served by teller " << it->second
                          << " and teller " << x.teller_id << "\n";
                return 0;
            }
        }
    }

    // 2. 检查是否有柜员同时服务多个顾客
    {
        std::map<int, std::vector<Result>> by_teller;
        for (auto &x : results) {
            by_teller[x.teller_id].push_back(x);
        }
        for (auto &pair : by_teller) {
            auto &vec = pair.second;
            std::sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.start < b.start; });
            for (std::size_t i = 1; i < vec.size(); ++i) {
                if (vec[i].start < vec[i-1].leave) {
                    std::cout << "ERROR: Teller " << pair.first
                              << " overlaps serving customer " << vec[i-1].id
                              << " (ends at " << vec[i-1].leave << ") and customer "
                              << vec[i].id << " (starts at " << vec[i].start << ")\n";
                    return 0;
                }
            }
        }
    }

    // 3. 检查是否存在柜员空闲但顾客等待的情况
    {
        std::map<int, std::vector<std::pair<int,int>>> intervals;
        for (auto &x : results) {
            intervals[x.teller_id].emplace_back(x.start, x.leave);
        }
        for (auto &p : intervals) {
            std::sort(p.second.begin(), p.second.end());
        }

        for (auto &x : results) {
            if (x.start == x.arrive) continue;  // 立即被服务，无等待
            bool found_idle = false;
            int idle_teller = -1;
            for (auto &p : intervals) {
                int t_id = p.first;
                auto &iv = p.second;
                auto it = std::upper_bound(iv.begin(), iv.end(), std::make_pair(x.arrive, INT_MAX));
                bool busy = false;
                if (it != iv.begin()) {
                    auto prev = std::prev(it);
                    if (prev->second == it->first || x.start <= prev->second) {  // 柜员无空闲，或在柜员空闲前（含）已被服务
                        busy = true;
                    }
                }
                if (!busy) {
                    found_idle = true;
                    idle_teller = t_id;
                    break;
                }
            }
            if (found_idle) {
                std::cout << "ERROR: Customer " << x.id
                            << " waited from " << x.arrive << " to " << x.start
                            << " while teller " << idle_teller << " was idle at " << x.start << "\n";
                return 0;
            }
        }
    }

    std::cout << "All checks passed successfully.\n";
    return 0;
}
