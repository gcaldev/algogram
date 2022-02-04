#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#define CAPACIDAD_INICIAL 17

size_t hash_function(unsigned char* str) {
    // Funcion de hash djb2
    // Fuente: http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return (size_t)hash;
}





enum status_t{VACIO,OCUPADO,BORRADO};
//Cada campo representa una celda del arreglo.
typedef struct campo{
	char *key;
	void *value;
    enum status_t status;
}campo_t;

struct hash_iter {
	const hash_t* hash;
	size_t act;
	size_t recorridos;
};

struct hash{
    campo_t* tabla;
	size_t ocupados;
	size_t borrados;
	size_t capacidad;
    hash_destruir_dato_t destruir_dato;
};


//Se inicializan todos los campos de la tabla y la devuelve o devuelve NULL.
campo_t* _init_campos(const size_t capacidad){
    campo_t* tabla = malloc(capacidad*(sizeof(campo_t))); 
    for (size_t i = 0; i < capacidad; i++){
        campo_t campo_actual;
        campo_actual.key = NULL;
        campo_actual.value = NULL;
        campo_actual.status = VACIO;
        tabla[i] = campo_actual;
    }
    return tabla;
}

bool es_primo(size_t n){
    for (size_t i = 2; i < n; i++){
        if(n % i == 0) return false;
    }
    return true;
}

//Busca un primo mayor o menor dependiendo si recibe 1 o -1.
size_t buscar_primo(size_t n, const int operacion){
    do {
        n+=operacion;
    } 
    while(!es_primo(n));
    return n;
}

//Busca la posicion que le corresponderia a la clave y la devuelve.
size_t hash_obtener_pos_key(const hash_t* hash, const char* key) {
	size_t pos_key = hash_function((unsigned char*)key) % hash->capacidad;
    const char* actual_key;
	while (!(hash->tabla[pos_key].status == VACIO)) {
		actual_key = hash->tabla[pos_key].key;
        if (hash->tabla[pos_key].status == OCUPADO && strcmp(actual_key, key) == 0) break; // si es la clave la devuelve
		if (pos_key == hash->capacidad-1) pos_key = 0;  // si llega al final vuelve a empezar
        else pos_key++;
	}
	return pos_key; // si termina es por que no lo encontro, por eso devuelve el borrado, asumimos que el borrado era el valor que se buscaba pero fue elminado del hash
}

//Busca el siguiente elemento ocupado en la tabla.
size_t hash_buscar_ocupado(const hash_t* hash, size_t pos_inicial) {
	size_t contador = pos_inicial;
	while(contador < hash->capacidad && hash->tabla[contador].status != OCUPADO) {
		contador++;
	}
	return contador;
}

//Devuelve la nueva capacidad en caso de necesitar una redimension o 0 por el contrario.
size_t _debo_redimensionar(hash_t * hash){
    if ((double)(hash->ocupados+hash->borrados)/(double)hash->capacidad > 0.7){
        return buscar_primo(hash->capacidad*2,1); 
    }
    if ((double)hash->ocupados/(double)hash->capacidad<0.08){
        size_t cap_reducida = buscar_primo(hash->capacidad/8,-1);
        if (cap_reducida >= CAPACIDAD_INICIAL && cap_reducida>= hash->ocupados){
            return cap_reducida;
        }
    }
    return 0;
}

//Redimensiona la tabla en caso de ser necesario, devuelve false en caso de que falle el pedido de memoria de la nueva tabla.
//Pre: El hash debe estar creado.
//Post: Se redimensiona la capacidad del hash, descartando los elementos borrados.
bool _redimensionar_hash(hash_t * hash){
    size_t nueva_capacidad = _debo_redimensionar(hash);
    if(!nueva_capacidad) return true;
    campo_t* nueva_tabla = _init_campos(nueva_capacidad);
    if(!nueva_tabla) return false;
    campo_t* anterior_tabla = hash->tabla;
    size_t anterior_capacidad = hash->capacidad;
    hash->tabla = nueva_tabla;
    hash->capacidad = nueva_capacidad;
    hash->borrados = 0;
    for (size_t i = 0; i < anterior_capacidad; i++){
        if(anterior_tabla[i].status == OCUPADO){
            size_t pos_key = hash_obtener_pos_key(hash, anterior_tabla[i].key);//Volvi a guardarlo manualmente para no relacionar con guardar_hash.
            hash->tabla[pos_key].key = anterior_tabla[i].key;
            hash->tabla[pos_key].value = anterior_tabla[i].value;
            hash->tabla[pos_key].status = OCUPADO;
        }
    }
    free(anterior_tabla);
    return true;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t *hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;
    hash->destruir_dato = destruir_dato;
    hash->ocupados = 0;
    hash->borrados = 0;
    hash->capacidad = CAPACIDAD_INICIAL; //Debe ser un numero impar
    hash->tabla = _init_campos(hash->capacidad);
    return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if(!_redimensionar_hash(hash)) return false;
    size_t pos_elem = hash_obtener_pos_key(hash,clave);
    if(hash->tabla[pos_elem].status == OCUPADO && hash->destruir_dato){
        hash->destruir_dato(hash->tabla[pos_elem].value);//Libera memoria del dato almacenado previamente
    }
    if(hash->tabla[pos_elem].status == VACIO){
        hash->ocupados++;
        hash->tabla[pos_elem].key = strdup(clave);
    }
    
    hash->tabla[pos_elem].value = dato;
    hash->tabla[pos_elem].status = OCUPADO;
    return true;
}

void* hash_borrar(hash_t* hash, const char* clave) {
	size_t pos_key = hash_obtener_pos_key(hash, clave);
	if (hash->tabla[pos_key].status == VACIO) return NULL;
	void* dato = hash->tabla[pos_key].value;
	hash->tabla[pos_key].status = BORRADO;
	hash->ocupados--;
	hash->borrados++;
    free(hash->tabla[pos_key].key);
    _redimensionar_hash(hash);
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash)) return NULL;
    size_t pos_key = hash_obtener_pos_key(hash, clave);
    return hash->tabla[pos_key].status == VACIO ? NULL : hash->tabla[pos_key].value;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash)) return false;
    size_t pos_key = hash_obtener_pos_key(hash, clave);
    return hash->tabla[pos_key].status == OCUPADO ? true : false;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->ocupados;
}

void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->capacidad; i++){
        if(hash->tabla[i].status == OCUPADO){
            if(hash->destruir_dato) hash->destruir_dato(hash->tabla[i].value);
            free(hash->tabla[i].key);//Liberamos la copia de la clave que habiamos hecho.
        }
    }
    free(hash->tabla);
    free(hash);
}


hash_iter_t* hash_iter_crear(const hash_t* hash) {
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter)return NULL;
	iter->hash = hash;
	if (hash->ocupados == 0) iter->act = hash->ocupados;
	else iter->act = hash_buscar_ocupado(iter->hash, 0);
	iter->recorridos = 0;
	return iter;
}


void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}

bool hash_iter_avanzar(hash_iter_t* iter) {
	if (hash_iter_al_final(iter)) return false;
	iter->act = hash_buscar_ocupado(iter->hash, iter->act + 1);
	iter->recorridos++;
	return true;
}


const char* hash_iter_ver_actual(const hash_iter_t* iter) {
	if (hash_iter_al_final(iter)) return NULL;
	return iter->hash->tabla[iter->act].key;
}


bool hash_iter_al_final(const hash_iter_t* iter) {
	return iter->recorridos >= iter->hash->ocupados;
}
