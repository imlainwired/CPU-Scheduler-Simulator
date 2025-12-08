#include "comparison_tool.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <queue> 
#include <map>
#include <cmath> 
#include <string> // For std::to_string

using namespace std;

// --- FAST FCFS (Non-Visual) ---
AlgoResult calculate_fcfs(const vector<Proc>& input_procs) {
    if (input_procs.empty()) return {"FCFS", 0.0, 0.0};
    vector<Proc> p = input_procs;
    sort(p.begin(), p.end(), compareByAT);

    int current_time = 0;
    double total_tat = 0;
    double total_wt = 0;

    for (size_t i = 0; i < p.size(); ++i) {
        if (current_time < p[i].at) {
            current_time = p[i].at;
        }

        p[i].ct = current_time + p[i].bt;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        current_time = p[i].ct;
        total_tat += p[i].tat;
        total_wt += p[i].wt;
    }

    return {"FCFS", total_tat / p.size(), total_wt / p.size()};
}

// --- FAST SJF (Non-Preemptive, Non-Visual) ---
AlgoResult calculate_sjf_np(const vector<Proc>& input_procs) {
    if (input_procs.empty()) return {"SJF (NP)", 0.0, 0.0};
    vector<Proc> p = input_procs;
    int n = p.size();
    vector<bool> completed(n, false);
    int current_time = 0;
    int completed_count = 0;
    double total_tat = 0;
    double total_wt = 0;

    while (completed_count < n) {
        int shortest_job_index = -1;
        int min_bt = numeric_limits<int>::max();

        for (int i = 0; i < n; ++i) {
            if (!completed[i] && p[i].at <= current_time) {
                if (p[i].bt < min_bt) {
                    min_bt = p[i].bt;
                    shortest_job_index = i;
                }
                // Tie-breaker: FCFS (Compare by AT)
                else if (p[i].bt == min_bt && shortest_job_index != -1 && p[i].at < p[shortest_job_index].at) {
                    min_bt = p[i].bt;
                    shortest_job_index = i;
                }
            }
        }

        if (shortest_job_index == -1) {
            int next_arrival = numeric_limits<int>::max();
            for (int i = 0; i < n; ++i) {
                if (!completed[i] && p[i].at > current_time && p[i].at < next_arrival) {
                    next_arrival = p[i].at;
                }
            }
            if (next_arrival != numeric_limits<int>::max()) {
                current_time = next_arrival;
            } else {
                break; 
            }
        } else {
            int i = shortest_job_index;
            current_time += p[i].bt;
            p[i].ct = current_time;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            completed[i] = true;
            completed_count++;

            total_tat += p[i].tat;
            total_wt += p[i].wt;
        }
    }

    return {"SJF (NP)", total_tat / n, total_wt / n};
}

// --- FAST PRIORITY (Non-Preemptive, Non-Visual) ---
AlgoResult calculate_priority_np(const vector<Proc>& input_procs) {
    if (input_procs.empty()) return {"Priority (NP)", 0.0, 0.0};
    vector<Proc> p = input_procs;
    int n = p.size();
    vector<bool> completed(n, false);
    int current_time = 0;
    int completed_count = 0;
    double total_tat = 0;
    double total_wt = 0;

    while (completed_count < n) {
        int high_priority_index = -1;
        int min_prio = numeric_limits<int>::max();

        for (int i = 0; i < n; ++i) {
            if (!completed[i] && p[i].at <= current_time) {
                if (p[i].priority < min_prio) {
                    min_prio = p[i].priority;
                    high_priority_index = i;
                }
                else if (p[i].priority == min_prio && high_priority_index != -1 && p[i].at < p[high_priority_index].at) {
                    min_prio = p[i].priority;
                    high_priority_index = i;
                }
            }
        }

        if (high_priority_index == -1) {
            int next_arrival = numeric_limits<int>::max();
            for (int i = 0; i < n; ++i) {
                if (!completed[i] && p[i].at > current_time && p[i].at < next_arrival) {
                    next_arrival = p[i].at;
                }
            }
            if (next_arrival != numeric_limits<int>::max()) {
                current_time = next_arrival;
            } else {
                break;
            }
        } else {
            int i = high_priority_index;
            current_time += p[i].bt;
            p[i].ct = current_time;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            completed[i] = true;
            completed_count++;

            total_tat += p[i].tat;
            total_wt += p[i].wt;
        }
    }
    return {"Priority (NP)", total_tat / n, total_wt / n};
}


