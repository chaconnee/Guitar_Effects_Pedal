#ifndef CAB_SIM_H
#define CAB_SIM_H

#include "model/effect.h"
#include <stdint.h>

#define IR_COUNT 3

extern Effect cab_sim_effect;
extern uint8_t cab_current_ir;

void CabSim_SelectIR(uint8_t idx);

#endif /* CAB_SIM_H */
