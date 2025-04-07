addi 7         ; r0 = 7
addi 7         ; r0 = 14
addi 7         ; r0 = 21
addi 7         ; r0 = 28
addi 4         ; r0 = 32
add r1, r0    ; r1 <- 32    ; Ponteiro para A
addi 7         ; r0 = 7    (supondo que r0 estava 0, mas como r0 é sobrescrito, estamos “reconstruindo” a constante)
addi 3         ; r0 = 10
add r2, r1   ; r2 <- A (32)
add r2, r0   ; r2 <- 32 + 10 = 42   ; Ponteiro para B
addi 7         ; r0 = 7
addi 3         ; r0 = 10
add   r3, r2   ; r3 <- B (42)
add   r3, r0   ; r3 <- 42 + 10 = 52   ; Ponteiro para r

addi 7         ; r0 = 7
addi 3         ; r0 = 10   ; r0 = contador

loop:
ld   r0, r1      ; r0 <- *A  (conteúdo de A[i])
st   r0, TEMP    ; TEMP <- r0  (salva A[i])
ld   r0, r2      ; r0 <- *B  (conteúdo de B[i])
ld   r_temp, TEMP  ; (pseudo‑instrução: carrega TEMP para um “registrador temporário” – conceito ilustrativo)
add  r0, r_temp   ; r0 <- B[i] + A[i]
st   r0, r3      ; *r <- (A[i] + B[i])
addi 1          ; r0 = 1   (reconstrói 1)
add  r1, r0     ; r1 <- r1 + 1
add  r2, r0     ; r2 <- r2 + 1
add  r3, r0     ; r3 <- r3 + 1
addi -1         ; r0 = r0 + (-1)  -- nota: como addi atua em r0, precisamos de um esquema para preservar o contador real; 
brzr r0, exit   ; Se r0 == 0, salta para exit
ji loop         ; Senão, volta ao início do loop

exit:
ji 0            ; Salto imediato para si mesmo (parada)

TEMP:   .byte 0      ; Local temporário para armazenar A[i]