// --- FAST SRTF (Preemptive, Non-Visual) ---
AlgoResult calculate_srtf(const vector<Proc>& input_procs) {
    if (input_procs.empty()) return {"SRTF (P)", 0.0, 0.0};
    int n = input_procs.size();
    
    // Using a simple struct to track remaining time and results
    struct TempSRTFProc {
        int id; int at; int bt; int rt; int wt = 0; int tat = 0; int final_ct = 0;
    };
    vector<TempSRTFProc> srtf_procs;
    for (const auto& proc : input_procs) {
        // C++11 compatibility fix
        TempSRTFProc new_srtf_proc;
        new_srtf_proc.id = proc.no;
        new_srtf_proc.at = proc.at;
        new_srtf_proc.bt = proc.bt;
        new_srtf_proc.rt = proc.bt;
        srtf_procs.push_back(new_srtf_proc);
    }

    int current_time = 0;
    int completed_count = 0;
    double total_tat = 0;
    double total_wt = 0;

    while (completed_count < n) {
        int shortest_job_index = -1;
        int min_rt = numeric_limits<int>::max();

        for (int i = 0; i < n; ++i) {
            if (srtf_procs[i].at <= current_time && srtf_procs[i].rt > 0) {
                if (srtf_procs[i].rt < min_rt) {
                    min_rt = srtf_procs[i].rt;
                    shortest_job_index = i;
                }
                else if (srtf_procs[i].rt == min_rt && shortest_job_index != -1 && srtf_procs[i].at < srtf_procs[shortest_job_index].at) {
                    min_rt = srtf_procs[i].rt;
                    shortest_job_index = i;
                }
            }
        }

        if (shortest_job_index == -1) {
            int next_arrival = numeric_limits<int>::max();
            for (int i = 0; i < n; ++i) {
                if (srtf_procs[i].rt > 0 && srtf_procs[i].at > current_time && srtf_procs[i].at < next_arrival) {
                    next_arrival = srtf_procs[i].at;
                }
            }
            if (next_arrival != numeric_limits<int>::max()) {
                current_time = next_arrival;
            } else {
                break;
            }
        } else {
            int i = shortest_job_index;
            srtf_procs[i].rt--;
            current_time++;

            for (int j = 0; j < n; ++j) {
                if (srtf_procs[j].rt > 0 && srtf_procs[j].at <= current_time && j != i) {
                    srtf_procs[j].wt++;
                }
            }

            if (srtf_procs[i].rt == 0) {
                srtf_procs[i].final_ct = current_time;
                srtf_procs[i].tat = srtf_procs[i].final_ct - srtf_procs[i].at;
                total_tat += srtf_procs[i].tat;
                total_wt += srtf_procs[i].wt;
                completed_count++;
            }
        }
    }
    return {"SRTF (P)", total_tat / n, total_wt / n};
}

