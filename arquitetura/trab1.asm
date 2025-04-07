main: 
    addi -1
    add r3, r0
    addi 5 ; r0 = 4
    slr r0,r0 ; r0 = 4 x 16 = 64
    addi -4
    ld r0,r3
    add r1,r0
    sub r0,r0
    addi 5
    addi 5
    jump loop guarda
    sub r2,r2 # r2 = 0 
    addi 5
    addi 5
    sub r2,r3 # r2 = 1
    add r1,r0 # r1 = B[]
    jump 6

fim_vetor_A_ou_B:
    ji -8

soma: 
    addi 6
    addi 6
    add r3,r0 ; r3 = 11 
    ji 2
    ji 7 
    sub r0,r0 ; r0 = 0
    ji 6

loop_guarda: ; r1 sai dessa função apontando para o último elemento do vetor e com r0 = 2. 
    sub r1,r3 ; aumenta em 1 o endereço
    st r2,r1 ; guarda o valor de r2 em r1
    brzr r0, r2
    add r0,r3
    ji 2 ; ignorar o pular para loop_soma
    ji 4 ; pular para loop_soma
    sub r2, r3 ;soma 1 no registrador r2
    sub r2, r3 ;soma 1 no registrador r2
    ji -8

loop_soma: 
    sub 1 ; r0 = -1
    sub r1, r0 ; r1 vai guardar o endereço de R[t]. r1 += 1
    add r3,r0 ; r3 -= 1
    ld r0,r0 ; r0 = mem[-1]
    brzr r3,r0
    sub r0,r0
    addi 5 ; r0 = 5
    ji 2
    ji -8
    addi 5 ; r0 = 10
    sub r1,r0 ; end(B[t])
    ld r2, r1  ; r2 = B[t]
    sub r1, r0
    ld r0,r1 ; r0 = A[t]    
    add r2,r0 ; r2 = A[t] + B[t]
    ji 2
    ji -8
    sub r0,r0 ; r0 = 0
    addi 3
    addi 7 ; r0 = 10
    add r1,r0
    add r1,r0 ; r1 = end(R[t])
    st r2,r1 ; R[t] = A[t] + B[t]
    sub r0,r0 ; r0 = 0
    ji -8

Fim:
    ji 0 ;
