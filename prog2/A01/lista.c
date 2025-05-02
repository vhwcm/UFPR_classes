#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strcmp and strdup
#include <time.h>   // For ctime

// Implementation of criar_metadados
metadados* criar_metadados(const char *filename) {
    metadados *meta = (metadados*)malloc(sizeof(metadados));
    if (!meta) {
        perror("Failed to allocate memory for metadados");
        return NULL;
    }
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        // Handle error, maybe return an error code or NULL
        return NULL; // Or appropriate error handling
    }

    int fd = fileno(fp); // Get the file descriptor from the FILE stream
    if (fd == -1) {
        perror("Error getting file descriptor");
        fclose(fp); // Close the file stream
        // Handle error
        return NULL;
    }

    struct stat file_info; // Declare the struct stat variable itself, not a pointer
                           // fstat needs the address of an existing struct to fill.

    // Pass the address of file_info to fstat
    if (fstat(fd, &file_info) == -1) {
        perror("Error getting file stats");
        fclose(fp); // Close the file stream
        // Handle error
        return NULL;
    }
    snprintf(meta->nome, TAM_MAX_FILENAME + 1, "%.*s", TAM_MAX_FILENAME, filename);    // Now you can access the members of file_info:
    meta->nome[20] = '\0';
    printf("meta->nome = %s\n", meta->nome);
    meta->o_size = file_info.st_size;       // File size in bytes
    meta->c_size = meta->o_size;
    meta->u_acesso = file_info.st_atime;    // Time of last access
    meta->u_mod = file_info.st_mtime;       // Time of last modification
    meta->perm = file_info.st_mode & 0777;
    meta->pos = 0;
    // Remember to close the file stream when done
    fclose(fp);

    return meta;
}

metadados* dump_metadados(const char *filename, unsigned int o_size, unsigned int c_size, unsigned int pos,  time_t u_acesso, time_t u_mod, mode_t perm) {
    metadados *meta = (metadados*)malloc(sizeof(metadados));
    if (!meta) {
        perror("Failed to allocate memory for metadados");
        return NULL;
    }
    strcpy(meta->nome, filename);
    meta->o_size = o_size;       // File size in bytes
    meta->c_size = c_size;
    meta->pos = pos;
    meta->u_acesso = u_acesso;    // Time of last access
    meta->u_mod = u_mod;       // Time of last modification
    meta->perm = perm & 0777;

    return meta;
}

// Implementation of free_metadados
void free_metadados(metadados *meta) {
    if (meta) {
        free(meta->nome); 
        free(meta);
    }
}


void inicializa_lista(List *lista) {
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->tamanho = 0;
}

int insere_lista(List *lista, metadados *data) {
    ListNode *novo = (ListNode*)malloc(sizeof(ListNode));
    if (!novo) return -1;
    novo->data = data;
    novo->next = NULL;
    if (lista->ultimo) {
        lista->ultimo->next = novo;
    } else {
        lista->primeiro = novo;
    }
    lista->ultimo = novo;
    lista->tamanho++;
    return 0;
}

int remove_lista(List *lista, const char *nome) {
    ListNode *atual = lista->primeiro, *anterior = NULL;
    while (atual) {
        if (atual->data && atual->data->nome && strcmp(atual->data->nome, nome) == 0) {
            if (anterior) {
                anterior->next = atual->next;
            } else {
                lista->primeiro = atual->next;
            }
            if (atual == lista->ultimo) {
                lista->ultimo = anterior;
            }
            free_metadados(atual->data);
            free(atual);
            lista->tamanho--;
            return 0;
        }
        anterior = atual;
        atual = atual->next;
    }
    return -1;
}

ListNode* busca_lista(List *lista, const char *nome) {
    ListNode *atual = lista->primeiro;
    while (atual) {
        if (atual->data && atual->data->nome && strcmp(atual->data->nome, nome) == 0) {
            return atual;
        }
        atual = atual->next;
    }
    return NULL;
}

void libera_lista(List *lista) {
    ListNode *atual = lista->primeiro;
    while (atual) {
        ListNode *prox = atual->next;
        free_metadados(atual->data);
        free(atual);
        atual = prox;
    }
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->tamanho = 0;
}

