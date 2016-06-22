/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge :                         */
/**************************************************/

#include "sem_svr.h"

void checkArgs(int argc,char *argv[]) {
	// Chequeo del numero de parametros
	if (argc != 7) { 
		fprintf(stderr,"Error: Por favor introduzca los parametros correctos\n"); 
		exit(1); 
	} 

	// Chequeo de los parametros
	int i = 1;
	while (i < 7) {
		if (!((strcmp(argv[i],"-l") == 0) || (strcmp(argv[i],"-i") == 0) || (strcmp(argv[i],"-o") == 0))) {
			fprintf(stderr,"Error: Por favor introduzca los parametros correctos\n");
			exit(1);
		}
		i = i+2;
	}
}

int main(int argc, char *argv[]) 
{ 
	int sockfd; // File descriptor del socket
	struct sockaddr_in info_serv;  // Direccion IP y numero de puerto local 
	struct sockaddr_in info_cl; // Direccion IP y numero de puerto del cliente  
	int addr_len, numbytes; //Tamaño de la estructura sockadd_in y bytes recibidos
	char buf[BUFFER_LEN]; // Buffer de recepción 

	char puerto_sem_svr[5];
	char* bitacora_entrada; 
	char* bitacora_salida; 

	pid_t pid; // PID para los procesos

	int fdPipes[2]; // File descriptor de un pipe
	int pipeGeneral[2]; // Pipe que comparten todos los hijos para leer
	int pipesHLectura[3]; // FD de lectura de los tres clientes
	int pipesHEscritura[3]; // FD de escritura de los tres clientes
	int numPipe = 0; // Identificador para la asignación de los pipes
	char str[4], strPipeGeneral[4], strPipeH[4];

	int puestosOcupados = 0; // Total de puestos ocupados en el estacionamiento
	int i;

	pipe(fdPipes);
	pipeGeneral[0] = fdPipes[0];
	pipeGeneral[1] = fdPipes[1];
	//printf("Pipe General %d %d\n",pipeGeneral[0],pipeGeneral[1] );
	for (i = 0; i < 3; i++){
		pipe(fdPipes);
		pipesHLectura[i] = fdPipes[0];
		pipesHEscritura[i] = fdPipes[1];
		//printf("Pipe %d: %d %d\n",i,pipesHLectura[i],pipesHEscritura[i] );
	}

	// Chequeos de entrada.
	checkArgs(argc,argv);

	// Asignacion de puerto y archivos de bitacoras
	i = 1;
	while (i < 7) {
		if (strcmp(argv[i],"-l") == 0) {
			strcpy(puerto_sem_svr,argv[i+1]);
		}
		if (strcmp(argv[i],"-i") == 0) {
			bitacora_entrada = (char*)malloc(strlen(argv[i+1])+1);
			strcpy(bitacora_entrada,argv[i+1]);
		}
		if (strcmp(argv[i],"-o") == 0) {
			bitacora_salida = (char*)malloc(strlen(argv[i+1])+1);
			strcpy(bitacora_entrada,argv[i+1]);
		}
		i = i+2;
	}

	int opt = 1;
	while (1) {

		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
			perror("socket"); 
			exit(1); 
		} 
		/* Se establece la estructura info_serv para luego llamar a bind() */ 
		info_serv.sin_family = AF_INET; // Usa host byte order 
		info_serv.sin_port = htons(atoi(puerto_sem_svr)); // usa network byte order 
		info_serv.sin_addr.s_addr = INADDR_ANY; // escuchamos en todas las IPs 
		bzero(&(info_serv.sin_zero), 8); // rellena con ceros el resto de la estructura 
		/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 

		// SetsocketOptions. 
		// SOL_SOCKET es para que aplique la opcion a nivel de socket
		// SO_REUSEADDR para que la misma aplicacion pueda usar el mismo puerto varias veces
		// opt es 1 para "prender" la opcion.
		// No se para que es el size of int,pero en stack overflow lo ponian asi.
		setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

		

		printf("Asignado direccion al socket ....\n"); 
		if (bind(sockfd,(struct sockaddr *)&info_serv, sizeof(struct sockaddr)) == -1) { 
			perror("bind"); 
			exit(2); 
		} 
		/* Se reciben los datos (directamente, UDP no necesita conexión) */ 
		addr_len = sizeof(struct sockaddr); 
		printf("Esperando datos ....\n"); 
		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&info_cl,
			(socklen_t *)&addr_len)) == -1) { 
			perror("recvfrom"); 
			exit(3); 
		} 

		pid = fork();
		if (pid == 0) {
			/*close(pipeGeneral[1]); // Cierro el fd de escribir porque no lo uso
			for (i = 0; i < 3; i++){
				if (numPipe != i){
					close(pipesHEscritura[i]);
				}*/
				//close(pipesHLectura[i]);
			//}
			sprintf(strPipeGeneral,"%d",pipeGeneral[0]);
			sprintf(strPipeH,"%d",pipesHEscritura[numPipe]);
			if (execlp("./sem_svr_h",strPipeGeneral,strPipeH,bitacora_entrada,bitacora_salida,puerto_sem_svr,NULL)<0){
                perror("exec: ");
            }
		}
		else {
			sprintf(str,"%d",puestosOcupados);
			write(pipeGeneral[1],str,strlen(str)+1);
			read(pipesHLectura[numPipe],str,4);
			puestosOcupados = atoi(str);
			numPipe = (numPipe + 1) % 3; 
			close(sockfd);
		}

		//close(pipeGeneral[0]); // Cierro el fd de leer porque no lo uso
		/*for (i = 0; i < 3; i++){
			close(pipesHEscritura[i]); // Cierro el fd de escribir porque no lo uso
		}*/

		waitpid(-1, NULL, WNOHANG); // Limpiando los zombies

		free(bitacora_entrada);
		free(bitacora_salida);
	}

	exit (0); 
}