#ifndef COMPARISON_TOOL_HPP
#define COMPARISON_TOOL_HPP

#include <vector>
// Proc structure aur common functions ke liye
#include "scheduler_common.hpp" 
#include <string> // std::string ke liye

// Structure to hold the result of one algorithm
struct AlgoResult {
    std::string algo_name;
    double avg_tat;
    double avg_wt;
};

// Function to run multiple algorithms on the same dataset
void run_all_algorithms_comparison();

#endif // COMPARISON_TOOL_HPP
