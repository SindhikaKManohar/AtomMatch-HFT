#include <iostream>
#include <string_view> 
#include <vector>

class FastFixParser {
public:
    // We take a 'string_view' (Just a look, not a copy)
    void parse(std::string_view message) {
        std::cout << "\n[FAST PARSER] Processing: " << message << "\n";

        size_t start = 0;
        size_t end = message.find('|'); 

        while (end != std::string_view::npos) {
            std::string_view token = message.substr(start, end - start);
            size_t eqPos = token.find('=');
            
            if (eqPos != std::string_view::npos) {
                std::string_view tag = token.substr(0, eqPos);
                std::string_view value = token.substr(eqPos + 1);

                // DECODE IN PLACE (No memory allocation!)
                if (tag == "35") {
                    std::cout << "  > TYPE: " << (value == "D" ? "New Order" : value) << "\n";
                }
                else if (tag == "55") {
                    std::cout << "  > SYMBOL: " << value << "\n";
                }
                else if (tag == "54") {
                    std::cout << "  > SIDE: " << (value == "1" ? "Buy" : "Sell") << "\n";
                }
                else if (tag == "38") {
                    std::cout << "  > QTY: " << value << "\n";
                }
                else if (tag == "44") {
                    std::cout << "  > PRICE: " << value << "\n";
                }
            }
            start = end + 1;
            end = message.find('|', start);
        }
    }
};

int main() {
    std::cout << "=== ZERO-COPY FIX PARSER (C++17) ===\n";
    FastFixParser parser;
    const char* raw_data = "8=FIX.4.2|35=D|55=INFY|54=2|38=500|44=1450.00|";
    parser.parse(raw_data);
    return 0;
}