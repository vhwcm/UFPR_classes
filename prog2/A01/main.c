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

int inserir_compressao(const char *nome_arquivo, FILE *arquivo, List *diretorio);

int ler_diretorio_arquivo(FILE *arquivo, List *diretorio);

int main(int argc, char *argv[])
{
    printf("\ntamanho do TAM_METADADOS_NO_DISCO:%ld\n", TAM_METADADOS_NO_DISCO);
    if (argc < 4)
    {
        printf("Formato Incorreto!\nUtilize: ./vina -[arg] [arquivo] [arquivo1...arquivoN]\n");
        return -1;
    }
    int argumento_atual = 1;
    char *opcao = argv[argumento_atual];
    argumento_atual++;
    FILE *arquivo = NULL;

    if (strcmp(opcao, "-ip") == 0 || strcmp(opcao, "-ic") == 0)
    {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo)
        {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo)
            {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
        struct stat info_arquivo;
        char proxima_opcao;
        List *diretorio = (List *)malloc(sizeof(List));
        if (!diretorio)
            return 2;

        int tam_diretorio = ler_diretorio_arquivo(arquivo, diretorio);

        argumento_atual++;
        if (tam_diretorio)
        {
            fseek(arquivo, -(tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int)), SEEK_END);
            long posicao = ftell(arquivo);
            printf("\nPosição no arquivo: %ld e tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int) = %ld\n", posicao, tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int));
        }
        else
        {
            printf("\nDiretório vazio\n");
        }

        while (argumento_atual < argc)
        {
            if (strcmp(opcao, "-ip") == 0)
            {
                if (inserir_p(argv[argumento_atual], arquivo, diretorio) != 0)
                    fprintf(stderr, "Falha ao processar a opção -p para %s\n", opcao);
            }
            else
            {
                if (inserir_compressao(argv[argumento_atual], arquivo, diretorio) != 0)
                    fprintf(stderr, "Falha ao processar a opção -p para %s\n", opcao);
            }
            imprime_lista(diretorio);
            argumento_atual++;
        }

        if (escreve_metadados_arquivo(arquivo, diretorio))
        {
            fprintf(stderr, "Erro ao escrever metadados");
        }
    }
    else if (strcmp(opcao, "-m") == 0)
    {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo)
        {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo)
            {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    }
    else if (strcmp(opcao, "-x") == 0)
    {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo)
        {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo)
            {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    }
    else if (strcmp(opcao, "-r") == 0)
    {
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo)
        {
            arquivo = fopen("arquivo.vc", "wb+");
            if (!arquivo)
            {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
    }
    else
    {
        printf("Argumento incorreto!!\n");
        return -2;
    }

    printf("Finalizando...\n");

    fclose(arquivo);
    return 0;
}

int inserir_compressao(const char *nome_arquivo, FILE *arquivo, List *diretorio)
{
    printf("-ic foi incluído, seu valor associado é: %s\n", nome_arquivo);
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        perror("Erro ao tentar abrir o arquivo para compressão");
        fprintf(stderr, "Nome do arquivo: %s\n", nome_arquivo);
        return 1;
    }

    // Obter informações do arquivo de entrada
    int fd_in = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd_in, &info_arquivo) == -1)
    {
        perror("Erro ao obter informações do arquivo de entrada");
        fclose(fp);
        return 1;
    }

    // Ler arquivo de entrada para buffer_entrada
    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0)
    {
        perror("Erro ao obter tamanho do arquivo de entrada");
        fclose(fp);
        return 1;
    }
    unsigned int tam_original = (unsigned int)tamanho_arquivo;
    if (tam_original == 0)
    {
        printf("Arquivo de entrada %s está vazio. Nada a fazer.\n", nome_arquivo);
        fclose(fp);
        return 0; // Não é um erro, mas nada foi adicionado
    }

    char *buffer_entrada = malloc(tam_original);
    if (buffer_entrada == NULL)
    {
        perror("Erro ao alocar buffer de entrada");
        fclose(fp);
        return 1;
    }
    rewind(fp);
    if (fread(buffer_entrada, 1, tam_original, fp) != tam_original)
    {
        perror("Erro ao ler arquivo de entrada");
        free(buffer_entrada);
        fclose(fp);
        return 1;
    }
    fclose(fp); // Fecha arquivo de entrada

    // Alocar buffer de saída para compressão (ligeiramente maior)
    // Usar unsigned int para o tamanho do buffer para evitar problemas com trunc
    unsigned int tam_buffer_saida = (unsigned int)(1.004 * tam_original + 1);
    char *buffer_saida = malloc(tam_buffer_saida);
    if (buffer_saida == NULL)
    {
        perror("Erro ao alocar buffer de saída");
        free(buffer_entrada);
        return 1;
    }

    // Comprimir
    printf("Comprimindo %s (%u bytes)...\n", nome_arquivo, tam_original);
    int tam_comprimido = LZ_Compress(buffer_entrada, buffer_saida, tam_original);
    printf("Tamanho comprimido: %d bytes\n", tam_comprimido);

    // Determinar qual buffer e tamanho usar
    char *buffer_para_escrever = NULL;
    unsigned int tamanho_para_escrever = 0;
    unsigned int c_size_meta = 0; // Tamanho a ser armazenado nos metadados

    if (tam_comprimido > 0 && tam_comprimido < tam_original)
    {
        printf("Usando dados comprimidos.\n");
        buffer_para_escrever = buffer_saida;
        tamanho_para_escrever = (unsigned int)tam_comprimido;
        c_size_meta = (unsigned int)tam_comprimido;
    }
    else
    {
        printf("Usando dados originais (compressão não vantajosa ou falhou).\n");
        buffer_para_escrever = buffer_entrada;
        tamanho_para_escrever = tam_original;
        c_size_meta = tam_original; // Armazena tamanho original se não comprimiu bem
    }

    // Obter posição atual no arquivo de arquivamento ANTES de escrever
    long data_start_pos = ftell(arquivo);
    if (data_start_pos == -1)
    {
        perror("Erro ao obter posição no arquivo de arquivamento (-ic)");
        free(buffer_entrada);
        free(buffer_saida);
        return 1;
    }

    // Escrever dados (comprimidos ou originais) no arquivo de arquivamento
    if (fwrite(buffer_para_escrever, 1, tamanho_para_escrever, arquivo) != tamanho_para_escrever)
    {
        perror("Erro ao escrever dados no arquivo de arquivamento (-ic)");
        free(buffer_entrada);
        free(buffer_saida);
        return 1;
    }
    printf("Dados escritos no arquivamento (%u bytes) na posição %ld.\n", tamanho_para_escrever, data_start_pos);

    // Criar e preencher metadados
    metadados *fmeta = criar_metadados(nome_arquivo); // Assumindo que criar_metadados pega info da struct stat
    if (!fmeta)
    {
        fprintf(stderr, "Falha ao criar metadados para %s\n", nome_arquivo);
        // Os dados já foram escritos, o arquivo está parcialmente corrompido.
        // Idealmente, deveria haver uma forma de reverter ou marcar como inválido.
        free(buffer_entrada);
        free(buffer_saida);
        return 1;
    }
    fmeta->pos = (unsigned int)data_start_pos; // Posição onde os dados começam
    fmeta->c_size = c_size_meta;               // Tamanho dos dados escritos no arquivo
    // fmeta->o_size já deve ter sido preenchido por criar_metadados a partir de info_arquivo.st_size

    // Inserir metadados na lista em memória
    insere_lista(diretorio, fmeta); // fmeta agora pertence à lista

    // Liberar buffers
    free(buffer_entrada);
    free(buffer_saida);

    return 0; // Sucesso
}

