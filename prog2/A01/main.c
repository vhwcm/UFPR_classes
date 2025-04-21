#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h> 
#include "lz.h"

typedef struct metadados {
    char *nome;
    unsigned int tam;
    unsigned int pos; //posição no arquivo
    time_t u_acesso;
    time_t u_mod;
    mode_t perm;
} metadados;

typedef struct directory {
    metadados *f_meta;
    unsigned int tam;
} directory;


int handle_p(const char *filename, FILE *archive, directory *dir);

int handle_compress(const char *filename, FILE *archive, directory *dir);

// Function to handle the 'c' (compress and add) option


int main (int argc, char *argv[]) {
    // Use "ab+" for appending in binary mode, create if not exists
    // Or "rb+" if you need to read/write existing data before appending
    FILE *archive = fopen("archive.vc", "ab+");
    if (!archive) {
        perror("Não foi possível abrir/criar archive.vc");
        return 1;
    }
    
    struct stat info_arquivo; // Keep declaration if needed elsewhere
    char next_option;
    directory *directory_dump = NULL; // Initialize to NULL
    int tam_directory = 0;
    
    // Attempt to read existing directory (basic example, needs robust error handling)
    fseek(archive, 0, SEEK_END);
    long archive_size = ftell(archive);
    if (archive_size >= sizeof(int)) {
        // Corrected fseek and fread
        if (fseek(archive, -(sizeof(int)), SEEK_END) == 0) {
            if (fread(&tam_directory, sizeof(int), 1, archive) == 1) {
                 printf("Tamanho do diretório lido: %d\n", tam_directory);
                 // Basic sanity check for directory size
                if (tam_directory > 0 && tam_directory < archive_size) {
                     // Position before the directory data
                     if (fseek(archive, -((long)sizeof(int) + tam_directory), SEEK_END) == 0) {
                        // This simple fread won't work for complex structs with pointers
                        directory_dump = malloc(tam_directory); 
                        if (!directory_dump)
                            return 2; 
                        fread(directory_dump, tam_directory, 1, archive); // tenho o diretorio em memoria
                        
                        printf("Placeholder: Lógica de leitura do diretório precisa ser implementada corretamente.\n");
                        // Reset file pointer to end for appending new data initially
                        fseek(archive, 0, SEEK_END);
                     } else {
                         perror("Erro ao posicionar para leitura do diretório");
                          tam_directory = 0; // Reset size if seek failed
                     }
                } else {
                    printf("Tamanho do diretório inválido encontrado: %d\n", tam_directory);
                    tam_directory = 0; // Treat as invalid
                }
            } else {
                 perror("Erro ao ler tamanho do diretório do arquivo");
                 tam_directory = 0; // Reset size if read failed
            }
        } else {
             perror("Erro ao posicionar para leitura do tamanho do diretório");
            }
    } else {
        printf("Arquivo de arquivamento vazio ou muito pequeno para conter um diretório.\n");
    }


    // Process command line options
    while ((next_option = getopt(argc, argv, "c:p:x:")) != -1) { // Added 'x:' for consistency
        switch (next_option)
        {
        case 'c':
        // Call the dedicated function for 'c'
            fseek(archive, -tam_directory, SEEK_END);
            if (handle_compress(optarg, archive) != 0) {
                fprintf(stderr, "Falha ao processar a opção -c para %s\n", optarg);
                // Decide if the program should terminate on error
            }
            fwrite(directory_dump, directory_dump->tam, 1, archive); // reescreve no final do arquivo o directory
            break;
        case 'p':
             // Call the dedicated function for 'p'
             fseek(archive, -tam_directory, SEEK_END);
             if (handle_p(optarg, archive) != 0) {
                fprintf(stderr, "Falha ao processar a opção -p para %s\n", optarg);
                // Decide if the program should terminate on error
            }
            fwrite(directory_dump, directory_dump->tam, 1, archive);
            break;
        case 'x': //extract
            printf("-x foi incluido, seu valor associado e: %s\n", optarg);
            // TODO: Implement extraction logic, possibly in its own function handle_extract()
            break;
            
            default:
            // getopt prints an error message for unknown options
            // You might want to print usage instructions here
            fprintf(stderr, "Uso: %s [-c arquivo] [-p arquivo] [-x arquivo]\n", argv[0]);
            fclose(archive);
            // Free directory_dump if allocated
            return 1; // Exit on invalid option
        }
    }
    
    printf("Finalizando...\n");
    
    fclose(archive); // Close the archive file
    return 0;
}

