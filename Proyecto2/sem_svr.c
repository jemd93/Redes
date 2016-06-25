/**************************************************/
/* Servidor para la Asignacion 3.                 */
/* Jorge Marcano : 11-10566                       */
/* Maria Victoria Jorge : 11-10495                */
/**************************************************/

#include "sem_svr.h"


/*
	Método que permite comprobar que se ingresaron la cantidad correcta de parámetros y que las banderas utilizadas son las predeterminadas.

	Parámetros:
		int argc: cantidad de argumentos pasados por consola.
		char* argv[]: arreglo que contiene los argumentos pasados por consola.
*/
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

/*
	Función que calcula el monto que debe pagar un vehículo al salir del estacionamiento.

	Parámetros:
		char* fname: nombre del archivo donde se encuentra la bitácora del estacionamiento.
		char* id: identificador único del vehículo al que se le calcula la tarifa.
		char* tiempoAct: fecha y hora del momento en el que vehículo decide retirarse.
	Salida:
		Monto que deberá pagar el vehículo para salir del estacionamiento.
*/
int calcularMonto(char *fname,char *id,char *tiempoAct) {

	FILE *arch;
	int numlin = 1;
	int encontrado = 0;
	char temp[512];
	char* fechaEntStr;

	struct tm fechaEnt;
	struct tm fechaSal;

	if ((arch = fopen(fname,"r")) == NULL) {
		return -1;
	}

	// Obtiene la ultima ocurrencia en la bitacora con ese ID
	while (fgets(temp,512,arch) != NULL) {
		if ((strstr(temp,id)) != NULL) {
			encontrado++;
		}
		numlin++;
	}

	if (encontrado == 0) {
		return -2;
	}

	if (arch) {
		fclose(arch);
	}

	fechaEntStr = strtok(temp," ");
	int i = 0;
	while (fechaEntStr != NULL) {
		if (i == 1) 
			break;
		fechaEntStr = strtok(NULL,"\n");
		i++;
	}

	strptime(fechaEntStr,"%a %b %d %H:%M:%S %Y",&fechaEnt);
	strptime(tiempoAct,"%a %b %d %H:%M:%S %Y",&fechaSal);

	time_t tEnt = mktime(&fechaEnt);
	time_t tSal = mktime(&fechaSal);

	time_t totalSegs = difftime(tSal,tEnt);

	if (totalSegs < 3600) {
		return 30;
	}
	else {
		if ((totalSegs % 3600) == 0) {
			return (totalSegs / 3600) * 80;
		}
		else {
			return ((totalSegs / 3600) * 80) + 30;
		}
	}

	return 1;
}

