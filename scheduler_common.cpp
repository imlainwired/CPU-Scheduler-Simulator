#include "scheduler_common.hpp"
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    void clearScreen() {
        system("cls");
    }
#endif

// Input Function:
Proc read_proc(int i) {
    Proc p;
    std::cout << "\nProcess No: " << i << std::endl;
    p.no = i;
    p.progress = 0; 
    p.ct = 0;      
    p.is_completed = false; // Initialized here for safety
    std::cout << "Enter Arrival Time (AT in seconds): ";
    std::cin >> p.at;
    std::cout << "Enter Burst Time (BT in seconds): ";
    std::cin >> p.bt;
    return p;
}

// Comparison function for sorting by Arrival Time:
bool compareByAT(const Proc& a, const Proc& b) {
    return a.at < b.at;
}
