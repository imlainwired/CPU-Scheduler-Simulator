#ifndef SCHEDULER_COMMON_HPP
#define SCHEDULER_COMMON_HPP

#include <vector>
#include <iostream>

// FINAL Proc Structure:
struct Proc {
    int no;          // Process number
    int at;          // Arrival Time
    int bt;          // Burst Time
    int priority;    // Priority
    
    // Calculated Metrics
    int ct;          // Completion Time
    int tat;         // Turn Around Time
    int wt;          // Waiting Time
    
    // Simulation tracking
    int progress;
    bool is_completed; // Process state flag
};

// Shared Function Declarations:
void clearScreen();
Proc read_proc(int i);
bool compareByAT(const Proc& a, const Proc& b);

#endif 
