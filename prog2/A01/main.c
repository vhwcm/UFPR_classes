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

int inserir_p(const char *nome_arquivo, FILE *arquivo, List *diretorio);

int tratar_compressao(const char *nome_arquivo, FILE *arquivo, List *diretorio);

int ler_diretorio_arquivo(FILE *arquivo, List* diretorio);

int main (int argc, char *argv[]) {
    if (argc < 4){
        printf("Formato Incorreto!\nUtilize: ./vina -[arg] [arquivo] [arquivo1...arquivoN]\n");
        return -1;
    }
    int argumento_atual = 1;
    char* opcao = argv[argumento_atual];
    argumento_atual++;
    FILE *arquivo = NULL;
    
    if (strcmp(opcao, "-ip") == 0) {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo) {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo) {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
        struct stat info_arquivo; 
        char proxima_opcao;
        List *diretorio = (List*)malloc(sizeof(List)); 
        if (!diretorio)
            return 2; 
    
        int tam_diretorio = ler_diretorio_arquivo(arquivo, diretorio);

        argumento_atual++;
        if (tam_diretorio) {
            fseek(arquivo, -(tam_diretorio * TAM_METADADOS + sizeof(int)), SEEK_END);
            long posicao = ftell(arquivo);
            printf("\nPosição no arquivo: %ld e tam_diretorio * TAM_METADADOS + sizeof(int) = %ld\n", posicao, tam_diretorio * TAM_METADADOS + sizeof(int));
        } else {
            printf("\nDiretório vazio\n");
        }

        while (argumento_atual < argc) {
            if (inserir_p(argv[argumento_atual], arquivo, diretorio) != 0) {
                fprintf(stderr, "Falha ao processar a opção -p para %s\n", opcao);
            }
            imprime_lista(diretorio);
            argumento_atual++;
        }

        if (escreve_metadados_arquivo(arquivo, diretorio)) {
            fprintf(stderr, "Erro ao escrever metadados");
        }
        
    } else if (strcmp(opcao, "-ic") == 0) {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo) {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo) {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    } else if (strcmp(opcao, "-m") == 0) {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo) {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo) {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    } else if (strcmp(opcao, "-x") == 0) {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo) {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo) {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    } else if (strcmp(opcao, "-r") == 0) {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo) {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo) {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    } else {
        printf("Argumento incorreto!!\n");
        return -2;
    }

    printf("Finalizando...\n");
    
    fclose(arquivo);
    return 0;
}

