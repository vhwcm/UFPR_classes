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

int ler_diretorio_arquivo(FILE *arquivo, Lista *diretorio);

int inserir_p(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int inserir_c(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int mover(const char *nome_arquivo,const char *target, FILE *arquivo, Lista *diretorio);

int extrair(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int remover(const char *nome_arquivo, FILE *arquivo, Lista *diretorio);

int listar_conteudo(FILE *arquivo, Lista *diretorio);

int substituir_arquivo(const char *nome_arquivo, FILE *arquivo, Lista *diretorio, 
                      char *buffer_novo, int tamanho_novo);

int main(int argc, char *argv[])
{

    /* Verifica se o número de argumentos está correto */
    if (argc < 3) 
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
    Lista *diretorio = (Lista *)calloc(1,sizeof(Lista));
    if (!diretorio)
        return 2;
    
    int escreve = 0;
    if(strcmp(opcao,"-m") == 0){
        if (argc != 5) {
            printf("Formato Incorreto!\nUtilize: ./vina -m [membro] [arquivo] [membro target]\n");
            return -1;
        } else {
            argumento_atual++;
        }    
    }
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
        
        /* Caso haja diretório, posiciona o ponteiro para o depois do último arquivo */
        argumento_atual++;
        if (tam_diretorio)
        {
            fseek(arquivo, -(tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int)), SEEK_END);
            long posicao = ftell(arquivo);
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
                escreve = 1;
                if (inserir_p(argv[argumento_atual], arquivo, diretorio) != 0)
                {
                    fprintf(stderr, "Falha ao processar a opção -ip para %s\n", argv[argumento_atual]);
                }
                argumento_atual++;
            }
        } 
        else if (strcmp(opcao, "-ic") == 0)
        {     
            /* Loop para inserir os membros até o fim */
            while (argumento_atual < argc)
            {
                escreve = 1;
                if (inserir_c(argv[argumento_atual], arquivo, diretorio) != 0)
                {
                    fprintf(stderr, "Falha ao processar a opção -ic para %s\n", argv[argumento_atual]);
                }
                argumento_atual++;
            }
        }
        else if (strcmp(opcao, "-m") == 0) 
        {
            escreve = 1;
            if (mover(argv[2],argv[4],arquivo,diretorio) != 0){
                fprintf(stderr, "Falha ao processar a opção -ic para %s e %s\n", argv[2], argv[4]);
                return -1;
            }
        }
        else if (strcmp(opcao, "-x") == 0) 
        {
            /* Caso nenhum argumento seja passado, extrair tudo */
            if(argc == 3){
                No* file = diretorio->primeiro;
                while (file)
                {
                    /* Se C permitisse, eu iria fazer sobrecarga de função na função extrair ;)*/
                    if (extrair(file->data->nome, arquivo, diretorio) != 0)
                    {
                        fprintf(stderr, "Falha ao processar a opção -x para %s\n", file->data->nome);
                    } 
                    file = file->prox;
                }
            }else{
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
        }
        else if (strcmp(opcao, "-r") == 0)
        {
            escreve = 1;
            /* Loop para remover os membros até o fim */
            while (argumento_atual < argc)
            {
                if (remover(argv[argumento_atual], arquivo, diretorio) != 0)
                {
                    fprintf(stderr, "Falha ao processar a opção -r para %s\n", argv[argumento_atual]);
                }
                argumento_atual++;
            }
        } 
        else if (strcmp(opcao,"-c") == 0)
        {
            listar_conteudo(arquivo, diretorio);
        }
        else
        {
            printf("Argumento incorreto!!\n");
            return -2;
        }

    /* Depois de escrever todos os membros, escreve o diretório*/
    if (escreve)
    {
        long pos = 0;
        /* Se diretório vázil*/
        if (diretorio->ultimo)
            pos = diretorio->ultimo->data->local + diretorio->ultimo->data->c_size;
        
            fseek(arquivo, pos, SEEK_SET);
            if(escreve_metadados_arquivo(arquivo, diretorio))
            fprintf(stderr, "Erro ao escrever metadados");

        int fd = fileno(arquivo);
        ftruncate(fd,ftell(arquivo));
    }


    
    libera_lista(diretorio);    
    fclose(arquivo);
    return 0;
}

int inserir_c(const char *nome_arquivo, FILE *arquivo, Lista *diretorio) {
    if(!arquivo || !diretorio || !nome_arquivo)
        return -1;

    No* arquivo_existente = busca_lista(diretorio, nome_arquivo);
    
    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        return 1;
    }

    int fd_in = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd_in, &info_arquivo) == -1)
    {
        fclose(fp);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0)
    {
        fclose(fp);
        return 1;
    }
    int tam_original = (int)tamanho_arquivo;
    if (tam_original == 0)
    {
        fclose(fp);
        return 0; 
    }

    char *buffer_entrada = malloc(tam_original);
    if (buffer_entrada == NULL)
    {
        fclose(fp);
        return 1;
    }
    rewind(fp);
    if (fread(buffer_entrada, 1, tam_original, fp) != tam_original)
    {
        free(buffer_entrada);
        fclose(fp);
        return 1;
    }
    fclose(fp); 

    int tam_buffer_saida = (int)(1.004 * tam_original + 1);
    char *buffer_saida = malloc(tam_buffer_saida);
    if (buffer_saida == NULL)
    {
        free(buffer_entrada);
        return 1;
    }

    int tam_comprimido = LZ_Compress(buffer_entrada, buffer_saida, tam_original);

    char *buffer_para_escrever = NULL;
    int tamanho_para_escrever = 0;
    int c_size_meta = 0; 

    if (tam_comprimido > 0 && tam_comprimido < tam_original)
    {
        buffer_para_escrever = buffer_saida;
        tamanho_para_escrever = (int)tam_comprimido;
        c_size_meta = (int)tam_comprimido;
    }
    else
    {
        buffer_para_escrever = buffer_entrada;
        tamanho_para_escrever = tam_original;
        c_size_meta = tam_original; 
    }

    if (arquivo_existente) {
        int resultado = substituir_arquivo(nome_arquivo, arquivo, diretorio, 
                                          buffer_para_escrever, tamanho_para_escrever);
        if (resultado == 0) {
            arquivo_existente->data->o_size = tam_original;
            arquivo_existente->data->c_size = c_size_meta;
            arquivo_existente->data->u_mod = time(NULL);
        }
        free(buffer_entrada);
        free(buffer_saida);
        return resultado;
    } else {
        long local = ftell(arquivo);
        if (fwrite(buffer_para_escrever, 1, tamanho_para_escrever, arquivo) != tamanho_para_escrever) {
            free(buffer_entrada);
            free(buffer_saida);
            return 1;
        }

        metadados *fmeta = criar_metadados(nome_arquivo); 
        if (!fmeta)
        {
            free(buffer_entrada);
            free(buffer_saida);
            return 1;
        }
        fmeta->pos = diretorio->tamanho;
        fmeta->local = local;
        fmeta->o_size = tam_original;  
        fmeta->c_size = c_size_meta;  

        insere_lista(diretorio, fmeta); 

        free(buffer_entrada);
        free(buffer_saida);

        return 0; 
    }
}

