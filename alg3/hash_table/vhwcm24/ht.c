
#include "ht.h"      
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>  

struct Ht* criaHt(void) {
    struct Ht* ht = (struct Ht*)malloc(sizeof(struct Ht));
    if (ht == NULL) {
        fprintf(stderr, "Falha na alocação de memória para Hash-Table em criaHt.\n");
        return NULL;
    }

    for (int i = 0; i < M; i++) {
        ht->t1[i] = NULL;
        ht->t1_deleted[i] = false;
        ht->t2[i] = NULL;
        ht->t2_deleted[i] = false;
    }

    return ht;
}

// Estruturas para a lista de impressão e ordenação
struct No_Imprime {
    int chave;
    bool witch_table; // 0 para t1, 1 para t2
    int slot;
    struct No_Imprime* prox;
};

struct Imprime {
    struct No_Imprime* prim;
    struct No_Imprime* ult;
};

struct No_t* cria_no(int chave){
  struct No_t* n_no = (struct No_t*)malloc(sizeof(struct No_t)); // Corrigido: struct, cast
  if(!n_no)
    return NULL;
 
  n_no->chave = chave;
  return n_no;
}

int h1(int chave){
  return chave % M;  
}

int h2(int chave){
  return floor(M*(chave*0.9 - floor(chave * 0.9)));
}

int buscaChave(int chave, struct Ht* hash_table, bool *witch_table_ptr){ 
  if (!hash_table || !witch_table_ptr) 
      return -2;

  int slot = h1(chave);
  if(hash_table->t1[slot] != NULL && hash_table->t1[slot]->chave == chave && hash_table->t1_deleted[slot] == false){
    *witch_table_ptr = 0;
    return slot; 
  } else {
    slot = h2(chave);
    if(hash_table->t2[slot] != NULL && hash_table->t2[slot]->chave == chave && hash_table->t2_deleted[slot] == false){
      *witch_table_ptr = 1;
      return slot;
    } else {
      return -1;
    }
  }
}  

int inserir(int chave, struct Ht* hash_table){ 
  if(!hash_table)
    return -1; 
 
  struct No_t* n_no = cria_no(chave); 
  if(!n_no)
    return -1;

  int slot = h1(chave);
  if(hash_table->t1[slot] == NULL){
    hash_table->t1[slot] = n_no;
    hash_table->t1_deleted[slot] = false; 
  } else if(hash_table->t1_deleted[slot] == true){
    if(hash_table->t1[slot] != NULL) free(hash_table->t1[slot]);
    hash_table->t1[slot] = n_no;
    hash_table->t1_deleted[slot] = false; 
    } else {
    int chave_existente = hash_table->t1[slot]->chave;
    int slot_chave_existente_t2 = h2(chave_existente);
    if(hash_table->t2[slot_chave_existente_t2] != NULL) free(hash_table->t2[slot_chave_existente_t2]);
    hash_table->t2[slot_chave_existente_t2] = hash_table->t1[slot];
    hash_table->t2_deleted[slot_chave_existente_t2] = false; 
    hash_table->t1_deleted[slot] = false;
    hash_table->t1[slot] = n_no;
    }
  return 0;
}

int deletar(int chave, struct Ht* hash_table){ 
  if(!hash_table)
    return -1; 
  bool local_witch_table; 
  int slot = buscaChave(chave, hash_table, &local_witch_table);
  if(slot < 0) 
    return -2;   
  if(local_witch_table == 1){ 
    hash_table->t2_deleted[slot] = true;
    free(hash_table->t2[slot]); 
    hash_table->t2[slot] = NULL;
  } else {
    hash_table->t1_deleted[slot] = true;
    free(hash_table->t1[slot]);
    hash_table->t1[slot] = NULL;
  }
  return 0; 
}


void inserirOrdenado(struct No_Imprime** sorted_head_ptr, struct No_Imprime* novo_no) {
    if (*sorted_head_ptr == NULL || (*sorted_head_ptr)->chave >= novo_no->chave) {
        novo_no->prox = *sorted_head_ptr;
        *sorted_head_ptr = novo_no;
    } else {
        struct No_Imprime* atual = *sorted_head_ptr;
        while (atual->prox != NULL && atual->prox->chave < novo_no->chave) {
            atual = atual->prox;
        }
        novo_no->prox = atual->prox;
        atual->prox = novo_no;
    }
}

