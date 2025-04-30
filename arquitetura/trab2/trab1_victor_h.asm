main:
    addi 6 ; r0 = 6 
    add2i 7; r0 = 20
    set r3,r0 ; r3 = 20 (r3 vai ser os valores a serem armazenados)
    add2i 7; r0 = 34
    add2i 5; r0 = 44

loop_guarda:    
    sub1 r3  ; deixa r3 pronto para ser colocado em B  
    st r3,r0 ; guarda o valor do vetor B
    add2i -5 ; diminui em 10 o endereço, para chegar no vetor A
    sub1 r3  ; r3 -= 1, para ser armazenado em A 
    st r3,r0 ; guarda o valor do vetor A
    add2i 5  ; avança em 10 o endereço, para voltar para B
    addi -1  ; diminui em 1 o endereço para voltar nos vetores
    brzr 7 

loop_soma: 
    ld r3, r0 ; r0 = A[t]
    add2i 5   ; r0 = end(B[t])
    ld r1, r0 ; r1 = B[t]
    add r1,r3 ; r1 = B[t] + A[t]
    add2i 5   ; r0 = end(R[t])
    st r1,r0  ; R[t] = B[t] + A[t]
    addi -1   ; volta em 1 o endereço
    add2i 5; r0 = end(B[t - 1])
    add2i 5; r0 = end(A[t - 1])
    brzr 9 ; 

fim_programa: 
    ji 0; finaliza o programa, igual a ultima instrução