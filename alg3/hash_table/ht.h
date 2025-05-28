#ifndef HASH-TABLE
#define HASH-TABLE

struct No_t {
  int chave;
  bool deleted;  
}

struct Ht{
  No *t1[11];
  No *t2[11];
}

int h1(int chave);

int h2(int chave);

int buscaNo(int chave,struct *Ht hash_table,int *witch_table);

void insere(int chave,struct *Ht hash_table);

void deletar(int cahve, struct *Ht hash_table);

void imprime(struct *Ht hash_table);

#endif
