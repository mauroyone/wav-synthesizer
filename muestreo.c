#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "muestreo.h"
#include "sintetizador.h"

status_t crear_vector_muestras (float **muestras,const uint cantidad_muestras)
{
	
	if(muestras == NULL || cantidad_muestras < 0)
		return ERROR_PUNTERO_NULO;
	
	 *muestras = (float*) calloc(cantidad_muestras, sizeof(float));
	 
	 if(*muestras == NULL)	
	 	return ERROR_MEMORIA;
		 
	 return OK;
 }

status_t muestrear (const uint cant_notas, const float notas[], const float inicios[], const float duraciones[],
					const uint cant_armonicos, const float intensidades[], const float armonicos[],
					const float intervalo, const uint frecuencia, float muestras[],
					TDA_sintetizador * sint){
	float t, tiempo_decaimiento;
	size_t i, j, k;
	
	tiempo_decaimiento = *((sint -> moduladoras[INDICE_DECAIMIENTO]).param1);
	for(i = 0; i < cant_notas; i++){
		for(t = inicios[i]; t < (inicios[i]+duraciones[i]+tiempo_decaimiento); t += intervalo){
			for(j = 0; j < cant_armonicos; j++){
				k = (size_t) (t * frecuencia);
				muestras[k] += intensidades[j] * sin(2 * PI * armonicos[j] * notas[i] * (t - inicios[i]));
				obtener_amplitud(t-inicios[i], duraciones[i], sint, &muestras[k]);
			}
		}
	}
	return OK;
} 

status_t obtener_amplitud(const float delta_tiempo,const float duracion_nota, TDA_sintetizador *sint, float *muestra)
{
	/*aplica las funciones de ataque, sostenido y decaimiento*/
	double tiempo_ataque, tiempo_decaimiento;
	double *p1, *p2, *p3;
	
	if(delta_tiempo < 0)
		return ERROR_TIEMPO_INVALIDO;
		
	if(muestra == NULL || sint == NULL)
		return ERROR_PUNTERO_NULO;
		
	tiempo_ataque = *((sint->moduladoras[INDICE_ATAQUE]).param1);
	tiempo_decaimiento = *((sint->moduladoras[INDICE_DECAIMIENTO]).param1);
	
	if(delta_tiempo < tiempo_ataque)
	{
		p1 = (sint->moduladoras[INDICE_ATAQUE]).param1;
		p2 = (sint->moduladoras[INDICE_ATAQUE]).param2;
		p3 = (sint->moduladoras[INDICE_ATAQUE]).param3;
		*muestra *= sint->pataque(delta_tiempo, p1, p2, p3);
	}
	else 
	{
		if((delta_tiempo<duracion_nota)&&(delta_tiempo>tiempo_ataque))
		{		
			p1 = (sint->moduladoras[INDICE_SOSTENIDO]).param1;
			p2 = (sint->moduladoras[INDICE_SOSTENIDO]).param2;
			p3 = (sint->moduladoras[INDICE_SOSTENIDO]).param3;
			*muestra *= sint->psostenido(delta_tiempo - tiempo_ataque, p1, p2, p3);
		}
		else
		{
			if((delta_tiempo>duracion_nota)&&(delta_tiempo<duracion_nota+tiempo_decaimiento))
			{
				p1 = (sint->moduladoras[INDICE_SOSTENIDO]).param1;
				p2 = (sint->moduladoras[INDICE_SOSTENIDO]).param2;
				p3 = (sint->moduladoras[INDICE_SOSTENIDO]).param3;
				*muestra *= sint->psostenido(duracion_nota - tiempo_ataque,p1, p2, p3);
				p1 = (sint->moduladoras[INDICE_DECAIMIENTO]).param1;
				p2 = (sint->moduladoras[INDICE_DECAIMIENTO]).param2;
				p3 = (sint->moduladoras[INDICE_DECAIMIENTO]).param3;
				*muestra *= sint->pdecaimiento(delta_tiempo - duracion_nota, p1, p2, p3);
			}
			else
			{
				*muestra = 0;
			}
		}	
	}
	return OK;
}
	
status_t multiplicar_factor_amplitud(float *muestras, const uint cant_muestras)
{
	float factor_amplitud;
	size_t i;
	
	if(muestras == NULL)
		return ERROR_PUNTERO_NULO;
		
	if(obtener_factor_amplitud(&factor_amplitud, muestras, cant_muestras) != OK)
		return ERROR_OBTENER_FACTOR_AMPLITUD;
	
	for(i = 0; i < cant_muestras; i++)
		muestras[i] *= factor_amplitud;
		
	return OK;
}

status_t obtener_factor_amplitud(float *factor_amplitud, float *muestras, const uint cant_muestras)
{
	float max, min;
	
	if(factor_amplitud == NULL || muestras == NULL || cant_muestras < 0)
		return ERROR_PUNTERO_NULO;
		
	if(obtener_maximo(&max, muestras, cant_muestras) != OK)
		return ERROR_OBTENER_MAXIMO;
		
	if(obtener_minimo(&min, muestras, cant_muestras) != OK)
		return ERROR_OBTENER_MINIMO;
	
	if(max + min < 0)
	{
		*factor_amplitud = INT_LIMIT / abs(min);
		return OK;
	}
	if(max + min > 0)
	{
		*factor_amplitud = INT_LIMIT / abs(max);
		return OK;
	}
	return OK;
}

status_t obtener_maximo(float *max, const float *muestras, const uint cant_muestras)
{
	size_t i;
	
	if(max == NULL || muestras == NULL || cant_muestras < 0)
		return ERROR_PUNTERO_NULO;
	
	*max = muestras[0];
	
	for(i = 1; i < cant_muestras; i++)
		if(*max < muestras[i])
			*max = muestras[i];
			
	return OK;
}
	
status_t obtener_minimo(float *min, const float *muestras, const uint cant_muestras)
{
	size_t i;
	
	if(min == NULL || muestras == NULL || cant_muestras < 0)
		return ERROR_PUNTERO_NULO;
	
	*min = muestras[0];
	for(i = 1; i < cant_muestras; i++)
		if(*min > muestras[i])
			*min = muestras[i];
			
	return OK;
}
	
	
status_t liberar_memoria_muestras(float **muestras)
{
	if(muestras == NULL)
		return ERROR_PUNTERO_NULO;
		
	free(*muestras);
	*muestras = NULL;
	return OK;
}
