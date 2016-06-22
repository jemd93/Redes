/**************************************************/
/* Cliente para la Asignacion 3.                  */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include "sem_cli.h"

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
	struct hostent *he; /* para obtener nombre del host */ 
	int numbytes; /* conteo de bytes a escribir */ 

	char nombre_modulo[21];
	char puerto_sem_svr[6];
	char op[2];
	char identificador_v[11];

	char mensaje[44]; // (IP/HOST (24),puerto(5),op(1),id(10) )

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
	strncpy(&mensaje[0],nombre_modulo,24);
	strcat(mensaje,",");
	strcat(mensaje,puerto_sem_svr);
	strcat(mensaje,",");
	strcat(mensaje,op);
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
	printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(info_serv.sin_addr)); 
	/* cierro sockfd */ 
	close(sockfd); 
	exit(0);
}