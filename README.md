# AtomMatch: High-Frequency Trading Engine 🚀

AtomMatch is a simulated low-latency Limit Order Book and Matching Engine built in **C++17**. It demonstrates extreme optimization techniques including **Lock-Free Concurrency**, **CPU Pinning**, and **Branch Prediction**.

## ⚡ Performance Benchmarks
* **Throughput:** ~12,300,000 orders/second (Core Matching Loop on Consumer Hardware)
* **Latency:** Optimized for sub-microsecond tick-to-trade processing.

## 🛠️ Key Technologies
* **Language:** C++17 (STL, Atomics, std::string_view)
* **Concurrency:** Lock-Free Single-Producer Single-Consumer (SPSC) Ring Buffer.
* **Memory Management:** Custom Memory Pool (Arena) to prevent runtime `malloc` overhead.
* **OS Optimization:**
    * **CPU Affinity:** Threads are pinned to specific cores (Core 0 & Core 1) to prevent context switching.
    * **Branch Prediction:** Hot paths annotated with `likely()`/`unlikely()` hints.

## 📂 Architecture
The system follows a typical HFT "Producer-Consumer" architecture:

1.  **Ticker Plant (Producer - Core 0):** Generates synthetic high-velocity market orders.
2.  **Lock-Free Queue:** A circular buffer using `std::atomic` to transmit data without Mutex locks.
3.  **Matching Engine (Consumer - Core 1):** Processes orders using a Price-Time Priority algorithm (`std::map`).

## 🚀 How to Run
1. **Compile with optimizations:**
   ```bash
   g++ Tower_Sim.cpp -o engine -O3 -lws2_32
