#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pila.h"

typedef struct nodo{
    struct nodo* izq;
    struct nodo* der;
    void* dato;
    char* clave;

}abb_nodo;

struct abb_iter {
	pila_t *pila;
};

struct abb{
    abb_nodo* raiz;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir;
    size_t cantidad;
};


abb_nodo* nodo_abb_crear(const char* clave, void* dato){
    abb_nodo* nodo = malloc(sizeof(abb_nodo));
    if(!nodo) return NULL;
    nodo->clave = strdup(clave);
    nodo->dato = dato;
    nodo->izq = NULL;
    nodo->der = NULL;
    return nodo;
}

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
    abb_t* arbol = malloc(sizeof(abb_t));
    if(!arbol) return NULL;
    arbol->cmp = cmp;
    arbol->destruir = destruir_dato;
    arbol->cantidad = 0;
    arbol->raiz = NULL;//Se inicializa en NULL ya que no se recibe una clave y dato inicial, se debe agregar con abb_guardar
    return arbol;
}

//Esta funcion busca el nodo que tenga la misma clave o devuelve null en caso de no estar
//Tambien devuelve por el puntero del parametro padre el padre del nodo
abb_nodo *_buscar_nodo(abb_nodo *nodo, abb_comparar_clave_t cmp,const char *clave, abb_nodo** padre){
    if(!nodo) return NULL;
    int comparacion = cmp(nodo->clave,clave);
    if(!comparacion) return nodo;
    *padre = nodo; 
    if(comparacion > 0) return _buscar_nodo(nodo->izq,cmp,clave, padre);
    return _buscar_nodo(nodo->der,cmp,clave, padre);
}

void *abb_obtener(const abb_t *arbol, const char *clave){
    abb_nodo* padre = NULL;
    abb_nodo *obtenido = _buscar_nodo(arbol->raiz,arbol->cmp,clave, &padre);
    if(!obtenido) return NULL; 
    return obtenido->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
    abb_nodo* padre = NULL;
    return _buscar_nodo(arbol->raiz,arbol->cmp,clave, &padre) ? true : false;
}

//Encuentra el nodo que sustituira al elemento eliminado y elimina el vinculo su padre.
abb_nodo *_buscar_sustituto(abb_nodo *nodo){
    if(!nodo->izq) return nodo;//Significa que es el ultimo nodo del lado izq del hijo derecho del nodo a reemplazar
    abb_nodo * sustituto = _buscar_sustituto(nodo->izq);
    if(nodo->izq == sustituto) nodo->izq = sustituto->der;//Se rompe la relacion del nodo sustituto con el arbol
    return sustituto;
}

void _borrado_2_hijos(abb_nodo *borrado){
    abb_nodo *sustituto = _buscar_sustituto(borrado->der);
    if(sustituto == borrado->der){
        borrado->der = sustituto->der;
    }
    //Se reemplazan los atributos del nodo que se quiere borrar por los del sustituto
    borrado->dato = sustituto->dato;
    free(borrado->clave);
    borrado->clave = sustituto->clave;
    free(sustituto);
}


//Reubica el vinculo de su padre con los vinculos de los hijos del elemento a eliminar
void _reubicar_hijos(abb_t *arbol, abb_nodo *padre, abb_nodo *borrado){
    if(arbol->destruir) arbol->destruir(borrado->dato);
    if(borrado->izq && borrado->der){
        _borrado_2_hijos(borrado);
        return;
    }
    if(!padre){
        abb_nodo *aux = arbol->raiz;
        arbol->raiz = borrado->izq ? borrado->izq : borrado->der; //En caso de que la raiz no tenga hijos queda NULL
        free(aux->clave);
        free(aux);
        return;
    }
    if(padre->izq == borrado){
        if(borrado->izq) padre->izq = borrado->izq;
        else if(borrado->der) padre->izq = borrado->der;
        else padre->izq = NULL;
    }
    else{
        if(borrado->izq) padre->der = borrado->izq;
        else if(borrado->der) padre->der = borrado->der;
        else padre->der = NULL;
    }
    free(borrado->clave);
    free(borrado);
}


