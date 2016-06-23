/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <time.h>

// #define SERVER_PORT 20495
#define BUFFER_LEN 1024 

void checkArgs(int argc,char *argv[]);