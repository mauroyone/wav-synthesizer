#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "partitura.h" 

static const float freq_notas[]={
					FREQ_C,
					FREQ_CS,
					FREQ_D,
					FREQ_DS,
					FREQ_E,
					FREQ_F,
					FREQ_FS,
					FREQ_G,
					FREQ_GS,
					FREQ_A,
					FREQ_AS,
					FREQ_B};
					
static const char simbolo_notas[7]={
					DO,
					RE,
					MI,
					FA,
					SOL,
					LA,
					SI};
				
float obtener_frecuencia_nota (const char nota_americana[])
{	
	notas_t nota;

	switch(nota_americana[PRIMER_ELEMENTO_ARREGLO]){
		case DO:
		nota = NOTA_DO;
		break;
		case RE:
		nota = NOTA_RE;
		break;
		case MI:
		nota = NOTA_MI;
		break;
		case FA:
		nota = NOTA_FA;
		break;
		case SOL:
		nota = NOTA_SOL;
		break;
		case LA:
		nota = NOTA_LA;
		break;
		case SI:
		nota = NOTA_SI;
		break;
	}
	if(nota_americana[SEGUNDO_ELEMENTO_ARREGLO] == SOSTENIDO)
		return freq_notas[nota + 1];
	if(nota_americana[SEGUNDO_ELEMENTO_ARREGLO] == BEMOL)
		return freq_notas[nota-1];
	return freq_notas[nota];
}

status_t TDA_partitura_creadora(TDA_partitura **partitura)
{
	if(partitura == NULL)
	{
		return ERROR_PUNTERO_NULO;
	}
	
	*partitura = (TDA_partitura *) malloc(sizeof(TDA_partitura));
	if(*partitura == NULL)
	{
		return ERROR_MEMORIA;
	}
	return OK;
}

status_t TDA_partitura_inicializar(TDA_partitura *partitura)
{
	if(partitura == NULL)
	{
		return ERROR_PUNTERO_NULO;
	}
	
	partitura -> contador_notas = 0;
	partitura -> aux_duracion_nota = NULL;
	partitura -> aux_frecuencia_nota = NULL;
	partitura -> aux_inicio_nota = NULL;
	partitura -> lineas = NULL;
	
	return OK;
}

status_t TDA_partitura_cargar (TDA_partitura * partitura, FILE * archivo_partitura)
/*el archivo partitura seria un   FILE * fi    que se iniciaria con
 * fopen (argv[partitura], "rt"); */
{
	size_t used, i, j;
	char **paux;
	
	if(partitura == NULL)
	{
		return ERROR_PUNTERO_NULO;
	}
	
	partitura -> contador_notas = 0;
	
	/*crea arreglo de punteros a cadenas*/
	(partitura -> lineas) = (char **) malloc (sizeof(char*) * INIT_SIZE_PARTITURA);
	if ((partitura -> lineas) == NULL)
	{
		return ERROR_MEMORIA;
	}
	used = INIT_SIZE_PARTITURA;
	
	/*crea strings*/
	for(j = 0; j < used; j++)
	{
		(partitura -> lineas) [j] = (char*) malloc (sizeof(char) * MAX_LINEA);
		if ((partitura -> lineas)[j] == NULL)
		{
			return ERROR_MEMORIA;
		}	
	}
	
	/*carga los strings*/
	i = 0;
	while(fgets((partitura -> lineas)[i], MAX_LINEA, archivo_partitura) != NULL)
	{
		partitura -> contador_notas ++;
		if (partitura -> contador_notas == used)
		{
			paux = (char**) realloc(*(partitura -> lineas), used * CHOP_SIZE_PARTITURA);
			if (paux == NULL){
				return ERROR_MEMORIA;
			}
			used *= CHOP_SIZE_PARTITURA;
			(partitura -> lineas) = paux;
			for (j=(partitura -> contador_notas); j <	used; j++)
			{
				(partitura -> lineas)[j] = (char*) malloc (sizeof(char) * MAX_LINEA);
				if ((partitura -> lineas)[j] == NULL)
				{
					return ERROR_MEMORIA;
				}
			}	
		}
		i++;
	}
	return OK;	
}

