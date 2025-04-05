org 0x7C00
bits 16
%define ENDL 0x0D, 0x0A

jmp short main
nop

; FLOPPY DISK FAT12 HEADER
bdb_oem:                    db "abcdefgh"           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0


; extended boot record
ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
ebr_volume_label:           db 'NEXUS OS   '        ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '           ; 8 bytes, padded with spaces


main:
    ; set up data segments
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; set up stack
    mov ss, ax
    mov sp, 0x7C00
    mov bp, sp


    mov si, message
    call puts

    ; read FAT root directory
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx                          ; ax = ax * bx(lba of root directory)
    add ax, [bdb_reserved_sectors]
    push ax

    mov ax, [bdb_dir_entries_count]
    shl ax, 5
    xor dx, dx
    div word [bdb_bytes_per_sector] ; ax = sizeof(root directory) / BytesPerSector
                                    ; dx = sizeof(root directory) % BytesPerSector
    test dx, dx
    jz .root_dir_after
    inc ax
.root_dir_after:
    mov cl, al
    pop ax
    mov dl, [ebr_drive_number]
    mov bx, buffer
    call disk_read

    xor bx, bx
    mov di, buffer
.search_kernel:
    mov si, file_kernel_bin
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .found_kernel
    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    jmp kernel_not_found_error

.found_kernel:
    mov ax, [di + 26]
    mov [kernel_cluster], ax

    ;Read FAT table
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    mov ax, [kernel_cluster]
    ;bad bad
    add ax, 31

    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx

    mov si, buffer
    add si, ax
    mov ax, [ds:si]

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after
.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8
    jae .done
    jmp .load_kernel_loop
.done:
    mov dl, [ebr_drive_number]
    mov ax, KERNEL_LOAD_SEGMENT
    mov ds, ax
    mov es, ax

    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

    jmp await_key_and_reboot
    cli
    hlt

;
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



;
; DISK RELATED
;


;
    ; Converts an LBA address to a CHS address
    ; Parameters:
    ;   - ax: LBA address
    ; Returns:
    ;   - cx [bits 0-5]: sector number
    ;   - cx [bits 6-15]: cylinder
    ;   - dh: head
    ;

    lba_to_chs:

        push ax
        push dx

        xor dx, dx                          ; dx = 0
        div word [bdb_sectors_per_track]    ; ax = LBA / SectorsPerTrack
                                            ; dx = LBA % SectorsPerTrack

        inc dx                              ; dx = (LBA % SectorsPerTrack + 1) = sector
        mov cx, dx                          ; cx = sector

        xor dx, dx                          ; dx = 0
        div word [bdb_heads]                ; ax = (LBA / SectorsPerTrack) / Heads = cylinder
                                            ; dx = (LBA / SectorsPerTrack) % Heads = head
        mov dh, dl                          ; dh = head
        mov ch, al                          ; ch = cylinder (lower 8 bits)
        shl ah, 6
        or cl, ah                           ; put upper 2 bits of cylinder in CL

        pop ax
        mov dl, al                          ; restore DL
        pop ax
        ret


;
    ;reads data from the disk at the specified LBA address
    ; params:
        ; ax = LBA address 
        ; cl = number of sectors to read
        ; dl = drive number
        ; es:bx = where to store the read data
    disk_read:
        push ax
        push bx
        push cx
        push dx
        push di

        push cx
        call lba_to_chs
        pop ax

        mov ah, 0x02
        mov di, 3
        jmp .retry
    .retry:
        pusha
        stc
        int 0x13
        jnc .done

        popa
        call disk_reset
        dec di
        test di, di
        jnz .retry
    .fail:
        jmp floppy_error
    .done:
        popa

        pop di
        pop dx
        pop cx
        pop bx
        pop ax
        ret


;
    ; resets any errors that occurred when dealing with a disk
    ; params:
        ; dl = drive number
    disk_reset:
        pusha
        mov ah, 0
        stc
        int 0x13
        jc floppy_error
        popa
        ret



;
; ERROR HANDLERS
;


floppy_error:
    mov si, message_read_failed
    call puts
    jmp await_key_and_reboot

kernel_not_found_error:
    mov si, msg_kernel_not_found
    call puts
    jmp await_key_and_reboot

await_key_and_reboot:
    mov ah, 0
    int 0x16
    jmp 0xFFFF:0
    cli
    hlt


message: db 'Booting Nexus OS!', ENDL, 0
message_read_failed: db 'Failed to read from disk.', ENDL, 0
msg_kernel_not_found:   db 'KERNEL.BIN file not found!', ENDL, 0
file_kernel_bin: db 'KERNEL  BIN'
kernel_cluster: dw 0

KERNEL_LOAD_SEGMENT equ 0x2000
KERNEL_LOAD_OFFSET equ 0

; pad the program until 510 bytes
times 510-($-$$) db 0
dw 0xAA55

buffer: