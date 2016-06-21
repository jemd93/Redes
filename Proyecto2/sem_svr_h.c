/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include "sem_svr_h.h"

int main(int argc, char *argv[]){
	char lectPipe[4];
	fdGeneral = argv[0];
	fdHijo = argv[1];
	printf("Pipe: %s pipeH: %s\n",argv[0],argv[1]);
	read(fdGeneral, lectPipe, 4);

	write(fdHijo,"000",4);

}