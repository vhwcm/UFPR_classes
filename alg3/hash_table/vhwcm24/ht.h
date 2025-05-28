#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include<stdbool.h>

#define M 11

struct No_t {
  int chave;
};

struct Ht{
  struct No_t *t1[M];
  bool t1_deleted[M];
  struct No_t *t2[M];
  bool t2_deleted[M];
};

struct Ht* criaHt(void);

int h1(int chave);

int h2(int chave);

int buscarChave(int chave,struct Ht* hash_table,bool *witch_table);

int inserir(int chave,struct Ht* hash_table);

int deletar(int chave, struct Ht* hash_table);

int imprime(struct Ht* hash_table);

void libera_hash_table(struct Ht* hash_table);




#endif
