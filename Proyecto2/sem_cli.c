/**************************************************/
/* Cliente para la Asignacion 3.                  */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge :                         */
/**************************************************/

#include "sem_cli.h"

void checkArgs(int argc,char *argv[]) {
	// Chequeo del numero de parametros
	if (argc != 9) { 
		fprintf(stderr,"\nError: Por favor introduzca los parametros correctos", argv[0]); 
		exit(1); 
	} 

	// Chequeo de los parametros
	int i = 1;
	while (i < 9) {
		if (!((strcmp(argv[i],"-d") == 0) || (strcmp(argv[i],"-p") == 0) 
			|| (strcmp(argv[i],"-c") == 0) || (strcmp(argv[i],"-i") == 0))) {
			fprintf(stderr,"\nError: Por favor introduzca los parametros correctos",argv[0]);
			exit(1);
		}
		i = i+2;
	}
}

int main(int argc, char *argv[]) 
{ 
	// int sockfd; /* descriptor a usar con el socket */ 
	// struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */ 
	// struct hostent *he; /* para obtener nombre del host */ 
	// int numbytes; /* conteo de bytes a escribir */ 

	char nombre_modulo[20];
	int puerto_sem_svr;
	char op[1];
	int identificador_v;

	// Chequeos de entrada.
	checkArgs(argc,argv);

	// Asignacion de los argumentos.
	int i = 1;
	while (i < 9) {
		if (strcmp(argv[i],"-d") == 0) {
			strcpy(nombre_modulo,argv[i+1]);
		}
		if (strcmp(argv[i],"-p") == 0) {
			puerto_sem_svr = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-c") == 0) {
			strncpy(op,argv[i+1],1);
		}
		if (strcmp(argv[i],"-i") == 0) {
			identificador_v = atoi(argv[i+1]);
		}
		i = i+2;
	}

	// /* convertimos el hostname a su direccion IP */ 
	// if ((he=gethostbyname(argv[1])) == NULL) { 
	// 	perror("gethostbyname"); 
	// 	exit(1); 
	// } 
	// /* Creamos el socket */ 
	// if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
	// 	perror("socket"); 
	// 	exit(2); 
	// } 
	// /* a donde mandar */ 
	// their_addr.sin_family = AF_INET; /* usa host byte order */ 
	// their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */ 
	// their_addr.sin_addr = *((struct in_addr *)he->h_addr); 
	// bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */ 
	// /* enviamos el mensaje */ 
	// if ((numbytes=sendto(sockfd,argv[2],strlen(argv[2]),0,(struct sockaddr *)&their_addr,
	// 	sizeof(struct sockaddr))) == -1) { 
	// 	perror("sendto"); 
	// 	exit(2); 
	// } 
	// printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(their_addr.sin_addr)); 
	// /* cierro socket */ 
	// close(sockfd); 
	exit(0);
}