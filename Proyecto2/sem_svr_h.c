/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

/*
	La entrada de este programa es:
	sem_sevr_h fdPipeGeneral fdPipeHijo bitacora_entrada bitacora_salida puerto_local mensaje fdSocket
*/

#include "sem_svr_h.h"

int main(int argc, char *argv[]){
	int fdGeneral, fdHijo;
	char lectPipe[4]; // Para leer la cantidad de puestos ocupados
	char *pt; // Para separar el mensaje
	char accion[2]; // e o s
	char id[6]; // ID del vehiculo
	int puestosOcupados; // Cantidad de puestos ocupados actualmente
	int sockfd;
	char strPuestos[4];

	// Creando y abriendo los archivos de bitacoras
	FILE *fin;
	FILE *fout;
	fin = fopen(argv[2],"a");
	fout = fopen(argv[3],"a");

	time_t t = time(NULL); // Para obtener la hora actual;

	fdGeneral = atoi(argv[0]);
	fdHijo = atoi(argv[1]);
	sockfd = atoi(argv[6]);
	
	read(fdGeneral, lectPipe, 4);
	puestosOcupados = atoi(lectPipe);


	// En argv[5] Esta el mensaje enviado por el cliente.
	// Separando la info que necesitamos (accion y id del vehiculo);
	pt = strtok(argv[5],",");
	int i = 0;
	while (pt != NULL) {
		if (i == 0) {
			strcpy(accion,pt);
		}
		else if (i == 1) {
			int a = atoi(pt);
			sprintf(id,"%d",a);
		}

		i++;

		pt = strtok(NULL,",");
	}

	if (strcmp(accion,"e") == 0) {
		printf("Un carro trata de entrar\n");
		if (puestosOcupados < 200) {
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s,sizeof(s),"%c",tm);
			printf("Se entregara un ticket con id %s y fecha %s \n",id,s);
			// Se escribe en la bitacora de entrada.
			fprintf(fin,"%s %s \n",id,s);
			sprintf(strPuestos,"%d",puestosOcupados+1);
			write(fdHijo,strPuestos,strlen(strPuestos)+1);

			// Aqui se manda un mensaje al cliente.
		}
		else {
			printf("El estacionamiento esta lleno. No se enviara ticket \n");

			// Aqui se manda un mensaje al cliente.
		}
	}
	else if (strcmp(accion,"s") == 0) {
		struct tm *tm = localtime(&t);
		char s[64];
		strftime(s,sizeof(s),"%c",tm);
		printf("Un carro trata de salir \n");
		// Se escribe en la bitacora de salida
		fprintf(fout,"%s %s \n",id,s);
		sprintf(strPuestos,"%d",puestosOcupados-1);
		write(fdHijo,strPuestos,strlen(strPuestos)+1);
		// Aqui se manda un mensaje al cliente ?? no ando claro,supongo que si.
	}
	else {
		write(fdHijo,lectPipe,4);
	}

	fclose(fin);
	fclose(fout);
}