void *abb_borrar(abb_t *arbol, const char *clave){
    abb_nodo *padre = NULL;
    abb_nodo * nodo_elim = _buscar_nodo(arbol->raiz, arbol->cmp, clave, &padre);
    if(!nodo_elim) return NULL;
    void *aux = nodo_elim->dato;
    _reubicar_hijos(arbol,padre,nodo_elim);//Libera el nodo o un sustituto y reubica el resto de nodos afectados por la eliminacion
    arbol->cantidad--;
    return aux;
}

bool abb_guardar(abb_t* arbol, const char* clave, void* dato) {
	abb_nodo* padre = NULL;
    int comparacion;
	abb_nodo* nodo =  _buscar_nodo(arbol->raiz, arbol->cmp, clave, &padre);
    if(nodo){
        if (arbol->destruir) arbol->destruir(nodo->dato);
		nodo->dato = dato;
    } else {
        abb_nodo* nodo_nuevo = nodo_abb_crear(clave, dato);
		if (!nodo_nuevo) return false;
		if (!arbol->raiz) arbol->raiz = nodo_nuevo;
		else {
			comparacion = arbol->cmp(clave, padre->clave);
			if (comparacion < 0) padre->izq = nodo_nuevo;
		    else padre->der = nodo_nuevo;
		}
		arbol->cantidad++;
    } 
	return true;	
}

size_t abb_cantidad(const abb_t *arbol){
    return arbol->cantidad;
}

void _abb_destruir(abb_destruir_dato_t destruir, abb_nodo* nodo) { //Destruyo el abb con recorrido postorder ya que necesito destruir la raiz a lo ultimo
	if (!nodo) return;
	_abb_destruir(destruir, nodo->izq);
	_abb_destruir(destruir, nodo->der);
	if (destruir) destruir(nodo->dato);
	free(nodo->clave);
	free(nodo);
}

void abb_destruir(abb_t* arbol) {
	_abb_destruir(arbol->destruir, arbol->raiz);
	free(arbol);
}

bool _abb_in_order(abb_nodo* actual, bool visitar(const char*, void*, void*), void* extra) {
	if (!actual) return true; 
    else if (!_abb_in_order(actual->izq, visitar, extra)) return false; 
	else if (!visitar(actual->clave, actual->dato, extra)) return false; 
    else if (!_abb_in_order(actual->der, visitar, extra)) return false; 
    return true;
}

void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra) {
	_abb_in_order(arbol->raiz, visitar, extra);
}


void _apilar_izq(abb_nodo *nodo,pila_t *pila){
    if(!nodo) return;
    pila_apilar(pila, (void*)nodo);
    _apilar_izq(nodo->izq,pila);
} 

abb_iter_t* abb_iter_in_crear(const abb_t* arbol) {
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (!iter) return NULL;
	pila_t *pila = pila_crear();
    if(!pila){
        free(iter);
        return NULL;
    }
    iter->pila = pila;
    _apilar_izq(arbol->raiz,iter->pila);

    return iter;
}


bool abb_iter_in_avanzar(abb_iter_t* iter) {
	if(abb_iter_in_al_final(iter)) return false;
    abb_nodo *nodo = pila_desapilar(iter->pila);
    if(nodo->der){
        pila_apilar(iter->pila, (void*)nodo->der);
        _apilar_izq(nodo->der->izq,iter->pila);
    }
    return true;
}


const char* abb_iter_in_ver_actual(const abb_iter_t* iter) {
    if(abb_iter_in_al_final(iter)) return NULL;
    abb_nodo *nodo = pila_ver_tope(iter->pila);

    return nodo->clave;
}


bool abb_iter_in_al_final(const abb_iter_t* iter) {
    return pila_esta_vacia(iter->pila);
}


void abb_iter_in_destruir(abb_iter_t* iter) {
    pila_destruir(iter->pila);
    free(iter);
}

