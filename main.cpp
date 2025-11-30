#include "sjf_non_preemptive.hpp" 
#include "fcfscomplete.hpp"       
#include "scheduler_common.hpp"
#include <iostream>
#include <limits>

void display_menu() {
    clearScreen();
//    std::cout << "===========================================" << std::endl;
    std::cout << "|| CPU Scheduling Algorithm Simulator ||" << std::endl;
//    std::cout << "===========================================" << std::endl;
    std::cout << "Select the option:" << std::endl;
    std::cout << "1. Shortest-Job-First-Algorithm (Non-Preemptive)." << std::endl;
    std::cout << "2. First-Come-First-Served-Algorithm (Non-Preemptive)." << std::endl;
    std::cout << "0. Exit." << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    int choice;

    do {
        display_menu();
        
        // Input validation loop
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1; 
        }

        switch (choice) {
            case 1:
                // Assuming run_sjf_non_preemptive_simulation() is defined in sjf_non_preemptive.cpp
                run_sjf_non_preemptive_simulation();
                break;
            case 2:
                // NEW: Call the function from the fcfscomplete files
                run_fcfs_non_preemptive_simulation();
                break;
            case 0:
                std::cout << "\nExiting the simulator. Goodbye!\n";
                break;
            default:
                clearScreen();
                std::cout << "Invalid choice. Please enter 1, 2, or 0.\n";
                std::cout << "Press Enter to try again...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                break;
        }

    } while (choice != 0);

    return 0;
}
