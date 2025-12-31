#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <thread>  
#include <chrono>

struct MarketUpdate {
    std::string symbol;
    double price;
    int quantity;
    char side; 
};

class TickerPlant {
private:
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    
public:
    TickerPlant() {
        std::srand(std::time(0)); 
    }

    void startFeed(int totalUpdates) {
        std::cout << "=== MARKET DATA FEED ONLINE ===\n";
        std::cout << "Listening to Exchange...\n\n";

        for (int i = 0; i < totalUpdates; i++) {
            MarketUpdate tick = generateRandomTick();
            processTick(tick);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        }
        std::cout << "\n=== MARKET CLOSED ===\n";
    }

private:
    MarketUpdate generateRandomTick() {
        MarketUpdate tick;
        tick.symbol = symbols[std::rand() % symbols.size()];
        tick.price = 100.0 + (std::rand() % 1400); 
        tick.price += (std::rand() % 100) / 100.0;
        tick.quantity = (std::rand() % 10 + 1) * 10; 
        tick.side = (std::rand() % 2 == 0) ? 'B' : 'A';
        return tick;
    }

    void processTick(const MarketUpdate& tick) {
        std::cout << "[TICK] " << tick.symbol 
                  << " | " << (tick.side == 'B' ? "BID" : "ASK")
                  << " | " << tick.quantity << " @ $" << tick.price << "\n";
    }
};

int main() {
    TickerPlant feed;
    feed.startFeed(50);
    return 0;
}