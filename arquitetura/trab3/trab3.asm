v.add v3,v0 // v3 = {0, 1, 2, 3}, valores que serão guardados
v.add v2,v0 // v2 = {0, 1, 2, 3}, endereço que serão guardados
s.movl 0011 // r1 = 3
s.add r2,r1 // r2 = 3
v.movl 0100 //inicia v1 = 4, vai ser o incrementador de endereço
s.movl 0011 // r1 = 3
s.add r3, r1 // r3 = 3

Loop_A_B:
    v.st v3,v2  // guarda os valores de v3 em m[v2]
    v.add v2,v1 // soma 4 nos endereços
    v.add v3,v1 //soma 4 nos valores
    addri r3, -1 // subtrai 1 de r3(contador)
    addri r2, -1 // subtrai 1 de r2(verificador se A ou B)
    s.bnzr 0,-3 // volta par o inicio do loop se r3 != 0

Fim_loop:
    bnze 1,2 // se r2 != 0, vai para Soma
    v.movl 1000 // v1 = 8
    v.add v3,v1 // v3 = {20,21,22,23} 
    s.movl 0100 // r1 = 4
    brzr r0,r1

Soma:
    s.add r3,r1 // r3 = 3
    s.movl 1011
    s.movh 0001 // vira a primeira instrução do loop soma + 3 = 27;
    s.add r2,r1; // vira a primeira instrução do loop soma;
    s.movl 0111
    s.movh 0010 // r1 = fim do programa;

Loop_soma:
    brzr r3, r1  // se r3==0, pula para o fim do programa 
    v.movl 1100;   // v1 = 12 
    s.addri r3, -1 // subtrai 1 no contador r3
    sub v2,v1; // v2 = End(B[i]) [][]
    v.ld v3,v2 // v3 = B[i]
    sub v2,v1; // v2 = End(A[i])
    v.ld v1,v2 // v1 = A[i]
    v.add v3,v1 // v3 = A[i] + B[i]
    v.movl 1100;   // v1 = 12
    v.add v2,v1 // v2 = End(B[i])
    v.add v2,v1 // v2 = End(R[i])
    v.st v3,v2; // R[i] = v3
    v.movl 0100;
    v.add v2,v1 // v2 = End(R[i+1])
    brzr r0,r2;

fim_programa:
    brzr r0,r1

//40 instruções