int inserir_p(const char *nome_arquivo, FILE *arquivo, Lista *diretorio)
{
    if(!arquivo || !diretorio || !nome_arquivo)
        return -1;

    No* arquivo_existente = busca_lista(diretorio, nome_arquivo);
    

    FILE *fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        return 1;
    }

    /* Retira as informações do arquivo selecionado */
    int fd = fileno(fp);
    struct stat info_arquivo;
    if (fstat(fd, &info_arquivo) == -1)
    {
        fclose(fp);
        return 1;
    }

    /* Obtém o tamanho do arquivo */
    fseek(fp, 0, SEEK_END);
    long tamanho_arquivo = ftell(fp);
    if (tamanho_arquivo < 0)
    {
        fclose(fp);
        return 1;
    }
    int tam = (int)tamanho_arquivo;
    
    /* Aloca o buffer */
    char *buffer = malloc(tam);
    if (buffer == NULL)
    {
        fclose(fp);
        return 1;
    }

    /* Repocisiona para o inicio do arquivo e lê o conteúdo */
    rewind(fp);
    if (fread(buffer, 1, tam, fp) != tam)
    {
        free(buffer);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (arquivo_existente) {
        int resultado = substituir_arquivo(nome_arquivo, arquivo, diretorio, buffer, tam);
        if (resultado == 0) {
            arquivo_existente->data->o_size = tam;
            arquivo_existente->data->c_size = tam;
            arquivo_existente->data->u_mod = time(NULL);
            free(buffer);
            return 0;
        } else {
            free(buffer);
            return resultado;
        }
    } else {
        /* Escreve dados no arquivo.vc */
        int local = ftell(arquivo);
        if (fwrite(buffer, 1, tam, arquivo) != tam)
        {
            free(buffer);
            return 1;
        }

        metadados *fmeta = criar_metadados(nome_arquivo);
        if (!fmeta)
        {
            free(buffer);
            return 1;
        }
        
        fmeta->pos = diretorio->tamanho;
        fmeta->local = local;
        
        insere_lista(diretorio, fmeta);

        local = ftell(arquivo);

        free(buffer);
        return 0;
    }
}

