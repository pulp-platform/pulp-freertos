// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/***************************************************/
/*
* Library to perform the measurement for the project,
* Using FreeRTOS, GAP Implementation and compile
*
* Date of creation: 31/05/2019
*
* Date of Last Modification: 31/05/2019
*
*/

#include "measure.h"

#include "EPI_Config.h"
//#include "gap_common.h"
#include "pmsis.h"
//#include "/home/eventine/gap_sdk/freeRTOS/lib/third_party/mcu_vendor/gwt/cmsis/TARGET_RISCV_32/core_gap.h"
/* PCER Register Definitions */
#define PCER_CYCLE_Pos                 0U                                            /*!< PCER: CYCLE Position */
#define PCER_CYCLE_Msk                 (0x1UL /*<< PCER_CYCLE_Pos*/)                 /*!< PCER: CYCLE Mask */

#define PCER_INSTR_Pos                 1U                                            /*!< PCER: _INSTR Position */
#define PCER_INSTR_Msk                 (0x1UL << PCER_INSTR_Pos)                     /*!< PCER: _INSTR Mask */

#define PCER_LD_STALL_Pos              2U                                            /*!< PCER: LD_STALL Position */
#define PCER_LD_STALL_Msk              (0x1UL << PCER_LD_STALL_Pos)                  /*!< PCER: LD_STALL Mask */

#define PCER_JMP_STALL_Pos             3U                                            /*!< PCER: JMP_STALL Position */
#define PCER_JMP_STALL_Msk             (0x1UL << PCER_JMP_STALL_Pos)                 /*!< PCER: JMP_STALL Mask */

#define PCER_IMISS_Pos                 4U                                            /*!< PCER: IMISS Position */
#define PCER_IMISS_Msk                 (0x1UL << PCER_IMISS_Pos)                     /*!< PCER: IMISS Mask */

#define PCER_WBRANCH_Pos               5U                                            /*!< PCER: WBRANCH Position */
#define PCER_WBRANCH_Msk               (0x1UL << PCER_WBRANCH_Pos)                   /*!< PCER: WBRANCH Mask */

#define PCER_WBRANCH_CYC_Pos           6U                                            /*!< PCER: WBRANCH_CYC Position */
#define PCER_WBRANCH_CYC_Msk           (0x1UL << PCER_WBRANCH_CYC_Pos)               /*!< PCER: WBRANCH_CYC Mask */

#define PCER_LD_Pos                    7U                                            /*!< PCER: LD Position */
#define PCER_LD_Msk                    (0x1UL << PCER_LD_Pos)                        /*!< PCER: LD Mask */

#define PCER_ST_Pos                    8U                                            /*!< PCER: ST Position */
#define PCER_ST_Msk                    (0x1UL << PCER_ST_Pos)                        /*!< PCER: ST Mask */

#define PCER_JUMP_Pos                  9U                                            /*!< PCER: JUMP Position */
#define PCER_JUMP_Msk                  (0x1UL << PCER_JUMP_Pos)                      /*!< PCER: JUMP Mask */

#define PCER_BRANCH_Pos                10U                                           /*!< PCER: BRANCH Position */
#define PCER_BRANCH_Msk                (0x1UL << PCER_BRANCH_Pos)                    /*!< PCER: BRANCH Mask */

#define PCER_DELAY_SLOT_Pos            11U                                           /*!< PCER: DELAY_SLOT Position */
#define PCER_DELAY_SLOT_Msk            (0x1UL << PCER_DELAY_SLOT_Pos)                /*!< PCER: DELAY_SLOT Mask */

#define PCER_LD_EXT_Pos                12U                                           /*!< PCER: LD_EXT Position */
#define PCER_LD_EXT_Msk                (0x1UL << PCER_LD_EXT_Pos)                    /*!< PCER: LD_EXT Mask */

#define PCER_ST_EXT_Pos                13U                                           /*!< PCER: ST_EXT Position */
#define PCER_ST_EXT_Msk                (0x1UL << PCER_ST_EXT_Pos)                    /*!< PCER: ST_EXT Mask */

#define PCER_LD_EXT_CYC_Pos            14U                                           /*!< PCER: LD_EXT_CYC Position */
#define PCER_LD_EXT_CYC_Msk            (0x1UL << PCER_LD_EXT_CYC_Pos)                /*!< PCER: LD_EXT_CYC Mask */

#define PCER_ST_EXT_CYC_Pos            15U                                           /*!< PCER: ST_EXT_CYC Position */
#define PCER_ST_EXT_CYC_Msk            (0x1UL << PCER_ST_EXT_CYC_Pos)                /*!< PCER: ST_EXT_CYC Mask */

#define PCER_TCDM_CONT_Pos             16U                                           /*!< PCER: TCDM_CONT Position */
#define PCER_TCDM_CONT_Msk             (0x1UL << PCER_TCDM_CONT_Pos)                 /*!< PCER: TCDM_CONT Mask */

#define PCER_EVENTS_NUM                17U                                           /*!< PCER: All events number */

#define PCMR_GLBEN_Pos                 0U
#define PCMR_SATU_Pos                  1U


extern int lPerformanceCheck = 0;
extern int CSCheck = 0;

int Timerindex = 0;
#ifdef __PULP__
Timer_Data_t timerBuffer[15*MEASURE_N_ITERATION+7+2000] = { {'0', 0} };
#else
Timer_Data_t timerBuffer[15*MEASURE_N_ITERATION+7] = { {'0', 0} };
#endif


void measureInitializeNStart ( void )
{
	/* Reset all to 0 */
	__ASM volatile ("csrw 0x79F, %0" :: "r" (0));

	/* Configure */
	/* Cycles */
	uint32_t maskConfig = (PCER_CYCLE_Msk & ~(0x1UL << 17U));
	/* Instructions */
	//uint32_t maskConfig = (PCER_INSTR_Msk & ~(0x1UL << 17U));
#ifdef __PULP__
	__ASM volatile ("csrw 0x7E0, %0" : "+r" (maskConfig) );
#else
	__ASM volatile ("csrw 0x7A0, %0" : "+r" (maskConfig) );
#endif
	/* Enable */
	uint32_t maskEnable = (1 << PCMR_GLBEN_Pos) | (1 << PCMR_SATU_Pos);
#ifdef __PULP__
	__ASM volatile ("csrw 0x7E1, %0" :: "r" (maskEnable));
#else
	__ASM volatile ("csrw 0x7A1, %0" :: "r" (maskEnable));
#endif
}


inline uint32_t measureReadCycle ( void ) {

	/* Read */
	uint32_t value = 0;
	/* Cycles */
	__ASM volatile ("csrr %0, 0x780" : "=r" (value));
	/* Instructions */
	//asm volatile ("csrr %0, 0x781" : "=r" (value));
	/* LD stall */
	//asm volatile ("csrr %0, 0x782" : "=r" (value));
	/* Istr Stall */
	//asm volatile ("csrr %0, 0x784" : "=r" (value));
	/* LD */
	//asm volatile ("csrr %0, 0x785" : "=r" (value));
	/* Store */
	//asm volatile ("csrr %0, 0x786" : "=r" (value));

	return value;
}
