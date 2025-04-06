main: 
    addi 5
    add r3,r0 ; r3 = 5
    add r3,r0 ; r3 = 10
    add r1,r3
    add r1, r1 
    add r1, r1 
    add r1, r1 
    addi para o que falta
    addi -4 # r0 = 1
    jump loop guarda
    addi para deixar r0 = 10
    add r3,r0 ; r3 = 10
    and r0,r2 # r0 = 0
    addi 1 # r0 = 1
    add r2,r0 # r2 = 1
    jump loop_guarda
    jump soma
    jump loop_guarda

fim_vetor_A_ou_B:
    addi -6 # lugar onde para se r2 == 18
    addi 6 # lugar onde para se r2 == 19
    addi 6
    brzr r3, r0

jump loop_guarda


soma: 
    setar r3 para 10
    addi para fazer r0 = 1

loop_guarda: ; r1 sai dessa função apontando para o último elemento do vetor e com r0 = 2. r0 e r3 são resetados
    st r2,r1 ; guarda o valor de r2 em r1
    brzr r3, r2
    addi tirar lugar para onde voltar
    add r1,r0 ; aumenta em 1 o endereço , "avançando na stack" 
    sub r3,r0
    addi 1 # r0 = 2
    add r2, r0 ;soma 2 no registrador r2
    ji -8

loop_soma:
    add r1, r0 ; r1 vai quardar o endereço de R[t].
    sub r1,r3 ; end(B[t])
    ld r2, r1  ; r2 = B[t]
    sub r1, r3 ; end(A[t])
    ld r0,r1 ; r0 = A[t]    
    add r2,r0 ; r2 = A[t] + B[t]
    add r1,r3
    add r1,r3 ; r1 = end(R[t])
    st r2,r1 ; R[t] = A[t] + B[t]
    jump -8

Fim:
