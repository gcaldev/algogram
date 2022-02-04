#include "pila.h"
#include<stdbool.h>
#include <stdlib.h>

#define MIN_SIZE_PILA 10
#define MUL_REDIMENSION_PILA 4

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...

pila_t *pila_crear(void){
    pila_t *pila = malloc(sizeof(pila_t));
    if (pila == NULL){
        return NULL;
    }
    pila->datos = malloc(MIN_SIZE_PILA * sizeof(void*));
    if (pila->datos == NULL){
        free(pila);
        return NULL;
    }
    pila->cantidad = 0;
    pila->capacidad = MIN_SIZE_PILA;
    return pila;

}

bool pila_redimensionar(pila_t *pila, size_t tam_nuevo) {
    void *datos_nuevo = realloc(pila->datos, tam_nuevo * sizeof(void*));

    if (tam_nuevo > 0 && datos_nuevo == NULL) {
        return false;
    }
    
    pila->datos = datos_nuevo;
    pila->capacidad= tam_nuevo;
    return true;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}


bool pila_esta_vacia(const pila_t *pila){
    return pila->cantidad == 0;
}


bool pila_apilar(pila_t *pila, void *valor){
    if (pila->cantidad == pila->capacidad && !pila_redimensionar(pila, pila->capacidad * 2)) {
        return false;
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad += 1;
    return true;
}

void *pila_ver_tope(const pila_t *pila){
    if(pila_esta_vacia(pila)){
        return NULL;
    }
    return pila->datos[(pila->cantidad) - 1];
}

void *pila_desapilar(pila_t *pila){
    if(pila_esta_vacia(pila))
        return NULL;
    if (pila->cantidad * MUL_REDIMENSION_PILA <= pila->capacidad){
        pila_redimensionar(pila, pila->capacidad / 2);
    }
    void *dato = pila->datos[(pila->cantidad) - 1];
    pila->cantidad -= 1;
    return dato;
}


