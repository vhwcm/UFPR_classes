#include<stdio.h>

#include"ht.h"
int main(int argc, char* argv[]) {
    if(argc > 2){
      fprintf(stderr,"Numero de argumentos incorretos");
      return -1;
    }
    if(argc == 2){
      if(argv[1] == "-h"){
        printf("Nome: Victor Hugo Weigmann Chequer Maia\n");
        printf("GRR: GRR5275\n");
        printf("Login: vhwcm24\n");
        return 0;
      }else{
        printf("Argumento Errado!!\n");
        return 0;
      }
    }
      
    // Inicializa a estrutura global da Hash-Table
    char op;
    int chave;

    struct Ht* ht = criaHt();
    if (ht == NULL) {
        fprintf(stderr, "Falha ao inicializar a Hash-Table em main.\n");
        return 1;
    }

   // Lê operações e chaves da entrada padrão até parar as entradas
   while (scanf(" %c %d", &op, &chave) == 2) {
     if (op == 'i') {
       if(inserir(chave,ht) != 0){
         fprintf(stderr,"Erro ao inserir chave");
        } // insere a nova chave
      } else if (op == 'r') {
        if (deletar(chave,ht) != 0) {
          fprintf(stderr,"Erro ao deletar chave");
        }
      }
    }

    // Imprime e estrutura 
    imprime(ht);
    
    // Libera a memória alocada
    libera_hash_table(ht);
    return 0;
}

