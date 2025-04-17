#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // Adicionado para malloc/free
#define VERMELHO 1
#define PRETO 0
#define TIPO_NO 1
#define NO_NULO 0
#define TAM 20
// Definição da struct movida para o escopo global
struct tNo {
  int chave;
  struct tNo *esq, *dir, *pai;
  bool cor;
  bool tipo;
};

// Declarações antecipadas para funções
struct tNo *antecessor(struct tNo *no); // Renomeado de sucessor
void ajustaNoPai(struct tNo *no, struct tNo *novo);
struct tNo *rotEsquerda(struct tNo *p);
struct tNo* rotDireita(struct tNo *p);
struct tNo *criaNoNulo (struct tNo *pai);
struct tNo *criaNo (int chave);
struct tNo *ajustaInclusao (struct tNo *no,struct tNo *raiz);
struct tNo* inclui (struct tNo *no, int c, struct tNo *r);
struct tNo *ajustaExclusao (struct tNo *no, struct tNo *raiz);
struct tNo* exclui (struct tNo *no, struct tNo *raiz);
struct tNo *busca(struct tNo *no, int chave);
void freeTree(struct tNo *no); // Declaração adicionada para limpeza
void ajustaNo(struct tNo *no,struct tNo *novo);


int main(int argc, char* argv[]){
  struct tNo *raiz = NULL;
  char op;
  int key;

  // Lê operações do stdin até EOF
  while (scanf(" %c %d", &op, &key) == 2) {
      if (op == 'i') {
        raiz = inclui(raiz, key, raiz);
      } else if (op == 'r') {
          struct tNo *no_to_delete = busca(raiz, key);
          if (no_to_delete != NULL) {
              raiz = exclui(no_to_delete, raiz);
          } else {
              fprintf(stderr, "Chave %d não encontrada para remoção.\n", key);
          }
      } else {
          fprintf(stderr, "Operação inválida: %c\n", op);
      }
      printf("\n----\n");
  }

  // Limpa a árvore
  freeTree(raiz);

  return 0;
}

void ajustaNoPai(struct tNo *no, struct tNo *novo) {
  if (no->pai->tipo != NO_NULO) //verfica se chegou na raiz 
  {
    if (no->pai->esq == no)
      no->pai->esq = novo;
    else
      no->pai->dir = novo;
    if(novo != NULL)
      novo->pai = no->pai;
  }
  else
  {
    novo->pai = no->pai; 
  }
}

struct tNo *rotEsquerda(struct tNo *p){
  struct tNo *q = p->dir;
  p->dir = q->esq;
  q->pai = p->pai;
  p->pai = q;
  q->esq->pai = p;
  q->esq = p;
  return q; // retorna a nova raiz da subarvore
}

struct tNo* rotDireita(struct tNo *p){
  struct tNo *q = p->esq;
  p->esq = q->dir;
  q->pai = p->pai;
  p->pai = q;
  q->dir->pai = p;
  q->dir = p;
  return q;
}

struct tNo *criaNoNulo (struct tNo *pai){
  struct tNo *no = (struct tNo*)malloc(sizeof(struct tNo));
  no->pai = pai;
  no->esq = NULL;
  no->dir = NULL;
  no->cor = VERMELHO;
  no->tipo = NO_NULO;
  return no;
}

struct tNo *criaNo (int chave){
  struct tNo *no = (struct tNo*)malloc(sizeof(struct tNo));
  no->chave = chave;
  no->esq = criaNoNulo(no);
  no->dir = criaNoNulo(no);
  no->cor = VERMELHO;
  no->tipo = TIPO_NO;
  return no;
}

struct tNo *ajustaInclusao (struct tNo *no,struct tNo *raiz){
  struct tNo *tio = NULL, *noAsc = NULL; //no para armazenar o tio e o nó da árvore ascendente anterior 
  while (no->pai->cor == VERMELHO){
      if (no->pai == no->pai->pai->esq){//sub arvore da esquerda
          tio = no->pai->pai->dir;
          if (tio->cor == VERMELHO){ // caso 1: tio (direita do avô) é vermelho -> recolorir
              no->pai->cor = PRETO;
              tio->cor = PRETO;
              no->pai->pai->cor = VERMELHO;
              no = no->pai->pai;
          } else { //caso 2: tio é preto e o novo nó e filho direito
               if (no == no->pai->dir){ // situação zig zag
                   no = no->pai;
                   noAsc = no->pai;
                   noAsc->esq = rotEsquerda(no); // joga o no para cima, no se torna filho da esquerda do avo dele
              } //caso 3: tio é preto e novo nó e filho esquerdo
              no->pai->cor = PRETO;
              no->pai->pai->cor = VERMELHO;
              noAsc = no->pai->pai->pai; //arvore ascendente
              struct tNo *novoAvo = rotDireita(no->pai->pai);
              ajustaNo(noAsc, novoAvo);
              if (novoAvo->pai->tipo == NO_NULO)
                  raiz = novoAvo;            
          }
      } 
      else { //sub arvore da direita. Mesmo princípio do código acima, porém com ponteiros (esq-dir) "trocados"
          tio = no->pai->pai->esq;
          if (tio->cor == VERMELHO){ //caso 2:
              no->pai->cor = PRETO;
              tio->cor = PRETO;
              no->pai->pai->cor = VERMELHO;
              no = no->pai->pai;
          } else {
               if (no == no->pai->esq){
                   no = no->pai;
                   noAsc = no->pai;
                   noAsc->dir = rotDireita(no);
              }
              no->pai->cor = PRETO;
              no->pai->pai->cor = VERMELHO;
              noAsc = no->pai->pai->pai; //arvore ascendente
              struct tNo *novoAvo = rotEsquerda(no->pai->pai);
              ajustaNo(noAsc, novoAvo);
              if (novoAvo->pai->tipo == NO_NULO)
                  raiz = novoAvo;            
          }           
      } 
  }
  raiz->cor = PRETO;
  return raiz;
}

