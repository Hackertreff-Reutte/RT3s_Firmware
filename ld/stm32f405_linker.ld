

EXTERN(vector_table)
ENTRY(reset_handler)
MEMORY
{
 sram (rwx) : ORIGIN = 0x20000200, LENGTH = 128k - 0x200  /* 0x200 some space for the bootloader? */
 flash (rx) : ORIGIN = 0x0800C000, LENGTH = 1M - 48K
 ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}
SECTIONS
{

 .text : {
  *(.vectors)
  *(.text*)
  . = ALIGN(4);
  *(.rodata*)
  . = ALIGN(4);
 } > flash

 .preinit_array : {
  . = ALIGN(4);
  __preinit_array_start = .;
  KEEP (*(.preinit_array))
  __preinit_array_end = .;
 } >flash

 .init_array : {
  . = ALIGN(4);
  __init_array_start = .;
  KEEP (*(SORT(.init_array.*)))
  KEEP (*(.init_array))
  __init_array_end = .;
 } >flash

 .fini_array : {
  . = ALIGN(4);
  __fini_array_start = .;
  KEEP (*(.fini_array))
  KEEP (*(SORT(.fini_array.*)))
  __fini_array_end = .;
 } >flash

 .ARM.extab : {
  *(.ARM.extab*)
 } >flash

 .ARM.exidx : {
  __exidx_start = .;
  *(.ARM.exidx*)
  __exidx_end = .;
 } >flash

 . = ALIGN(4);
 _etext = .;
 .data : {
  _data = .;
  *(.data*)
  . = ALIGN(4);
  _edata = .;
 } >sram AT >flash

 _data_loadaddr = LOADADDR(.data);
 .bss : {
  *(.bss*)
  *(COMMON)
  . = ALIGN(4);
  _ebss = .;
 } >sram

 .ccm : {
  *(.ccmram*)
  . = ALIGN(4);
 } >ccm

 /DISCARD/ : { *(.eh_frame) }
 . = ALIGN(4);
 end = .;
}

PROVIDE(_stack = ORIGIN(sram) + LENGTH(sram) - 4);