int ler_diretorio_arquivo(FILE *arquivo, Lista *diretorio)
{
    if(!arquivo || !diretorio)
        return -1;

    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    int tam_diretorio = 0;
    long pos;
    if (tamanho_arquivo >= sizeof(int))
    {
        if (fseek(arquivo, -(sizeof(int)), SEEK_END) == 0)
        {
            pos = ftell(arquivo);
            if (fread(&tam_diretorio, sizeof(int), 1, arquivo) == 1)
            {
                if (fseek(arquivo, -(tam_diretorio * TAM_METADADOS_NO_DISCO + sizeof(int)), SEEK_END) == 0)
                {
                    pos = ftell(arquivo);
                    le_metadados_arquivo(arquivo, diretorio, tam_diretorio);
                    return tam_diretorio;
                }
                else
                {
                    return -2;
                }
            }
            else
            {
                return -3;
            }
        }
        else
        {
            return -4;
        }
    }
    else
    {
        inicializa_lista(diretorio);
        return 0;
    }
}

int extrair(const char *nome_arquivo, FILE *arquivo, Lista *diretorio) {
    if(!arquivo || !diretorio || !nome_arquivo)
        return -1;

    /* Acha o nó da do arquivo no diretorio*/
    No *target = busca_lista(diretorio,nome_arquivo);
    if (target == NULL){
        return -1;
    }

    /* variavel auxiliar para acelerar os acessos */
    metadados *t_meta = target->data;
    int t_tam = t_meta->c_size;
    
    /* Aloca buffer com tamanho apropriado */
    char *buffer = malloc(t_tam);
    if (!buffer) {
        return -1;
    }
    
    /* Ponteiro do arquivo para o inicio dos dados do arquivo*/
    if (fseek(arquivo, t_meta->local, SEEK_SET) != 0){
        return -2;
    }

    /* Le o conteúdo do arquivo */
    if(fread(buffer,1,t_tam,arquivo) != t_tam){
        return -3;
    }
        
    FILE *fp = fopen(nome_arquivo,"wb+");
    
    if(!fp){
        return -2;
    }

    if(t_tam != t_meta->o_size){
        int o_size = t_meta->o_size;
        char *buffer_out = malloc(o_size);
        if (!buffer_out) {
            free(buffer);
            return -1;
        }
        
        LZ_Uncompress(buffer, buffer_out, t_tam);   
        if (fwrite(buffer_out, 1, o_size, fp) != o_size)
        
        free(buffer_out);
    } else {
        if (fwrite(buffer, 1, t_tam, fp) != t_tam)
            return -3;
    }
    
    free(buffer);
    
    fclose(fp);
    return 0;
}


