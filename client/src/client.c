#include "client.h"
#include <readline/readline.h>

int main(void)
{
   //todo ---------------------------------------------------PARTE 2-------------------------------------------------------------

   int conexion;
   char* ip;
   char* puerto;
   char* valor;

   t_log* logger;
   t_config* config;

   //todo ---------------- LOGGING ----------------

   logger = iniciar_logger();  //! aca hacemos uso de la funcion que terminamos de definir mas abajo

   //todo Usando el logger creado previamente Escribi: "Hola! Soy un log"

   //! como vemos arriba nos da el contexto de lo que tenemos que hacer aca, vamos a hacer uso de otra funcion de las commons, justamente con el logger que creamos anteriormente mas arriba
   log_info(logger, "Soy un log");


   //todo ---------------- ARCHIVOS DE CONFIGURACION ----------------

   config = iniciar_config();  //! lo mismo aca, en config vamos a guardar lo que devuelve la funcion que definimos mas abajo que vimos que es como un wrapper de la funcion de la common para crear una estructura de una config

   //todo Usando el config creado previamente, leemos los valores del config y los dejamos en las variables 'ip', 'puerto' y 'valor'

   //! siguiendo con lo que pide arriba entonces aca nuevamente hacemos uso de las common especificamente para config, mas especificamente obtendremos todos los valores asociados a las keys en formato string 
   ip = config_get_string_value(config, "IP");  // entonces le pasamos la config creada anteriormente, el nombre de la key y nos devolvera en un string el valor asociado a cada una
   puerto = config_get_string_value(config, "PUERTO");
   valor = config_get_string_value(config, "CLAVE");

   //todo Loggeamos el valor de config
   //! lo mismo
   log_info(logger, "El valor de la CLAVE es: %s", valor);  // mas arriba guardamos todos los valores asociados a las keys en formato string, pero especificamente nos piden solo guardar en el log el valor asociado a la clave 


   //todo ---------------- LEER DE CONSOLA ----------------

   leer_consola(logger);

   //todo ---------------------------------------------------PARTE 3-------------------------------------------------------------

   //todo ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

   //todo Creamos una conexión hacia el servidor
   conexion = crear_conexion(ip, puerto);

   //todo Enviamos al servidor el valor de CLAVE como mensaje

   //todo Armamos y enviamos el paquete
   paquete(conexion);

   terminar_programa(conexion, logger, config);

   //todo ---------------------------------------------------PARTE 5-------------------------------------------------------------
   //todo Proximamente
}

//! bien, aca esta la firma que nos mandan a definir
t_log* iniciar_logger(void)
{
   t_log* nuevo_logger = log_create("tp0.log", "CLIENTE", true, LOG_LEVEL_INFO);  // en esta funcion en realidad es un llamado a "log_create(...)" para obtener la instancia de un log

   // nos fijamos que el mismo justamente se cree bien y no sea NULL, en caso de serlo mostramos este mensaje y abortamos la operacion
   if (nuevo_logger == NULL) {
      fprintf(stderr, "No se pudo cargar el logger\n");
      abort();  // esta funcion es propia de la libreria de C, en sintesis es mas botona que "exit(1)" porque esta ultima flushea los buffers de stdout, esta la recomienda la catedra porque van a haber procesos que mueren por una condicion inesperada y abort es mas botona, la otra te borra la escena digamos
   }

   return nuevo_logger;  // terminamos de devolver el logger que nos aseguramos que se haya creado bien
}

//! aca esta la segunda firma que tenemos que definir
t_config* iniciar_config(void)
{
   t_config* nuevo_config = config_create("cliente.config");  // ya nos dan esta variable en la misma firma tonc nos dan lugar a que unicamente el llamado y guardar lo que devuelve la funcion de la common, guardado dicho archivo de config

   // lo mismo nos fijamos que se cree bien, si no entonces damos ese mensaje de alerta y abortamos todo
   if (nuevo_config == NULL) {
      fprintf(stderr, "No se pudo cargar el config\n");
      abort();
   }

   return nuevo_config;
}

//! aca la tercer firma a definir 
void leer_consola(t_log* logger)
{
   char* leido;

   while(1) {
      //todo La primera te la dejo de yapa
      leido = readline("> ");

      //todo El resto, las vamos leyendo y logueando hasta recibir un string vacío
      if (leido == NULL) {
         break;
      }

      if(string_is_empty(leido)) {
         free(leido);
         break;
      }

      log_info(logger, "%s", leido);
      
      //todo ¡No te olvides de liberar las lineas antes de regresar!
      free(leido);
   }
}

void paquete(int conexion)
{
   //todo Ahora toca lo divertido!
   char* leido;
   t_paquete* paquete;

   //todo Leemos y esta vez agregamos las lineas al paquete


   //todo ¡No te olvides de liberar las líneas y el paquete antes de regresar!

}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
   //todo Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
   log_destroy(logger);  //! LOGGER, unicamente uso de esta funcion de las commons para liberar los logs, con el mismo que vinimos trabajando hasta ahora
   config_destroy(config); //! CONFIG, libero los config 
}
