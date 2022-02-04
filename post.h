#ifndef POST_H
#define POST_H
#include <stdbool.h>
#include <stddef.h>

// El struct se llamara post.
struct post;

typedef struct post post_t;

// Crea el post
post_t *post_crear(char *mensaje, size_t post_id, char *username, size_t autor_id);

/* Muestra los usuarios que dieron like al post
 * Pre: El post fue creado
 * Post: Devuelve una lista de los que likearon el post o NULL en caso de que no hayan likes o falle algo
 */
char **post_mostrar_likes(post_t *post);

/* Devuelve la cantidad de likes que tiene el post.
 * Pre: El post fue creado
*/
size_t post_cantidad_likes(post_t *post);

/* Devuelve el username del autor del post
 * Pre: El post fue creado
*/
char *ver_autor(post_t *post);

/* Devuelve el id del autor del post
 * Pre: El post fue creado
*/
size_t ver_autor_id(post_t *post);

/* Devuelve el id del post.
 * Pre: El post fue creado
*/
size_t ver_post_id(post_t *post);

/* Muestra el mensaje del post
 * Pre: El post fue creado
 * Post: Devuelve un string con el mensaje del post
 */
const char *mostrar_post(post_t *post);

/* El usuario likea un post.
* Pre: El username y el post deben estar creados.
* Post: El usuario es parte de los que likearon el post.
*/
bool likear_post_actual(char *username, post_t *post);

//Destruye el post.
void post_destruir(post_t *post);

#endif  // POST_H
