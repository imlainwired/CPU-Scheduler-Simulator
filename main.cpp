#include "sjf_non_preemptive.hpp"
#include "fcfscomplete.hpp"       
#include "priority_non_preemptive.hpp" 
#include "srtf_preemptive.hpp"     
#include "scheduler_common.hpp" 
#include <iostream>
#include <limits> 

void display_menu() {
    clearScreen();
    // std::cout << "===========================================" << std::endl;
    std::cout << "|| CPU Scheduling Algorithm Simulator ||" << std::endl;
    // std::cout << "===========================================" << std::endl;
    std::cout << "Select the option:" << std::endl;
    std::cout << "1. Shortest Job First (SJF) Algorithm (Non-Preemptive)" << std::endl;
    std::cout << "2. First-Come, First-Served (FCFS) Algorithm (Non-Preemptive)" << std::endl;
    std::cout << "3. Priority Scheduling Algorithm (Non-Preemptive)" << std::endl;
    std::cout << "4. Shortest Remaining Time First (SRTF) Algorithm (Preemptive)" << std::endl; // NEW OPTION
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter choice: ";
}

int main() {
    int choice;

    do {
        display_menu();
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1; 
        }

        switch (choice) {
            case 1:
                run_sjf_non_preemptive_simulation();
                break;
            case 2:
                run_fcfs_non_preemptive_simulation();
                break;
            case 3:
                run_priority_non_preemptive_simulation();
                break;
            case 4: 
                run_srtf_preemptive_simulation();
                break;
            case 0:
                std::cout << "\nExiting the simulator. Goodbye!\n";
                break;
            default:
                clearScreen();
                std::cout << "Invalid choice. Please enter 1, 2, 3, 4, or 0.\n";
                std::cout << "Press Enter to try again...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                break;
        }

    } while (choice != 0);

    return 0;
}
