#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap.h"
#include "user.h"
#include <math.h>

struct user{
    size_t user_id;
    char *username;
    heap_t *feed;
};

struct user_post {
   size_t feed_id;
   post_t *post;
};

int calcular_prioridad(const void* a, const void* b){
    const user_post_t *user_1 = a;
    const user_post_t *user_2 = b;
    // Casteamos primero a entero y despues a double para evitar errores con el flag -Wbad-function-cast.
    int autor_id= (int)ver_autor_id(user_1->post);
    int feed_id = (int)user_1->feed_id;
    double valor_1 = pow(((double)feed_id - (double)autor_id), 2);
    autor_id= (int)ver_autor_id(user_2->post);
    double valor_2 = pow(((double)feed_id - (double)autor_id), 2);
    if(valor_1 == valor_2){
        size_t post_1 = ver_post_id(user_1->post);
        size_t post_2 = ver_post_id(user_2->post);
        return post_1 < post_2 ? 1 : -1;
    }
    return valor_1 < valor_2 ? 1 : -1;
}

user_t *crear_usuario(char *username,size_t user_id){
    user_t *user = malloc(sizeof(user_t));
    if(!user) return NULL;
    user->user_id = user_id;
    user->feed = heap_crear(calcular_prioridad);
    user->username = strdup(username);
    return user;
}

void agregar_a_feed(user_t *user, post_t *post){
    user_post_t *user_feed = malloc(sizeof(user_post_t));
    user_feed->feed_id = user->user_id;
    user_feed->post = post;
    heap_encolar(user->feed, user_feed);
}

char *ver_username(user_t *user){
    return user->username;
}


size_t ver_user_id(user_t *user){
    return user->user_id;
}

post_t *ver_siguiente_post(user_t *user){
    user_post_t *user_feed = heap_desencolar(user->feed);
    if(!user_feed) return NULL;
    post_t *post = user_feed->post;
    free(user_feed);
    return post;
}

void user_destruir(user_t *user){
    heap_destruir(user->feed, free);
    free(user->username);
    free(user);
}
