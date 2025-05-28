#include"ht.h"
#include<stdio.h>
#include<math.h>
#include<stdbool.h>

struct No_Imprime{
  int chave;
  bool witch_table;
  int slot;
  No_Imprime* prox;
}

struct Imprime{
  No_Imprime* prim;
  No_Imprime* ult;
}

struct No_t* cria_no(int chave){
  strcut No_t* n_no = malloc(sizeof(No_t));
  if(!n_no)
    return NULL;
  
  n_no->chave = chave;
  return n_no;
}



int h1(int chave){
  retun chave % M; 
}

int h2(int chave){
  retun floor(M*(chave*0.9 - floor(chave * 0.9)));
}


int buscaChave(int chave,struct *Ht hash_table, bool *witch_table){
  if(!witch_table || !hash_table2 || !hash_table1)
    return -2;

  int slot = h1(chave);
  if(hash_table->t1[slot]->chave == chave && hash_table->t1_deleted[slot] == false){
    *witch_table = 0;
    return slot; 
  }else{
    slot = h2(chave);
    if(hash_table2->t2[slot]->chave == chave && hash_table->t2_deleted[slot] == false){
      *witch_table = 1;
    }else{
      return -1;
    }
  }
}   

int insere(int chave,struct *Ht hash_table){
  if(!hash_table)
    return -1; 
 
  strct No_t* n_no = cria_no(chave);
  if(!n_no)
    return -1;

  int slot = h1(chave);
  if(hash_table->t1[slot] == NULL){
    hash_table->t1[slot] = n_no;
    hash_table->t1_deleted[slot] = FALSE;
  }else if hash_table->t1_deleted[slot] == true{
    free(hash_table->t1[slot]);
    hash_table->t1[slot] = n_no;
    hash_table->t1_deleted[slot] = FALSE;
  }else{
    slot = h2(chave);
    hash_table->t2[slot] = n_no;
    hash_table->t2_deleted[slot] = FALSE;
  }
  return 0;
}


int deletar(int chave, struct *Ht hash_table){
  if(!hash_table)
    retun -1;

  bool witch_table;

  int slot = buscaChave(chave,hash_table, &witch_table);
  if(slot == -1)
    return -2;
  
  if(witch_table){
    hash_table->t2_deleted[slot] = true;
  }else{
    hash_table->t1_deleted[slot] = true;
  }
  return 0; 
}

int imprime(struct *Ht hash_table){
  if(!hash_table)
    return -1;
   
  struct Imprime* = malloc(sizeof(Imprime));
  Imprime->prim = NULL;
  Imprime->ult = NULL;

  for(int i=0;i<M;i++){
     if(hash_table->t1[i] != NULL && hash_table->t1_deleted[i] == false){

     }
  }

}

