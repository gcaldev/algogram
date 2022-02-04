#ifndef USER_H
#define USER_H
#include "post.h"

struct user;

typedef struct user_post user_post_t;

typedef struct user user_t;


// Crea el usuario
user_t *crear_usuario(char *username,size_t user_id);

//Pre: El usuario y el post deben estar creados.
//Post: Se agrega el nuevo post al feed del usuario.
void agregar_a_feed(user_t *user ,post_t *post);


// Devuelve el username del usuario
//Pre: El usuario debe estar creado
char *ver_username(user_t *user);

// Devuelve el id del usuario.
//Pre: El usuario debe estar creado
size_t ver_user_id(user_t *user);


//Devuelve el siguiente post y lo remueve del feed.
//Pre: El usuario debe estar creado.
post_t *ver_siguiente_post(user_t *user);


//Destruye el user
void user_destruir(user_t *user);
#endif //USER_H