int tratar_compressao(const char *nome_arquivo, FILE *arquivo, List *diretorio) {
    printf("-c foi incluído, seu valor associado é: %s\n", nome_arquivo);
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL) {
        perror("Erro ao tentar abrir o arquivo para compressão");
        fprintf(stderr, "Nome do arquivo: %s\n", nome_arquivo);
        return 1;
    }

    int fd = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd, &info_arquivo) == -1) {
        perror("Erro ao obter informações do arquivo");
        fclose(fp);
        return 1;
    }

    metadados *fmeta = criar_metadados(nome_arquivo);
    insere_lista(diretorio, fmeta);
    if (!fmeta) {
        fprintf(stderr, "Falha ao criar metadados para %s\n", nome_arquivo);
        fclose(fp);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0) {
        perror("Erro ao obter tamanho do arquivo");
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    unsigned int tam = (unsigned int)tamanho_arquivo;
    char *buffer_entrada = malloc(tam);
    if (buffer_entrada == NULL) {
        perror("Erro ao alocar buffer de entrada");
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    rewind(fp);
    if (fread(buffer_entrada, 1, tam, fp) != tam) {
        perror("Erro ao ler arquivo de entrada");
        free(buffer_entrada);
        free_metadados(fmeta);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    char *buffer_saida = malloc(trunc(1.004 * tam + 1));
    if (buffer_saida == NULL) {
        perror("Erro ao alocar buffer de saída");
        free(buffer_entrada);
        free_metadados(fmeta);
        return 1;
    }

    unsigned int c_tam = LZ_Compress(buffer_entrada, buffer_saida, tam);

    long posicao_atual = ftell(arquivo);
    if (posicao_atual == -1) {
        perror("Erro ao obter posição no arquivo de arquivamento");
        free(buffer_saida);
        free(buffer_entrada);
        free_metadados(fmeta);
        return 1;
    }
    fmeta->pos = (unsigned int)posicao_atual;

    size_t escrito = 0;
    if (c_tam > 0 && c_tam < tam) {
        printf("Escrevendo dados comprimidos (%u bytes)\n", c_tam);
        escrito = fwrite(buffer_saida, 1, c_tam, arquivo);
        if (escrito != c_tam) {
            perror("Erro ao escrever dados comprimidos no arquivo");
        }
    } else {
        printf("Escrevendo dados originais (%u bytes)\n", tam);
        escrito = fwrite(buffer_entrada, 1, tam, arquivo);
        if (escrito != tam) {
            perror("Erro ao escrever dados originais no arquivo");
        }
    }

    printf("Metadados para %s:\n", fmeta->nome);
    printf("  Tamanho Original: %u\n", fmeta->o_size);
    printf("  Posição no Arquivo: %u\n", fmeta->pos);
    printf("  Permissões: %o\n", fmeta->perm);

    free(buffer_entrada);
    free(buffer_saida);
    free_metadados(fmeta);

    return (escrito > 0) ? 0 : 1;
}

int inserir_p(const char *nome_arquivo, FILE *arquivo, List *diretorio) {
    printf("-ip foi incluído, seu valor associado é: %s\n", nome_arquivo);
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL) {
        perror("Erro ao tentar abrir o arquivo para opção -p");
        fprintf(stderr, "Nome do arquivo: %s\n", nome_arquivo);
        return 1;
    }
    int fd = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd, &info_arquivo) == -1) {
        perror("Erro ao obter informações do arquivo");
        fclose(fp);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0) {
        perror("Erro ao obter tamanho do arquivo (-p)");
        fclose(fp);
        return 1;
    }

    unsigned int tam = (unsigned int)tamanho_arquivo;
    char *buffer = malloc(tam);
    if (buffer == NULL) {
        perror("Erro ao alocar buffer (-p)");
        fclose(fp);
        return 1;
    }

    rewind(fp);
    if (fread(buffer, 1, tam, fp) != tam) {
        perror("Erro ao ler arquivo (-p)");
        free(buffer);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (fwrite(buffer, 1, tam, arquivo) != tam) {
        perror("Erro ao escrever dados no arquivo (-p)");
    }

    unsigned int posicao = ftell(arquivo);
    printf("\nPosição %d tam bytes, após escrever dados\n", posicao);

    printf("Dados de %s (tamanho %u) processados pela opção -p.\n", nome_arquivo, tam);
    
    metadados *fmeta = criar_metadados(nome_arquivo);
    if (!fmeta) {
        fprintf(stderr, "Falha ao criar metadados para %s\n", nome_arquivo);
        return 1;
    }

    insere_lista(diretorio, fmeta);
    
    posicao = ftell(arquivo);
    printf("\nFinalizado com %d tam bytes\n", posicao);

    free(buffer);
    return 0;
}

int ler_diretorio_arquivo(FILE *arquivo, List* diretorio) {
    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    int tam_diretorio = 0;
    long pos;
    if (tamanho_arquivo >= sizeof(int)) {
        if (fseek(arquivo, -(sizeof(int)), SEEK_END) == 0) {
            pos = ftell(arquivo);
            fprintf(stderr, "\npos apos ler o int: %ld\n", pos);
            if (fread(&tam_diretorio, sizeof(int), 1, arquivo) == 1) {
                printf("Tamanho do diretório lido: %d\n", tam_diretorio);
                if (fseek(arquivo, -(tam_diretorio * TAM_METADADOS + sizeof(int)), SEEK_END) == 0) {
                    pos = ftell(arquivo);
                    fprintf(stderr, "\npos: %ld \n-(tam_diretorio * TAM_METADADOS + sizeof(int)) == %ld\n", pos, tam_diretorio * TAM_METADADOS + sizeof(int));
                    le_metadados_arquivo(arquivo, diretorio, tam_diretorio);
                    return tam_diretorio;
                } else {
                    perror("Erro ao posicionar para leitura do diretório");
                }
            } else {
                perror("Erro ao ler tamanho do diretório do arquivo");
                return -2;
            }
        } else {
            perror("Erro ao posicionar para leitura do tamanho do diretório");
            return -1;
        }
    } else {
        inicializa_lista(diretorio);
        printf("Arquivo de arquivamento vazio ou muito pequeno para conter um diretório.\n");
        return 0;
    }
}
