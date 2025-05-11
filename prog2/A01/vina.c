#include "vina.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strcmp and strdup
#include <time.h>   // For ctime

// Implementation of criar_metadados
metadados *criar_metadados(const char *filename)
{
    /* Aloca o metadado novo */
    metadados *meta = (metadados *)malloc(sizeof(metadados));
    if (!meta)
    {
        perror("Failed to allocate memory for metadados");
        return NULL;
    }
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("Error opening file");
        // Handle error, maybe return an error code or NULL
        return NULL; // Or appropriate error handling
    }

    int fd = fileno(fp); // Get the file descriptor from the FILE stream
    if (fd == -1)
    {
        perror("Error getting file descriptor");
        fclose(fp); // Close the file stream
        // Handle error
        return NULL;
    }

    struct stat file_info; // Declare the struct stat variable itself, not a pointer
                           // fstat needs the address of an existing struct to fill.

    // Pass the address of file_info to fstat
    if (fstat(fd, &file_info) == -1)
    {
        perror("Error getting file stats");
        fclose(fp); // Close the file stream
        // Handle error
        return NULL;
    }
    snprintf(meta->nome, TAM_MAX_FILENAME + 1, "%.*s", TAM_MAX_FILENAME, filename); // Now you can access the members of file_info:
    meta->nome[TAM_MAX_FILENAME] = '\0';
    printf("meta->nome = %s\n", meta->nome);
    meta->uid = file_info.st_uid;
    meta->o_size = file_info.st_size; // File size in bytes
    meta->c_size = meta->o_size;
    meta->u_mod = file_info.st_mtime; // Time of last modification
    meta->perm = file_info.st_mode & 0777;
    meta->pos = 0;
    meta->local = 0;
    // Remember to close the file stream when done
    fclose(fp);

    return meta;
}

metadados *dump_metadados(const char *filename, uid_t uid, unsigned int o_size, unsigned int c_size,
                          time_t u_mod, mode_t perm, unsigned int pos, unsigned int local)
{

    metadados *meta = (metadados *)malloc(sizeof(metadados));
    if (!meta)
    {
        perror("Failed to allocate memory for metadados in dump_metadados");
        return NULL;
    }

    // Copy filename, ensuring truncation and null termination
    snprintf(meta->nome, sizeof(meta->nome), "%.*s", TAM_MAX_FILENAME, filename);
    // snprintf handles null termination if space (sizeof(meta->nome)) allows.
    // "%.*s" with TAM_MAX_FILENAME ensures at most TAM_MAX_FILENAME chars are written from filename.

    meta->uid = uid;
    meta->o_size = o_size;
    meta->c_size = c_size;
    meta->u_mod = u_mod;
    meta->perm = perm & 0777; // Apply mask to ensure only permission bits are stored
    meta->pos = pos;
    meta->local = local;

    return meta;
}

// Implementation of free_metadados
void free_metadados(metadados *meta)
{
    if (meta)
    {
        free(meta);
    }
}

void inicializa_lista(Lista *lista)
{
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->tamanho = 0;
}

int insere_lista(Lista *lista, metadados *data)
{
    /* Aloca um novo no */
    No *novo = (No *)malloc(sizeof(No));
    if (!novo)
        return -1;
    novo->data = data;
    novo->prox = NULL;
    if (lista->ultimo)
    {
        lista->ultimo->prox = novo;
    }
    else
    {
        lista->primeiro = novo;
    }
    lista->ultimo = novo;
    lista->tamanho++;
    return 0;
}

