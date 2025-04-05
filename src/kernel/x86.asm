bits 16


global _x86_WriteCharTeletype
; arguments:
    ;page
    ;character
_x86_WriteCharTeletype:
    push bp
    mov bp, sp

    push bx

    
    mov ah, 0x0E        ; call bios interrupt
    mov bh, [bp + 6]
    mov al, [bp + 4]          
    int 0x10

    pop bx

    mov sp, bp
    pop bp
    ret