#define _POSIX_C_SOURCE 200809L
#include "algogram.h"
#include "post.h"
#include "user.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CANTIDAD_INICIAL 20

// Borra los fin de linea de la cadena ingresada
void borrar_barra_linea(char *cadena){
    size_t len = strlen(cadena);
    if(cadena[len - 1] == '\n') cadena[len - 1] = '\0';
}

// Carga la lista de usuarios ingresadas dentro de un archivo de texto.
char **load_users(const char *filename, size_t *len){
    FILE *file_p = fopen(filename,"r");
    if(!file_p) return NULL;
    size_t n = CANTIDAD_INICIAL;
    char **users_arr = malloc(n * sizeof(char *));
    char *linea = NULL;
    size_t tam;
    size_t i = 0;
    char **aux;
// Vamos agrandando el tamaño a medida que iteramos ya que en un principio no sabemos que largo tendra.
    while (getline(&linea,&tam,file_p) != EOF){
        if(i == n){
            n*=2;
            aux = realloc(users_arr,n*sizeof(char *));
            if(aux) users_arr = aux;
        }
        borrar_barra_linea(linea);
        users_arr[i] = strdup(linea);
        i++;
    }
    aux = realloc(users_arr,i*sizeof(char *));// Redimensionamos al tamaño mas adecuado para el arreglo obtenido.
    if(aux) users_arr = aux;
    *len = i;
    fclose(file_p);
    free(linea);
    return users_arr;
}

//Logea a un usuario que esta incluido en el archivo de texto ingresado
void login_output(app_t *app){
    if(esta_loggeado(app)){
        printf("Error: Ya habia un usuario loggeado\n");
        return;
    }
    char *input_user = NULL;
    size_t tam;
    size_t unused __attribute__((unused));
    unused = getline(&input_user, &tam, stdin);
    borrar_barra_linea(input_user);
    bool login_exitoso = login_user(app, input_user);
    login_exitoso ? printf("Hola %s\n",input_user) : printf("Error: usuario no existente\n");
    free(input_user);

}

//Publica los post
void publicar_output(app_t *app){
    char *msj = NULL;
    size_t tam;
    size_t unused __attribute__((unused));
    unused = getline(&msj, &tam, stdin);
    borrar_barra_linea(msj);
    bool post_correcto = publicar_post(app,msj);
    post_correcto ? printf("Post publicado\n") : printf("Error: no habia usuario loggeado\n");
    free(msj);
}

//Muestra el siguiente post al usuario actual
void post_siguiente_output(app_t *app){
    post_t *post = ver_proximo_post(app);
    if(!post){
        printf("Usuario no loggeado o no hay mas posts para ver\n");
        return;
    }
    char *username = ver_autor(post);
    size_t post_id = ver_post_id(post);
    const char *msj = mostrar_post(post);

    printf("Post ID %ld\n",post_id);
    printf("%s dijo: %s\n", username, msj);
    printf("Likes: %ld\n", post_cantidad_likes(post));
    
}

//Likea un post
void likear_post_output(app_t *app){
    char *post_id = NULL;
    size_t tam;
    size_t unused __attribute__((unused));
    unused = getline(&post_id, &tam, stdin);
    borrar_barra_linea(post_id);
    likear_post(app, post_id) ? printf("Post likeado\n") : printf("Error: Usuario no loggeado o Post inexistente\n");
    free(post_id);
}

//Muestra los likes y quienes likearon un post
void mostrar_likes_output(app_t *app){
    char *post_id = NULL;
    size_t tam;
    size_t unused __attribute__((unused));
    unused = getline(&post_id, &tam, stdin);
    borrar_barra_linea(post_id);
    size_t len;
    char **users = mostrar_likes(app, post_id, &len);
    free(post_id);
    if(!users || !len){
        printf("Error: Post inexistente o sin likes\n");
        return;
    }
    printf("El post tiene %ld likes:\n",len);
    for(size_t i = 0; i < len; i++){
        printf("\t%s\n",users[i]);
        free(users[i]);
    }
    free(users);    
}


int main(int argc,const char* argv[]){
    if(argc != 2) return -1;
    size_t len;
    char **users = load_users(argv[1],&len);
    app_t *app = app_crear(users,len);
    char *operation = NULL;
    size_t tam;
    while (getline(&operation,&tam,stdin) != EOF){
        if(!strcmp(operation,"login\n")) login_output(app);
        else if(!strcmp(operation,"logout\n")) logout_user(app) ? printf("Adios\n") : printf("Error: no habia usuario loggeado\n");
        else if(!strcmp(operation,"publicar\n")) publicar_output(app);
        else if(!strcmp(operation,"ver_siguiente_feed\n")) post_siguiente_output(app);
        else if(!strcmp(operation,"likear_post\n")) likear_post_output(app);
        else if(!strcmp(operation,"mostrar_likes\n")) mostrar_likes_output(app);
    }

    for (size_t i = 0; i < len; i++){
        free(users[i]);
    }

    free(users);
    free(operation);
    app_destruir(app);
    return 0;
}