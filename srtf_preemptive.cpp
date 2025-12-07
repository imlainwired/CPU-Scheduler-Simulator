#include "srtf_preemptive.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>
#include <vector>

using namespace std;

// Helper structure to track runtime variables
struct SRTFProc {
    int id;
    int at;
    int bt;
    int remaining_time;
    int ct = 0;
    int tat = 0;
    int wt = 0;
    int progress = 0; 
    int final_ct = 0;
};

// Global vector to store SRTF-specific data:
vector<SRTFProc> srtf_processes;


// --- Simulation Display Logic (SRTF version) ---
void printSimulation_srtf(const vector<Proc>& processes, int n, int current_time) {
    clearScreen();
    
    cout << "=== SRTF (Preemptive SJF) Scheduling Simulation ===" << endl;
    cout << "Current Global Time: " << current_time << " seconds" << endl;
    cout << "---------------------------------" << endl;
    
    for(int i = 0; i < n; ++i) {
        // Find the corresponding SRTFProc data
        auto it = find_if(srtf_processes.begin(), srtf_processes.end(), 
                          [&](const SRTFProc& p) { return p.id == processes[i].no; });

        if (it == srtf_processes.end()) continue;

        int current_rt = it->remaining_time;
        int total_bt = processes[i].bt;
        
        // Calculate progress based on remaining time
        int executed_time = total_bt - current_rt;
        int current_progress = (total_bt == 0) ? 100 : (int)((double)executed_time * 100.0 / total_bt);
        
        const int total_bar_length = 50;
        int display_progress = current_progress / 2;

        stringstream info;
        info << "P" << processes[i].no 
             << " (AT:" << processes[i].at << " | BT:" << total_bt << "s | RT:" << current_rt << "s): ";

        cout << left << setw(35) << info.str();
        
        cout << "[";
        for(int j = 0; j < display_progress; j++) {
            cout << "#";
        }
        for(int j = display_progress; j < total_bar_length; j++) {
            cout << " ";
        }
        cout << "] ";

        cout << setw(3) << current_progress << "%";
        
        if (it->remaining_time == 0) {
            cout << " - \033[1;32mCOMPLETED\033[0m (CT: " << it->final_ct << ")";
        } else if (processes[i].progress > 0) {
            // processes[i].progress is set to 1 for the currently running job
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


// Main SRTF Scheduling Logic
void run_srtf_preemptive_simulation() {
    int n;
    cout << "\n<--SRTF (Preemptive SJF) Simulation Selected-->" << endl;
    cout << "Enter Number of Processes: ";
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid number of processes." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    vector<Proc> p;
    srtf_processes.clear(); 

    for (int i = 0; i < n; ++i) {
        Proc proc_data = read_proc(i + 1);
        p.push_back(proc_data);
        
        SRTFProc new_srtf_proc;
        new_srtf_proc.id = proc_data.no;
        new_srtf_proc.at = proc_data.at;
        new_srtf_proc.bt = proc_data.bt;
        new_srtf_proc.remaining_time = proc_data.bt;
        
        srtf_processes.push_back(new_srtf_proc);
    }
    
    sort(p.begin(), p.end(), compareByAT);

    cout << "\nProcesses sorted. Simulation starting in 2 seconds...\n";
    this_thread::sleep_for(chrono::seconds(2));

    int current_time = 0;
    int completed_count = 0;
    
    int min_rt;
    int shortest_job_index = -1; 
    int prev_shortest_job_index = -1;

    while (completed_count < n) {
        
        min_rt = numeric_limits<int>::max();
        shortest_job_index = -1;

        for (int i = 0; i < n; ++i) {
            if (srtf_processes[i].at <= current_time && srtf_processes[i].remaining_time > 0) {
                if (srtf_processes[i].remaining_time < min_rt) {
                    min_rt = srtf_processes[i].remaining_time;
                    shortest_job_index = i;
                }
                // Tie-breaker: If RTs are same, use FCFS (compare by AT)
                else if (srtf_processes[i].remaining_time == min_rt && shortest_job_index != -1 && srtf_processes[i].at < srtf_processes[shortest_job_index].at) {
                    min_rt = srtf_processes[i].remaining_time;
                    shortest_job_index = i;
                }
            }
        }
        if (shortest_job_index == -1) {
                        
            bool future_processes_exist = false;
            for(const auto& proc : srtf_processes) {
                if (proc.remaining_time > 0) {
                    future_processes_exist = true;
                    break;
                }
            }

            if (future_processes_exist) {
                printSimulation_srtf(p, n, current_time);
                cout << "\nCPU is IDLE at T=" << current_time << ". Waiting for next arrival.\n";
                
                this_thread::sleep_for(chrono::seconds(1));
                current_time++; 
                continue; 
            } else {
                break;
            }
            
}
        
        int i = shortest_job_index;

        if (shortest_job_index != prev_shortest_job_index) {
            for(auto& proc : p) proc.progress = 0;
            p[i].progress = 1; 
        }

        printSimulation_srtf(p, n, current_time);
        
        this_thread::sleep_for(chrono::seconds(1));

        srtf_processes[i].remaining_time--;
        current_time++;

        for (int j = 0; j < n; ++j) {
            if (srtf_processes[j].remaining_time > 0 && srtf_processes[j].at <= current_time && j != i) {
                srtf_processes[j].wt++;
            }
        }

        if (srtf_processes[i].remaining_time == 0) {
            srtf_processes[i].final_ct = current_time;
            srtf_processes[i].tat = srtf_processes[i].final_ct - srtf_processes[i].at;
            completed_count++;
            
            p[i].progress = 0;
            
            printSimulation_srtf(p, n, current_time);
            cout << "\nProcess P" << srtf_processes[i].id << " COMPLETED at T=" << current_time << ".\n";
            this_thread::sleep_for(chrono::seconds(1)); 
        }

        prev_shortest_job_index = shortest_job_index;
    }

    double avgtat = 0.0;
    double avgwt = 0.0;
    
    for(auto& srtf_p : srtf_processes) {
        avgtat += srtf_p.tat;
        avgwt += srtf_p.wt;

        for(auto& proc : p) {
            if (proc.no == srtf_p.id) {
                proc.ct = srtf_p.final_ct;
                proc.tat = srtf_p.tat;
                proc.wt = srtf_p.wt;
                break;
            }
        }
    }

    avgtat /= n;
    avgwt /= n;

    clearScreen();
    cout << "\n=== SRTF Preemptive Simulation Complete ===" << endl;
    
    sort(p.begin(), p.end(), [](const Proc& a, const Proc& b) { return a.no < b.no; });

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

    cout << fixed << setprecision(2);
    cout << "\n\033[1;35mAverage Turnaround Time =\033[0m " << avgtat << " seconds" << endl;
    cout << "\033[1;35mAverage Waiting Time =\033[0m " << avgwt << " seconds" << endl;

    cout << "\n--- SRTF Simulation Finished ---\n" << endl;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Press Enter to return to main menu...";
    cin.get();
}