int inserir_p(const char *nome_arquivo, FILE *arquivo, List *diretorio)
{
    printf("-ip foi incluído, seu valor associado é: %s\n", nome_arquivo);
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        perror("Erro ao tentar abrir o arquivo para opção -p");
        fprintf(stderr, "Nome do arquivo: %s\n", nome_arquivo);
        return 1;
    }
    int fd = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd, &info_arquivo) == -1)
    {
        perror("Erro ao obter informações do arquivo");
        fclose(fp);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0)
    {
        perror("Erro ao obter tamanho do arquivo (-p)");
        fclose(fp);
        return 1;
    }

    unsigned int tam = (unsigned int)tamanho_arquivo;
    char *buffer = malloc(tam);
    if (buffer == NULL)
    {
        perror("Erro ao alocar buffer (-p)");
        fclose(fp);
        return 1;
    }

    rewind(fp);
    if (fread(buffer, 1, tam, fp) != tam)
    {
        perror("Erro ao ler arquivo (-p)");
        free(buffer);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (fwrite(buffer, 1, tam, arquivo) != tam)
    {
        perror("Erro ao escrever dados no arquivo (-p)");
    }

    unsigned int posicao = ftell(arquivo);
    printf("\nPosição %d tam bytes, após escrever dados\n", posicao);

    printf("Dados de %s (tamanho %u) processados pela opção -p.\n", nome_arquivo, tam);

    metadados *fmeta = criar_metadados(nome_arquivo);
    if (!fmeta)
    {
        fprintf(stderr, "Falha ao criar metadados para %s\n", nome_arquivo);
        return 1;
    }

    insere_lista(diretorio, fmeta);

    posicao = ftell(arquivo);
    printf("\nFinalizado com %d tam bytes\n", posicao);

    free(buffer);
    return 0;
}

int ler_diretorio_arquivo(FILE *arquivo, List *diretorio)
{
    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    int tam_diretorio = 0;
    long pos;
    if (tamanho_arquivo >= sizeof(int))
    {
        if (fseek(arquivo, -(sizeof(int)), SEEK_END) == 0)
        {
            pos = ftell(arquivo);
            fprintf(stderr, "\npos apos ler o int: %ld\n", pos);
            if (fread(&tam_diretorio, sizeof(int), 1, arquivo) == 1)
            {
                printf("Tamanho do diretório lido: %d\n", tam_diretorio);
                if (fseek(arquivo, -(tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int)), SEEK_END) == 0)
                {
                    pos = ftell(arquivo);
                    fprintf(stderr, "\npos: %ld \n-(tam_diretorio * TAM_METADADOS + sizeof(int)) == %ld\n", pos, tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int));
                    le_metadados_arquivo(arquivo, diretorio, tam_diretorio);
                    return tam_diretorio;
                }
                else
                {
                    perror("Erro ao posicionar para leitura do diretório");
                }
            }
            else
            {
                perror("Erro ao ler tamanho do diretório do arquivo");
                return -2;
            }
        }
        else
        {
            perror("Erro ao posicionar para leitura do tamanho do diretório");
            return -1;
        }
    }
    else
    {
        inicializa_lista(diretorio);
        printf("Arquivo de arquivamento vazio ou muito pequeno para conter um diretório.\n");
        return 0;
    }
}
