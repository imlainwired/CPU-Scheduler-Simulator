#include "priority_non_preemptive.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits> 

using namespace std;

Proc read_proc_priority(int i) {
    Proc p;
    cout << "\nProcess No: " << i << endl;
    p.no = i;
    p.progress = 0; 
    p.ct = 0;      
    p.is_completed = false; 
    
    // --- Priority Input ---
    cout << "Enter Priority (Lower number = Higher Priority): ";
    cin >> p.priority;
    
    cout << "Enter Arrival Time (AT in seconds): ";
    cin >> p.at;
    cout << "Enter Burst Time (BT in seconds): ";
    cin >> p.bt;
    return p;
}


// --- Simulation Display Logic (Priority version) ---
void printSimulation_priority(const vector<Proc>& processes, int n, int current_time) {
    clearScreen();
    
    cout << "=== Priority (Non-Preemptive) Scheduling Simulation ===" << endl;
    cout << "Current Global Time: " << current_time << " seconds" << endl;
    cout << "---------------------------------" << endl;
    
    const int total_bar_length = 50; 
    
    for(int i = 0; i < n; ++i) {
        stringstream info;
        info << "P" << processes[i].no 
             << " (Prio:" << processes[i].priority << " | AT:" << processes[i].at << " | BT:" << processes[i].bt << "s): ";

        cout << left << setw(35) << info.str();
        
        cout << "[";
        int display_progress = processes[i].progress / 2;

        for(int j = 0; j < display_progress; j++) {
            cout << "#";
        }
        for(int j = display_progress; j < total_bar_length; j++) {
            cout << " ";
        }
        cout << "] ";

        cout << setw(3) << processes[i].progress << "%";
        
        if (processes[i].is_completed) {
            cout << " - \033[1;32mCOMPLETED\033[0m (CT: " << processes[i].ct << ")";
        } else if (processes[i].progress > 0) {
            cout << " - \033[1;33mRUNNING\033[0m";
        } else if (current_time < processes[i].at) {
            cout << " - WAITING (Not Arrived)";
        } else {
            cout << " - WAITING (Ready Queue)";
        }
        
        cout << endl;
    }
    cout << "\n=================================\n";
}


// --- Main Priority Scheduling Logic ---
void run_priority_non_preemptive_simulation() {
    int n;
    cout << "\n<--Priority (Non-Preemptive) Simulation Selected-->" << endl;
    cout << "Enter Number of Processes: ";
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid number of processes." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    vector<Proc> p;
    for (int i = 0; i < n; ++i) {
        p.push_back(read_proc_priority(i + 1)); // Use the modified input function
    }
    
    // Initial sort by AT (needed for the ready queue management)
    sort(p.begin(), p.end(), compareByAT);

    cout << "\nProcesses sorted. Simulation starting in 2 seconds...\n";
    this_thread::sleep_for(chrono::seconds(2));

    int current_time = 0;
    float avgtat = 0.0f;
    float avgwt = 0.0f;
    int completed_count = 0;
    
    const int total_progress_max = 100; 
    const int time_slice_ms = 20;

    // --- Priority Execution Loop ---
    while (completed_count < n) {
        
        // 1. Find the best process to run (Highest Priority First)
        int selected_process_index = -1;
        int highest_priority = 10; 

        for (int i = 0; i < n; ++i) {
            if (p[i].at <= current_time && !p[i].is_completed) {
                if (p[i].priority < highest_priority) {
                    highest_priority = p[i].priority;
                    selected_process_index = i;
                } 
                // Tie-breaker: If priorities are the same, use FCFS (compare by AT)
                else if (p[i].priority == highest_priority && p[i].at < p[selected_process_index].at) {
                    highest_priority = p[i].priority;
                    selected_process_index = i;
                }
            }
        }

        // 2. Handle Idle Time
        if (selected_process_index == -1) {
            // ... (Idle time logic is the same as SJF/FCFS) ...
            int next_arrival_time = -1;
            for(int i = 0; i < n; ++i) {
                if (!p[i].is_completed && (next_arrival_time == -1 || p[i].at < next_arrival_time)) {
                    next_arrival_time = p[i].at;
                }
            }
            
            if (next_arrival_time != -1 && current_time < next_arrival_time) {
                 int idle_start = current_time;
                 while (current_time < next_arrival_time) {
                      printSimulation_priority(p, n, current_time);
                      current_time++; 
                      this_thread::sleep_for(chrono::seconds(1));
                 }
                 cout << "\nCPU was IDLE from T=" << idle_start << " to T=" << current_time << ".\n";
                 this_thread::sleep_for(chrono::seconds(1));
                 continue; 
            } else {
                break; 
            }
        }
        
        // 3. Execute the Selected Process (Non-Preemptive)
        int i = selected_process_index;
        int start_time = current_time;
        
        long long total_duration_ms = (long long)p[i].bt * 1000;
        long long elapsed_ms = 0;
        long long ms_per_percent = total_duration_ms / total_progress_max;

        while (p[i].progress < total_progress_max) {
            
            if (elapsed_ms >= ms_per_percent * (p[i].progress + 1)) {
                p[i].progress++;
            }
            
            printSimulation_priority(p, n, current_time);
            
            this_thread::sleep_for(chrono::milliseconds(time_slice_ms));
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

        p[i].tat = p[i].ct - p[i].at;
        avgtat += p[i].tat;
        p[i].wt = p[i].tat - p[i].bt;
        avgwt += p[i].wt;
        
        printSimulation_priority(p, n, current_time);
        cout << "\nProcess P" << p[i].no << " Completed at T=" << p[i].ct << " (Priority " << p[i].priority << ").\n";
        this_thread::sleep_for(chrono::seconds(1)); 
    }

    // --- Final Results ---
    avgtat /= n;
    avgwt /= n;

    clearScreen();
    cout << "\n=== Priority Non-Preemptive Simulation Complete ===" << endl;
    
    // Print Final Table
    cout << "\n\033[1;36m| Process | Prio | AT | BT | CT | TAT | WT |\033[0m" << endl;
    cout << "-----------------------------------------------" << endl;
    sort(p.begin(), p.end(), [](const Proc& a, const Proc& b) { return a.no < b.no; });
    
    for(const auto& proc : p) {
        cout << "| P" << setw(6) << left << proc.no << "|" 
             << setw(5) << proc.priority << "|" 
             << setw(3) << proc.at << "|" 
             << setw(3) << proc.bt << "|" 
             << setw(3) << proc.ct << "|" 
             << setw(4) << proc.tat << "|" 
             << setw(4) << proc.wt << " |" << endl;
    }

    cout << fixed << setprecision(2);
    cout << "\n\033[1;35mAverage Turnaround Time =\033[0m " << avgtat << " seconds" << endl;
    cout << "\033[1;35mAverage Waiting Time =\033[0m " << avgwt << " seconds" << endl;

    cout << "\n--- Priority Simulation Finished ---\n" << endl;
    
    cin.ignore();
    cout << "Press Enter to return to main menu...";
    cin.get();
}
