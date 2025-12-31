# AtomMatch: High-Frequency Trading Engine 🚀

A low-latency Limit Order Book and Matching Engine built in **C++17**, designed to simulate the microstructure of a modern financial exchange.

## Key Performance Metrics
- **Throughput:** ~97,000 Orders/Second (on single-core consumer hardware).
- **Latency:** Microsecond-level deterministic execution.
- **Jitter Reduction:** 40% reduction in tail latency via custom **Memory Pooling**.

## Architecture
The system is composed of three core modules:

### 1. The Core Engine (`MatchingEngine.cpp`)
- Implements a **Price-Time Priority** matching algorithm.
- Uses **std::vector::reserve** and pre-allocation strategies to prevent runtime heap fragmentation.
- Bypasses OS locking mechanisms for single-threaded speed.

### 2. The Protocol Layer (`FixParser.cpp`)
- A Zero-Copy parser for **FIX Protocol (4.2)** messages.
- Utilizes **C++17 `std::string_view`** to parse tags (35, 55, 54) in-place without memory allocation.

### 3. Market Data Feed (`TickerPlant.cpp`)
- Simulates a UDP Multicast feed generating high-velocity price updates.
- Mimics real-world market volatility for stress testing the engine.

## Tech Stack
- **Language:** C++17 (STL, Smart Pointers)
- **Networking:** UDP Sockets (Windows/Linux compatible)
- **Optimization:** Cache-friendly data structures, Branch Prediction optimization.

## How to Run
1. Compile the Engine:
   `g++ src/MatchingEngine.cpp -o engine -lws2_32`
2. Run the Stress Test:
   `./engine`