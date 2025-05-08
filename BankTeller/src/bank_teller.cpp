#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <semaphore.h>
#include <mutex>
#include <cmath>

#ifdef DEBUG
std::mutex mutex_debug;
#define DEBUG_PRINT(x) \
    do { std::lock_guard<std::mutex> lk(mutex_debug); std::cout << x << std::endl; } while(0)
#else
#define DEBUG_PRINT(x) do {} while(0)
#endif


class Timer {
public:
    using time_unit = std::chrono::milliseconds;
    using time_t = typename time_unit::rep;
    using Clock = std::chrono::system_clock;

    Timer(int time_zoom) : t0(_raw_time()), time_zoom(time_zoom) {}
    int get_time() {
        return static_cast<int>(std::round(static_cast<double>(_get_time()) / time_zoom));
    }
    void sleep(int seconds) {
        std::this_thread::sleep_for(time_unit(seconds * time_zoom));
    }

private:
    static time_t _raw_time() {
        return std::chrono::duration_cast<time_unit>(Clock::now().time_since_epoch()).count();
    }
    time_t _get_time() {
        return _raw_time() - t0;
    }
    time_t t0;
    int time_zoom;
};

struct Customer {
    int id;         // 顾客序号
    int arrive;     // 到达时间
    int service;    // 服务时长
    int ticket;     // 取号号码
    int teller_id;  // 服务柜台
    int start;      // 开始服务时间
    int leave;      // 离开时间
    sem_t sem;      // 信号量
};

int cust_ticket = 1;                        // 取号号码
std::vector<Customer> customers;            // 顾客列表
std::queue<Customer*> ticket_queue;         // 叫号队列
std::mutex mutex_ticket;                    // 取号/叫号互斥锁
sem_t sem_customer;                         // 同步信号量
Timer timer(100);                           // 定时器


void teller_thread(int id) {
    try {
        DEBUG_PRINT("[Teller]   " << id << " \t" << "created.");
        while (true) {
            DEBUG_PRINT("[Teller]   " << id << " \t" << "waiting.");
            sem_wait(&sem_customer);  // 等待顾客取号
            Customer* c;
            {  // 防止不同的柜台叫同一个号
                std::lock_guard<std::mutex> lk(mutex_ticket);
                c = ticket_queue.front();
                ticket_queue.pop();
            }
            c->teller_id = id;
            c->start = timer.get_time();
            DEBUG_PRINT("[Teller]   " << id << " \t" << "started serving customer " << c->id << " \t" << "with ticket " << c->ticket << " \t" << "at time " << c->start);
            timer.sleep(c->service);
            c->leave = timer.get_time();
            DEBUG_PRINT("[Teller]   " << id << " \t" << "finished serving customer " << c->id << " \t" << "with ticket " << c->ticket << " \t" << "at time " << c->leave);
            sem_post(&c->sem);  // 完成服务后同步顾客
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in thread" << std::endl;
    }
}

void customer_thread(Customer &c) {
    try {
        DEBUG_PRINT("[Customer] " << c.id << " \t" << "created.");
        timer.sleep(c.arrive);
        DEBUG_PRINT("[Customer] " << c.id << " \t" << "arrived.");
        {  // 防止不同的顾客取同一个号
            std::lock_guard<std::mutex> lk(mutex_ticket);
            c.ticket = cust_ticket++;
            ticket_queue.push(&c);
        }
        DEBUG_PRINT("[Customer] " << c.id << " \t" << "took ticket " << c.ticket);
        sem_post(&sem_customer);    // 顾客取号后同步柜台
        sem_wait(&c.sem);           // 等待柜台完成服务
        DEBUG_PRINT("[Customer] " << c.id << " \t" << "being served by teller " << c.teller_id);
    } catch (const std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in thread" << std::endl;
    }
}


int main(int argc, char *argv[]) {

    DEBUG_PRINT("Bank Teller Simulation");
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <n_tellers> <input_file>" << std::endl;
        return 1;
    }
    int n_tellers = std::stoi(argv[1]);
    std::string infile = argv[2];
    std::ifstream fin(infile);

    int id, a, s;
    while (fin >> id >> a >> s) {
        customers.emplace_back();
        Customer &c = customers.back();
        c.id = id; c.arrive = a; c.service = s;
        sem_init(&c.sem, 0, 0);  
    }
    fin.close();
    DEBUG_PRINT("Number of tellers: " << n_tellers);
    DEBUG_PRINT("Number of customers: " << customers.size());

    sem_init(&sem_customer, 0, 0);

    std::vector<std::thread> tell_threads;
    for (std::size_t i = 1; i <= n_tellers; ++i)
        tell_threads.emplace_back(teller_thread, i);
    DEBUG_PRINT("All teller threads started.");
    
    std::vector<std::thread> cust_threads;
    for (auto &c : customers)
        cust_threads.emplace_back(customer_thread, std::ref(c));
    DEBUG_PRINT("All customer threads started.");

    for (auto &t : cust_threads) t.join();
    DEBUG_PRINT("All customer threads finished.");
    for (auto &t : tell_threads) t.detach();
    DEBUG_PRINT("All teller threads detached.");

    auto end_time = timer.get_time();
    DEBUG_PRINT("ID\tArrive\tStart\tLeave\tTeller");
    for (auto &c : customers) {
        std::cout 
        << c.id << "\t"
        << c.arrive << "\t"
        << c.start << "\t"
        << c.leave << "\t"
        << c.teller_id << "\n";
    }
    std::cout << "Simulation finished at time " << end_time << " seconds." << std::endl;

    return 0;
}