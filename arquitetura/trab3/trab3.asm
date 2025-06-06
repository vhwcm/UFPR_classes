v.movl 4 //inicia v1 = 4, vai ser o incrementador de endereço
v.add v3,v0 // v3 = {0, 1, 2, 3}, valores que serão guardados
v.add v2,v0 // v2 = {0,1,2,3}, endereço que serão guardados
s.movl 3 // r1 = 3
s.add r2,r1 // r2 = 3
s.add r3, r1 // r3 = 3

Loop_A_B:
    v.ld v3,v2  // guarda os valores de v3 em m[v2]
    v.add v2,v1 // soma 4 nos endereços
    v.add v3,v1 //soma 4 nos valores
    addri 0, -1 // subtrai 1 de r3(contador)
    addri 1, -1 // subtrai 1 de r2(verificador se A ou B)
    s.bnze r3,-4 // loop

fim_loop:
    bnze r2, 4 // se fim a r2 == 0, se não r2 != 2
    v.movl 16 // v1 = 16
    v.add v3,v1 // v3 = {20,21,22,23} 
    s.movl r1 = Loop_A_B - 1
    brzr r0,r1; 

soma:
    s.add r2,r1;
    s.movl -3;
    add r3,r1;
    s.movh Loop_soma;
    s.add r2,r1;
    s.movh fim_programa;


Loop_soma:
    brzr
    v.movh 12;   // v1 = 12 
    s.addri 0, 1 // soma 1 no contador r3
    sub v2,v3; // v2 = End(B[i]);
    load v3,v2 // guarda em r1 o Valor(v2);
    sub v2,v3; // v2 = End(A[i]);
    load v1,v2 // guarda em r1 o Valor(v2);
    v.add v3,v1 // v3 = A + B
    v.movh 12;   // v1 = 12
    v.add v2,v1 // v2 = End(B[i])
    v.add v2,v1 // v2 = End(R[i])
    v.store v2,v3; // R[i] = v3
    v.movl 4;
    v.add v2,v1 // v2 = End(R[i+1])
    brzr r0,r2;

fim_programa:
    j 0;
36 instruções