int remove_lista(Lista *lista, const char *nome)
{
    No *atual = lista->primeiro, *anterior = NULL;
    while (atual)
    {
        if (atual->data && atual->data->nome && strcmp(atual->data->nome, nome) == 0)
        {
            if (anterior)
            {
                anterior->prox = atual->prox;
            }
            else
            {
                lista->primeiro = atual->prox;
            }
            if (atual == lista->ultimo)
            {
                lista->ultimo = anterior;
            }
            free_metadados(atual->data);
            free(atual);
            lista->tamanho--;
            return 0;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return -1;
}

No *busca_lista(Lista *lista, const char *nome)
{
    No *atual = lista->primeiro;
    while (atual)
    {
        if (atual->data && atual->data->nome && strcmp(atual->data->nome, nome) == 0)
        {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void libera_lista(Lista *lista)
{
    No *atual = lista->primeiro;
    while (atual)
    {
        No *prox = atual->prox;
        free_metadados(atual->data);
        free(atual);
        atual = prox;
    }
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->tamanho = 0;
}

void imprime_lista(Lista *lista)
{
    No *atual = lista->primeiro;
    printf("--- Lista de Metadados ---\n");
    while (atual)
    {
        if (atual->data)
        {
            metadados *m = atual->data;
            printf("  Nome: %s\n", m->nome);
            printf("  UID: %u\n", (unsigned int)m->uid);
            printf("  Tamanho Original: %u bytes\n", m->o_size);
            printf("  Tamanho Comprimido: %u bytes\n", m->c_size);
            printf("  Posição: %u\n", m->pos);
            printf("  Local: %u\n", m->local);
            char time_buf[64];
            struct tm *tm_info = localtime(&m->u_mod);
            if (tm_info && strftime(time_buf, sizeof(time_buf), "%c", tm_info))
                printf("  Última Modificação: %s\n", time_buf);
            else
                printf("  Última Modificação: (invalid time)\n");
            printf("  Permissões: %o\n", m->perm);
            printf("  ----\n");
        }
        atual = atual->prox;
    }
    printf("--- Fim da Lista ---\n");
}

int escreve_metadados_arquivo(FILE *arquivo, Lista *dir)
{
    if (arquivo == NULL || dir == NULL)
    {
        fprintf(stderr, "Erro: Ponteiro de arquivo ou diretório nulo em escreve_metadados_arquivo.\n");
        return -1;
    }

    No *atual = dir->primeiro;
    int i;

    // Write each metadata entry
    for (i = 0; i < dir->tamanho; i++)
    {
        if (atual == NULL || atual->data == NULL)
        {
            fprintf(stderr, "Erro: Nó ou dados do nó nulos ao escrever metadados (entrada %d).\n", i);
            return -1; // Error in Lista structure
        }
        metadados *meta = atual->data;

        // Write nome (fixed size TAM_MAX_FILENAME + 1, but only TAM_MAX_FILENAME useful chars + null)
        if (fwrite(meta->nome, sizeof(char), TAM_MAX_FILENAME, arquivo) != (TAM_MAX_FILENAME))
        {
            perror("Erro ao escrever nome do metadado");
            return -1;
        }
        if (fwrite(&meta->uid, sizeof(uid_t), 1, arquivo) != 1)
        {
            perror("Erro ao escrever UID do metadado");
            return -1;
        }
        if (fwrite(&meta->o_size, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao escrever tamanho original do metadado");
            return -1;
        }
        if (fwrite(&meta->c_size, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao escrever tamanho comprimido do metadado");
            return -1;
        }
        if (fwrite(&meta->u_mod, sizeof(time_t), 1, arquivo) != 1)
        {
            perror("Erro ao escrever tempo de modificação do metadado");
            return -1;
        }
        if (fwrite(&meta->perm, sizeof(mode_t), 1, arquivo) != 1)
        {
            perror("Erro ao escrever permissões do metadado");
            return -1;
        }
        if (fwrite(&meta->pos, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao escrever posição do metadado");
            return -1;
        }
        if (fwrite(&meta->local, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao escrever 'local' do metadado");
            return -1;
        }

        atual = atual->prox;
    }

    int num_entradas = dir->tamanho;
    if (fwrite(&num_entradas, sizeof(int), 1, arquivo) != 1)
    {
        perror("Erro ao escrever o tamanho do diretório (número de entradas)");
        return -1;
    }

    printf("Diretório escrito com %d entradas.\n", num_entradas);
    return 0; // Success
}

int le_metadados_arquivo(FILE *arquivo, Lista *dir, unsigned int tam_dir)
{
    if (arquivo == NULL || dir == NULL)
    {
        fprintf(stderr, "Erro: Ponteiro de arquivo ou diretório nulo em le_metadados_arquivo.\n");
        return -1;
    }
    if (tam_dir <= 0)
    {
        printf("Nenhum metadado para ler (tam_dir = %d).\n", tam_dir);
        return 0; // Not an error, just nothing to read
    }

    int i;
    for (i = 0; i < tam_dir; i++)
    {
        metadados *meta_lido = (metadados *)malloc(sizeof(metadados));
        if (!meta_lido)
        {
            perror("Falha ao alocar memória para metadado lido");
            return -1; // Allocation error
        }

        // Read nome (TAM_MAX_FILENAME + 1 bytes)
        if (fread(meta_lido->nome, sizeof(char), TAM_MAX_FILENAME, arquivo) != (TAM_MAX_FILENAME))
        {
            perror("Erro ao ler nome do metadado");
            free(meta_lido);
            return -1;
        }
        // Ensure null termination, though fwrite wrote TAM_MAX_FILENAME + 1 bytes,
        // the last one should be the null char if snprintf worked correctly before writing.
        // However, if the file was corrupted or written differently, this is a safeguard.
        meta_lido->nome[TAM_MAX_FILENAME] = '\0';

        if (fread(&meta_lido->uid, sizeof(uid_t), 1, arquivo) != 1)
        {
            perror("Erro ao ler UID do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->o_size, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao ler tamanho original do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->c_size, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao ler tamanho comprimido do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->u_mod, sizeof(time_t), 1, arquivo) != 1)
        {
            perror("Erro ao ler tempo de modificação do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->perm, sizeof(mode_t), 1, arquivo) != 1)
        {
            perror("Erro ao ler permissões do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->pos, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao ler posição do metadado");
            free(meta_lido);
            return -1;
        }
        if (fread(&meta_lido->local, sizeof(unsigned int), 1, arquivo) != 1)
        {
            perror("Erro ao ler 'local' do metadado");
            free(meta_lido);
            return -1;
        }
        // u_acesso was removed from the struct, so no fread for it.

        insere_lista(dir, meta_lido); // Add the read metadata to the Lista
    }
    printf("%d entradas de metadados lidas e adicionadas à lista.\n", tam_dir);
    return 0; // Success
}
