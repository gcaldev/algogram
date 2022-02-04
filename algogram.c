#include "hash.h"
#include "abb.h"
#include "algogram.h"
#include <stdlib.h>
#define MAX_TAMANIO_ID 20

struct app{
    hash_t *users;
    hash_t *posts;
    user_t *act_user;
};

void _user_destruir(void *dato){
    user_t *user = dato;
    user_destruir(user);
}

void _post_destruir(void *dato){
    post_t *post = dato;
    post_destruir(post);
}

app_t *app_crear(char **users, size_t n){
    app_t *app = malloc(sizeof(app_t));
    if(!app) return NULL;
    app->users = hash_crear(_user_destruir);
    app->posts = hash_crear(_post_destruir);
    app->act_user = NULL;
    for(size_t i = 0; i < n; i++){
        user_t *user = crear_usuario(users[i], i);
        hash_guardar(app->users, users[i], user);
    }
    return app;
}

bool publicar_post(app_t *app, char *msj){
    if(!esta_loggeado(app)) return false;
    size_t post_id = hash_cantidad(app->posts);
    user_t *post_author = ver_user_actual(app);
    size_t autor_id = ver_user_id(post_author);
    post_t *post = post_crear(msj, post_id, ver_username(post_author), autor_id);
    hash_iter_t *iter = hash_iter_crear(app->users);
    while(!hash_iter_al_final(iter)){
        user_t *user = hash_obtener(app->users, hash_iter_ver_actual(iter));
        if(user != post_author) agregar_a_feed(user, post);
        hash_iter_avanzar(iter);
    }
    char post_id_str[MAX_TAMANIO_ID];
    sprintf(post_id_str,"%ld", post_id);
    hash_iter_destruir(iter);
    return hash_guardar(app->posts, post_id_str, post);
}

bool likear_post(app_t *app, char *post_id){
    if(!esta_loggeado(app)) return false;
    post_t * post = hash_obtener(app->posts, post_id);
    if(!post) return NULL;
    return likear_post_actual(ver_username(app->act_user), post);
}

bool login_user(app_t *app, char *username){
    if(esta_loggeado(app) || !hash_pertenece(app->users, username)) return false;
    app->act_user = hash_obtener(app->users, username);
    return true;
}

bool esta_loggeado(app_t *app){
    return app->act_user;
}

bool logout_user(app_t *app){
    if(!esta_loggeado(app)) return false;
    app->act_user = NULL;
    return true;
}

post_t *ver_proximo_post(app_t *app){
    if(!esta_loggeado(app)) return NULL;
    return ver_siguiente_post(app->act_user);
}

char **mostrar_likes(app_t *app,char *post_id, size_t *len){
    if(!hash_pertenece(app->posts,post_id)) return NULL;
    post_t *post = hash_obtener(app->posts, post_id);
    char **usuarios = post_mostrar_likes(post);
    *len = post_cantidad_likes(post);
    return usuarios;
}

user_t *ver_user_actual(app_t *app){
    return app->act_user;
}

void app_destruir(app_t *app){
    hash_destruir(app->users);
    hash_destruir(app->posts);
    free(app);
}