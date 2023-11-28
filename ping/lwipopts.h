#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

//
//
// This example uses a common include to avoid repetition
#include "lwipopts_examples_common.h"

#if !NO_SYS
#define TCPIP_THREAD_STACKSIZE 1024
#define DEFAULT_THREAD_STACKSIZE 1024
#define DEFAULT_RAW_RECVMBOX_SIZE 8
#define TCPIP_MBOX_SIZE 8
#define LWIP_TIMEVAL_PRIVATE 0

// This section enables HTTPD server with SSI, SGI
// and tells server which converted HTML files to use
#define LWIP_HTTPD 1
#define LWIP_HTTPD_SSI 1
#define LWIP_HTTPD_CGI 1
#define LWIP_HTTPD_SSI_INCLUDE_TAG 0
#define HTTPD_FSDATA_FILE "htmldata.c"
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1
#define LWIP_SO_RCVTIMEO 1
#endif


#endif