void imprime_lista(List *lista) {
    ListNode *atual = lista->primeiro;
    printf("--- Lista de Metadados ---\n");
    while (atual) {
        if (atual->data) {
            printf("  Nome: %s\n", atual->data->nome ? atual->data->nome : "(null)");
            printf("  Tamanho: %u bytes\n", atual->data->o_size);
            printf("  Posição: %u\n", atual->data->pos);
            char time_buf[30];
            struct tm *tm_info;
            tm_info = localtime(&atual->data->u_acesso);
            if (tm_info && strftime(time_buf, sizeof(time_buf), "%c", tm_info))
                printf("  Último Acesso: %s\n", time_buf);
            else
                printf("  Último Acesso: (invalid time)\n");
            tm_info = localtime(&atual->data->u_mod);
            if (tm_info && strftime(time_buf, sizeof(time_buf), "%c", tm_info))
                printf("  Última Modificação: %s\n", time_buf);
            else
                printf("  Última Modificação: (invalid time)\n");
            printf("  Permissões: %o\n", atual->data->perm);
            printf("  ----\n");
        }
        atual = atual->next;
    }
    printf("--- Fim da Lista ---\n");
}

int escreve_metadados_arquivo(FILE *arquivo, List *lista) {
    if (!arquivo || !lista) return -1;

    ListNode *atual = lista->primeiro;
    unsigned int tamm ;
    while (atual) {
        if (atual->data) {
            metadados *meta = atual->data;

            if (fwrite(meta->nome, sizeof(char), TAM_MAX_FILENAME, arquivo) != TAM_MAX_FILENAME) return -1;  
             tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever nome\n", tamm);
            if (fwrite(&meta->o_size, sizeof(unsigned int), 1, arquivo) != 1) return -1;
              tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever o_size\n", tamm);
            if (fwrite(&meta->c_size, sizeof(unsigned int), 1, arquivo) != 1) return -1;
            tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever u_size\n", tamm);
            if (fwrite(&meta->pos, sizeof(unsigned int), 1, arquivo) != 1) return -1;
              tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever pos\n", tamm);
            if (fwrite(&meta->u_acesso, sizeof(time_t), 1, arquivo) != 1) return -1;
              tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever u_acess\n", tamm);
            if (fwrite(&meta->u_mod, sizeof(time_t), 1, arquivo) != 1) return -1;
              tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever u_mod\n", tamm);
            if (fwrite(&meta->perm, sizeof(mode_t), 1, arquivo) != 1) return -1;
              tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever perm\n", tamm);
        }
        atual = atual->next;
    }
    fwrite(&lista->tamanho ,sizeof(int),1,arquivo);
    tamm = ftell(arquivo);
            printf("\npos %d tam bytes, apos escrever tamanho\n", tamm);    
    return 0; // Sucesso
}

int le_metadados_arquivo(FILE *arquivo, List *lista, unsigned int tam) {
    if (!arquivo || !lista) return -1;
    unsigned int metadado_id = 0;
    int posic = ftell(arquivo);
    printf("\nposic: %d\n", posic);
    while (metadado_id < tam) {        // Tenta ler o tamanho do nome
        char nome[21];
        if (fread(nome, sizeof(char), TAM_MAX_FILENAME, arquivo) != TAM_METADADOS) {
            nome[20] = '\0';
        } else {
            return -1;
        }

        unsigned int o_size, c_size, pos;
        time_t u_acesso, u_mod;
        mode_t perm;

        // Lê os outros campos
        fread(&o_size, sizeof(unsigned int), 1, arquivo);
        fread(&c_size, sizeof(unsigned int), 1, arquivo);
        fread(&pos, sizeof(unsigned int), 1, arquivo);
        fread(&u_acesso, sizeof(time_t), 1, arquivo);
        fread(&u_mod, sizeof(time_t), 1, arquivo);
        fread(&perm, sizeof(mode_t), 1, arquivo);

        // Cria o metadados (criar_metadados faz strdup do nome)
        metadados *novo_meta = dump_metadados(nome, o_size, c_size, pos, u_acesso, u_mod, perm);

        if (!novo_meta) return -1; // Erro na criação do metadados

        // Insere na lista
        if (insere_lista(lista, novo_meta) != 0) {
            free_metadados(novo_meta); // Libera metadados se a inserção falhar
            return -1; // Erro na inserção
        }
        metadado_id++;
    }

    return 0; // Sucesso
}

