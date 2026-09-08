#define DEBUG
#include <api.h>
#include "util_debug.h"
#include  "app.h"
#include  "drivers.h"



wait_event_t interface_event = NULL;            //是否进入物体识别模式


void interface_thread(void *parameter)
{  
    int res;
    interface_event = init_event(); 
    while (1) {
        sleep(1000);
        printf("进入interface_thread \r\n");
        res = wait_event_timeout(interface_event, 5000);    
        if(res != 0) {       
             continue;          
        }   
    }
}


void interface_display(void )
{

}




void interface_init(void)
{

//         thread_create(interface_thread,
//                        0,
//                        TASK_NRF_PRIO,
//                        0,
//                        TASK_IMG_SEND_STACK_SIZE,
//                        "web_server_thread");

}
