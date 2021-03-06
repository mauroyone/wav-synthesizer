#ifndef COMMON_H
#define COMMON_H

#define PI 3.141592/*65359*/

#define MSJ_ERROR_MEMORIA "No se pudo cargar memoria"
#define MSJ_ERROR_ABRIR_ARCHIVO "No se pudo abrir el archivo"
#define MSJ_ERROR_ARGC "La cantidad de parametros es incorrecta"
#define MSJ_ERROR_PUNTERO_NULO "Se recibio un puntero nulo como argumento"
#define MSJ_ERROR_LIBERAR_MEMORIA "No se pudo liberar la memoria"
#define MSJ_ERROR_ARGUMENTOS "Los argumentos son incorrectos"
#define MAX_LINEA 60

typedef unsigned int uint;

typedef enum{OK, ERROR_MEMORIA, ERROR_LIBERAR_MEMORIA, ERROR_PUNTERO_NULO,
			 ERROR_FORMATO_SINTETIZADOR, 
			 ERROR_NOMBRE_FUNCION, ERROR_FORMATO_PARTITURA, ERROR_NOTA,
			 ERROR_CANT_ARG, ERROR_ARGUMENTO_INVALIDO,
			 ERROR_PARTITURA_INVALIDA, ERROR_SINTETIZADOR_INVALIDO,
			 ERROR_FRECUENCIA_INVALIDA,ERROR_CARGAR_WAV,
			 ERROR_CREAR_ARCHIVO_WAV,ERROR_CONVERTIR, ERROR_TIEMPO_INVALIDO,
			 ERROR_OBTENER_FACTOR_AMPLITUD,ERROR_OBTENER_MAXIMO,ERROR_OBTENER_MINIMO}status_t; 

#endif
