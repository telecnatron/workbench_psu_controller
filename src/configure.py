#!/usr/bin/python3
# -----------------------------------------------------------------------------
import sys, os
from datetime import datetime
from pathlib import Path

# ---------------------------------------
# globals:
# count current line num in input file
lnum=0

# task number of next to be created task
tnum=0
#
tasks=[]
#
versions=[]
version_str=""
#
cmds=[]
# ---------------------------------------
def handle_pindef(params):
    (name, port, pin) = params
    print(f"""
// --- {name} configuration ---
#define {name}_DEFS
#define {name}_IP  PIN{port}
#define {name}_OP  PORT{port}
#define {name}_PIN PIN{pin}
#define {name}_DDR DDR{port}
// initialise the pin as output and set it low
#define {name}_INIT_OUTPUT()   PIN_HI({name}_DDR,{name}_PIN); PIN_LO({name}_OP,{name}_PIN)
// initialise the pin as an input and disable the pullup
#define {name}_INIT_INPUT()    PIN_LO({name}_DDR,{name}_PIN); PIN_LO({name}_OP,{name}_PIN)
// enable the pullup on an input
#define {name}_PU_EN()         PIN_HI({name}_OP, {name}_PIN)
#define {name}_ON()            PIN_HI({name}_OP,{name}_PIN)
#define {name}_OFF()           PIN_LO({name}_OP,{name}_PIN)
#define {name}_TOGGLE()        PIN_TOGGLE({name}_OP, {name}_PIN)
#define {name}_READ()          PIN_READ({name}_IP, {name}_PIN)

""".lstrip())

# ---------------------------------------
def handle_include(param):
    global lnum
    fname=Path(param[0])
    if fname.is_file():
        with fname.open() as f:
                print(f.read())
    else:
        raise Exception(f"no such file for inc_file macro at input file line {lnum}")

# ---------------------------------------
def handle_version(param, h=True):
    global versions
    global version_str
    # get git description for app
    gda=os.popen('git describe').read().strip()
    versions.append( ('APP_GIT_DESC',  gda) )
    # get git origin url, or if that's not set then cwd
    gurl=os.popen('git config --get remote.origin.url').read().strip()
    if gurl=="":
        gurl=os.getcwd()
    versions.append(('APP_GIT_URL', gurl))
    # get avr-libc info
    # get git description
    gdl=os.popen('cd lib; git describe').read().strip() 
    versions.append( ('LIB_GIT_DESC', gdl) )
    version_str=f'{gda} {gdl}'
    gurl=os.popen('cd lib; git config --get remote.origin.url').read().strip()
    if gurl=="":
        gurl=os.getcwd()
    versions.append(('LIB_GIT_URL', gurl))
    print(f"// version information")
    for (name, value) in versions:
        print(f"""
#define VERSION_{name} "{value}"
""".strip())
        
    
# ---------------------------------------
def handle_task(param):
    global tasks
    name=param[0]
    #sys.stderr.write(f"task parm: {param},j len: {len(param)}\n")
    run=1
    if len(param)== 2:
        # second param  was specified, this indicates that task should be initialised as not runnable
        run=0
    tasks.append( (name,len(tasks), run) )

# ---------------------------------------
def handle_mmp_cmd(param):
    global cmds
    name=param[0]
    cmds.append((name,len(cmds)))
    
# ---------------------------------------
def write_task_init():
    global tasks;
    if len(tasks)==0:
        # no tasks were defined
        return
    print('// initialise the tasks')
    print('void init_tasks()\n{')
    for (t, n, r ) in tasks:
        print(f'    task_init(TASK_{t.upper()}, task_{t}, NULL, {r});') 
    print('}\n')
    
# ---------------------------------------    
def write_task_defines():
    global tasks

    if len(tasks)==0:
        # no tasks were defined
        return
    
    print(f"// task definitions")
    for (t,n,r) in tasks:
        # eg: #define TASK_BLINK 0
        print(f"#define TASK_{t.upper()} {n}")
    # eg: #define TASK_NUM_TASKS 2
    print(f"#define TASK_NUM_TASKS {len(tasks)}\n")

    # task function forward declarations
    print(f'// task function forward declarations') 
    for (t,n,r)  in tasks:
        print(f'void task_{t}();')
    print()
