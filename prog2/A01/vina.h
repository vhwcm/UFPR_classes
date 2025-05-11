#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>  // For mode_t
#include <sys/types.h> // For time_t
// Define the metadados structure (assuming it's defined elsewhere or define it here)
// If defined elsewhere, ensure that header is included before this one.
// For self-containment, let's define it here based on the provided main.c

#define TAM_MAX_FILENAME 1024
#define TAM_NOME_NO_DISCO TAM_MAX_FILENAME
#define TAM_UID_NO_DISCO sizeof(uid_t)
#define TAM_OSIZE_NO_DISCO sizeof(unsigned int)
#define TAM_CSIZE_NO_DISCO sizeof(unsigned int)
#define TAM_UMOD_NO_DISCO sizeof(time_t)
#define TAM_PERM_NO_DISCO sizeof(mode_t)
#define TAM_POS_NO_DISCO sizeof(unsigned int)
#define TAM_LOCAL_NO_DISCO sizeof(unsigned int)

#define TAM_METADADOS_NO_DISCO (TAM_NOME_NO_DISCO + TAM_UID_NO_DISCO +    \
                                TAM_OSIZE_NO_DISCO + TAM_CSIZE_NO_DISCO + \
                                TAM_UMOD_NO_DISCO + TAM_PERM_NO_DISCO +   \
                                TAM_POS_NO_DISCO + TAM_LOCAL_NO_DISCO)

typedef struct metadados
{
    char nome[TAM_MAX_FILENAME + 1];
    uid_t uid;
    unsigned int o_size; 
    unsigned int c_size;
    time_t u_mod;       
    mode_t perm;         
    unsigned int pos;  
    unsigned int local;
} metadados;


typedef struct No
{
    metadados *data;
    struct No *prox;
} No;

typedef struct Lista
{
    No *primeiro;
    No *ultimo;
    size_t tamanho;
} Lista;

metadados *criar_metadados(const char *filename);
metadados *dump_metadados(const char *filename, uid_t uid, unsigned int o_size, unsigned int c_size,
                          time_t u_mod, mode_t perm, unsigned int pos, unsigned int local);


void free_metadados(metadados *meta);

// Insere no final da lista
int insere_lista(Lista *lista, metadados *data);

void inicializa_lista(Lista *lista);

// Remove um nó pelo nome
int remove_lista(Lista *lista, const char *nome);

// Busca um nó pelo nome
No *busca_lista(Lista *lista, const char *nome);

// Libera toda a lista
void libera_lista(Lista *lista);

// Imprime a lista
void imprime_lista(Lista *lista);

// Imprime todos os metadados de uma struct metadados
void imprime_metadados(metadados *meta);

int escreve_metadados_arquivo(FILE *arquivo, Lista *lista);

int le_metadados_arquivo(FILE *arquivo, Lista *lista, unsigned int tam);

#endif // LISTA_H