struct tNo* inclui (struct tNo *no, int c, struct tNo *r){
  struct tNo *novoNo;
  if (no == NULL) { //inclui na raiz
      novoNo = criaNo(c);
      novoNo->cor = PRETO;
      return novoNo;
  }
  struct tNo *pai, *raiz = r;
  while (no->tipo != NO_NULO)  {
      pai = no;
      if ( c < no->chave)
          no = no->esq;
      else
          no = no->dir;
  }
  novoNo = criaNo(c);
  if (c < pai->chave)
      pai->esq = novoNo;
  else
      pai->dir = novoNo;
  novoNo->pai = pai;
  raiz = ajustaInclusao(novoNo, raiz);
  return raiz;        
}

struct tNo *ajustaExclusao (struct tNo *no, struct tNo *raiz){
  struct tNo *noAsc, *noAux;
  struct tNo *irmao;
  while (no != raiz && no->cor == PRETO){
      if (no == no->pai->esq) {//sub-árvore da esquerda
          irmao = no->pai->dir;
          if (irmao->cor == VERMELHO) {//caso 1
              irmao->cor = PRETO;
              no->pai->cor = VERMELHO;
              noAsc = no->pai->pai;
              noAux = rotEsquerda(no->pai);
              ajustaNoPai(noAsc, noAux);
              if (noAsc->tipo == NO_NULO) raiz = noAux;
              irmao = no->pai->dir;
          }
          if (irmao->esq->cor == PRETO && irmao->dir->cor == PRETO){ //caso 2
              irmao->cor = VERMELHO;
              no = no->pai;
          }
          else {
              if (irmao->esq->cor == VERMELHO && irmao->dir->cor == PRETO) { //caso 3
                  irmao->cor = VERMELHO;
                  irmao->esq->cor = PRETO;
                  noAsc = irmao->pai;
                  noAsc->dir = rotDireita(irmao);
                  irmao = no->pai->dir;
              } //caso 4
              irmao->cor = no->pai->cor;
              no->pai->cor = PRETO;
              //if (irmao->dir != NULL)
              irmao->dir->cor = PRETO;
              noAsc = no->pai->pai;
              noAux = rotEsquerda(no->pai);
              if (noAsc->tipo != NO_NULO){
                  if (noAsc->esq == no->pai)
                      noAsc->esq = noAux;
                  else
                      noAsc->dir = noAux;
              }
              noAux->pai = noAsc;
              if (noAsc->tipo == NO_NULO) raiz = noAux;
              no = raiz;
          }
      }
      else { //sub-árvore da direita
          irmao = no->pai->esq;
          if (irmao->cor == VERMELHO) {//caso 1
              irmao->cor = PRETO;
              no->pai->cor = VERMELHO;
              noAsc = no->pai->pai;
              noAux = rotDireita(no->pai);
              ajustaNoPai(noAsc, noAux);
              if (noAsc->tipo == NO_NULO) raiz = noAux;
              irmao = no->pai->esq;
          }
          if (irmao->dir->cor == PRETO && irmao->esq->cor == PRETO){ //caso 2
              irmao->cor = VERMELHO;
              no = no->pai;
          }
          else {
              if (irmao->dir->cor == VERMELHO && irmao->esq->cor == PRETO) { //caso 3
                  irmao->cor = VERMELHO;
                  irmao->dir->cor = PRETO;
                  noAsc = irmao->pai;
                  noAsc->esq = rotEsquerda(irmao);
                  irmao = no->pai->esq;
              } //caso 4
              irmao->cor = no->pai->cor;
              no->pai->cor = PRETO;
              //if (irmao->esq != NULL)
              irmao->esq->cor = PRETO;
              noAsc = no->pai->pai;
              noAux = rotDireita(no->pai);
              if (noAsc->tipo != NO_NULO){
                  if (noAsc->esq == no->pai)
                      noAsc->esq = noAux;
                  else
                      noAsc->dir = noAux;
              }
              noAux->pai = noAsc;
              if (noAsc->tipo == NO_NULO) raiz = noAux;
              no = raiz;
          }
      }
  }
  no->cor = PRETO;
  return raiz;
}

