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

void procesarMsg(int fdGeneral, int fdHijo, char* puerto_sem_svr, char* msg, int sockfd, struct sockaddr_in info_cl){
	char lectPipe[4]; // Para leer la cantidad de puestos ocupados
	char *pt; // Para separar el mensaje
	char accion[2]; // e o s
	char id[6]; // ID del vehiculo
	int puestosOcupados; // Cantidad de puestos ocupados actualmente
	char strPuestos[4];
	int numbytes;
	char* respuesta;

	// Creando y abriendo los archivos de bitacoras
	/*FILE *fin;
	FILE *fout;
	fin = fopen(argv[2],"a");
	fout = fopen(argv[3],"a");*/

	time_t t = time(NULL); // Para obtener la hora actual;
	
	read(fdGeneral, lectPipe, 4);
	puestosOcupados = atoi(lectPipe);

	// Separando la info que necesitamos (accion y id del vehiculo);
	pt = strtok(msg,",");
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
			//fprintf(fin,"%s %s \n",id,s);
			sprintf(strPuestos,"%d",puestosOcupados+1);
			write(fdHijo,strPuestos,strlen(strPuestos)+1);

			respuesta = (char*)malloc(strlen(id)+strlen(s)+2);
			strcpy(respuesta,id);
			strcat(respuesta,",");
			strcat(respuesta,s);
			printf("Respuesta : %s\n",respuesta );
			if ((numbytes=sendto(sockfd,respuesta,strlen(respuesta)+1,0,(struct sockaddr *)&info_cl,
			 	sizeof(struct sockaddr))) == -1) { 
			 	perror("sendto"); 
			 	exit(2); 
			} 
		}
		else {
			printf("El estacionamiento esta lleno. No se enviara ticket \n");
			respuesta = (char*)malloc(53);
			strcpy(respuesta,"Disculpe, en este momento no hay puestos disponibles");
			if ((numbytes=sendto(sockfd,respuesta,strlen(respuesta)+1,0,(struct sockaddr *)&info_cl,
			 	sizeof(struct sockaddr))) == -1) { 
			 	perror("sendto"); 
			 	exit(2); 
			} 

		}
	}
	else if (strcmp(accion,"s") == 0) {
		if (puestosOcupados > 0) {
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s,sizeof(s),"%c",tm);
			printf("Un carro de ID %s trata de salir\n",id);
			// Se escribe en la bitacora de salida
			//fprintf(fout,"%s %s \n",id,s);
			sprintf(strPuestos,"%d",puestosOcupados-1);
			write(fdHijo,strPuestos,strlen(strPuestos)+1);

			//Hay que calcular la tarifa. Ese es el mensaje para el cliente
			respuesta = respuesta = (char*)malloc(53);
			sprintf(respuesta,"Saliendo carro con ID : %s",id);
			if ((numbytes=sendto(sockfd,respuesta,strlen(respuesta)+1,0,(struct sockaddr *)&info_cl,
			 	sizeof(struct sockaddr))) == -1) { 
			 	perror("sendto"); 
			 	exit(2); 
			} 
		}
		else {
			printf("Error, el estacionamiento esta vacio \n");
			respuesta = respuesta = (char*)malloc(53);
			sprintf(respuesta,"Error, el estacionamiento esta vacio \n");
			if ((numbytes=sendto(sockfd,respuesta,strlen(respuesta)+1,0,(struct sockaddr *)&info_cl,
			 	sizeof(struct sockaddr))) == -1) { 
			 	perror("sendto"); 
			 	exit(2); 
			} 
		}

	}
	else {
		write(fdHijo,lectPipe,4);
	}
	free(respuesta);
	//fclose(fin);
	//fclose(fout);
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
	char str[4], strPipeGeneral[4], strPipeH[4], strSocketfd[4];

	int puestosOcupados = 0; // Total de puestos ocupados en el estacionamiento
	int i;

	pipe(fdPipes);
	pipeGeneral[0] = fdPipes[0];
	pipeGeneral[1] = fdPipes[1];
	for (i = 0; i < 3; i++){
		pipe(fdPipes);
		pipesHLectura[i] = fdPipes[0];
		pipesHEscritura[i] = fdPipes[1];
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
			strcpy(bitacora_salida,argv[i+1]);
		}
		i = i+2;
	}

	int opt = 1;
	while (1) {

		// Crear un socket UDP
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

			procesarMsg(pipeGeneral[0], pipesHEscritura[numPipe], puerto_sem_svr, buf, sockfd, info_cl);
			free(bitacora_entrada);
			free(bitacora_salida);
			exit (0); 
			/*sprintf(strPipeGeneral,"%d",pipeGeneral[0]);
			sprintf(strPipeH,"%d",pipesHEscritura[numPipe]);
			sprintf(strSocketfd,"%d",sockfd);
			if (execlp("./sem_svr_h",strPipeGeneral,strPipeH,bitacora_entrada,bitacora_salida,puerto_sem_svr,buf,
				strSocketfd,NULL)<0){
                perror("exec: ");
            }*/
		}
		else {
			sprintf(str,"%d",puestosOcupados);
			write(pipeGeneral[1],str,strlen(str)+1);
			read(pipesHLectura[numPipe],str,4);
			puestosOcupados = atoi(str);
			numPipe = (numPipe + 1) % 3; 
			printf("Cantidad de puestos %s\n", str);
			close(sockfd);
		}

		waitpid(-1, NULL, WNOHANG); // Limpiando los zombies

	}

	free(bitacora_entrada);
	free(bitacora_salida);
	exit (0); 
}