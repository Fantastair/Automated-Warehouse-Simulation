#ifndef __SIMULATE_H__
#define __SIMULATE_H__

#include "../../SDL3.h"

extern bool thread_running;
extern bool Simulating;
extern int simulation_speed;
extern Uint64 system_runtime;
extern Uint64 simulation_time;

void set_simulation_speed(int speed);
int get_simulation_speed(void);

void simulate(void);
void update_simulation(Uint64 t);

void start_simulation(void);

#endif
