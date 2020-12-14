/*
 * Copyright 2020 GreenWaves Technologies
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
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__
#define __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * pi_task:
 * data[0] = l2_buf
 * data[1] = size
 * data[2] = repeat_size ?
 */

struct cpi_itf_data_s
{
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
    uint32_t device_id;
    uint32_t nb_open;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Init conf. */
void __pi_cpi_conf_init(struct pi_cpi_conf *conf);

/* Open device. */
int32_t __pi_cpi_open(struct pi_cpi_conf *conf, struct cpi_itf_data_s **device_data);

/* Close device. */
void __pi_cpi_close(struct cpi_itf_data_s *device_data);

/* Copy in UDMA. */
void __pi_cpi_copy(struct cpi_itf_data_s *device_data, void *l2_buf,
                   uint32_t size, struct pi_task *task);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_cpi_control_start(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_glob_en_set(device_data->device_id, 1);
}

static inline void pi_cpi_control_stop(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_glob_en_set(device_data->device_id, 0);
}

static inline void pi_cpi_set_format(struct pi_device *device,
                                     pi_cpi_format_e format)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_format_set(device_data->device_id, format);
}

static inline void pi_cpi_set_frame_drop(struct pi_device *device,
                                         uint32_t nb_frame_dropped)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_framedrop_en_set(device_data->device_id, nb_frame_dropped);
    cpi_framedrop_val_set(device_data->device_id, nb_frame_dropped);
}

static inline void pi_cpi_set_filter(struct pi_device *device, uint32_t r_coeff,
                                     uint32_t g_coeff, uint32_t b_coeff, uint32_t shift)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_shift_set(device_data->device_id, shift);
    hal_cpi_filter_rgb_coeff_set(device_data->device_id, r_coeff, g_coeff, b_coeff);
}

static inline void pi_cpi_set_slice(struct pi_device *device,
                                    uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_frameslice_en_set(device_data->device_id, w);
    if (w)
    {
        /* Lower left. */
        hal_cpi_frameslice_ll_set(device_data->device_id, x, y);
        /* Upper right. */
        hal_cpi_frameslice_ur_set(device_data->device_id, (x + w - 1),  (y + h - 1));
        /* Size. */
        cpi_rowlen_set(device_data->device_id, (w - 1));
    }
}

#endif  /* __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__ */
