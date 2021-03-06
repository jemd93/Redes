/**************************************************/
/* Cliente para la Asignacion 3.                  */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include "sem_cli.h"

/*
	Método que permite comprobar que se ingresaron la cantidad correcta de parámetros y que las banderas utilizadas son las predeterminadas.

	Parámetros:
		int argc: cantidad de argumentos pasados por consola.
		char* argv[]: arreglo que contiene los argumentos pasados por consola.
*/
void checkArgs(int argc,char *argv[]) {
	// Chequeo del numero de parametros
	if (argc != 9) { 
		fprintf(stderr,"Error: Por favor introduzca los parametros correctos\n"); 
		exit(1); 
	} 

	// Chequeo de los parametros
	int i = 1;
	while (i < 9) {
		if (!((strcmp(argv[i],"-d") == 0) || (strcmp(argv[i],"-p") == 0) 
			|| (strcmp(argv[i],"-c") == 0) || (strcmp(argv[i],"-i") == 0))) {
			fprintf(stderr,"Error: Por favor introduzca los parametros correctos");
			exit(1);
		}
		i = i+2;
	}
}

int main(int argc, char *argv[]) 
{ 
	int sockfd; /* descriptor a usar con el sockfd */ 
	struct sockaddr_in info_serv; /* almacenara la direccion IP y numero de puerto del servidor */ 
	struct sockaddr_in info_cl;
	struct hostent *he; /* para obtener nombre del host */ 
	int numbytes, addr_len; /* conteo de bytes a escribir */ 
	char buf[BUFFER_LEN]; /* Buffer de recepción */

	char nombre_modulo[21];
	char puerto_sem_svr[6];
	char op[2];
	char identificador_v[11];

	//char mensaje[44]; // (IP/HOST (24),puerto(5),op(1),id(10) )
	char* mensaje;

	int intentos = 0;
	struct timeval tv;

	// Chequeos de entrada.
	checkArgs(argc,argv);

	// Asignacion de los argumentos.
	int i = 1;
	while (i < 9) {
		if (strcmp(argv[i],"-d") == 0) {
			strcpy(nombre_modulo,argv[i+1]);
		}
		if (strcmp(argv[i],"-p") == 0) {
			strcpy(puerto_sem_svr,argv[i+1]);
		}
		if (strcmp(argv[i],"-c") == 0) {
			strcpy(op,argv[i+1]);
		}
		if (strcmp(argv[i],"-i") == 0) {
			strcpy(identificador_v,argv[i+1]);
		}
		i = i+2;
	}

	if (strcmp(op,"e")!=0 && strcmp(op,"s")!=0){
		printf("Operación inválida. Intente de nuevo con \"e\" o \"s\"\n");
		exit(0);
	}

	mensaje = (char*)malloc(3+strlen(identificador_v));
	strncpy(&mensaje[0],op,2);
	strcat(mensaje,",");
	strcat(mensaje,identificador_v);

	/* convertimos el hostname a su direccion IP */ 
	if ((he=gethostbyname(nombre_modulo)) == NULL) { 
		perror("gethostbyname"); 
		exit(1); 
	} 
	/* Creamos el sockfd */ 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
		perror("socket"); 
		exit(2); 
	} 

	int opt = 1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int)) < 0){
		perror("setsockopt:");
		exit(3);
	}

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0){
		perror("setsockopt:");
		exit(3);
	}

	/* a donde mandar */ 
	info_serv.sin_family = AF_INET; /* usa host byte order */ 
	info_serv.sin_port = htons(atoi(puerto_sem_svr)); /* usa network byte order */ 
	info_serv.sin_addr = *((struct in_addr *)he->h_addr); 
	bzero(&(info_serv.sin_zero), 8); /* pone en cero el resto */ 
	 // enviamos el mensaje  
	if ((numbytes=sendto(sockfd,mensaje,strlen(mensaje),0,(struct sockaddr *)&info_serv,
		sizeof(struct sockaddr))) == -1) { 
		perror("sendto"); 
		exit(2); 
	}
	printf("Se envio la siguiente informacion al estacionamiento : %s \n",mensaje); 

	/* Se reciben los datos (directamente, UDP no necesita conexión) */ 
	addr_len = sizeof(struct sockaddr); 
	printf("Esperando respuesta de la puerta ....\n"); 

	while (intentos < 3){
		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&info_serv,
			(socklen_t *)&addr_len)) == -1) { 
			intentos++;
			if ((numbytes=sendto(sockfd,mensaje,strlen(mensaje),0,(struct sockaddr *)&info_serv,
				sizeof(struct sockaddr))) == -1) { 
				perror("sendto"); 
				exit(2); 
			}
			printf("Error en la comunicación con el servidor. Intentando nuevamente...\n");
		} else {
			break;
		}
	}
		 
	if (intentos == 3){
		printf("Lo sentimos, el tiempo de respuesta se agotó.\n");
	} else {
		printf("Mensaje: %s\n",buf);	
	}
	

	/* cierro sockfd */ 
	close(sockfd); 
	free(mensaje);
	exit(0);
}