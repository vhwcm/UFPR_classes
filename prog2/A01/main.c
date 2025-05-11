#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include "lz.h"
#include "vina.h"

int inserir_p(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int inserir_c(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int ler_diretorio_arquivo(FILE *arquivo, Lista *diretorio);

int extrair(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int remover(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);


int main(int argc, char *argv[])
{
    printf("\ntamanho do TAM_METADADOS_NO_DISCO:%ld\n", TAM_METADADOS_NO_DISCO);
    if (argc < 4) /* Verifica se o número de argumentos está correto*/
    {
        printf("Formato Incorreto!\nUtilize: ./vina -[arg] [arquivo] [arquivo1...arquivoN]\n");
        return -1;
    }
    /* Variável que será incrementada para iterar os argumentos*/
    int argumento_atual = 1;
    char *opcao = argv[argumento_atual];
    argumento_atual++;
    FILE *arquivo = NULL;
    
    /* Cria struct para ter as informações do arquivo e inicializa o Diretório */
    struct stat info_arquivo;
    Lista *diretorio = (Lista *)malloc(sizeof(Lista));
    if (!diretorio)
    return 2;
    
    int escreve = 1;
    if(strcmp(opcao,"-m") == 0){
        if (argc < 5) {
            printf("Formato Incorreto!\nUtilize: ./vina -m [membro target] [arquivo] [arquivo1...arquivoN]\n");
            return -1;
        }

        /* Trata opção -m */
    
    } else { /* Trata as outras opções */
        /* Abre o arquivo existente ou cria um novo */
        arquivo = fopen(argv[argumento_atual], "rb+");
        if (!arquivo)
        {
            arquivo = fopen(argv[argumento_atual], "wb+");
            if (!arquivo)
            {
                perror("Não foi possível abrir/criar arquivo.vc");
                return 1;
            }
        }
        
        
        /* Lê o diretório do arquivo. Retorna 0 se diretório inexistente */
        int tam_diretorio = ler_diretorio_arquivo(arquivo, diretorio);
        imprime_lista(diretorio);
        
        /* Caso haja diretório, posiciona o ponteiro para o depois do último arquivo */
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
        
        if (strcmp(opcao, "-ip") == 0)
        {     
            /* Loop para inserir os membros até o fim */
            while (argumento_atual < argc)
            {
                if (inserir_p(argv[argumento_atual], arquivo, diretorio) != 0)
                {
                    fprintf(stderr, "Falha ao processar a opção -ip para %s\n", argv[argumento_atual]);
                }
                argumento_atual++;
            }
            imprime_lista(diretorio);
        } 
        else if (strcmp(opcao, "-ic") == 0)
        {     
            /* Loop para inserir os membros até o fim */
            while (argumento_atual < argc)
            {
                if (inserir_c(argv[argumento_atual], arquivo, diretorio) != 0)
                {
                    fprintf(stderr, "Falha ao processar a opção -ic para %s\n", argv[argumento_atual]);
                }
                argumento_atual++;
            }
            imprime_lista(diretorio);
        } 

        else if (strcmp(opcao, "-x") == 0) 
        {
            escreve = 0; /* Porque essa opção não mexe no diretório */
           /* Loop para inserir os membros até o fim */
           while (argumento_atual < argc)
           {
               if (extrair(argv[argumento_atual], arquivo, diretorio) != 0)
               {
                   fprintf(stderr, "Falha ao processar a opção -x para %s\n", argv[argumento_atual]);
               }
               argumento_atual++;
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
    }

    /* Depois de escrever todos os membros, escreve o diretório*/
    if (escreve && escreve_metadados_arquivo(arquivo, diretorio))
    {
        fprintf(stderr, "Erro ao escrever metadados");
    }
    
    printf("Finalizando...\n");
    
    fclose(arquivo);
    return 0;
}

int inserir_c(const char *nome_arquivo, FILE *arquivo, Lista *diretorio)
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
    long local = ftell(arquivo);
    if (local == -1)
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
    printf("Dados escritos no arquivamento (%u bytes) na posição %ld.\n", tamanho_para_escrever, local);

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
    fmeta->pos = diretorio->tamanho;
    fmeta->local = local;
    fmeta->c_size = c_size_meta;  
    // fmeta->o_size já deve ter sido preenchido por criar_metadados a partir de info_arquivo.st_size

    // Inserir metadados na lista em memória
    insere_lista(diretorio, fmeta); // fmeta agora pertence à lista

    // Liberar buffers
    free(buffer_entrada);
    free(buffer_saida);

    return 0; // Sucesso
}

int inserir_p(const char *nome_arquivo, FILE *arquivo, Lista *diretorio)
{

    printf("-ip foi incluído, seu valor associado é: %s\n", nome_arquivo);

    /* Abre o arquivo selecionado*/
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        perror("Erro ao tentar abrir o arquivo para opção -p");
        fprintf(stderr, "Nome do arquivo: %s\n", nome_arquivo);
        return 1;
    }

    /* Retira as informações do arquivo selecionado */
    int fd = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd, &info_arquivo) == -1)
    {
        perror("Erro ao obter informações do arquivo");
        fclose(fp);
        return 1;
    }

    /* Obtém o tamanho do arquivo*/
    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0)
    {
        perror("Erro ao obter tamanho do arquivo (-ip)");
        fclose(fp);
        return 1;
    }
    unsigned int tam = (unsigned int)tamanho_arquivo;
    
    /* Alcoa o buffer*/
    char *buffer = malloc(tam);
    if (buffer == NULL)
    {
        perror("Erro ao alocar buffer (-ip)");
        fclose(fp);
        return 1;
    }

    /* Repocisiona para o inicio do arquivo e lê o conteúdo*/
    rewind(fp);
    if (fread(buffer, 1, tam, fp) != tam)
    {
        perror("Erro ao ler arquivo (-ip)");
        free(buffer);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    /* Escreve dados no arquivo.vc */
    unsigned int local = ftell(arquivo);
    if (fwrite(buffer, 1, tam, arquivo) != tam)
    {
        perror("Erro ao escrever dados no arquivo (-p)");
    }

    printf("\nPosição %ld tam bytes, após escrever dados\n", ftell(arquivo));
    printf("Dados de %s (tamanho %u) processados pela opção -p.\n", nome_arquivo, tam);
    
    metadados *fmeta = criar_metadados(nome_arquivo);
    fmeta->pos = diretorio->tamanho;
    fmeta->local = local;

    if (!fmeta)
    {
        fprintf(stderr, "Falha ao criar metadados para %s\n", nome_arquivo);
        return 1;
    }

    insere_lista(diretorio, fmeta);

    local = ftell(arquivo);
    printf("\nFinalizado com %d tam bytes\n", local);

    free(buffer);
    return 0;
}

int ler_diretorio_arquivo(FILE *arquivo, Lista *diretorio)
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
        perror("Arquivo de arquivamento vazio ou muito pequeno para conter um diretório.\n");
        return 0;
    }
}

