#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h> // For mode_t
#include <sys/types.h> // For time_t
// Define the metadados structure (assuming it's defined elsewhere or define it here)
// If defined elsewhere, ensure that header is included before this one.
// For self-containment, let's define it here based on the provided main.c

#define TAM_MAX_FILENAME 20
#define TAM_METADADOS 52


typedef struct metadados {
    char nome[TAM_MAX_FILENAME + 1];
    unsigned int o_size; // Original size
    unsigned int c_size; // compressed size
    unsigned int pos; // Position in the archive file
    time_t u_acesso;  // Last access time
    time_t u_mod;     // Last modification time
    mode_t perm;      // File permissions
} metadados;

// Define the structure for a node in the linked list
typedef struct ListNode {
    metadados *data;
    struct ListNode *next;
} ListNode;

// Nova estrutura para a lista
typedef struct List {
    ListNode *primeiro;
    ListNode *ultimo;
    size_t tamanho;
} List;

metadados* create_metadados(const char *filename);
metadados* dump_metadados(const char *filename, unsigned int o_size, unsigned int c_size, unsigned int pos,  time_t u_acesso, time_t u_mod, mode_t perm);

/**
 * @brief Frees the memory allocated for a metadados struct, including the name.
 * @param meta Pointer to the metadados struct to free.
 */
void free_metadados(metadados *meta);

// Insere no final da lista
int insere_lista(List *lista, metadados *data);

void inicializa_lista(List *lista);

// Remove um nó pelo nome
int remove_lista(List *lista, const char *nome);

// Busca um nó pelo nome
ListNode* busca_lista(List *lista, const char *nome);

// Libera toda a lista
void libera_lista(List *lista);

// Imprime a lista
void imprime_lista(List *lista);

/**
 * @brief Escreve os metadados de todos os nós da lista em um arquivo.
 * Grava os metadados de forma consecutiva no arquivo fornecido.
 * O formato de gravação para cada metadados é:
 *   - tamanho da string nome (size_t)
 *   - caracteres da string nome (char*)
 *   - campo tam (unsigned int)
 *   - campo pos (unsigned int)
 *   - campo u_acesso (time_t)
 *   - campo u_mod (time_t)
 *   - campo perm (mode_t)
 * @param arquivo Ponteiro para o arquivo aberto em modo de escrita binária ('wb').
 * @param lista Ponteiro para a lista a ser gravada.
 * @return 0 em caso de sucesso, -1 em caso de erro de escrita.
 */
int escreve_metadados_arquivo(FILE *arquivo, List *lista);

/**
 * @brief Lê metadados de um arquivo e os insere em uma lista.
 * Lê os metadados do arquivo no formato definido por escreve_metadados_arquivo
 * e os insere na lista fornecida (que deve ser inicializada previamente).
 * @param arquivo Ponteiro para o arquivo aberto em modo de leitura binária ('rb').
 * @param lista Ponteiro para a lista onde os metadados serão inseridos.
 * @return 0 se a leitura for bem-sucedida (mesmo que o arquivo esteja vazio),
 *         -1 em caso de erro de leitura ou alocação de memória.
 */
int le_metadados_arquivo(FILE *arquivo, List *lista, unsigned int tam);

#endif // LISTA_H