#ifndef __SIMULATE_H__
#define __SIMULATE_H__

#include "../../SDL3.h"

extern bool thread_running;
extern bool Simulating;

void simulate(void);
void update_simulation(Uint64 t);

void start_simulation(void);

#endif
