main:
    add2i 5   ; r0 = 10
    set r3,r0 ; r3 = 10; condicional do loop
    add2i numeros de instrução
    set r2, r0
    sub r0,r0 ; zera r0
    ji loop_guarda


loop_guarda_A_B:    
    addr1 0  ; aumenta em 1 o endereço
    subr1 1  ; r3 -= 1 conta 1 laço do loop 
    st r0,r2 ; guarda o valor de r0 em r2
    addi 2   ;
    brzr 4   ;

Fim_loop_A:
    add1 0   ; define para o proximo o loop

soma:
    add r3 ;r3 = 10 
    set r0,r2 ; r0 = end  

loop_soma: ; 
    sub1 1 ; conta um laço do loop
    addi 1 ; aumenta em 1 o endereço
    sub2i 5; r0 = end(B[t])
    ld r2, r0 ; r2 = B[t]
    sub2i5 ; r0 = end(A[t])
    ld r1, r0 ; r1 = A[t]
    add r0,r1 ; r0 = B[t] + A[t]
    afx4 r2 ; r2 = end(R[t])
    st r0,r2 ; R[t] = B[t] + A[t]
    sub r1,r1;
    afx6 r1; 
    addi -1 ; r0 = end(R[t-1])
    brzr r3, r1 ; 

fim_programa: 
    ji 0; finaliza o programa, igual a ultima instrução