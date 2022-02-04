#ifndef ALGOGRAM_H
#define ALGOGRAM_H
#include "post.h"
#include "user.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct app app_t;

//Crea el controlador principal de la app.
//Pre: Recibe un arreglo con los nombres de los usuarios a crear.
//Post: El controlador fue creado.
app_t *app_crear(char **users, size_t n);


//Devuelve true en caso de que se loggee correctamente, false en caso contrario.
//Pre: El controlador debe estar creado.
//Post: Permite el acceso a operaciones exclusivas para usuarios.
bool login_user(app_t *app, char *username);

//Devuelve true si hay algun usuario loggeado, false en caso contrario.
//Pre: El controlador debe estar creado.
bool esta_loggeado(app_t *app);

//Devuelve true en caso de que se pueda hacer el logout, false en caso de que ya este en logout.
//Pre: El controlador debe estar creado.
//Post: Restringe el uso de operaciones que requieren estar loggeado.
bool logout_user(app_t *app);

//Devuelve true si se pudo postear con exito, false en caso contrario
//Pre: El controlador debe estar creado.
//Post: Agrega el post al feed de todos los usuarios salvo el que lo crea. 
bool publicar_post(app_t *app, char *msj);

//Agrega al usuario al listado de los que likearon el post.
//Pre: El controlador debe estar creado.
bool likear_post(app_t *app, char *post_id);

//Devuelve el proximo mensaje posteado en el feed del usuario actual o NULL en caso de que no haya proximo o no haya user loggeado.
//Pre: El controlador debe estar creado.
post_t *ver_proximo_post(app_t *app);

//Devuelve un arreglo con los usuarios que dieron like al post, NULL en caso de que sea un post invalido o algo falle.
//Pre: El controlador debe estar creado.
char **mostrar_likes(app_t *app,char *post_id, size_t *len);


//Devuelve el user que esta loggeado actualmente, NULL en caso de que no haya user loggeado.
//Pre: El controlador debe estar creado.
user_t *ver_user_actual(app_t *app);


//Destruye el controlador de la app.
//Pre: El controlador debe estar creado.
void app_destruir(app_t *app);



#endif //ALGOGRAM_H