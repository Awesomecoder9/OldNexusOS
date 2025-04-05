org 0x0
bits 16
%define ENDL 0x0D, 0x0A


main:
    mov si, message_kernel_boot
    call puts
    cli
    hlt


; prints a string to the screen
; params:
;   ds:si points to a string
puts:
    ; saves the registers we are going to modify on the stack 
    push si
    push ax
.loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp .loop
.done:
    ; restores the registers we modified
    pop ax
    pop si
    ret

message_kernel_boot: db 'Booting Nexus Kernel!', ENDL, 0