int mover(const char *nome_arquivo,const char *target, FILE *arquivo, Lista *diretorio) {
    No* membro = busca_lista(diretorio,nome_arquivo);
    if (!membro) {
        return -1;
    }
    int pos_membro = membro->data->pos;

    int pos_target = 0;
    No* no_target;
    if(strcmp(target,"NULL") != 0){
        no_target = busca_lista(diretorio,target);
        if (!no_target) {
            return -1;
        }
        pos_target = no_target->data->pos;
    }

    if (pos_target == membro->data->pos){
        return 0;
    }
    int m_size = membro->data->c_size;
    char *m_buffer = malloc(m_size);
    if (!m_buffer) {
        return -1;
    }
    
    fseek(arquivo,membro->data->local ,SEEK_SET);
    if (fread(m_buffer,1,m_size,arquivo) != m_size) {
        free(m_buffer);
        return -1;
    }

    int tam_dir = diretorio->tamanho;
    int tam_max = 0;
    No** vetor = (No**)malloc(tam_dir * sizeof(No*));
    if (!vetor) {
        free(m_buffer);
        return -1;
    }


    No* aux = diretorio->primeiro;
    /* O vetor e preenchido com os ponteiros da lista*/
    /* Vai ser util porque minha lista não é duplamente encadeada*/
    while (aux != NULL)
    {
        vetor[aux->data->pos] = aux;
        if(aux->data->c_size > tam_max)
            tam_max = aux->data->c_size;
        aux = aux->prox;
    }

    char *aux_buffer = calloc(1,tam_max);
    if (!aux_buffer) {
        free(m_buffer);
        return -1;
    }
    
    int pos_atual = membro->data->pos;
    aux = membro;

    /* Caso que deve ser deslocado para frente*/
    if(pos_target > membro->data->pos){
        /* Itera sobre todos os membros entre o membro e o target-inclusive */
        for (int i = pos_atual; i < pos_target; i++){
            
            /*Le o dado do próximo arquivo*/
            aux = aux->prox;
            aux->data->pos -= 1;
            fseek(arquivo, aux->data->local,SEEK_SET);
            if (fread(aux_buffer, 1, aux->data->c_size,arquivo) != aux->data->c_size) {
                free(m_buffer);
                free(aux_buffer);
                return -1;
            }
            fseek(arquivo, aux->data->local,SEEK_SET);
            fseek(arquivo, -m_size,SEEK_CUR);
            int n_local = ftell(arquivo);
            aux->data->local = n_local;
            fwrite(aux_buffer,1,aux->data->c_size,arquivo);
        }
        if(diretorio->ultimo == no_target){
            diretorio->ultimo = membro;
        } 
        if(diretorio->primeiro == membro){
            diretorio->primeiro = membro->prox; 
        }

        membro->data->pos = (aux->data->pos) + 1; 
    } else { /* Caso que deve ser deslocado para trás */
        /* Move o ponteiro para o fim do membro que será deslocado*/
        for (int i = pos_atual; i > pos_target + 1; i--){
            /* Lê o membro anterior*/
            aux = vetor[i - 1];
            aux->data->pos -= 1;
            fseek(arquivo, aux->data->local, SEEK_SET);
            fread(aux_buffer, 1, aux->data->c_size,arquivo);

            /* Faz o shift rigth no tamanho do membro*/
            fseek(arquivo, aux->data->local, SEEK_SET);
            fseek(arquivo, m_size, SEEK_CUR);
            int n_local = ftell(arquivo);
            aux->data->local = n_local;
            fwrite(aux_buffer,1,aux->data->c_size,arquivo);
        }

        /* Se for NULL, vai faltar mexer o primeiro elemento*/
        if(strcmp(target,"NULL") == 0){
            aux = vetor[pos_target];
            fseek(arquivo, 0, SEEK_SET);
            fread(aux_buffer, 1, aux->data->c_size,arquivo);
            fseek(arquivo, 0, SEEK_SET);
            fseek(arquivo, m_size, SEEK_CUR);
            int n_local = ftell(arquivo);
            aux->data->local = n_local;
            fwrite(aux_buffer,1,aux->data->c_size,arquivo);
            fseek(arquivo,0,SEEK_SET);
            diretorio->primeiro = membro;
        }else{
            aux = vetor[pos_target];
            fseek(arquivo, aux->data->local + aux->data->c_size, SEEK_SET);
        }

        if(diretorio->ultimo == membro){
            diretorio->ultimo = vetor[membro->data->pos-1];
        }
        /* Escreve o membro que se moveu*/

        membro->data->pos = pos_target + 1; 
    }
    int n_local = ftell(arquivo);
    membro->data->local = n_local;
    fwrite(m_buffer, 1, m_size, arquivo);

    /* Corrige diretorio*/
    No* temp;
    if(pos_membro){
        temp = vetor[pos_membro - 1];
        temp->prox = membro->prox;    
    }
    if(strcmp(target,"NULL") == 0){
        membro->prox = vetor[0];    
        membro->data->pos = 0;
        membro->data->local = 0;
    }else{
        membro->prox = no_target->prox;
        no_target->prox = membro;
    }

    free(vetor);
    free(m_buffer);
    free(aux_buffer);
    return 0;
}

