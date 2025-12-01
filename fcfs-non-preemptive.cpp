#include "scheduler_common.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

void printSimulation_fcfs(const vector<Proc>& processes, int n, int current_time) {
    clearScreen();
    
    cout << "=== FCFS Scheduling Simulation ===" << endl;
    cout << "Current Global Time: " << current_time << " seconds" << endl;
    cout << "---------------------------------" << endl;
    
    const int total_bar_length = 50;
    
    for(int i = 0; i < n; ++i) {
        stringstream info;
        info << "P" << processes[i].no 
             << " (AT:" << processes[i].at << " | BT:" << processes[i].bt << "s): ";

        cout << left << setw(25) << info.str();
        
        cout << "[";
        for(int j = 0; j < processes[i].progress; j++) {
            cout << "#";
        }
        for(int j = processes[i].progress; j < total_bar_length; j++) {
            cout << " ";
        }
        cout << "] ";

        cout << setw(3) << (processes[i].progress * 100 / total_bar_length) << "%";
        
        if (processes[i].progress == total_bar_length) {
            cout << " - \033[1;32mCOMPLETED\033[0m (CT: " << processes[i].ct << ")";
        } else if (processes[i].progress > 0) {
            cout << " - \033[1;33mRUNNING\033[0m";
        } else if (current_time < processes[i].at) {
            cout << " - WAITING (Not Arrived)";
        } else {
            cout << " - WAITING (In Queue)";
        }
        
        cout << endl;
    }
}

void run_fcfs_non_preemptive_simulation() {
    int n;
    cout << "\n<--FCFS Scheduling Algorithm Simulation Selected-->" << endl;
    cout << "Enter Number of Processes: ";
    cin >> n;

    if (n <= 0) {
        cout << "Invalid number of processes." << endl;
        return;
    }

    vector<Proc> p;
    for (int i = 0; i < n; ++i) {
        p.push_back(read_proc(i + 1));
    }
    
    sort(p.begin(), p.end(), compareByAT);

    cout << "\nProcesses sorted by Arrival Time. Simulation starting in 2 seconds...\n";
    this_thread::sleep_for(chrono::seconds(2));

    int current_time = 0;
    float avgtat = 0.0f;
    float avgwt = 0.0f;
    const int total_bar_length = 50; 
    const int time_slice_ms = 20; 
    
    for (int i = 0; i < n; ++i) {
        
        if (current_time < p[i].at) {
            int idle_start = current_time;
            
            while (current_time < p[i].at) {
                 printSimulation_fcfs(p, n, current_time);
                 current_time++; 
                 this_thread::sleep_for(chrono::seconds(1));
            }
            cout << "\nCPU was IDLE from T=" << idle_start << " to T=" << p[i].at << ".\n";
            this_thread::sleep_for(chrono::seconds(1));
        }
                
        int start_time = current_time;
        long long total_duration_ms = (long long)p[i].bt * 1000;
        long long elapsed_ms = 0;
        
        long long ms_per_hash = total_duration_ms / total_bar_length;

        while (p[i].progress < total_bar_length) {
            
            if (elapsed_ms >= ms_per_hash * (p[i].progress + 1)) {
                p[i].progress++;
            }
            
            printSimulation_fcfs(p, n, current_time);
            
            this_thread::sleep_for(chrono::milliseconds(time_slice_ms));
            elapsed_ms += time_slice_ms;

            if (elapsed_ms / 1000 > (current_time - start_time)) {
                 current_time = start_time + (elapsed_ms / 1000);
            }
        }

        p[i].ct = start_time + p[i].bt;
        current_time = p[i].ct; 

        p[i].tat = p[i].ct - p[i].at;
        avgtat += p[i].tat;
        p[i].wt = p[i].tat - p[i].bt;
        avgwt += p[i].wt;
        
        printSimulation_fcfs(p, n, current_time);
        cout << "\nProcess P" << p[i].no << " Completed at T=" << p[i].ct << ".\n";
        this_thread::sleep_for(chrono::seconds(1)); 
    }

    avgtat /= n;
    avgwt /= n;

    clearScreen();
    cout << "\n=== FCFS Non-Preemptive Simulation Complete ===" << endl;
    
    cout << "\n\033[1;36m| Process | AT | BT | CT | TAT | WT |\033[0m" << endl;
    cout << "---------------------------------------" << endl;
    for(const auto& proc : p) {
        cout << "| P" << setw(6) << left << proc.no << "|" 
             << setw(3) << proc.at << "|" 
             << setw(3) << proc.bt << "|" 
             << setw(3) << proc.ct << "|" 
             << setw(4) << proc.tat << "|" 
             << setw(4) << proc.wt << " |" << endl;
    }

    cout << std::fixed << std::setprecision(2);
    cout << "\n\033[1;35mAverage Turnaround Time =\033[0m " << avgtat << " seconds" << endl;
    cout << "\033[1;35mAverage Waiting Time =\033[0m " << avgwt << " seconds" << endl;
    
    // Wait for user before returning to the main menu
    cin.ignore();
    cout << "Press Enter to return to main menu...";
    cin.get();
}
