#ifndef CAB_SIM_H
#define CAB_SIM_H

#include "model/effect.h"
#include <stdint.h>

#define IR_COUNT 3

extern Effect cab_sim_effect;
extern uint8_t cab_current_ir;

extern volatile uint32_t g_cab_us_max;
extern volatile uint32_t g_cab_us_min;
extern volatile uint64_t g_cab_us_sum;
extern volatile uint32_t g_cab_cnt;
extern volatile uint32_t g_cab_us_last;

void CabSim_SelectIR(uint8_t idx);

#endif /* CAB_SIM_H */
