main:
    addi 6    ; r0 = 6 
    add2i 7   ; r0 = 20
    add r3,r0 ; r3 = 20 (r3 vai ser os valores a serem armazenados)
    add2i 7   ; r0 = 34
    add2i 5   ; r0 = 44

loop_guarda:    
    addi -1       ; diminui em 1 o endereço para voltar nos vetores
    addri r3, -1  ; deixa r3 pronto para ser colocado em B  
    st r3,r0      ; guarda o valor do vetor B
    add2i -5      ; diminui em 10 o endereço, para chegar no vetor A
    addri r3, -1  ; r3 -= 1, para ser armazenado em A 
    st r3,r0      ; guarda o valor do vetor A
    add2i 5       ; avança em 10 o endereço, para voltar para B
    bnzi 7        ; controla o loop

loop_soma: 
    addi -1   ; volta em 1 o endereço
    ld r3, r0 ; r0 = A[t]
    add2i 5   ; r0 = end(B[t])
    ld r1, r0 ; r1 = B[t]
    add r1,r3 ; r1 = B[t] + A[t]
    add2i 5   ; r0 = end(R[t])
    st r1,r0  ; R[t] = B[t] + A[t]
    add2i -5   ; r0 = end(B[t - 1])
    add2i -5   ; r0 = end(A[t - 1])
    bnzi 9    ; controla o loop 

fim_programa: 
    ji 0; finaliza o programa, igual a ultima instrução