; Soma de vetores A e B, salvando em R
; Vetores A, B e R estão alinhados às especificações do REDUX-V

main:
    addi r0, 10         ; Inicializa o contador em r0 com 10 (número de elementos)
    addi r1, inicioA    ; r1 aponta para o início do vetor A
    addi r2, inicioB    ; r2 aponta para o início do vetor B
    addi r3, inicioR    ; r3 aponta para o início do vetor R

loop_soma:
    ld r1, r1           ; Carrega A[i] em r1
    ld r2, r2           ; Carrega B[i] em r2
    add r1, r1, r2      ; Soma A[i] + B[i] e armazena em r1
    st r1, r3           ; Salva o resultado no vetor R em r3
    addi r1, r1, 1      ; Incrementa o ponteiro para A
    addi r2, r2, 1      ; Incrementa o ponteiro para B
    addi r3, r3, 1      ; Incrementa o ponteiro para R
    sub r0, r0, 1       ; Decrementa o contador
    brzr r0, fim        ; Se o contador for zero, encerra o loop
    ji loop_soma        ; Continua o loop

fim:
    ; Fim do programa

; Dados: vetores A, B e R alinhados na memória
inicioA:
    .bits8 0x00 0x02 0x04 0x06 0x08 0x0a 0x0c 0x0e 0x10 0x12 ; Vetor A
inicioB:
    .bits8 0x01 0x03 0x05 0x07 0x09 0x0b 0x0d 0x0f 0x11 0x13 ; Vetor B
inicioR:
    .space 10            ; Espaço reservado para vetor R
