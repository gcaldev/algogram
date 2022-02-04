#define _POSIX_C_SOURCE 200809L
#include"heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#define TAM 20
#define AUMENTO_TAM 4
#define REDUCIR_TAM 2
#define FACTOR_CARGA 8



void swap(void **x, void **y) {
    void *aux = *x;
    *x = *y;
    *y = aux;
}

void downheap(void **datos, size_t cant, cmp_func_t cmp, size_t i){
    size_t izq = 2 * i + 1;
    size_t der = 2 * i + 2;
    size_t mayor;
    if(izq < cant && cmp(datos[izq] , datos[i]) > 0) mayor = izq;
    else mayor = i;
    if(der < cant && cmp(datos[der] , datos[mayor]) > 0) mayor = der;
    if(mayor != i){
        swap(&datos[i], &datos[mayor]);
        downheap(datos, cant, cmp, mayor);
    }
}

void max_heap(void **datos, size_t cant, cmp_func_t cmp){
    int padre = (int)(cant - 1) / 2;
    for(int i = padre; i >= 0; i--){
        downheap(datos,cant, cmp ,i);
    }
}

struct heap {
    void **datos;
    size_t tam;
    size_t cant;
    cmp_func_t cmp;
};

//Es responsabilidad del que usa la funcion verificar que el tamaño no sea menor que la cantidad ocupada.
bool redimension(heap_t *heap, size_t tam_nuevo){
    void **datos_nuevo = realloc(heap->datos, tam_nuevo * sizeof(void*));
    if(!datos_nuevo) return false;
    heap->datos = datos_nuevo;
    heap->tam = tam_nuevo;
    return true;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    max_heap(elementos, cant, cmp);
    for (size_t i = cant - 1; i > 0; i--){
        swap(&elementos[i], &elementos[0]);
        downheap(elementos, i, cmp, 0);
    }
}

heap_t *heap_crear(cmp_func_t cmp){
    heap_t *heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;
    void **datos = malloc(TAM*sizeof(void*));
    if(!datos) return NULL;
    heap->datos = datos;
    heap->tam = TAM;
    heap->cant = 0;
    heap->cmp = cmp;

    return heap;
}



heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    heap_t *heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;
    heap->datos = malloc(sizeof(void*) * n);
    if(!heap->datos) return NULL;
    memcpy(heap->datos, arreglo, n*sizeof(void *));
    heap->cmp = cmp;
    heap->tam = heap->cant = n;
    max_heap(heap->datos, heap_cantidad(heap), heap->cmp);
    return heap;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){
    if(destruir_elemento){
        for(size_t i = 0; i < heap->cant; i++) destruir_elemento(heap->datos[i]);
    } 
    free(heap->datos);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap){
    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){
    return !heap->cant; 
}

void upheap(void **datos, size_t cant, cmp_func_t cmp, size_t i){
    if(!i) return;
    size_t padre = (i-1)/2;
    if(cmp(datos[padre],datos[i]) < 0){
        swap(&datos[padre],&datos[i]);
        upheap(datos, cant, cmp, padre);
    }
}

bool heap_encolar(heap_t *heap, void *elem){
    if(heap->cant == heap->tam && !redimension(heap,heap->tam*AUMENTO_TAM)) return false;

    heap->datos[heap->cant] = elem;
    heap->cant++;
    upheap(heap->datos,heap->cant,heap->cmp,heap->cant-1);
    return true;
}

void *heap_ver_max(const heap_t *heap){
    if(heap_esta_vacio(heap)) return NULL;
    return heap->datos[0];
}

void *heap_desencolar(heap_t *heap){
    if(heap_esta_vacio(heap)) return NULL;
    void * dato = heap->datos[0];
    heap->datos[0] = heap->datos[heap->cant - 1];
    heap->cant--;
    downheap(heap->datos, heap->cant, heap->cmp, 0);
    if((heap->cant * FACTOR_CARGA < heap->tam) && heap->tam/REDUCIR_TAM >= TAM) redimension(heap, heap->tam / REDUCIR_TAM);// El tamaño obtenido debe ser mayor o igual que el tamaño inicial.
    return dato;
}