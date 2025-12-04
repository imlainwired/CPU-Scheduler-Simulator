#include "round_robin.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>
#include <queue>
#include <map>  
using namespace std;

struct RRProcessData {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time; 
    int waiting_time = 0;
    int turn_around_time = 0;
    int completion_time = 0;
};

vector<RRProcessData> rr_processes;

// Simulation Display Logic
void printSimulation_rr(const vector<Proc>& p_display, int n, int current_time, int quantum, int running_id) {
    clearScreen();
    
    cout << "=== Round Robin (RR) Scheduling Simulation ===" << endl;
    cout << "Time Quantum (Q): " << quantum << "s | Current Global Time: " << current_time << " seconds" << endl;
    
    for(int i = 0; i < n; ++i) {
        auto it = find_if(rr_processes.begin(), rr_processes.end(), 
                          [&](const RRProcessData& p) { return p.id == p_display[i].no; });

        if (it == rr_processes.end()) continue;

        int current_rt = it->remaining_time;
        int total_bt = it->burst_time;
        
        int executed_time = total_bt - current_rt;
        int current_progress = (total_bt == 0) ? 100 : (int)((double)executed_time * 100.0 / total_bt);
        
        const int total_bar_length = 50;
        int display_progress = current_progress / 2;

        stringstream info;
        info << "P" << it->id 
             << " (AT:" << it->arrival_time << " | BT:" << total_bt << "s | RT:" << current_rt << "s): ";

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
            cout << " - \033[1;32mCOMPLETED\033[0m (CT: " << it->completion_time << ")";
        } else if (it->id == running_id) {
            cout << " - \033[1;33mRUNNING\033[0m";
        } else if (current_time < it->arrival_time) {
            cout << " - WAITING (Not Arrived)";
        } else {
            cout << " - WAITING (Ready Queue)";
        }
        
        cout << endl;
    }
}


// Main Round Robin Scheduling Logic
void run_round_robin_simulation() {
    int n;
    cout << "\n<--Round Robin (RR) Simulation Selected-->" << endl;
    cout << "Enter Number of Processes: ";
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid number of processes." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    int quantum;
    cout << "Enter Time Quantum (Q) in seconds: ";
    if (!(cin >> quantum) || quantum <= 0) {
        cout << "Invalid Time Quantum." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    
    vector<Proc> p;
    rr_processes.clear();

    for (int i = 0; i < n; ++i) {
    Proc proc_data = read_proc(i + 1);
    p.push_back(proc_data);
    
      // Initialize RR tracking structure using object initialization
    RRProcessData new_rr_proc;
    new_rr_proc.id = proc_data.no; 
    new_rr_proc.arrival_time = proc_data.at; 
    new_rr_proc.burst_time = proc_data.bt; 
    new_rr_proc.remaining_time = proc_data.bt;
    rr_processes.push_back(new_rr_proc);
}
    
 
    sort(p.begin(), p.end(), compareByAT);

    cout << "\nProcesses sorted. Simulation starting in 2 seconds...\n";
    this_thread::sleep_for(chrono::seconds(2));

    int current_time = 0;
    int completed_count = 0;
    
    queue<int> ready_queue; 
    
    map<int, bool> in_queue; 

    while (completed_count < n) {
        
        for (int i = 0; i < n; ++i) {
            if (rr_processes[i].arrival_time <= current_time && !in_queue[rr_processes[i].id] && rr_processes[i].remaining_time > 0) {
                ready_queue.push(i); 
                in_queue[rr_processes[i].id] = true;
            }
        }
        
        int running_index = -1;
        int running_id = -1;

        if (!ready_queue.empty()) {
            running_index = ready_queue.front(); 
            ready_queue.pop();
            running_id = rr_processes[running_index].id;
            
            in_queue[running_id] = false; 
        }

        if (running_index == -1) {
            int next_arrival_time = numeric_limits<int>::max();
            bool processes_waiting = false;

            for(const auto& proc : rr_processes) {
                if (proc.remaining_time > 0) {
                    processes_waiting = true;
                    if (proc.arrival_time > current_time && proc.arrival_time < next_arrival_time) {
                        next_arrival_time = proc.arrival_time;
                    }
                }
            }

            if (processes_waiting && next_arrival_time != numeric_limits<int>::max()) {
                 int idle_duration = next_arrival_time - current_time;
                 
                 printSimulation_rr(p, n, current_time, quantum, running_id);
                 cout << "\nCPU is IDLE for " << idle_duration << "s (T=" << current_time << " to T=" << next_arrival_time << ").\n";
                 this_thread::sleep_for(chrono::seconds(2)); 
                 
                 current_time = next_arrival_time;
                 continue; 
            } else {
                break; 
            }
        }
        
        int run_time = min(rr_processes[running_index].remaining_time, quantum);
        int execution_start_time = current_time;
        
        printSimulation_rr(p, n, current_time, quantum, running_id);

        for(int t = 0; t < run_time; ++t) {
            
            for (int j = 0; j < n; ++j) {
                if (j != running_index && rr_processes[j].remaining_time > 0 && rr_processes[j].arrival_time <= current_time) {
                    rr_processes[j].waiting_time++;
                }
            }
            
            rr_processes[running_index].remaining_time--;
            current_time++;

            // Check for new arrivals every second and add to queue
            for (int i = 0; i < n; ++i) {
                if (rr_processes[i].arrival_time == current_time && !in_queue[rr_processes[i].id] && rr_processes[i].remaining_time > 0) {
                    ready_queue.push(i);
                    in_queue[rr_processes[i].id] = true;
                }
            }

            printSimulation_rr(p, n, current_time, quantum, running_id);
            this_thread::sleep_for(chrono::seconds(1));
        }

        
        if (rr_processes[running_index].remaining_time == 0) {
            rr_processes[running_index].completion_time = current_time;
            rr_processes[running_index].turn_around_time = rr_processes[running_index].completion_time - rr_processes[running_index].arrival_time;
            completed_count++;

            printSimulation_rr(p, n, current_time, quantum, -1); // -1 means no process is running
            cout << "\nProcess P" << running_id << " COMPLETED at T=" << current_time << ".\n";
            this_thread::sleep_for(chrono::seconds(1)); 
            
        } else {
            
            ready_queue.push(running_index); 
            in_queue[running_id] = true; 
            
            printSimulation_rr(p, n, current_time, quantum, -1);
            cout << "\nProcess P" << running_id << " preempted. Quantum expired at T=" << current_time << ".\n";
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    double avgtat = 0.0;
    double avgwt = 0.0;
    
    for(auto& rr_p : rr_processes) {
        avgtat += rr_p.turn_around_time;
        avgwt += rr_p.waiting_time;

        for(auto& proc : p) {
            if (proc.no == rr_p.id) {
                proc.ct = rr_p.completion_time;
                proc.tat = rr_p.turn_around_time;
                proc.wt = rr_p.waiting_time;
                break;
            }
        }
    }

    avgtat /= n;
    avgwt /= n;

    clearScreen();
    cout << "\n=== Round Robin Simulation Complete ===" << endl;
    
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

    cout << "\n--- Round Robin Simulation Finished ---\n" << endl;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Press Enter to return to main menu...";
    cin.get();
}

