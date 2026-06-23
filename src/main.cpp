#include <iostream>
#include "simulation.hpp"

int main() {
    try {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "DZHANIBEKOV - PHYSICALLY ACCURATE C++ SIMULATION" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "\nInitializing simulation...\n" << std::endl;
        
        Simulation sim(900, 600);
        sim.run();
        
        std::cout << "\n✅ Simulation closed safely\n" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
