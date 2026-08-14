global _start

section .bss
x resd 1
y resd 1

section .text

_start:
mov EAX, 10
mov [x], EAX
mov EAX, [x]
mov EBX, 20
add EAX, EBX
mov [y], EAX
.L_while_start_0:
mov EAX, [x]
mov EBX, 20
cmp EAX, EBX
jge .L_while_end_1
mov EAX, [x]
mov EBX, 1
add EAX, EBX
mov [x], EAX
jmp .L_while_start_0
.L_while_end_1:

mov EAX, 1
xor EBX, EBX
int 0x80
