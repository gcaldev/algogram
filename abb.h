#ifndef ABB
#define ABB


#include <stdio.h>
#include <stdbool.h>


typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);




//Recibe las funcion de comparacion y una funcion de destruccion de dato o NULL.
//Post: El arbol fue creado.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

//Se guarda el dato relacionandolo a la clave recibida, devuelve true o false en caso de que algo falle.
//Pre: El arbol debe estar creado.
//Post: El par fue agregado.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

//Se elimina el par del arbol, devolviendo el dato o NULL en caso de no estar.
//Pre: El arbol debe estar creado.
//Post: El par fue removido.
void *abb_borrar(abb_t *arbol, const char *clave);

//Se obtiene el par, devuelve el dato o NULL en caso de no aparecer.
//Pre: El arbol debe estar creado.
//Post: Mantiene el estado del arbol.
void *abb_obtener(const abb_t *arbol, const char *clave);

//Devuelve true si se encuentra la clave en el arbol o false si ocurre lo contrario.
//Pre: El arbol debe estar creado.
//Post: Mantiene el estado del arbol.
bool abb_pertenece(const abb_t *arbol, const char *clave);

//Devuelve la cantidad de pares contenidos en el arbol
//Pre: El arbol debe estar creado.
//Post: Mantiene el estado del arbol.
size_t abb_cantidad(const abb_t *arbol);

//Pre: El arbol debe estar creado.
//Post: El arbol fue destruido junto a los pares que se encontraban dentro.
void abb_destruir(abb_t *arbol);

//Aplica la funcion visitar a cada uno de los pares del arbol.
//Pre: El arbol debe estar creado.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);




typedef struct abb_iter abb_iter_t;


//Devuelve un iterador externo para recorrer el arbol.
//Pre: El arbol debe estar creado.
//Post: El iterador fue creado.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

//Devuelve true y avanza al siguiente par o false en caso de no poder avanzar mas.
//Pre: El iterador debe estar creado.
//Post: Mantiene el estado del arbol.
bool abb_iter_in_avanzar(abb_iter_t *iter);


// Devuelve la clave actual, esa clave no se puede modificar ni liberar.
//Pre: El iterador debe estar creado.
//Post: Mantiene el estado del iterador y del arbol.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);


// Devuelve true en caso de que se haya terminado con la iteracion, false en caso contrario.
//Pre: El iterador debe estar creado.
//Post: Mantiene el estado del iterador y del arbol.
bool abb_iter_in_al_final(const abb_iter_t *iter);


//Libera la memoria del iterador.
//Pre: El iterador debe estar creado.
//Post: El iterador fue destruido.
void abb_iter_in_destruir(abb_iter_t* iter);

void imprimir_arbol(abb_t *arbol);

#endif  // ABB
