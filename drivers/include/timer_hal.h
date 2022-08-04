/*
 * Copyright (C) 2022 ETH Zurich, University of Bologna and
 * GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#include "memory_map.h"
#include "timer.h"

#define TIMER_LO_ID 0
#define TIMER_HI_ID 1
#define ADV_TIMER0_ID 2
#define ADV_TIMER1_ID 3
#define ADV_TIMER2_ID 4
#define ADV_TIMER3_ID 5

typedef enum 
{ 
    TIMER0 = TIMER_LO_ID, 
    TIMER1 = TIMER_HI_ID,  
} timer_id_t;

static inline unsigned int timer_conf_get(unsigned int addr, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      return timer_cfg_lo_get(addr);
  else if (timer_id == TIMER1) // TIMER_HI
      return timer_cfg_hi_get(addr);
}

static inline void timer_conf_set(unsigned int addr, unsigned int value, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      timer_cfg_lo_set(addr, value);
  else if (timer_id == TIMER1) // TIMER_HI
      timer_cfg_hi_set(addr, value);
}

static inline unsigned int timer_count_get(unsigned int addr, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      return timer_cnt_lo_get(addr);
  else if (timer_id == TIMER1) // TIMER_HI
      return timer_cnt_hi_get(addr);
}

static inline void timer_count_set(unsigned int addr, unsigned int value, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      timer_cnt_lo_set(addr, value);
  else if (timer_id == TIMER1) // TIMER_HI
      timer_cnt_hi_set(addr, value);
}

static inline unsigned int timer_cmp_get(unsigned int addr, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      return timer_cmp_lo_get(addr);
  else if (timer_id == TIMER1) // TIMER_HI
      return timer_cmp_hi_get(addr);
}

static inline void timer_cmp_set(unsigned int addr, unsigned int cmp, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      timer_cmp_lo_set(addr, cmp);
  else if (timer_id == TIMER1) // TIMER_HI
      timer_cmp_hi_set(addr, cmp);
}


static inline void timer_reset(unsigned int addr, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      timer_reset_lo_set(addr, 1);
  else if (timer_id == TIMER1) // TIMER_HI
      timer_reset_hi_set(addr, 1);
}

static inline void timer_start(unsigned int addr, timer_id_t timer_id)
{
  if (timer_id == TIMER0) // TIMER_LO
      timer_start_lo_set(addr, 1);
  else if (timer_id == TIMER1) // TIMER_HI
      timer_start_hi_set(addr, 1);
}


// FC base timers (TIMER0_LO and TIMER0_HI)
static inline unsigned int timer_base_fc(int id, int sub_id)
{
    return FC_TIMER_ADDR + id * ARCHI_TIMER_SIZE + sub_id * 4;
}

static inline void start_timer_fc(timer_id_t timer_id)
{
    timer_start(timer_base_fc(0, 0), timer_id);
}

static inline void stop_timer_fc(timer_id_t timer_id)
{
    timer_conf_set(timer_base_fc(0, 0), 0, timer_id);
}

static inline void reset_timer_fc(timer_id_t timer_id)
{
    timer_reset(timer_base_fc(0, 0), timer_id);
}

static inline int get_timer_cnt_fc(timer_id_t timer_id)
{
    return timer_count_get(timer_base_fc(0, 0), timer_id);
}

static inline int set_timer_irq_freq(unsigned int cmp, timer_id_t timer_id)
{
    timer_cmp_set(timer_base_fc(0, 0), cmp, timer_id);
}

static inline int get_timer_irq_freq(timer_id_t timer_id)
{
    return timer_cpm_get(timer_base_fc(0, 0), timer_id);
}

static inline void set_timer_cfg(unsigned int cfg, timer_id_t timer_id)
{
    timer_conf_set(timer_base_fc(0, 0), cfg, timer_id);
}

static inline void get_timer_cfg(timer_id_t timer_id)
{
    return timer_conf_get(timer_base_fc(0, 0), timer_id);
}


// Cluster base timers (TIMER0_LO and TIMER0_HI)
static inline unsigned int timer_base_cl(int cid, int id, int sub_id)
{
  return ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(0) + ARCHI_TIMER_OFFSET + id * ARCHI_TIMER_SIZE + sub_id * 4;
}

static inline void start_timer_cl(timer_id_t timer_id)
{
    timer_start(timer_base_cl(0, 0, 0), timer_id);
}

static inline void stop_timer_cl(timer_id_t timer_id)
{
    timer_conf_set(timer_base_cl(0, 0, 0), 0, timer_id);
}

static inline void reset_timer_cl(timer_id_t timer_id)
{
    timer_reset(timer_base_cl(0, 0, 0), timer_id);
}

static inline int get_timer_cnt_cl(timer_id_t timer_id)
{
    return timer_count_get(timer_base_cl(0, 0, 0), timer_id);
}

static inline int set_timer_irq_freq_cl(unsigned int cmp, timer_id_t timer_id)
{
    timer_cmp_set(timer_base_cl(0, 0, 0), cmp, timer_id);
}

static inline int get_timer_irq_freq_cl(timer_id_t timer_id)
{
    return timer_cpm_get(timer_base_cl(0, 0, 0), timer_id);
}

static inline void set_timer_cfg_cl(unsigned int cfg, timer_id_t timer_id)
{
    timer_conf_set(timer_base_cl(0, 0, 0), cfg, timer_id);
}

static inline void get_timer_cfg_cl(timer_id_t timer_id)
{
    return timer_conf_get(timer_base_cl(0, 0, 0), timer_id);
}

#endif /* __HAL_TIMER_H__ */
