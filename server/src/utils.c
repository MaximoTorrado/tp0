#include"utils.h"

t_log* logger;

//! 1) primer paso, iniciar el servidor, para eso creamos el file descriptor osea el socket con un "formulario"
int iniciar_servidor(void)
{
   // a) primero creamos una instancia del formulario "hints" y el puntero que guardara el resultado del formulario "servinfo"
   struct addrinfo hints, *servinfo;

   // aca arrancamos las propiedades que queremos en el formulario
   memset(&hints, 0, sizeof(hints));  // obligatorio, el formulario al ser una variable local tiene que setearse en 0 porque puede almacenar basura del stack  
   hints.ai_family = AF_INET;         // IPv4  
   hints.ai_socktype = SOCK_STREAM;   // TCP 
   hints.ai_flags = AI_PASSIVE;       // para que escuche de todos lados 

   // b) pido la traduccion, el resultado termina siendo apuntado por el puntero "servinfo", dentro tendra la estructura binaria necesaria para crear el socket y saber a donde conectarse 
   // NULL: la IP, significa mi propia maquina. 
   // PUERTO: puerto, especificamente una constante fija que tenemos en "utils.h" del servidor (NO CONFUNDIR con el valor asociado a la key en "cliente.config" eso es del CLIENTE aca estamos en SV). 
   // hints: el formulario de preferencias. 
   // servinfo: donde apuntamos el resultado 
   getaddrinfo(NULL, PUERTO, &hints, &servinfo);

   // c) uso el resultado para crear el socket, como el puntero "servinfo" apunta a la estructura binaria con todas las preferencias que guardamos en el formulario, ahora vamos a pasarle al file descriptor todas las 2 preferencias guardadas MENOS la flag, esa no va, pero protocol si ahi mediante la funcion "socket()" y terminaremos de guardar el socket en la variable "socket_servidor"
   // recordemos que "servinfo" es un puntero, entonces con la azucar sintactica "->" puedo ingresar a sus campos, aca estamos haciendo eso 
   int socket_servidor = socket(servinfo -> ai_family, servinfo -> ai_socktype, servinfo -> ai_protocol); 

   // OPCIONAL) explicado en notion
   int activado = 1;
   setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
   
   // d) sociamos el socket_servidor creado anteriormente a un puerto
   // "ai_addr" y "ai_addrlen" nos lo devuelve la funcion "getaddrinfo()" en base a los 2 primeros parametros, lo usamos para conectar el socket a un puerto 
   bind(socket_servidor, servinfo -> ai_addr, servinfo -> ai_addrlen);

   // e) ponemos el socket_servidor creado anteriormente a escuchar las conexiones entrantes
   // SOMAXCONN, el segundo parametro de "listen()" define el tamaño de la cola de espera o "backlog", aca especificamente es una constante que vale el maximo que permite el sistema 
   listen(socket_servidor, SOMAXCONN);

   // f) liberamos memoria, el puntero usado hasta ahora que apuntaba a la estructura binaria que nos sirvio para armar el socket debemos liberarlo porque es nuestro 
   freeaddrinfo(servinfo);

   log_trace(logger, "Listo para escuchar a mi cliente");  // mensaje opcional para dar conocimiento que estamos en condiciones de escuchar a los clientes 

   // g) devolvemos finalmente el socket creado con sus especificaciones 
   return socket_servidor;  
}

//! 2) aceptamos al cliente que se quiere conectar y lo hacemos esperar 
int esperar_cliente(int socket_servidor)
{
   // aceptamos un nuevo cliente y guardamos su "extremo" para comunicarnos con el mediante la variable "socket_cliente" (NO confundir con el socket_cliente que luego crearemos en los "utils.c" del cliente, este nos servira para comunicarnos con el, el otro es propio del cliente y se conectara con el servidor) => lo usaremos para "recv()" y "send()"
   // "accept(...)" los ultimos 2 parametros son opcionales, nos sirve para ver a quien aceptamos, pero aca no es necesario. DEVUELVE un fd NUEVO y BLOQUEA el proceso hasta que llega un cliente
   int socket_cliente = accept(socket_servidor, NULL, NULL);

   log_info(logger, "Se conecto un cliente!");  // bueno y guardamos en el log que se conecto un cliente con ese mensaje 

   return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
   int cod_op;
   if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
      return cod_op;
   else
   {
      close(socket_cliente);
      return -1;
   }
}

void* recibir_buffer(int* size, int socket_cliente)
{
   void * buffer;

   recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
   buffer = malloc(*size);
   recv(socket_cliente, buffer, *size, MSG_WAITALL);

   return buffer;
}

void recibir_mensaje(int socket_cliente)
{
   int size;
   char* buffer = recibir_buffer(&size, socket_cliente);
   log_info(logger, "Me llego el mensaje %s", buffer);
   free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
   int size;
   int desplazamiento = 0;
   void * buffer;
   t_list* valores = list_create();
   int tamanio;

   buffer = recibir_buffer(&size, socket_cliente);
   while(desplazamiento < size)
   {
      memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
      desplazamiento+=sizeof(int);
      char* valor = malloc(tamanio);
      memcpy(valor, buffer+desplazamiento, tamanio);
      desplazamiento+=tamanio;
      list_add(valores, valor);
   }
   free(buffer);
   return valores;
}
