main: 
    addi 10
    loop_soma

loop_soma:
    
    add r1, r0 ; r1 vai quardar o endereço de R[t].
    brzr r1, 
    sub r1,r3 ; end(B[t])
    ld r2, r1  ; r2 = B[t]
    sub r1, r3 ; end(A[t])
    ld r0,r1 ; r0 = A[t]
    add r2,r0 ; r2 = A[t] + B[t]
    add r1,r3
    add r1,r3 ; r1 = end(R[t])
    st r2,r1 ; R[t] = A[t] + B[t]
    jump loop_soma

fim:
    ; Fim do programa

; Dados: vetores A, B e R alinhados na memória
inicioA:
    .bits8 0x00 0x02 0x04 0x06 0x08 0x0a 0x0c 0x0e 0x10 0x12 ; Vetor A
inicioB:
    .bits8 0x01 0x03 0x05 0x07 0x09 0x0b 0x0d 0x0f 0x11 0x13 ; Vetor B
inicioR:
    .space 10            ; Espaço reservado para vetor R