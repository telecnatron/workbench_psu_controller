// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _LOAD_SWITCH_H
#define _LOAD_SWITCH_H 1

//!
void task_load_switch();

//!
void load_switch_init(uint8_t adc_channel);

//!
void cmd_load_switch(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data);

#endif /* _LOAD_SWITCH_H */

