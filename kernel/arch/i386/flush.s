;
; Gdt.s -- contains global descriptor table and interrupt descriptor table
;          setup code.
;          Based on code from Bran's kernel development tutorials.
;          Rewritten for JamesM's kernel development tutorials.

global gdt_flush
extern gdt_ptr

gdt_flush:
    lgdt [gdt_ptr]        ; Load the new GDT pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax        ; Load all data segment selectors
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush   ; 0x08 is the offset to our code segment: Far jump!

flush:
    ret


global idt_flush
extern idt_ptr

idt_flush:
  lidt [idt_ptr]
  ret