/*
	Función que permite realizar toda la lógica del estacionamiento en el servidor y responderle al cliente.

	Parámetros:
		inf fdGeneral: file descriptor del pipe que comparten todos los procesos hijos con el padre para leer la cantidad dispobible de puestos.
		int fdHijo: file descriptor del proceso hijo con su padre para escribir la actualización de los puestos.
		char* puerto_sem_svr: puerto del servidor en el que está corriendo la aplicación.
		char* msg: mensaje enviado por el cliente.
		int sockfd: file descriptor del socket.
		struct sockaddr_in info_cl: dirección y puerto del cliente.
		char* fin: nombre del archivo de la bitácora de entrada.
*/
void procesarMsg(int fdGeneral, int fdHijo, char* puerto_sem_svr, char* msg, int sockfd, struct sockaddr_in info_cl,char* fin){
	char lectPipe[4]; // Para leer la cantidad de puestos ocupados
	char *pt; // Para separar el mensaje
	char accion[2]; // e o s
	char id[6]; // ID del vehiculo
	int puestosOcupados; // Cantidad de puestos ocupados actualmente
	char strPuestos[4];
	int numbytes;
	char* respuesta;
	char* mensajeBit;
	int monto;

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
		printf("Un carro trata de entrar por la puerta\n");
		if (puestosOcupados < 200) {
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s,sizeof(s),"%c",tm);
			printf("Se entregara un ticket con id %s y fecha %s \n",id,s);
			sprintf(strPuestos,"%d",puestosOcupados+1);

			// Agregando el resto de la info
			mensajeBit = (char *)malloc(strlen(strPuestos)+strlen(id)+strlen(s)+5);
			strcpy(mensajeBit,strPuestos);
			strcat(mensajeBit,",");
			strcat(mensajeBit,"E");
			strcat(mensajeBit,",");
			strcat(mensajeBit,id);
			strcat(mensajeBit,",");
			strcat(mensajeBit,s);

			write(fdHijo,mensajeBit,strlen(mensajeBit)+1);

			respuesta = (char*)malloc(strlen(id)+strlen(s)+2);
			strcpy(respuesta,id);
			strcat(respuesta,",");
			strcat(respuesta,s);
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
			printf("El carro de ID %s trata de salir por la puerta %d \n",id,((fdHijo-6)/2)+1);
			// Se escribe en la bitacora de salida
			//fprintf(fout,"%s %s \n",id,s);
			sprintf(strPuestos,"%d",puestosOcupados-1);

			// Agregando el resto de la info
			mensajeBit = (char *)malloc(strlen(strPuestos)+strlen(id)+strlen(s)+5);
			strcpy(mensajeBit,strPuestos);
			strcat(mensajeBit,",");
			strcat(mensajeBit,"S");
			strcat(mensajeBit,",");
			strcat(mensajeBit,id);
			strcat(mensajeBit,",");
			strcat(mensajeBit,s);

			write(fdHijo,mensajeBit,strlen(mensajeBit)+1);

			monto = calcularMonto(fin,id,s);

			//Hay que calcular la tarifa. Ese es el mensaje para el cliente
			respuesta = respuesta = (char*)malloc(53);
			sprintf(respuesta,"Precio a cancelar : %d",monto);
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
	free(mensajeBit);
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
	char* pt;

	char puerto_sem_svr[5];
	char* bitacora_entrada; 
	char* bitacora_salida; 

	pid_t pid; // PID para los procesos

	int fdPipes[2]; // File descriptor de un pipe
	int pipeGeneral[2]; // Pipe que comparten todos los hijos para leer
	int pipesHLectura[3]; // FD de lectura de los tres clientes
	int pipesHEscritura[3]; // FD de escritura de los tres clientes
	int numPipe = 0; // Identificador para la asignación de los pipes
	char str[50], strPipeGeneral[4], strPipeH[4], strSocketfd[4];

	int puestosOcupados; // Total de puestos ocupados en el estacionamiento
	int i;

	// Archivos de las bitacoras 
	FILE *fin;
	FILE *fout;

	FILE *puestos;
	char* auxPuestos;
	

	auxPuestos = (char*)malloc(4);
	puestos = fopen("puestos.txt","r");
	auxPuestos = fgets(auxPuestos, 4, puestos);
	puestosOcupados = atoi(auxPuestos);
	fclose(puestos);
	free(auxPuestos);

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

	printf("\nEl Sistema de Estacionamiento Moriah se encuentra abierto\n");
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
		setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

		// Estableciendo la conexion del socket
		if (bind(sockfd,(struct sockaddr *)&info_serv, sizeof(struct sockaddr)) == -1) { 
			perror("bind"); 
			exit(2); 
		} 

		/* Se reciben los datos (directamente, UDP no necesita conexión) */ 
		addr_len = sizeof(struct sockaddr); 
		printf("Esperando carros ....\n"); 
		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&info_cl,
		(socklen_t *)&addr_len)) == -1) { 
			perror("recvfrom"); 
			exit(3); 
		}	 	
		
		// Utilizamos concurrencia para atender a varios clientes a la vez
		pid = fork();
		// Codigo de los hijos
		if (pid == 0) {
			procesarMsg(pipeGeneral[0], pipesHEscritura[numPipe], puerto_sem_svr, buf, sockfd, info_cl,bitacora_entrada);
			free(bitacora_entrada);
			free(bitacora_salida);
			exit (0); 
		}
		else { // Codigo del padre
			sprintf(str,"%d",puestosOcupados);
			write(pipeGeneral[1],str,strlen(str)+1);
			read(pipesHLectura[numPipe],str,50);

			// Escribiendo en las bitacoras
			pt = strtok(str,",");
			int j = 0;
			int ent = 0;
			while (pt != NULL) {
				if (j == 0) {
					puestosOcupados = atoi(pt);
					puestos = fopen("puestos.txt","w+");
					fprintf(puestos, "%d\n",puestosOcupados );
					fclose(puestos);
				}
				else if (j == 1) {
					if (strcmp(pt,"E") == 0) {
						fin = fopen(bitacora_entrada,"a");
						ent = 1;
					}
					else if (strcmp(pt,"S") == 0) {
						fout = fopen(bitacora_salida,"a");
						ent = -1;
					}
					else if (strcmp(pt,"N") == 0) {
						ent = 0;
						break;
					}
				}
				else if (j == 2) {
					if (ent == 1) {
						fprintf(fin,"%s ",pt);	
					}
					else if (ent == -1) {
						fprintf(fout,"%s ",pt);
					}
				}
				else if (j == 3) {
					if (ent == 1) { 
						fprintf(fin,"%s\n",pt);
						fclose(fin);
					}
					else if (ent == -1) {
						fprintf(fout,"%s\n",pt);
						fclose(fout);
					}
				}

				j++;

				pt = strtok(NULL,",");
			}

			numPipe = (numPipe + 1) % 3; 
			printf("PUESTOS OCUPADOS : %s\n\n", str);
			close(sockfd);

		}

		waitpid(-1, NULL, WNOHANG); // Limpiando los zombies

	}

	free(bitacora_entrada);
	free(bitacora_salida);
	exit (0); 
}