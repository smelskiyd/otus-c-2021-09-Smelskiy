    bits 64
    extern malloc, puts, printf, fflush, abort
    global main

    section   .data
empty_str: db 0x0
int_format: db "%ld ", 0x0
data: dq 4, 8, 15, 16, 23, 42
data_length: equ ($-data) / 8

    section   .text
;;; print_int proc
print_int:
    mov rsi, rdi
    mov rdi, int_format
    xor rax, rax
    call printf

    xor rdi, rdi
    call fflush

    ret

;;; p proc
p:
    mov rax, rdi
    and rax, 1
    ret

;;; add_element proc
;;; add_element(rdi = current value, rsi = pointer to the previously added element)
add_element:
    push rbp                ; save frame pointer
    push rbx                ; save base pointer

    mov rbp, rdi            ; save input value to rbp
    mov rbx, rsi            ; save rsi to rbx

    mov rdi, 16
    call malloc             ; malloc 16 bytes of memory
    test rax, rax           ; rax is the pointer to just allocated memory
    jz abort                ; if malloc returned NULL pointer -> abort

    mov [rax], rbp          ; place current value in [rax] (just allocated memory)
    mov [rax + 8], rbx      ; save pointer to the previously added element to rax+8

    pop rbx
    pop rbp

    ret

;;; m proc
;;; m(rdi = pointer to the list element, rsi = print_int label)
m:
    test rdi, rdi
    jz outm                 ; if it's NULL element -> return

    push rbp                ; save frame pointer
    push rbx                ; save base pointer

    mov rbx, rdi            ; temporally save rdi
    mov rbp, rsi            ; temporally save rsi

    mov rdi, [rdi]          ; rdi = current element value
    call rsi                ; print rdi

    mov rdi, [rbx + 8]      ; get next list element pointer
    mov rsi, rbp            ; pass print_int label to the next 'm' call
    call m                  ; recursively call 'm' for the next list element

    pop rbx
    pop rbp

outm:
    ret

;;; f proc
;;; f(rdi = list element pointer, rsi, rdx = 'p' label)
f:
    mov rax, rsi

    test rdi, rdi
    jz outf                 ; return if it's the NULL element

    push rbx                ; save some values
    push r12                ; ^
    push r13                ; ^

    mov rbx, rdi            ; temporaly save input arguments
    mov r12, rsi            ; ^
    mov r13, rdx            ; ^

    mov rdi, [rdi]          ;
    call rdx                ; call 'p' function
    test rax, rax           ;
    jz z                    ; if current list value is even -> jump to z

    mov rdi, [rbx]          ; move current element value as the first argument of add_element
    mov rsi, r12            ; move pointer of the first element of the second list as the second argument of add_element
    call add_element        ; add current value to the new list (list of odd values)
    mov rsi, rax
    jmp ff                  ; recursively call 'f'

z:
    mov rsi, r12

ff:
    mov rdi, [rbx + 8]
    mov rdx, r13
    call f                  ; recursively call 'f'

    pop r13                 ; restore previously saved values
    pop r12                 ; ^
    pop rbx                 ; ^

outf:
    ret

;;; main proc
main:                               ; main function
    push rbx

    xor rax, rax
    mov rbx, data_length                ; actual array element id
adding_loop:                            ; loop
    mov rdi, [data - 8 + rbx * 8]       ; move last element of the array to rdi (1-st input argument)
    mov rsi, rax                        ; move rax to rsi (2-nd input argument)
    call add_element                    ; call add_element procedure
    dec rbx                             ; decrement rbx
    jnz adding_loop                     ; if rbx != 0 we complete the cycle (not the last element of the array)

    mov rbx, rax                        ; save the pointer to the last added element

    mov rdi, rax                        ; pass pointer to the first list element as the first 'm' argument
    mov rsi, print_int                  ; pass print_int label as the second 'm' argument
    call m                              ; print list values

    mov rdi, empty_str
    call puts                           ; put the end of the line character

    mov rdx, p                          ; 3-d argument to the function: 'p' label
    xor rsi, rsi                        ; reset rsi value (2-d argument)
    mov rdi, rbx                        ; 1-st argument to the function: first list element pointer
    call f

    mov rdi, rax
    mov rsi, print_int
    call m                              ; print list values again

    mov rdi, empty_str
    call puts                           ; put the end of the line character

    pop rbx

    xor rax, rax
    ret