# ---------------------------------------
def write_mmp_cmds_init():
    global cmd
    print('''
// command handler table
mmp_cmd_handler_t cmd_msg_tab[]={
'''.strip())
    for (t,n) in cmds:
        print(f"    cmd_{t}, // cmd {n}")
    print('};')

    print("""
// initialise the mmp messaging system:
// max message lentgh
#define MSG_MAX_LEN 128
// buffer for messages
static uint8_t msg_buf[MSG_MAX_LEN];
// mmp_cmd control structure
mmp_cmd_ctrl_t mmp_cmd_ctrl;

// call the init function
void init_mmp_cmd()
{
    mmp_cmd_init(&mmp_cmd_ctrl, msg_buf, MSG_MAX_LEN, cmd_msg_tab, CMD_TAB_NUM_ENTRIES(cmd_msg_tab), uart_putc);
}
""".lstrip())    

# ---------------------------------------
def write_mmp_cmds():
    global cmds
                
    if len(cmds)==0:
        return
    
    print('''
// MMP
#define MMP_DEFS
// protocol timeout in ticks
#define MMP_TIMER_TIMEOUT 4
// undef to disable logging
#define MMP_LOGGING
// Define MMP_NO_REBOOT to disable the reboot message
#undef MMP_NO_REBOOT
// MMP_CMD
#define MMP_CMD_DEFS
// undef to disable logging
#define MMP_CMD_LOGGING
'''.lstrip())

    print(f'// mmp_cmd command numbers')
    for(t,n) in cmds:
        t=f'CMD_{t.upper()}'
        print(f'#define {t:<20} {n}')
    print()
    print(f'// mmp_cmd function forward declarations') 
    for (t,n) in cmds:
        # forward declarations
        print(f'void cmd_{t}(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data);')
# ---------------------------------------
def write_version():
    """version infomation string variable for config.c"""
    global version_str
    print("")
    print(f"""
// version:  "GIT description for app" "GIT description for avr_libc"
char version[]="{version_str}";
""".lstrip())
    
# -----------------------------------------------------------------------------                        
# macro handler table
# macro name : handler function
macro_tab={
    'pin_def':  handle_pindef,
    'inc_file': handle_include,
    'task':     handle_task,
    'version':  handle_version,
    'mmp_cmd':  handle_mmp_cmd,
}
# ---------------------------------------
def handler(line):
    for m in macro_tab.keys():
        if line[1:].startswith(m+'(') and line[-1]==')':
            ln=line
            # wep, we match
            #print(f"line: {line}")
            # extract parameters
            line= line[len(m+'(')+1:-1].replace(' ','')
            params=line.split(',')
            #print(f"// --- {ln} ---")
            # call the handler function
            macro_tab[m](params)
            return 0
    raise Exception(f"no definition for macro at input file line {lnum}: {line}")
# ---------------------------------------
def file_marker(fname="", end=False):
    if fname == "":
        fname="this file"
    if end:
        fname="End of "+fname
    else:
        fname=fname.capitalize()
    print(f"""
// =================================================================================
// {fname} automatically generated by {sys.argv[0]} at {datetime.now()}
// =================================================================================
""".strip())

    
# -----------------------------------------------------------------------------                        

if __name__ == '__main__':

    # write config.h
    with open('config.h', 'w') as out:
        # stdout redirected to config.h
        sys.stdout=out
        file_marker('config.h')
        incs='#include "./lib/util/io.h"'
        with open('config.def','r') as inf:
            # redirect stdin so it reads from config.def
            sys.stdin=inf
            for line in sys.stdin:
                lnum+=1
                # remove trailing newline
                line=line.rstrip()
                if len(line):
                    if incs and not line.startswith('//'):
                        print(incs)
                        incs=None
                    if line.startswith('#.'):
                        # it's a comment line, skip
                        pass
                    else:
                        if line[0]=='.':
                            handler(line)
                else:
                    print(line)
            write_task_defines()
            write_mmp_cmds()
            file_marker('config.h',end=True)
            
        # write config.c    
        with open('config.c','w') as out:
            sys.stdout=out
            # stdout redirect to config.c
            file_marker('config.c')
            print('#include "config.h"');
            write_version()
            write_task_init()
            write_mmp_cmds_init()
            file_marker('config.c',end=True)
