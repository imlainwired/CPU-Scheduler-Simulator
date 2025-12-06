#include "sjf_non_preemptive.hpp"
#include "scheduler_common.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>

// --- Simulation Display Logic ---
void printSimulation_sjf(const std::vector<Proc>& processes, int n, int current_time) {
    clearScreen();
    
    std::cout << "=== SJF (Non-Preemptive) Scheduling Simulation ===" << std::endl;
    std::cout << "Current Global Time: " << current_time << " seconds" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    
    const int total_bar_length = 50; 
    
    for(int i = 0; i < n; ++i) {
        std::stringstream info;
        info << "P" << processes[i].no 
             << " (AT:" << processes[i].at << " | BT:" << processes[i].bt << "s): ";

        std::cout << std::left << std::setw(25) << info.str();
        
        std::cout << "[";
        // Scale progress from 100 max back to 50 for display
        int display_progress = processes[i].progress / 2; 

        for(int j = 0; j < display_progress; j++) {
            std::cout << "#";
        }
        for(int j = display_progress; j < total_bar_length; j++) {
            std::cout << " ";
        }
        std::cout << "] ";

        // Percentage
        std::cout << std::setw(3) << processes[i].progress << "%"; 
        
        // Status display
        if (processes[i].is_completed) {
            std::cout << " - \033[1;32mCOMPLETED\033[0m (CT: " << processes[i].ct << ")";
        } else if (processes[i].progress > 0) {
            std::cout << " - \033[1;33mRUNNING\033[0m";
        } else if (current_time < processes[i].at) {
            std::cout << " - WAITING (Not Arrived)";
        } else {
            std::cout << " - WAITING (Ready Queue)";
        }
        
        std::cout << std::endl;
    }
    std::cout << "\n=================================\n";
}

// --- Main SJF Logic Function ---
void run_sjf_non_preemptive_simulation() {
    int n;
    std::cout << "\n<--SJF (Non-Preemptive) Simulation Selected-->" << std::endl;
    std::cout << "Enter Number of Processes: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Invalid number of processes." << std::endl;
        return;
    }

    std::vector<Proc> p;
    for (int i = 0; i < n; ++i) {
        p.push_back(read_proc(i + 1));
    }
    
    // Initial sort by AT
    std::sort(p.begin(), p.end(), compareByAT);

    std::cout << "\nSimulation starting in 2 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    int current_time = 0;
    float avgtat = 0.0f;
    float avgwt = 0.0f;
    int completed_count = 0;
    
    // Settings for improved timing accuracy
    const int total_progress_max = 100; 
    const int time_slice_ms = 20;

    // --- SJF Execution Loop ---
    while (completed_count < n) {
        
        // 1. Find the best process to run
        int selected_process_index = -1;
        int shortest_bt = 99999; 

        for (int i = 0; i < n; ++i) {
            if (p[i].at <= current_time && !p[i].is_completed) {
                if (p[i].bt < shortest_bt) {
                    shortest_bt = p[i].bt;
                    selected_process_index = i;
                } else if (p[i].bt == shortest_bt && selected_process_index != -1 && p[i].at < p[selected_process_index].at) {
                    // Tie-breaker: FCFS
                    shortest_bt = p[i].bt;
                    selected_process_index = i;
                }
            }
        }

        // 2. Handle Idle Time
        if (selected_process_index == -1) {
            int next_arrival_time = -1;
            for(int i = 0; i < n; ++i) {
                if (!p[i].is_completed && (next_arrival_time == -1 || p[i].at < next_arrival_time)) {
                    next_arrival_time = p[i].at;
                }
            }
            
            if (next_arrival_time != -1 && current_time < next_arrival_time) {
                 int idle_start = current_time;
                 while (current_time < next_arrival_time) {
                      printSimulation_sjf(p, n, current_time);
                      current_time++; 
                      std::this_thread::sleep_for(std::chrono::seconds(1));
                 }
                 std::cout << "\nCPU was IDLE from T=" << idle_start << " to T=" << current_time << ".\n";
                 std::this_thread::sleep_for(std::chrono::seconds(1));
                 continue; 
            } else {
                break; 
            }
        }
        
        // 3. Execute the Selected Process
        int i = selected_process_index;
        int start_time = current_time;
        
        long long total_duration_ms = (long long)p[i].bt * 1000;
        long long elapsed_ms = 0;
        
        // Time taken for one progress percentage point 
        long long ms_per_percent = total_duration_ms / total_progress_max;

        while (p[i].progress < total_progress_max) {
            
            if (elapsed_ms >= ms_per_percent * (p[i].progress + 1)) {
                p[i].progress++;
            }
            
            printSimulation_sjf(p, n, current_time);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(time_slice_ms));
            elapsed_ms += time_slice_ms;

            if (elapsed_ms / 1000 > (current_time - start_time)) {
                 current_time = start_time + (elapsed_ms / 1000);
            }
        }

        // 4. Completion and Metric Update
        p[i].ct = start_time + p[i].bt;
        p[i].is_completed = true;
        completed_count++;
        current_time = p[i].ct;

        // Calculate metrics
        p[i].tat = p[i].ct - p[i].at;
        avgtat += p[i].tat;
        p[i].wt = p[i].tat - p[i].bt;
        avgwt += p[i].wt;
        
        printSimulation_sjf(p, n, current_time);
        std::cout << "\nProcess P" << p[i].no << " Completed at T=" << p[i].ct << " (Shortest Job First).\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
    }

    avgtat /= n;
    avgwt /= n;

    clearScreen();
    std::cout << "\n=== SJF Non-Preemptive Simulation Complete ===" << std::endl;
    
    // Print Final Table
    std::cout << "\n\033[1;36m| Process | AT | BT | CT | TAT | WT |\033[0m" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::sort(p.begin(), p.end(), [](const Proc& a, const Proc& b) { return a.no < b.no; });
    
    for(const auto& proc : p) {
        std::cout << "| P" << std::setw(6) << std::left << proc.no << "|" 
             << std::setw(3) << proc.at << "|" 
             << std::setw(3) << proc.bt << "|" 
             << std::setw(3) << proc.ct << "|" 
             << std::setw(4) << proc.tat << "|" 
             << std::setw(4) << proc.wt << " |" << std::endl;
    }

    // Print Averages
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n\033[1;35mAverage Turnaround Time =\033[0m " << avgtat << " seconds" << std::endl;
    std::cout << "\033[1;35mAverage Waiting Time =\033[0m " << avgwt << " seconds" << std::endl;

    std::cout << "\n--- SJF Simulation Finished ---\n" << std::endl;
    
    std::cin.ignore();
    std::cout << "Press Enter to return to main menu...";
    std::cin.get();
}

