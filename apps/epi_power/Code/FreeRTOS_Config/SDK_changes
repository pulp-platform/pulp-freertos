SDK version: tag SDK_3.2.1
I modified the SDK to obtain a more consistent beahviour and to fix some bugs:

	- in ./pmsis/pmsis_backend/pmsis_backend_native_task_api.c: changed the priority of the task to configMAX_PRIORITIES - 1
	- in ./pmsis/pmsis_backend/implementation_specific_defines.h: changed the PMSIS_TASK_EVENT_KERNEL_PRIORITY to 16
	- in ./pmsis/implem/pmsis_driver/spi.c: changed uint32_t periph_freq inside the function static uint32_t __pi_spi_clk_div_get(uint32_t spi_freq) to 500000