status_t TDA_partitura_cargar_inicio (TDA_partitura * partitura)
{
	size_t i;
	char *perr;
	
	if(partitura == NULL){
		return ERROR_PUNTERO_NULO;
	}
	
	partitura -> aux_inicio_nota = (float *) malloc (sizeof(float) * (partitura -> contador_notas));
	if ((partitura -> aux_inicio_nota) == NULL)
	{
		return ERROR_MEMORIA;
	}
	for (i = 0; i < (partitura -> contador_notas); i++)
	{
		(partitura -> aux_inicio_nota)[i] = strtod((partitura -> lineas)[i], &perr);
		if (!isspace(*perr))
		{
			return ERROR_FORMATO_PARTITURA;
		}
	}
	return OK;
}

status_t TDA_partitura_cargar_duracion (TDA_partitura * partitura)
{
	char *perr, *p, *q, aux[MAX_LINEA];
	size_t i;
	
	(partitura -> aux_duracion_nota) = (float*) malloc (sizeof(float) * (partitura -> contador_notas));
	if(partitura -> aux_duracion_nota == NULL){
		return ERROR_MEMORIA;
	}
	
	for (i = 0; i < (partitura ->contador_notas); i++){
		memcpy(aux, (partitura -> lineas)[i], strlen ((partitura -> lineas)[i] +1));
		for(q = aux; (p = strtok(q, " ")) != NULL; q = NULL)
			(partitura -> aux_duracion_nota)[i] = strtod(p, &perr);
	}
	return OK;
}

status_t TDA_partitura_calcular_duracion (TDA_partitura *partitura)
{
	size_t i;
	float duracion;
	
	if (partitura == NULL){
		return ERROR_PUNTERO_NULO;
	}
	
	duracion = 0;
	for (i = 0; i < (partitura -> contador_notas); i++)
		if (duracion < (partitura -> aux_inicio_nota)[i] + (partitura -> aux_duracion_nota)[i])
			duracion = (partitura -> aux_inicio_nota)[i] + (partitura -> aux_duracion_nota)[i];
	partitura -> duracion_partitura = duracion;
	return OK;
}

status_t TDA_partitura_calcular_frecuencias (TDA_partitura *partitura)
{
	size_t i;
	uint octava;
	float frecuencia;
	char *p, *q, aux[MAX_LINEA], *perr;
	
	if (partitura == NULL){
		return ERROR_PUNTERO_NULO;
	}
	
	(partitura -> aux_frecuencia_nota) = (float *) malloc (sizeof(float) * (partitura -> contador_notas));
	if(partitura -> aux_frecuencia_nota == NULL)
	{
		return ERROR_MEMORIA;
	}
	for (i = 0; i < (partitura -> contador_notas); i++)
	{
		memcpy(aux, (partitura -> lineas)[i], sizeof(char) * (strlen((partitura -> lineas)[i])+1));
		q = aux;
		p = strtok(q, NUMERO_O_ESPACIO);
		frecuencia = obtener_frecuencia_nota(p);
		if(frecuencia < FREQ_C){
			return ERROR_NOTA;
		}
		octava = strtoul((partitura -> lineas)[i] + strlen(p) + (p-q), &perr, 10);
		(partitura -> aux_frecuencia_nota)[i] = frecuencia * octava;
	}
	return OK;
}

status_t TDA_partitura_liberar_memoria(TDA_partitura **partitura)
{
	size_t i;
	
	if(partitura == NULL)
	{
		return ERROR_PUNTERO_NULO;
	}
	
	free((*partitura) -> aux_duracion_nota);
	free((*partitura) -> aux_frecuencia_nota);
	free((*partitura) -> aux_inicio_nota);
	if((*partitura) -> lineas != NULL)
		for(i = 0; i< (*partitura) -> contador_notas; i++)
			free(((*partitura) -> lineas)[i]);
	free((*partitura) -> lineas);
	free(*partitura);
	*partitura = NULL;
	return OK;
}

