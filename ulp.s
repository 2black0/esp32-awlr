#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_ulp.h"

  .bss
  .global next_edge_wind
next_edge_wind:
  .long 0

  .global next_edge_rain
next_edge_rain:
  .long 0
  .global debounce_counter_wind
debounce_counter_wind:
  .long 0
  
  .global debounce_counter_rain
debounce_counter_rain:
  .long 0

  .global debounce_max_count
debounce_max_count:
  .long 0

  .global edge_count_wind
edge_count_wind:
  .long 0

  .global edge_count_rain
edge_count_rain:
  .long 0
  
  .global edge_count_to_wake_up
edge_count_to_wake_up:
  .long 0

  .global io_number_wind
io_number_wind:
  .long 0

  .global io_number_rain
io_number_rain:
  .long 0
  .text
  .global entry
entry:

read_io_wind:
  move r3, io_number_wind
  ld r3, r3, 0

  move r0, r3
  jumpr read_io_high_wind, 16, ge

  READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S, 16)
  rsh r0, r0, r3
  jump read_done_wind

read_io_high_wind:
  READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 16, 2)
  sub r3, r3, 16
  rsh r0, r0, r3

read_done_wind:
  and r0, r0, 1
  move r3, next_edge_wind
  ld r3, r3, 0
  add r3, r0, r3
  and r3, r3, 1
  jump changed_wind, eq
  move r3, debounce_max_count
  move r2, debounce_counter_wind
  ld r3, r3, 0
  st r3, r2, 0
  jump read_io_rain

  .global changed_wind
changed_wind:
  move r3, debounce_counter_wind
  ld r2, r3, 0
  add r2, r2, 0
  jump edge_detected_wind, eq
  sub r2, r2, 1
  st r2, r3, 0
  jump read_io_rain

  .global edge_detected_wind
edge_detected_wind:
  move r3, debounce_max_count
  move r2, debounce_counter_wind
  ld r3, r3, 0
  st r3, r2, 0
  move r3, next_edge_wind
  ld r2, r3, 0
  add r2, r2, 1
  and r2, r2, 1
  st r2, r3, 0
  move r3, edge_count_wind
  ld r2, r3, 0
  add r2, r2, 1
  st r2, r3, 0
  move r3, edge_count_to_wake_up
  ld r3, r3, 0
  sub r3, r3, r2
  jump read_io_rain

read_io_rain:
  move r3, io_number_rain
  ld r3, r3, 0
  
  move r0, r3
  jumpr read_io_high_rain, 16, ge

  READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S, 16)
  rsh r0, r0, r3
  jump read_done_rain

read_io_high_rain:
  READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 16, 2)
  sub r3, r3, 16
  rsh r0, r0, r3

read_done_rain:
  and r0, r0, 1
  move r3, next_edge_rain
  ld r3, r3, 0
  add r3, r0, r3
  and r3, r3, 1
  jump changed_rain, eq
  move r3, debounce_max_count
  move r2, debounce_counter_rain
  ld r3, r3, 0
  st r3, r2, 0
  halt

  .global changed_rain
changed_rain:
  move r3, debounce_counter_rain
  ld r2, r3, 0
  add r2, r2, 0
  jump edge_detected_rain, eq
  sub r2, r2, 1
  st r2, r3, 0
  halt

  .global edge_detected_rain
edge_detected_rain:
  move r3, debounce_max_count
  move r2, debounce_counter_rain
  ld r3, r3, 0
  st r3, r2, 0
  move r3, next_edge_rain
  ld r2, r3, 0
  add r2, r2, 1
  and r2, r2, 1
  st r2, r3, 0
  move r3, edge_count_rain
  ld r2, r3, 0
  add r2, r2, 1
  st r2, r3, 0
  move r3, edge_count_to_wake_up
  ld r3, r3, 0
  sub r3, r3, r2
  halt
  
  .global wake_up
wake_up:
  READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
  and r0, r0, 1
  jump wake_up, eq
  wake
  halt