int extrair(const char *nome_arquivo, FILE *arquivo, Lista *diretorio) {
    /* Acha o nó da do arquivo no diretorio*/
    No *target = busca_lista(diretorio,nome_arquivo);
    if (target == NULL){
        perror("arquivo não encontrado o target");
        return -1;
    }

    /* variavel auxiliar para acelerar os acessos */
    metadados *t_meta = target->data;
    unsigned int t_tam = t_meta->c_size;
    char buffer[t_tam];

    /* Ponteiro do arquivo para o inicio dos dados do arquivo*/
    if (fseek(arquivo, t_meta->local, SEEK_SET) != 0){
        perror("arquivo não encontrado no diretorio");
        return -2;
    }

    /* Le o conteúdo do arquivo */
    if(fread(buffer,1,t_tam,arquivo) != t_tam){
        return -3;
        perror("\nNao foi possível ler o arquivo!\n");
    }
        
    FILE *fp = fopen(nome_arquivo,"wb+");
    
    if(!fp){
        perror("\nNao foi possível criar o arquivo!\n");
        return -2;
    }

    if(t_tam != t_meta->o_size){
        unsigned int o_size = t_meta->o_size;
        char buffer_out[o_size];
        LZ_Uncompress(buffer,buffer_out,t_tam);   
        if (fwrite(buffer_out,1,o_size,fp) != o_size)
            perror("\nNao foi possível escrever no arquivo!\n");
    } else {
        if (fwrite(buffer,1,t_tam,fp) != t_tam)
            perror("\nNao foi possível escrever no arquivo!\n");
        
    }
    
    fclose(fp);
    return 0;
}