// --- FAST ROUND ROBIN (Preemptive, Non-Visual) ---
// Note: Quantum is passed as argument
AlgoResult calculate_rr(const vector<Proc>& input_procs, int quantum) {
    if (input_procs.empty() || quantum <= 0) return {"RR (Q=" + to_string(quantum) + ")", 0.0, 0.0};
    int n = input_procs.size();

    struct TempRRProc {
        int id; int at; int bt; int rt; int wt = 0; int tat = 0; int final_ct = 0;
    };
    vector<TempRRProc> rr_procs;
    for (const auto& proc : input_procs) {
        // C++11 compatibility fix
        TempRRProc new_rr_proc;
        new_rr_proc.id = proc.no;
        new_rr_proc.at = proc.at;
        new_rr_proc.bt = proc.bt;
        new_rr_proc.rt = proc.bt;
        rr_procs.push_back(new_rr_proc);
    }

    queue<int> ready_queue; 
    map<int, bool> in_queue; // Maps Process ID (rr_procs index nahi) to a boolean

    int current_time = 0;
    int completed_count = 0;
    double total_tat = 0;
    double total_wt = 0;
    
    while (completed_count < n) {
        
        // Add arrived processes to queue
        for (int i = 0; i < n; ++i) {
            if (rr_procs[i].at <= current_time && !in_queue[rr_procs[i].id] && rr_procs[i].rt > 0) {
                ready_queue.push(i); // Push index
                in_queue[rr_procs[i].id] = true;
            }
        }
        
        if (ready_queue.empty()) {
            // Handle Idle Time
            int next_arrival = numeric_limits<int>::max();
            bool processes_waiting = false;
            for(const auto& proc : rr_procs) {
                if (proc.rt > 0) {
                    processes_waiting = true;
                    if (proc.at > current_time && proc.at < next_arrival) {
                        next_arrival = proc.at;
                    }
                }
            }
            if (processes_waiting && next_arrival != numeric_limits<int>::max()) {
                current_time = next_arrival;
            } else {
                break;
            }
            continue;
        }

        // Dequeue and Execute
        int running_index = ready_queue.front();
        ready_queue.pop();
        int running_id = rr_procs[running_index].id;
        in_queue[running_id] = false;

        int run_time = min(rr_procs[running_index].rt, quantum);

        // Calculate waiting time and update remaining time during execution
        for (int t = 0; t < run_time; ++t) {
            
            // Waiting time increment for all other arrived jobs
            for (int j = 0; j < n; ++j) {
                if (j != running_index && rr_procs[j].rt > 0 && rr_procs[j].at <= current_time) {
                    rr_procs[j].wt++;
                }
            }
            
            rr_procs[running_index].rt--;
            current_time++;

            // Check for new arrivals during execution (Crucial for RR)
            for (int i = 0; i < n; ++i) {
                if (rr_procs[i].at == current_time && !in_queue[rr_procs[i].id] && rr_procs[i].rt > 0) {
                    ready_queue.push(i);
                    in_queue[rr_procs[i].id] = true;
                }
            }
            
            // If the running process finishes early, break the loop
            if (rr_procs[running_index].rt == 0) break;
        }


        if (rr_procs[running_index].rt == 0) {
            // Completed
            rr_procs[running_index].final_ct = current_time;
            rr_procs[running_index].tat = rr_procs[running_index].final_ct - rr_procs[running_index].at;
            total_tat += rr_procs[running_index].tat;
            total_wt += rr_procs[running_index].wt;
            completed_count++;
            
        } else {
            // Preempted (Quantum expired) - Add back to the end
            ready_queue.push(running_index);
            in_queue[running_id] = true;
        }
    }
    
    return {"RR (Q=" + to_string(quantum) + ")", total_tat / n, total_wt / n};
}


// --- Main Comparison Function ---
void run_all_algorithms_comparison() {
    int n;
    cout << "\n<-- Algorithm Comparison Tool Selected -->" << endl;
    cout << "Enter Number of Processes: ";
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid number of processes." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    // Get Time Quantum for RR
    int quantum;
    cout << "Enter Time Quantum (Q) for Round Robin: ";
    if (!(cin >> quantum) || quantum <= 0) {
        cout << "Invalid Time Quantum. Using Q=2 for RR." << endl;
        quantum = 2;
    }
    
    vector<Proc> input_processes;

    for (int i = 0; i < n; ++i) {
        // read_proc function Proc structure return karta hai jo common.cpp se aata hai
        input_processes.push_back(read_proc(i + 1));
    }

    cout << "\nCalculating results for all algorithms (No Simulation Display)..." << endl;
    // this_thread::sleep_for(chrono::seconds(1)); // Commented out for faster calculation

    vector<AlgoResult> results;
    
    // 1. FCFS
    results.push_back(calculate_fcfs(input_processes));
    
    // 2. SJF Non-Preemptive
    results.push_back(calculate_sjf_np(input_processes));

    // 3. Priority Non-Preemptive
    results.push_back(calculate_priority_np(input_processes));
    
    // 4. SRTF Preemptive
    results.push_back(calculate_srtf(input_processes));

    // 5. Round Robin
    results.push_back(calculate_rr(input_processes, quantum));
    
    clearScreen();
    cout << "\n========================================================" << endl;
    cout << "|| FINAL CPU SCHEDULING ALGORITHM COMPARISON RESULTS ||" << endl;
    cout << "========================================================" << endl;
    
    cout << fixed << setprecision(2);
    
    cout << left << setw(20) << "\n| Algorithm"
         << right << setw(15) << "| Avg TAT (s)"
         << right << setw(15) << "| Avg WT (s) |" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (const auto& res : results) {
        // Find minimum waiting time (excluding zero)
        double min_wt = numeric_limits<double>::max();
        for(const auto& r : results) {
            if (r.avg_wt < min_wt) {
                min_wt = r.avg_wt;
            }
        }

        bool is_best = (res.avg_wt == min_wt); // Highlight all with min_wt
        
        string format_start = is_best ? "\033[1;32m" : ""; 
        string format_end = is_best ? "\033[0m" : "";

        cout << format_start
             << "| " << left << setw(18) << res.algo_name
             << right << setw(15) << res.avg_tat
             << right << setw(15) << res.avg_wt << " |"
             << format_end << endl;
    }
    
    cout << "--------------------------------------------------------" << endl;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\nPress Enter to return to main menu...";
    cin.get();
}
