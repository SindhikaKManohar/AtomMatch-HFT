#include <iostream>
#include <vector>
#include <winsock2.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

// --- 1. THE DATA STRUCTURES ---
#pragma pack(push, 1)
struct OrderPacket {
    int orderId;
    int quantity;
    int price;
    char side;
    long long timestamp;
};
#pragma pack(pop)

struct Order {
    int id;
    int price;
    int quantity;
};

// --- 2. THE ENGINE CLASS (Professional Encapsulation) ---
class MatchingEngine {
private:
    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;

public:
    // CONSTRUCTOR: Runs once when the engine starts
    MatchingEngine() {
        std::cout << "[ENGINE] Initializing Memory Pools...\n";
        // OPTIMIZATION: Reserve memory for 100,000 orders to handle the "Flood"
        buyOrders.reserve(100000); 
        sellOrders.reserve(100000);
        std::cout << "[ENGINE] Ready for High-Frequency Trading.\n";
    }

    // THE BRAIN: Handles one order
    void processOrder(int id, int price, int quantity, char side, long long timestamp) {
        
        // 1. MEASURE LATENCY (Stopwatch)
        auto now = std::chrono::high_resolution_clock::now();
        long long current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()
        ).count();
        long long latency = current_time - timestamp;

        // 2. MATCHING LOGIC
        if (side == 'B') {
            matchBuy(id, price, quantity, latency);
        } else {
            matchSell(id, price, quantity, latency);
        }
    }

private:
    void matchBuy(int id, int price, int quantity, long long latency) {
        bool matched = false;
        for (size_t i = 0; i < sellOrders.size(); i++) {
            if (sellOrders[i].price <= price) {
                // std::cout << ">>> 💰 TRADE! Buyer #" << id << " matched Seller #" << sellOrders[i].id << "\n";
                sellOrders.erase(sellOrders.begin() + i);
                matched = true;
                break;
            }
        }
        if (!matched) {
            buyOrders.push_back({id, price, quantity});
        }
    }

    void matchSell(int id, int price, int quantity, long long latency) {
        bool matched = false;
        for (size_t i = 0; i < buyOrders.size(); i++) {
            if (buyOrders[i].price >= price) {
                // std::cout << ">>> 💰 TRADE! Seller #" << id << " matched Buyer #" << buyOrders[i].id << "\n";
                buyOrders.erase(buyOrders.begin() + i);
                matched = true;
                break;
            }
        }
        if (!matched) {
            sellOrders.push_back({id, price, quantity});
        }
    }
};

// --- 3. THE SERVER (The Gateway) ---
int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    std::cout << "=== ZINDHU-X: PRO EDITION ===\n";
    std::cout << "Listening on Port 8888...\n\n";

    MatchingEngine engine; 

    OrderPacket packet;
    sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    while (true) {
        int bytes = recvfrom(serverSocket, (char*)&packet, sizeof(OrderPacket), 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytes == sizeof(OrderPacket)) {
            engine.processOrder(packet.orderId, packet.price, packet.quantity, packet.side, packet.timestamp);
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}