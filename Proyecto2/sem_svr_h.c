/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include "sem_svr_h.h"

int main(int argc, char *argv[]){
	int fdGeneral, fdHijo;
	char lectPipe[4];
	fdGeneral = atoi(argv[0]);
	fdHijo = atoi(argv[1]);
	printf("Pipe: %d pipeH: %d\n",fdGeneral, fdHijo);
	read(fdGeneral, lectPipe, 4);

	write(fdHijo,"000",4);

}