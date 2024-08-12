// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
// various commonly used command handlers
#include "config.h"
#include <string.h>
#include "../../log.h"
#include "../mmp_cmd.h"

extern char version[];

// They send us a single byte (or if more, we just use the first one), we return them their byte as reply_data[0] and their byte +1 as reply_data[1]
void cmd_ping(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    //LOG_INFO_FP("%s:%s: cmd_echo: cmd: 0x%x, rsubcmd: 0x%x, dmax: %i, dlen: %i: d123: %x, %x, %x, p: %p, rp: %p, dlen: %u", __FILE__,__LINE__, cmd, 0, data_max_len, data_len, data[0], data[1], data[2], data, reply_data, data_len);
    uint8_t num=data[0];
    reply_data[0]=num;
    reply_data[1]=num+1;
    mmp_cmd_reply(handle, 0, 2);
}


// Return them our version string
void cmd_version(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    uint8_t vlen = strlen(version);
    if(vlen > data_max_len){
	vlen = data_max_len-1;
    }
    strncpy((char *)reply_data, version, vlen);
    // ensure that it's null terminated
    reply_data[data_len-1]='\x0';
    mmp_cmd_reply(handle, 0, vlen);
}


