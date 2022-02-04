#include "post.h"
#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include <string.h>
#include <stdlib.h>

struct post{
    char *msj;
    abb_t *likes;
    size_t post_id;
    char *username;
    size_t autor_id;
};

post_t *post_crear(char *mensaje, size_t post_id, char *username, size_t autor_id){
    abb_t *likes = abb_crear(strcmp,NULL);
    if(!likes) return NULL;
    post_t *post = malloc(sizeof(post_t));
    if(!post){
        abb_destruir(likes);
        return NULL;
    }
    post->msj = strdup(mensaje);
    post->likes = likes;
    post->post_id = post_id;
    post->username = strdup(username);
    post->autor_id = autor_id;
    return post;
}

char **post_mostrar_likes(post_t *post){
    size_t cant_likes = post_cantidad_likes(post);
    if(!cant_likes) return NULL;
    char **arr_likes = malloc(cant_likes * sizeof(const char*));
    if(!arr_likes) return NULL;
    abb_iter_t *iter_likes = abb_iter_in_crear(post->likes);
    if(!iter_likes) return NULL;

    size_t i = 0;
    while (!abb_iter_in_al_final(iter_likes)){
        arr_likes[i] = strdup(abb_iter_in_ver_actual(iter_likes));
        i++;
        abb_iter_in_avanzar(iter_likes);
    }
    abb_iter_in_destruir(iter_likes);
    return arr_likes;
}

const char *mostrar_post(post_t *post){
    return post->msj;
}

bool likear_post_actual(char *username, post_t *post){
    return abb_guardar(post->likes,username,NULL);
}

size_t post_cantidad_likes(post_t *post){
    return abb_cantidad(post->likes);
}

char *ver_autor(post_t *post){
    return post->username;
}

size_t ver_autor_id(post_t *post){
    return post->autor_id;
}

size_t ver_post_id(post_t *post){
    return post->post_id;
}

void post_destruir(post_t *post){
    abb_destruir(post->likes);
    free(post->username);
    free(post->msj);
    free(post);
}