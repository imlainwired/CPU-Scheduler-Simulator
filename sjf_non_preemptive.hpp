#ifndef SJF_NON_PREEMPTIVE_HPP
#define SJF_NON_PREEMPTIVE_HPP
#include "scheduler_common.hpp"
#include <vector>
#include <iostream>

void printSimulation_sjf(const std::vector<Proc>& processes, int n, int current_time);
void run_sjf_non_preemptive_simulation();

#endif