struct tNo* exclui (struct tNo *no, struct tNo *raiz) {
    bool cor_no = no->cor; //armazena a cor do nó
    struct tNo *s, *novaRaiz = raiz;
    struct tNo *filhoAjuste = NULL;
    struct tNo *paiAjuste = NULL;
    if (no->esq->tipo == NO_NULO && no->dir->tipo == NO_NULO && no->pai->tipo == NO_NULO){
        freeNo(no); //exclusão da raiz, último nó
        return NULL;
    }
    if (no->esq->tipo == NO_NULO){     
        filhoAjuste = no->dir;
        ajustaNoPai(no, no->dir);
        if (no == raiz) novaRaiz = filhoAjuste;
       freeNo (no);
    } else {
        if (no->dir->tipo == NO_NULO){
            filhoAjuste = no->esq; 
            ajustaNoPai(no, no->esq);
            if (no == raiz) novaRaiz = filhoAjuste;
            freeNo(no);
        }
        else {   
            s = sucessor (no);
            cor_no = s->cor;
            filhoAjuste = s->dir;
            ajustaNoPai(s, filhoAjuste);
            s->esq = no->esq;
            s->esq->pai = s;
            //if (no->dir != s)
            s->dir = no->dir;
            s->dir->pai = s;
            ajustaNoPai(no, s);
            s->cor = no->cor; // o sucessor fica com a cor do nó
            if (no == raiz) novaRaiz = s;
            free(no);
        }
    }        
    if (cor_no == PRETO)
        novaRaiz = ajustaExclusao(filhoAjuste, novaRaiz);
    return novaRaiz;

struct tNo *busca(struct tNo *no, int chave){
  if (no->tipo == NO_NULO) return NULL;
  if (no->chave == chave) return no;
  if (chave < no->chave)
      return busca (no->esq, chave);
  else
      return busca (no->dir, chave);
}

// Renomeado de sucessor e lógica alterada
struct tNo *antecessor(struct tNo *no) {
    // Encontra o nó máximo na subárvore esquerda
    if (no == NULL || no->esq == NULL || no->esq->tipo == NO_NULO) return NULL; // Não deve acontecer se chamado corretamente do caso 3 de exclui
    struct tNo *temp = no->esq;
    while (temp->dir != NULL && temp->dir->tipo != NO_NULO) {
        temp = temp->dir; // Vai para a direita o máximo possível
    }
    return temp;
}

int token_to_num(const char *str, int *i) {
    // Implementação necessária: Analisar inteiro da string, avançar índice i
    // Exemplo de placeholder:
    int num = 0;
    // Pula caracteres não numéricos (como espaços)
    while (str[*i] != '\0' && (str[*i] < '0' || str[*i] > '9') && str[*i] != '-') {
        (*i)++;
    }
    bool negative = false;
    if (str[*i] == '-') {
        negative = true;
        (*i)++;
    }
    while (str[*i] >= '0' && str[*i] <= '9') {
        num = num * 10 + (str[*i] - '0');
        (*i)++;
    }
     // Pula caracteres não numéricos finais (como espaços) antes do próximo token
    while (str[*i] != '\0' && (str[*i] < '0' || str[*i] > '9')) {
         if (str[*i] == '-') break; // Para se o próximo caractere for um sinal de menos para o próximo número
        (*i)++;
    }
    return negative ? -num : num;
}

struct tNo *min(struct tNo *no){
    if (no->esq->tipo == NO_NULO) return no;
    else
        return min(no->esq);
}

// Função adicionada para liberar os nós da árvore recursivamente
void freeTree(struct tNo *no) {
    if (no == NULL || no->tipo == NO_NULO) {
        // Libera nós NULOS se forem alocados dinamicamente e não compartilhados
        // Assumindo que criaNoNulo aloca nós únicos que precisam ser liberados
        if (no != NULL) free(no);
        return;
    }
    freeTree(no->esq);
    freeTree(no->dir);
    free(no);
}

void ajustaNo(struct tNo *no, struct tNo *novo){ 
    if (no->tipo != NO_NULO) {
        if (no->esq == no)
            no->esq = novo;
        else 
            no->dir = novo;
        if (novo != NULL)
           novo->pai = no;
    } else
        novo->pai = no->pai;
}


//faz a busca em ordem :)
void printEmOrdem(int vetor[], struct tNo *raiz, int i){
    if (!raiz){
        printf("raiz nula!\n");
        return;
    }
    if(raiz->esq->tipo != NO_NULO)
        printEmOrdem(vetor, raiz->esq, i);
    vetor[i] = raiz->chave;
    i++;
    if(raiz->dir->tipo != NO_NULO)
        printEmOrdem(vetor,raiz->dir, i);
}

void printVetor(int vetor[]){
    printf("\n");
    for (int i = 0; i < TAM; i++) {
        printf("%d; ", vetor[i]);
    }
    printf("\n");
}
