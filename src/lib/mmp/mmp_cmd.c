// -----------------------------------------------------------------------------
// Copyright 2018 Stephen Stebbing. telecnatron.com
//
//    Licensed under the Telecnatron License, Version 1.0 (the “License”);
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        https://telecnatron.com/software/licenses/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an “AS IS” BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// -----------------------------------------------------------------------------   
#include "mmp_cmd.h"
#include "../log.h"

// define MSG_USE_LOGGER to have error messages logged
#ifdef MMP_CMD_LOGGING
#include "../log.h"
#define MMP_CMD_LOG(fmt, msg...) LOG_INFO_FP(fmt, msg)
#define MMP_CMD_LOG_WARN(fmt, msg...) LOG_WARN_FP(fmt, msg)
#define MMP_CMD_LOG_DEBUG(fmt, msg...) LOG_DEBUG_FP(fmt, msg)
#else
#define MMP_CMD_LOG(fmt, msg...)
#define MMP_CMD_LOG_WARN(fmt, msg...)
#define MMP_CMD_LOG_DEBUG(fmt, msg...)
#endif


void mmp_cmd_reply(void *handle, uint8_t status, uint8_t data_len)
{
    mmp_cmd_ctrl_t *ctrl = (mmp_cmd_ctrl_t *)handle;
    mmp_msg_t *msg = &(ctrl->mmp_ctrl.ctrl.msg);
    // set status
    msg->data[1]=status;
    // send reply message
    mmp_send(msg->data, data_len+2, msg->flags, ctrl->tx_byte_fn);
}


void mmp_cmd_msg_handler(void *user_data, mmp_msg_t *msg)
{
    // cast pointer to mmp_cmd_ctrl_t
    mmp_cmd_ctrl_t *ctrl = (mmp_cmd_ctrl_t *)user_data;

    if (MMP_FLAGS_IS_CMD(msg->flags)){
	//mmp_print_mmp_msg_t(msg, __FILE__, __LINE__, "it's a cmd message");
	// yup, it's a command-message that's been received.
	if(msg->len >=1 ){
	    // First byte of data is cmd, 
	    uint8_t cmd = msg->data[0];
	    if( cmd < ctrl->num_handlers ){
		// valid command, call handler

		// for the data that is sent in reply:
		//   byte 0 is left as the command
		//   byte 1 is for the status byte
		//   byte 3 onwards are for the user reply data
		// note that by writing to reply_data, data is overwritten
		uint8_t reply_data_max_len = ctrl->mmp_ctrl.ctrl.data_max_len-2;
		void *data=(msg->data)+1;
		void *reply_data=(msg->data)+2;
		// call the handler
		//LOG_INFO_FP("%s:%i: data: %p, reply_data: %p",__FILE__,__LINE__, data, reply_data);
		ctrl->cmd_handler_tab[cmd](ctrl, cmd, msg->len-1, reply_data_max_len, data,  reply_data);
	    }else{
		// invalid command, ie cmd number exceeds number of entries in cmd table
		MMP_CMD_LOG_WARN("invalid command: %u", cmd);
	    }
//	    mmp_send(msg->data, msg->len, msg->flags, ((mmp_cmd_ctrl_t *)user_data)->tx_byte_fn);
	}else{
	    // data length is invalid
	}
    }else{
	// received message wasn't a command-message
    }

}

void mmp_cmd_init(mmp_cmd_ctrl_t *ctrl, uint8_t* msg_buf, uint8_t msg_buf_size, 
		  mmp_cmd_handler_t *cmd_handler_tab, uint8_t num_handlers,
		  void (*tx_byte_fn)(const char c))
{
    ctrl->tx_byte_fn = tx_byte_fn;
    ctrl->cmd_handler_tab = cmd_handler_tab;
    ctrl->num_handlers = num_handlers;
    mmp_init(&(ctrl->mmp_ctrl), msg_buf, msg_buf_size, mmp_cmd_msg_handler, ctrl);
}

void mmp_async_send(uint8_t *msg_data, uint8_t len, void (*tx_byte_fn)(const char c))
{
    uint8_t flags=0;
    mmp_send(msg_data, len,  MMP_FLAGS_SET_ASYNC(flags), tx_byte_fn);
}


inline void mmp_cmd_rx_ch(mmp_cmd_ctrl_t *mmp_cmd_ctrl, uint8_t ch)
{
    // just call corresponding mmp function.
    mmp_rx_ch(&(mmp_cmd_ctrl->mmp_ctrl), ch);
}


inline void mmp_cmd_tick(mmp_cmd_ctrl_t *mmp_cmd_ctrl)
{
    // just call corresponding mmp function.
    mmp_tick(&(mmp_cmd_ctrl->mmp_ctrl));
}
