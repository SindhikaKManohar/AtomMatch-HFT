#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <map>
#include <windows.h> 

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

enum Side { BUY, SELL };

struct Order {
    int id;
    double price;
    int quantity;
    Side side;
};


template<typename T>
class LockFreeQueue {
private:
    std::vector<T> buffer;
    size_t capacity;
    std::atomic<size_t> head; 
    std::atomic<size_t> tail; 

public:
    LockFreeQueue(size_t size) : capacity(size), head(0), tail(0) {
        buffer.resize(size);
    }

    bool push(const T& item) {
        size_t currentTail = tail.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % capacity;
        if (nextTail == head.load(std::memory_order_acquire)) return false; 
        buffer[currentTail] = item;
        tail.store(nextTail, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        size_t currentHead = head.load(std::memory_order_relaxed);
        if (currentHead == tail.load(std::memory_order_acquire)) return false; 
        item = buffer[currentHead];
        head.store((currentHead + 1) % capacity, std::memory_order_release);
        return true;
    }
};


class MatchingEngine {
private:
    std::map<double, int, std::greater<double>> bids;
    std::map<double, int> asks;

public:
    void processOrder(const Order& order) {
        if (likely(order.side == BUY)) {
            auto bestAsk = asks.begin();
            if (bestAsk != asks.end() && bestAsk->first <= order.price) {
                asks.erase(bestAsk);
            } else {
                bids[order.price] += order.quantity; 
            }
        } 
        else { 
            auto bestBid = bids.begin();
            if (bestBid != bids.end() && bestBid->first >= order.price) {
                bids.erase(bestBid); 
            } else {
                asks[order.price] += order.quantity; 
            }
        }
    }
};

void pinThread(int core_id) {
    DWORD_PTR mask = (1 << core_id);
    SetThreadAffinityMask(GetCurrentThread(), mask);
}


void tickerPlant(LockFreeQueue<Order>& queue, std::atomic<bool>& running) {
    pinThread(0); 
    int orderID = 0;

    while (running) {
        Order ord;
        ord.id = ++orderID;
        ord.price = 100.0 + (rand() % 5); 
        ord.quantity = 10;
        ord.side = (rand() % 2 == 0) ? BUY : SELL;

        while (!queue.push(ord) && running) {
        }
    
        std::this_thread::sleep_for(std::chrono::microseconds(1)); 
    }
}

void coreEngine(LockFreeQueue<Order>& queue, std::atomic<bool>& running) {
    pinThread(1); 
    MatchingEngine engine;
    Order ord;
    long long processedCount = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (running) {
        if (queue.pop(ord)) {
            engine.processOrder(ord);
            processedCount++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\n=== SIMULATION ENDED ===\n";
    std::cout << "Orders Processed: " << processedCount << "\n";
    std::cout << "Time Elapsed:     " << duration.count() << " ms\n";
    std::cout << "Throughput:       " << (processedCount * 1000) / (duration.count() + 1) << " orders/sec\n";
}

int main() {
    std::cout << "============================================\n";
    std::cout << "   TOWER RESEARCH SIMULATOR (SINDHIKA V1)   \n";
    std::cout << "   [Lock-Free] [Pinned] [Optimized]         \n";
    std::cout << "============================================\n";

    LockFreeQueue<Order> queue(10000);
    std::atomic<bool> running(true);

    std::thread producer(tickerPlant, std::ref(queue), std::ref(running));
    std::thread consumer(coreEngine, std::ref(queue), std::ref(running));

    std::cout << ">>> Trading Engine RUNNING for 3 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    running = false;
    producer.join();
    consumer.join();

    return 0;
}