int remover(const char *nome_arquivo, FILE *arquivo, Lista *diretorio) {
    /* localiza o nó do arquivo a ser removido */
    No* membro = busca_lista(diretorio, nome_arquivo);
    if (!membro) {
        return -1;
    }
    
    int pos_remover = membro->data->pos;
    int tam_remover = membro->data->c_size;
    long local_remover = membro->data->local;
    
    /* Criar vetor auxiliar para acessar os nós por posição */
    int tam_dir = diretorio->tamanho;
    
    No *aux = membro;
    /* Deslocar todos os arquivos que vêm depois do removido para frente */
    for (int i = pos_remover + 1; i < tam_dir; i++) {
        aux = aux->prox;
        
        /* Atualiza a posição */
        aux->data->pos--;
        
        /* Lê os dados do arquivo */
        char *buffer = calloc(1,aux->data->c_size);
        if(!buffer)
            return -1;
        fseek(arquivo, aux->data->local, SEEK_SET);
        if (fread(buffer, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
            free(buffer);
            return -1;
        }
        
        /* Calcula a nova localização (diminuindo o tamanho do arquivo removido) */
        long nova_local = aux->data->local - tam_remover;
        aux->data->local = nova_local;
        
        fseek(arquivo, nova_local, SEEK_SET);
        if (fwrite(buffer, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
            free(buffer);
            return -1;
        }

        free(buffer);
    }
    
    /* Ajustar os ponteiros da lista encadeada */
    No* anterior = NULL;
    aux = diretorio->primeiro;
    
    /* Encontrar o nó anterior ao removido */
    while (aux != membro && aux != NULL) {
        anterior = aux;
        aux = aux->prox;
    }
    
    /* Ajusta os ponteiros */
    if (anterior) {
        anterior->prox = membro->prox;
    } else {
        /* Se o removido era o primeiro */
        diretorio->primeiro = membro->prox;
    }
    
    /* Se o removido era o último */
    if (diretorio->ultimo == membro) {
        diretorio->ultimo = anterior;
    }
    
    /* Libera a memória do nó removido */
    free(membro->data);
    free(membro);
    
    /* Atualiza o tamanho do diretório*/
    diretorio->tamanho--;
    return 0;
}

int listar_conteudo(FILE *arquivo, Lista *diretorio) {
    if(!diretorio || !arquivo){
        printf("erro!!");
        return -1;
    }
    printf("\n==Mostrando conteudo do arquivo.vc==\n\n");
    No* aux = diretorio->primeiro;
    int tamanho = diretorio->tamanho;
    int o_size;
    int local = 0;
    for(int i = 0; i < tamanho; i++){
        local = aux->data->local;
        int tam = aux->data->c_size;
        char *buffer = calloc(1,tam);
        if (!buffer) {
            perror("Erro ao alocar memória para exibir conteúdo");
            return -1;
        }
        printf("=Metadados=\n");
        imprime_metadados(aux->data);
        fseek(arquivo,local,SEEK_SET);
        if (fread(buffer,1,tam,arquivo) != tam) {
            perror("Erro ao ler conteúdo do arquivo");
            free(buffer);
            return -1;
        }
        if(tam != aux->data->o_size){
          o_size = aux->data->o_size; 
          char *buffer_out = calloc(1,o_size);
          if (!buffer_out) {
              perror("Erro ao alocar memória para descompressão");
              free(buffer);
              return -1;
          }
          LZ_Uncompress(buffer, buffer_out, tam); // Note: deve usar tam, não o_size aqui
          printf("\n=Conteudo=\n");
          for(int i = 0; i < o_size; i++){
            printf("%c",buffer_out[i]);
          }
          free(buffer_out);
        } else {
            printf("\n=Conteudo=\n");
            for(int i = 0; i < tam; i++){
                printf("%c",buffer[i]);
            }
        }
        free(buffer);
        printf("\n\n");
        aux = aux->prox;
    }
    return 0;
}

/* Substitui um arquivo existente no archive */
int substituir_arquivo(const char *nome_arquivo, FILE *arquivo, Lista *diretorio, 
                      char *buffer_novo, int tamanho_novo) {
    // Busca o arquivo que será substituído
    No* membro = busca_lista(diretorio, nome_arquivo);
    if (!membro) {
        return -1;
    }
    
    /* Tamanho do arquivo atual e sua posição no arquivo */
    int tamanho_atual = membro->data->c_size;
    int pos_membro = membro->data->pos;
    long local_membro = membro->data->local;
    int diferenca_tamanho = tamanho_novo - tamanho_atual;
    
    
    /* Se os tamanhos são iguais, simplesmente substituímos os dados */
    if (diferenca_tamanho == 0) {
        fseek(arquivo, local_membro, SEEK_SET);
        if (fwrite(buffer_novo, 1, tamanho_novo, arquivo) != tamanho_novo) {
            return -1;
        }
        return 0;
    }
    
    /* Criar vetor auxiliar para acessar os nós por posição */
    int tam_dir = diretorio->tamanho;
    if (tam_dir <= 0) {
        return -1;
    }
    
    No** vetor = calloc(1,tam_dir * sizeof(No*));
    if (!vetor) {
        return -1;
    }
    
    int tam_max = 0;
    No* aux = diretorio->primeiro;
    
    while (aux != NULL) {
        if (aux->data->pos >= tam_dir) {
            free(vetor);
            return -1;
        }
        vetor[aux->data->pos] = aux;
        if (aux->data->c_size > tam_max)
            tam_max = aux->data->c_size;
        aux = aux->prox;
    }
    
    /* Buffer para mover arquivos (usa o maior tamanho disponível)/*
    /* Evita buffer de tamanho zero */
    if (tam_max == 0) tam_max = 1;
    
    char *buffer_aux = calloc(1,tam_max);
    if (!buffer_aux) {
        free(vetor);
        return -1;
    }
    
    if (diferenca_tamanho > 0) {
        /* Deslocamos todos os arquivos à direita do atual */
        for (int i = tam_dir - 1; i > pos_membro; i--) {
            aux = vetor[i];
            
            /* Lê o arquivo */
            fseek(arquivo, aux->data->local, SEEK_SET);
            if (fread(buffer_aux, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
                free(buffer_aux);
                free(vetor);
                return -1;
            }
            
            /* Calcula nova localização */
            long nova_local = aux->data->local + diferenca_tamanho;
            
            // Escreve na nova posição
            fseek(arquivo, nova_local, SEEK_SET);
            if (fwrite(buffer_aux, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
                free(buffer_aux);
                free(vetor);
                return -1;
            }
            
            /* Só atualiza o metadado após confirmar a escrita bem-sucedida */ 
            aux->data->local = nova_local;
        }
    } else {
        for (int i = pos_membro + 1; i < tam_dir; i++) {
            aux = vetor[i];
            
            fseek(arquivo, aux->data->local, SEEK_SET);
            if (fread(buffer_aux, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
                free(buffer_aux);
                free(vetor);
                return -1;
            }
            
            long nova_local = aux->data->local + diferenca_tamanho;
            
            fseek(arquivo, nova_local, SEEK_SET);
            if (fwrite(buffer_aux, 1, aux->data->c_size, arquivo) != aux->data->c_size) {
                free(buffer_aux);
                free(vetor);
                return -1;
            }
            
            aux->data->local = nova_local;
        }
    }
    
    fseek(arquivo, local_membro, SEEK_SET);
    if (fwrite(buffer_novo, 1, tamanho_novo, arquivo) != tamanho_novo) {
        free(buffer_aux);
        free(vetor);
        return -1;
    }
    
    membro->data->c_size = tamanho_novo;
    membro->data->u_mod = time(NULL); 
    
    free(buffer_aux);
    free(vetor);
    return 0;
}

