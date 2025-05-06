    ji main   ; pula para a main
    ebreak    ; finaliza o programa
    addi 7    ; r0  = 7
    addi 3   ; r0 = 10
    add r2,r0 ; r2 = 10 , vai ser o controlador do loop
    add r0,r0 ; r0 = 20
    add r0,r0 ; r0 = 40 ; tem 41 instruções [0..40]
    add r1,r0 ; passa o ponteiro da ultima instrução para r1
    sub r0,r0 ; zera r0, para acumular os valor de B[]
    addi 1    ; começa com o valor 1
    add r3,r0 ; seta r3 para 1, será usado para incrementar o endereço
    ji loop_guarda
    addi -8   ; r0 = 11
    addi -1   ; r0 = 10
    add r2,r0 ; r2 = 10
    sub r0,r0 ; seta r0 para 0, para começar o acumulador do A  
    add r1,r3 ; aumenta em 1 o endereço
    ji salto
    ji soma
    ji fim_vetor_B
    sub r2,r3 ; conta 1 laço do loop 
    st r0,r1  ; guarda o valor de r0 em r1
    brzr r2, r0 verificação do laço
    addi 2
    ji loop_guarda
    addi -8   ; r0 = 10
    add r2,r0 ; r2 = 10
    add r0,r1 ; r0 = end(R[t]), começando pelo final do vetor 
    sub r0,r2 ; r0 = end(A[t])
    ld r3, r0 ; r3 = A[t]
    sub r0,r2 ; r0 = end(B[t])
    ld r1, r0 ; r1 = B[t]
    ji pulo_baixo
    ji loop_soma
    add r1,r3 ; r1 = A[t] + B[t]
    add r0,r2 ; r0 = end(A[t])
    add r0,r2; r0 = end(R[t])
    st r1,r0
    brzr r3, r1 ; quando chegar acabar, r3 será 0 e r1 será 1, finalizando o programa
    addi -1 ; r0 = end(R[t-1])
    ji pulo_cima