void insertionSortImprime(struct Imprime* lista) {
    if (lista == NULL || lista->prim == NULL || lista->prim->prox == NULL) {
        return;
    }

    struct No_Imprime* sorted_head = NULL;
    struct No_Imprime* atual = lista->prim;

    while (atual != NULL) {
        struct No_Imprime* proximo_no_original = atual->prox;
        // É importante que 'atual' seja tratado como um nó isolado ao ser inserido.
        // Se inserirOrdenado não define atual->prox, e atual já tem um prox,
        // isso pode criar problemas. No entanto, a lógica de coleta cria nós com prox=NULL.
        inserirOrdenado(&sorted_head, atual);
        atual = proximo_no_original;
    }

    lista->prim = sorted_head;

    if (lista->prim == NULL) {
        lista->ult = NULL;
    } else {
        struct No_Imprime* temp = lista->prim;
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        lista->ult = temp;
    }
}

int imprime(struct Ht* hash_table) {
    if (!hash_table) {
        return -1;
    }

    struct Imprime* lista_para_imprimir = (struct Imprime*)malloc(sizeof(struct Imprime));
    if (!lista_para_imprimir) {
        return -1;
    }
    lista_para_imprimir->prim = NULL;
    lista_para_imprimir->ult = NULL;

    // Coleta dados da tabela t1
    for (int i = 0; i < M; i++) { // M deve estar definido em ht.h ou globalmente
        if (hash_table->t1[i] != NULL && hash_table->t1_deleted[i] == false) {
            struct No_Imprime* no_imp = (struct No_Imprime*)malloc(sizeof(struct No_Imprime));
            if (!no_imp) {
                // Limpeza em caso de falha de alocação
                struct No_Imprime* curr_del = lista_para_imprimir->prim;
                while (curr_del != NULL) {
                    struct No_Imprime* next_del = curr_del->prox;
                    free(curr_del);
                    curr_del = next_del;
                }
                free(lista_para_imprimir);
                return -1;
            }
            no_imp->chave = hash_table->t1[i]->chave; // Assumindo que No_t tem o campo 'chave'
            no_imp->witch_table = 0; // 0 para t1
            no_imp->slot = i;
            no_imp->prox = NULL;

            if (lista_para_imprimir->prim == NULL) {
                lista_para_imprimir->prim = no_imp;
                lista_para_imprimir->ult = no_imp;
            } else {
                lista_para_imprimir->ult->prox = no_imp;
                lista_para_imprimir->ult = no_imp;
            }
        }
    }

    // Coleta dados da tabela t2
    for (int i = 0; i < M; i++) { // M deve estar definido em ht.h ou globalmente
        if (hash_table->t2[i] != NULL && hash_table->t2_deleted[i] == false) {
            struct No_Imprime* no_imp = (struct No_Imprime*)malloc(sizeof(struct No_Imprime));
             if (!no_imp) {
                // Limpeza em caso de falha de alocação
                struct No_Imprime* curr_del = lista_para_imprimir->prim;
                while (curr_del != NULL) {
                    struct No_Imprime* next_del = curr_del->prox;
                    free(curr_del);
                    curr_del = next_del;
                }
                free(lista_para_imprimir);
                return -1;
            }
            no_imp->chave = hash_table->t2[i]->chave; // Assumindo que No_t tem o campo 'chave'
            no_imp->witch_table = 1; // 1 para t2
            no_imp->slot = i;
            no_imp->prox = NULL;

            if (lista_para_imprimir->prim == NULL) {
                lista_para_imprimir->prim = no_imp;
                lista_para_imprimir->ult = no_imp;
            } else {
                lista_para_imprimir->ult->prox = no_imp;
                lista_para_imprimir->ult = no_imp;
            }
        }
    }

    // Ordena a lista coletada
    insertionSortImprime(lista_para_imprimir);

    // Imprime os nós ordenados
    struct No_Imprime* atual_print = lista_para_imprimir->prim;
    while (atual_print != NULL) {
        printf("%d,%s,%d\n",
               atual_print->chave,
               atual_print->witch_table ? "T2":"T1", 
               atual_print->slot);
        atual_print = atual_print->prox;
    }

    // Libera a memória alocada para a lista de impressão
    struct No_Imprime* curr_free = lista_para_imprimir->prim;
    while (curr_free != NULL) {
        struct No_Imprime* next_to_free = curr_free->prox;
        free(curr_free);
        curr_free = next_to_free;
    }
    free(lista_para_imprimir);

    return 0;
}

void libera_hash_table(struct Ht* hash_table) {
    if (hash_table == NULL) {
        return;
    }

    // Libera os nós dentro das tabelas t1 e t2
    for (int i = 0; i < M; i++) { // M deve ser acessível (global ou parte de Ht)
        if (hash_table->t1[i] != NULL) {
            free(hash_table->t1[i]);
            hash_table->t1[i] = NULL; // Boa prática: anular ponteiro após free
        }
        if (hash_table->t2[i] != NULL) {
            free(hash_table->t2[i]);
            hash_table->t2[i] = NULL; // Boa prática
        }
    }

    // Libera a estrutura da hash table em si
    free(hash_table);
}

