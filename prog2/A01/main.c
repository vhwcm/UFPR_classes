#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h> 
#include <math.h>
#include "lz.h"
#include "lista.h"


int handle_p(const char *filename, FILE *archive, List *dir);

int handle_compress(const char *filename, FILE *archive, List *dir);

int ler_diretorio_arquivo(FILE *archive, List* directory);
// Function to handle the 'c' (compress and add) option


int main (int argc, char *argv[]) {
    // Use "ab+" for appending in binary mode, create if not exists
    // Or "rb+" if you need to read/write existing data before appending
    if (argc < 3){
        printf("Formato Incorreto!\nUtilize: ./vina -[arg] [arquive] [arquivo1...arquivon]\n");
        return -1;
    }
    FILE *archive = fopen("archive.vc", "rb+");
    if (!archive) {
        // Se falhar, talvez o arquivo não exista, tente criar com "wb+"
        archive = fopen("archive.vc", "wb+");
        if (!archive) {
            perror("Não foi possível abrir/criar archive.vc");
            return 1;
        }
    }
    
    struct stat info_arquivo; // Keep declaration if needed elsewhere
    char next_option;
    List *directory = (List*)malloc(sizeof(List)); // Initialize to NULL
    if (!directory)
        return 2; 

    int tam_directory = ler_diretorio_arquivo(archive, directory);


    // Process command line options

        switch (next_option)
        {
        case 'i':
        // Call the dedicated function for 'c'
            if(tam_directory){
                fseek(archive,-(tam_directory*sizeof(metadados) + sizeof(int)), SEEK_END);
                long lala = ftell(archive);
                printf("\nlugar no arquivo: %ld e directory->tamanho*sizeof(metadados) + sizeof(int) %ld\n",lala,tam_directory*sizeof(metadados) + sizeof(int));
            }

            if (handle_compress(optarg, archive, directory) != 0) {
                fprintf(stderr, "Falha ao processar a opção -c para %s\n", optarg);
                // Decide if the program should terminate on error
            }
            imprime_lista(directory);
            break;
        case 'p':
             // Call the dedicated function for 'p'
             if(tam_directory){
                 fseek(archive, -(tam_directory*TAM_METADADOS + sizeof(int)), SEEK_END);
                 long lala = ftell(archive);
                 printf("\nlugar no arquivo: %ld e tam_directory*TAM_METADADOS + sizeof(int) = %ld\n",lala,tam_directory*TAM_METADADOS + sizeof(int)); 
             }else{
                printf("\ndiretorio vaziu\n");
             }
             if (handle_p(optarg, archive, directory) != 0) {
                    
                }
                fprintf(stderr, "Falha ao processar a opção -p para %s\n", optarg);
                // Decide if the program should terminate on error
    
            imprime_lista(directory);
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

int handle_compress(const char *filename, FILE *archive, List * dir) {
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
    metadados *fmeta = create_metadados(filename);
    insere_lista(dir, fmeta);
    if (!fmeta) {
        fprintf(stderr, "Falha ao criar metadados para %s\n", filename);
        fclose(fp);
        return 1;
    }

    // Read the input file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp); // Use long for file size
    if (file_size < 0) {
        perror("Erro ao obter tamanho do arquivo");
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    unsigned int tam = (unsigned int)file_size;
    char *input_buffer = malloc(tam);
    if (input_buffer == NULL) {
        perror("Erro ao alocar buffer de entrada");
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    rewind(fp);
    if (fread(input_buffer, 1, tam, fp) != tam) {
        perror("Erro ao ler arquivo de entrada");
        free(input_buffer);
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    fclose(fp); // Close input file pointer

    // Allocate output buffer (potentially larger for incompressible data)
    // Consider LZ_CompressBound or a similar function if available
    char *output_buffer = malloc(trunc(1.004 * tam + 1)); // Simple heuristic, might not be enough
    if (output_buffer == NULL) {
        perror("Erro ao alocar buffer de saída");
        free(input_buffer);
        free_metadados(fmeta);
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
        free_metadados(fmeta);
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
    printf("  Tamanho Original: %u\n", fmeta->o_size);
    printf("  Posição no Arquivo: %u\n", fmeta->pos);
    printf("  Permissões: %o\n", fmeta->perm);


    // Clean up
    free(input_buffer);
    free(output_buffer);
    free_metadados(fmeta);

    return (written > 0) ? 0 : 1; // Return 0 on success
}

// Function to handle the 'p' option (placeholder/example)
int handle_p(const char *filename, FILE *archive, List *dir) {
    printf("-p foi incluido, seu valor associado e: %s\n", filename);
    FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
        perror("Erro ao tentar abrir o arquivo para opção -p");
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
    fclose(fp);

    // Example: Write the buffer content to the archive (like -c without compression)
     if (fwrite(buffer, 1, tam, archive) != tam) {
         perror("Erro ao escrever dados no arquivo (-p)");
     }

    unsigned int tamm = ftell(archive);
    printf("\npos %d tam bytes, apos escrever data\n", tamm);

     printf("Dados de %s (tamanho %u) processados pela opção -p.\n", filename, tam);
    
    metadados *fmeta = create_metadados(filename);
    if (!fmeta) {
        fprintf(stderr, "Falha ao criar metadados para %s\n", filename);
        return 1;
    }

    insere_lista(dir, fmeta);
    if(escreve_metadados_arquivo(archive, dir)){
        fprintf(stderr, "erro ao escrever metadados");
    }
    
    tamm = ftell(archive);
    printf("\nfinalizado com %d tam bytes\n", tamm);

    free(buffer);
    return 0; // Indicate success
}

int ler_diretorio_arquivo(FILE *archive, List* directory){
    fseek(archive, 0, SEEK_END);
    long archive_size = ftell(archive);
    int tam_directory = 0;
    if (archive_size >= sizeof(int)) { // se o arquivo não está vaziu
        // Corrected fseek and fread
        if (fseek(archive, -(sizeof(int)), SEEK_END) == 0) { // se conseguiu ler o tamnho de desição
            if (fread(&tam_directory, sizeof(int), 1, archive) == 1) { // se conseguiu ler e guardar na variavel tam_directory
                 printf("Tamanho do diretório lido: %d\n", tam_directory);
                     if (fseek(archive, -(tam_directory*TAM_METADADOS + sizeof(int)), SEEK_END) == 0) {
                         // This simple fread won't work for complex structs with pointers
                        fprintf(stderr, "\n-(tam_directory*TAM_METADADOS + sizeof(int)) == %ld\n", tam_directory*TAM_METADADOS  + sizeof(int));
                        le_metadados_arquivo(archive,directory,tam_directory); //magic
                        return tam_directory;
                     } else {
                         perror("Erro ao posicionar para leitura do diretório");
                     }
            } else {
                 perror("Erro ao ler tamanho do diretório do arquivo");
                 return -2;
            }
        } else {
             perror("Erro ao posicionar para leitura do tamanho do diretório");
             return - 1;
        }
    } else {
        inicializa_lista(directory);
        printf("Arquivo de arquivamento vazio ou muito pequeno para conter um diretório.\n");
        return 0;
    }
}