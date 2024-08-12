// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _SHTDWN_H
#define _SHTDWN_H 1


extern uint8_t shtdwn_status;
#define SHUTDOWN_STATUS() shtdwn_status

//!control PSU shutdown:  sht=0: unshutdown (restart), =1 shutdown 
uint8_t psu_shutdown(uint8_t sht);

//!control PSU shutdown:  data[0] specifies: 0 - unshutdown (restart), 1 shutdown, 2 - return shutdowns status
void cmd_shtdwn(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data);

//!
void shutdown_init();


/** 
 * Indicate whether PSU is shutdown or not
 * @return non-zero if shutdown, zero otherwise (ie running)
 */
uint8_t is_shutdown();

#endif /* _SHTDWN_H */

