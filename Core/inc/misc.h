#ifndef __MISC_H__
#define __MISC_H__

#include "../../SDL3.h"
#include <string>

Uint64 GetNS(void);

double arc2angle(double arc, double radius);
double angle2degree(double angle);

double min_break_distance(double current_speed, double target_speed);

void RandomInit(void);
int RandomInt(double a, double b);

void GenerateRandomTask(CarTask &task);

std::string NS2String(Uint64 ns);

#endif
