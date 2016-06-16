/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge :                         */
/**************************************************/

#include "sem_svr.h"

void checkArgs(int argc,char *argv[]) {
	// Chequeo del numero de parametros
	if (argc != 7) { 
		fprintf(stderr,"\nError: Por favor introduzca los parametros correctos", argv[0]); 
		exit(1); 
	} 

	// Chequeo de los parametros
	int i = 1;
	while (i < 7) {
		if (!((strcmp(argv[i],"-l") == 0) || (strcmp(argv[i],"-i") == 0) || (strcmp(argv[i],"-o") == 0))) {
			fprintf(stderr,"\nError: Por favor introduzca los parametros correctos",argv[0]);
			exit(1);
		}
		i = i+2;
	}
}

int main(int argc, char *argv[]) 
{ 
	// int sockfd; /* descriptor para el socket */ 
	// struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */ struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */ 
	//  addr_len contendra el taman~o de la estructura sockadd_in y numbytes el 
	// * numero de bytes recibidos  
	// int addr_len, numbytes; 
	// char buf[BUFFER_LEN]; /* Buffer de recepción */ 
	// /* se crea el socket */ 

	int puerto_sem_svr;
	FILE *bitacora_entrada; 
	FILE *bitacora_salida; 

	// Chequeos de entrada.
	checkArgs(argc,argv);

	// Asignacion de puerto y archivos de bitacoras
	int i = 1;
	while (i < 7) {
		if (strcmp(argv[i],"-l") == 0) {
			puerto_sem_svr = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-i") == 0) {
			bitacora_entrada = fopen(argv[i+1],"w");
		}
		if (strcmp(argv[i],"-o") == 0) {
			bitacora_salida = fopen(argv[i+1],"w");
		}
		i = i+2;
	}

	// if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
	// 	perror("socket"); 
	// 	exit(1); 
	// } 
	// /* Se establece la estructura my_addr para luego llamar a bind() */ 
	// my_addr.sin_family = AF_INET; /* usa host byte order */ 
	// my_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */ 
	// my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */ 
	// bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */ 
	// /* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 
	// printf("Asignado direccion al socket ....\n"); 
	// if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) { 
	// 	perror("bind"); 
	// 	exit(2); 
	// } 
	// /* Se reciben los datos (directamente, UDP no necesita conexión) */ 
	// addr_len = sizeof(struct sockaddr); 
	// printf("Esperando datos ....\n"); 
	// if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
	// 	(socklen_t *)&addr_len)) == -1) { 
	// 	perror("recvfrom"); 
	// 	exit(3); 
	// } 
	// /* Se visualiza lo recibido */ 
	// printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr)); 
	// printf("longitud del paquete en bytes: %d\n",numbytes); 
	// buf[numbytes] = '\0'; 
	// printf("el paquete contiene: %s\n", buf); 
	// /* cerramos descriptor del socket */ 
	// close(sockfd); 

	fclose(bitacora_entrada);
	fclose(bitacora_salida);
	exit (0); 
}