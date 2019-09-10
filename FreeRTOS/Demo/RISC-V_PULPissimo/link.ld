OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  L2           : ORIGIN = 0x1c000004, LENGTH = 0x0007fffc
  L2_aliased   : ORIGIN = 0x00000004, LENGTH = 0x00003ffc
}

/*
 * This linker script try to put FC data in L2 private bank0 and FC code
 * in L2 private bank1 to avoid contention between FC code and data
 * as FC has no instruction cache and is so often accessing L2 to
 * get instructions. Everything can be shifted in case one bank is full.
 *
 * Cluster code and initialized data are put in shared banks to not polute
 * private banks which are quite small, and also avoid contentions between
 * cluster cache refill and FC.
 */


SECTIONS
{

  /* stack and heap related settings */
  __stack_size = DEFINED(__stack_size) ? __stack_size : 0x800;
  PROVIDE(__stack_size = __stack_size);
  __heap_size = DEFINED(__heap_size) ? __heap_size : 0x400;

  /*
   * L2 PRIVATE BANK0
   *
   * Contains FC data
   */

  /* This section is for tiny FC data which benefits
   * from the alias at 0
   */
  .data_tiny_fc : ALIGN(4)
  {
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > L2_aliased AT> L2


  .init : ALIGN(4)
  {
    KEEP( *(.init) )
  } > L2


  .fini : ALIGN(4)
  {
    KEEP( *(.fini) )
  } > L2


  .preinit_array : ALIGN(4)
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > L2


  .init_array : ALIGN(4)
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    __CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    KEEP(*(.ctors.start))
    KEEP(*(.ctors))
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    LONG(0)
    __CTOR_END__ = .;
    PROVIDE_HIDDEN (__init_array_end = .);
  } > L2


  .fini_array : ALIGN(4)
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    __DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    KEEP(*(.dtors.start))
    KEEP(*(.dtors))
    LONG(0)
    __DTOR_END__ = .;
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > L2


  .boot : ALIGN(4)
  {
    *(.boot)
    *(.boot.data)
  } > L2


  .rodata : ALIGN(4)
  {
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
  } > L2


  .got : ALIGN(4)
  {
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
  } > L2


  .shbss : ALIGN(4)
  {
    *(.shbss)
  } > L2


  .talias :
  {
  } > L2


  .gnu.offload_funcs : ALIGN(4)
  {
    KEEP(*(.gnu.offload_funcs))
  } > L2


  .gnu.offload_vars : ALIGN(4)
  {
    KEEP(*(.gnu.offload_vars))
  } > L2


  .data : ALIGN(4)
  {
    sdata  =  .;
    _sdata  =  .;
    __data_begin = .;
    *(.data_fc)
    *(.data_fc.*)
    *(.data);
    *(.data.*)
    __sdata_begin = .;
    *(.sdata);
    *(.sdata.*)
    *(.heapl2ram)
    *(.fcTcdm)
    *(.fcTcdm.*)
    *(.fcTcdm_g)
    *(.fcTcdm_g.*)
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .; /* man 3 end: first addr after data */
  } > L2


  .bss : ALIGN(8)
  {
    _bss_start = .;
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
    _bss_end = .;
    _end = .; /* man 3 end: first addr after bss */
  } > L2


  /* The compiler uses this to access data in the .sdata, .data, .sbss and .bss
     sections with fewer instructions (relaxation). This reduces code size. */
    PROVIDE(__global_pointer$ = MIN(__sdata_begin + 0x800,
			    MAX(__data_begin + 0x800, __bss_end - 0x800)));

  .heap : ALIGN(16)
  {
    __heap_start = .;
    . += __heap_size;
    __heap_end = .;
    . = ALIGN(16);
    _heap_end = __heap_end;
  } > L2


  .stack : ALIGN(16)
  {
    stack_start = .;
    __stack_bottom = .;
    . += __stack_size;
    __stack_top = .;
    __freertos_irq_stack_top = .; /* sytem stack */
    stack = .;
  } > L2


  __l2_priv0_end = ALIGN(4);




  /*
   * L2 PRIVATE BANK1
   *
   * Contains FC code
   */

  .vectors MAX(0x1c008000, ALIGN(256)) :
  {
    __irq_vector_base = .;
    __vector_start = .;
    KEEP(*(.vectors))
  } > L2

  .text : ALIGN(4)
  {
    _stext = .;
    *(.text)
    *(.text.*)
    _etext = .; /* man 3 end: first addr after text */
    *(.lit)
    *(.shdata)
    _endtext = .;
    . = ALIGN(4);
  } > L2

  __l2_priv1_end = ALIGN(4);


  /*
   * L2 SHARED BANKS
   *
   * Contains other data such as peripheral data and cluster code and data
   */

  .l2_data MAX(0x1c010000, ALIGN(4)) :
  {
    __cluster_text_start = .;
    *(.cluster.text)
    *(.cluster.text.*)
    . = ALIGN(4);
    __cluster_text_end = .;
    *(.l2_data)
    *(.l2_data.*)
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    . = ALIGN(4);
  } > L2

  __l2_shared_end = LOADADDR(.l2_data) + SIZEOF(.l2_data);

}
