#endereços são guardados no registrador r1
#r0 quarda valores a serem colocados nos vetores
#r2 guarda 1
#r3 guarda 2
main:
    addi A
    and r1, 0 # limpando r1
    add r1, r0

    and r2, 0 # limpa os registradores
    or r2, 1 # r2 = 1
    and r3, 0 
    or r3, 2 # r3 = 2
    or r1, 0

    addi 0 # inicializa o valor inicial como 0
    loop_guarda
    addi B
    and r1, 0
    add r1,r0
    addi 1 # inicializa o valor inicial como 1
    loop_guarda
    addi R
    and r1, 0 # r1 = R
    add r1, r0
    addi A # r2 = A
    and r2, 0
    add r2, r0
    and r3, 0
    loop_soma

loop_guarda:
    st r1,r0 # guarda o valor de r0 em r1
    add r1,r2 # avança em 1 o endereço
    add r0, r3 #soma 2 no registrador r0
    loop_guarda

# aqui o r3 guarda o endereço de b
loop_soma:
    addi A
    and r1, 0 # r1 = a
    add r1, r0=
    add r1,r3 #r1 = end(A[r3])
    ld r1, r1 # r0 = A[r3]
    addi B
    and r2, 0 # r2 = B
    add r2, r0 
    add r2,r3 #r1 = end(B[r3])
    ld r2, r2 # r2 = B[r3] ,
    add r1,r2 # r1 = r1 + r2 / a[r3] + b[r3]
    addi R
    and r2, 0 # r2 = R
    add r2, r0 
    add r2,r3 #r1 = end(R[r3])
    ld r2, r1 # R[r3] = a[r3] + b[r3]
    addi 1
    add r3,r0 # r3 = r3 + 1
    loop_soma
    