int handle_compress(const char *filename, FILE *archive, directory * dir) {
    printf("-c foi incluido, seu valor associado e: %s\n", filename);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Erro ao tentar abrir o arquivo para compressão");
        fprintf(stderr, "Nome do arquivo: %s\n", filename);
        return 1; // Indicate failure
    }

    int fd = fileno(fp);
    struct stat info_arquivo;
    // Corrected fstat call
    if (fstat(fd, &info_arquivo) == -1) {
            perror("Erro ao obter informações do arquivo");
            fclose(fp);
            return 1;
    }

    // Allocate and populate metadata (consider how this will be stored later)
    metadados *fmeta = malloc(sizeof(metadados));
    if (!fmeta) {
        perror("Erro ao alocar memória para metadados");
        fclose(fp);
        return 1;
    }
    // Allocate memory for the name and copy it
    fmeta->nome = strdup(filename); // Use strdup for safety
    if (!fmeta->nome) {
        perror("Erro ao duplicar nome do arquivo");
        free(fmeta);
        fclose(fp);
        return 1;
    }
    fmeta->perm = info_arquivo.st_mode;
    fmeta->tam = info_arquivo.st_size; // Original size
    fmeta->u_acesso = info_arquivo.st_atime;
    fmeta->u_mod = info_arquivo.st_mtime;
    // fmeta->pos needs to be determined based on archive structure

    // Read the input file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp); // Use long for file size
    if (file_size < 0) {
        perror("Erro ao obter tamanho do arquivo");
        free(fmeta->nome);
        free(fmeta);
        fclose(fp);
        return 1;
    }
    unsigned int tam = (unsigned int)file_size;
    char *input_buffer = malloc(tam);
    if (input_buffer == NULL) {
        perror("Erro ao alocar buffer de entrada");
        free(fmeta->nome);
        free(fmeta);
        fclose(fp);
        return 1;
    }
    rewind(fp);
    if (fread(input_buffer, 1, tam, fp) != tam) {
        perror("Erro ao ler arquivo de entrada");
        free(input_buffer);
        free(fmeta->nome);
        free(fmeta);
        fclose(fp);
        return 1;
    }
    fclose(fp); // Close input file pointer

    // Allocate output buffer (potentially larger for incompressible data)
    // Consider LZ_CompressBound or a similar function if available
    char *output_buffer = malloc(2 * tam); // Simple heuristic, might not be enough
    if (output_buffer == NULL) {
        perror("Erro ao alocar buffer de saída");
        free(input_buffer);
        free(fmeta->nome);
        free(fmeta);
        return 1;
    }

    // Compress data
    // Assuming LZ_Compress exists and is defined in lz.h
    unsigned int c_tam = LZ_Compress(input_buffer, output_buffer, tam);

    // Determine current position in archive for metadata (before writing data)
    long current_pos = ftell(archive);
        if (current_pos == -1) {
        perror("Erro ao obter posição no arquivo de arquivamento");
        free(output_buffer);
        free(input_buffer);
        free(fmeta->nome);
        free(fmeta);
        return 1;
    }
    fmeta->pos = (unsigned int)current_pos;


    // Write compressed or original data to archive
    size_t written = 0;
    if (c_tam > 0 && c_tam < tam) { // Check if compression was successful and smaller
        printf("Escrevendo dados comprimidos (%u bytes)\n", c_tam);
        written = fwrite(output_buffer, 1, c_tam, archive);
        if (written != c_tam) {
                perror("Erro ao escrever dados comprimidos no arquivo");
                // Handle partial write?
        }
    } else {
        printf("Escrevendo dados originais (%u bytes)\n", tam);
        written = fwrite(input_buffer, 1, tam, archive);
            if (written != tam) {
                perror("Erro ao escrever dados originais no arquivo");
                // Handle partial write?
            }
        // Update metadata size if original data was written
        // Or store a flag indicating compression status
    }

    // TODO: Store fmeta into the directory structure and write the directory later

    printf("Metadados para %s:\n", fmeta->nome);
    printf("  Tamanho Original: %u\n", fmeta->tam);
    printf("  Posição no Arquivo: %u\n", fmeta->pos);
    printf("  Permissões: %o\n", fmeta->perm);


    // Clean up
    free(input_buffer);
    free(output_buffer);
    free(fmeta->nome); // Free the duplicated name
    free(fmeta);       // Free the metadata struct itself

    return (written > 0) ? 0 : 1; // Return 0 on success
}

// Function to handle the 'p' option (placeholder/example)
int handle_p(const char *filename, FILE *archive, directory *dir) {
    printf("-p foi incluido, seu valor associado e: %s\n", filename);
    FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
        perror("Erro ao tentar abrir o arquivo para opção -p");
        fprintf(stderr, "Nome do arquivo: %s\n", filename);
        return 1; // Indicate failure
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    if (file_size < 0) {
        perror("Erro ao obter tamanho do arquivo (-p)");
        fclose(fp);
        return 1;
    }
    unsigned int tam = (unsigned int)file_size;
    char *buffer = malloc(tam);
        if (buffer == NULL) {
        perror("Erro ao alocar buffer (-p)");
        fclose(fp);
        return 1;
    }

    // The original code read the file into buffer but didn't use it.
    // This reads the file content. Decide what to do with it.
    rewind(fp);
    if (fread(buffer, 1, tam, fp) != tam) {
        perror("Erro ao ler arquivo (-p)");
        free(buffer);
        fclose(fp);
        return 1;
    }

    // Example: Write the buffer content to the archive (like -c without compression)
    // size_t written = fwrite(buffer, 1, tam, archive);
    // if (written != tam) {
    //     perror("Erro ao escrever dados no arquivo (-p)");
    // }
    // printf("Dados de %s (tamanho %u) processados pela opção -p.\n", filename, tam);

    printf("Opção -p processada para %s (tamanho %u). Buffer lido, mas não escrito no arquivo.\n", filename, tam);


    free(buffer);
    fclose(fp);
    return 0; // Indicate success
}

int add_metadado(directory *dir) {
    dir->f_meta = 
}
