#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>  
#include <sys/types.h> 

#define TAM_MAX_FILENAME 1024

/* Define com o tamanho de todos os dados, tendo em vista que guardo os metadados um a um */
#define TAM_NOME_NO_DISCO TAM_MAX_FILENAME
#define TAM_UID_NO_DISCO sizeof(uid_t)
#define TAM_OSIZE_NO_DISCO sizeof(unsigned int)
#define TAM_CSIZE_NO_DISCO sizeof(unsigned int)
#define TAM_UMOD_NO_DISCO sizeof(time_t)
#define TAM_PERM_NO_DISCO sizeof(mode_t)
#define TAM_POS_NO_DISCO sizeof(unsigned int)
#define TAM_LOCAL_NO_DISCO sizeof(unsigned int)

/* o tamanho dos metadados no disco é a soma de cada elemento */
#define TAM_METADADOS_NO_DISCO (TAM_NOME_NO_DISCO + TAM_UID_NO_DISCO +    \
                                TAM_OSIZE_NO_DISCO + TAM_CSIZE_NO_DISCO + \
                                TAM_UMOD_NO_DISCO + TAM_PERM_NO_DISCO +   \
                                TAM_POS_NO_DISCO + TAM_LOCAL_NO_DISCO)


                                
typedef struct metadados
{
    char nome[TAM_MAX_FILENAME];
    uid_t uid;
    unsigned int o_size; 
    unsigned int c_size;
    time_t u_mod;       
    mode_t perm;         
    unsigned int pos;  
    unsigned int local;
} metadados;


/* Estrutura do Nó da Lista */
typedef struct No
{
    metadados *data;
    struct No *prox;
} No;

/* Estrutura da Lista de metadados */
typedef struct Lista
{
    No *primeiro;
    No *ultimo;
    size_t tamanho;
} Lista;

/* Funções para criar um metadado de um arquivo apartir de seu nome */
metadados *criar_metadados(const char *filename);

/* Funções para criar um metadado de um arquivo apartir de informações já obtidas */
metadados *dump_metadados(const char *filename, uid_t uid, unsigned int o_size, unsigned int c_size,
                          time_t u_mod, mode_t perm, unsigned int pos, unsigned int local);



/* Insere no final da lista */
int insere_lista(Lista *lista, metadados *data);

void inicializa_lista(Lista *lista);

/* Remove um nó pelo nome */ 
int remove_lista(Lista *lista, const char *nome);

/* Busca um nó pelo nome */
No *busca_lista(Lista *lista, const char *nome);

/* Libera toda a lista */
void libera_lista(Lista *lista);

/* Imprime a lista */
void imprime_lista(Lista *lista);

/* Imprime todos os metadados de uma struct metadados */
void imprime_metadados(metadados *meta);

/* Escreve os metadados no arquivo */
int escreve_metadados_arquivo(FILE *arquivo, Lista *lista);

/* da um damp dos metadados do arquivo na memória */
int le_metadados_arquivo(FILE *arquivo, Lista *lista, unsigned int tam);

#endif 