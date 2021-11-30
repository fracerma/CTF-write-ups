typedef unsigned char   undefined;

typedef unsigned char    byte;
typedef unsigned char    dwfenc;
typedef unsigned int    dword;
typedef unsigned long    qword;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned int    undefined4;
typedef unsigned long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    word;
typedef struct struct_move struct_move, *Pstruct_move;

struct struct_move {
    char * name;
    void (* function)(undefined8 *, undefined *);
};

typedef struct struct_pkm struct_pkm, *Pstruct_pkm;

struct struct_pkm {
    ulonglong atk;
    ulonglong def;
    ulonglong hp;
    ulonglong max_hp;
    char status;
    char * name;
    ulonglong IVs[5];
    struct struct_move move[10];
};


void tackle(undefined8 *param_1,long param_2)

{
  uint local_c;
  
  local_c = (int)*param_1 - (int)*(undefined8 *)(param_2 + 8);
  if ((int)local_c < 0) {
    local_c = 0;
  }
  *(long *)(param_2 + 0x10) = *(long *)(param_2 + 0x10) - (long)(int)local_c;
  if ((int)local_c < 1) {
    printf("[%%] %s is safe!\n",*(undefined8 *)(param_2 + 0x28));
  }
  else {
    printf("[%%] %s loses %d hp\n",*(undefined8 *)(param_2 + 0x28),(ulong)local_c);
  }
  return;
}



void * get_string(void)

{
  long in_FS_OFFSET;
  uint local_20;
  uint local_1c;
  void *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_20 = 0;
  while (local_20 == 0) {
    printf("[.] insert length: ");
    __isoc99_scanf(&DAT_0040204e,&local_20);
  }
  local_18 = malloc((ulong)local_20);
  local_1c = 0;
  while ((local_1c < local_20 &&
         (read(0,(void *)((long)local_18 + (ulong)local_1c),1),
         *(char *)((long)local_18 + (ulong)local_1c) != '\n'))) {
    local_1c = local_1c + 1;
  }
  *(undefined *)((long)local_18 + (ulong)local_1c) = 0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return local_18;
  }
                    // WARNING: Subroutine does not return
  __stack_chk_fail();
}



struct_pkm * new_pkm(void)

{
  long lVar1;
  fun_move *pfVar2;
  struct_pkm *pkm_addr;
  byte local_11;
  
  pkm_addr = (struct_pkm *)malloc(248);
  *(undefined **)((long)&pkm_addr->name + 7) = UNKNOWN;
  pkm_addr->atk = 40;
  pkm_addr->def = 10;
  pkm_addr->hp = 100;
  pkm_addr->max_hp = 100;
  pfVar2 = TACKLE.function;
  *(char **)((long)&pkm_addr->move[0].name + 7) = TACKLE.name;
  *(fun_move **)((long)&pkm_addr->move[0].function + 7) = pfVar2;
  for (local_11 = 0; pfVar2 = M_EMPTY.function, local_11 < 10; local_11 = local_11 + 1) {
    lVar1 = (long)pkm_addr->IVs + (long)(int)(uint)local_11 * 0x10 + 0x27;
    *(char **)(lVar1 + 8) = M_EMPTY.name;
    *(fun_move **)(lVar1 + 16) = pfVar2;
  }
  return pkm_addr;
}



void add_pkm(void)

{
  undefined8 uVar1;
  byte local_19;
  
  puts("[*] New PKM!");
  local_19 = 0;
  while ((local_19 < 50 && (pkms[(int)(uint)local_19] != 0))) {
    local_19 = local_19 + 1;
  }
  if (local_19 == 50) {
    puts("[!] No more free slots for pkms");
  }
  else {
    uVar1 = new_pkm();
    pkms[(int)(uint)local_19] = uVar1;
    *(ulong *)(pkms[(int)(uint)local_19] + 48) = (ulong)local_19;
  }
  return;
}



void print_pkm_list(void)

{
  byte local_9;
  
  for (local_9 = 0; local_9 < 0x32; local_9 = local_9 + 1) {
    if (pkms[(int)(uint)local_9] != 0) {
      printf("[%d] %s\n",(ulong)local_9,*(undefined8 *)(pkms[(int)(uint)local_9] + 0x28));
    }
  }
  return;
}



byte get_pkm(void)

{
  long in_FS_OFFSET;
  byte local_11;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  do {
    puts("[*] Choice a PKM!");
    print_pkm_list();
    printf("> ");
    __isoc99_scanf(&DAT_0040209e,&local_11);
    if (pkms[(int)(uint)local_11] != 0) break;
  } while (local_11 < 50);
  if (local_10 != *(long *)(in_FS_OFFSET + 0x28)) {
                    // WARNING: Subroutine does not return
    __stack_chk_fail();
  }
  return local_11;
}



void rename_pkm(void)

{
  long lVar1;
  byte bVar2;
  undefined8 uVar3;
  
  puts("[*] Rename PKM!");
  bVar2 = get_pkm();
  if ((*(long *)(pkms[(int)(uint)bVar2] + 0x28) != 0) &&
     (*(undefined **)(pkms[(int)(uint)bVar2] + 0x28) != UNKNOWN)) {
    free(*(void **)(pkms[(int)(uint)bVar2] + 0x28));
  }
  lVar1 = pkms[(int)(uint)bVar2];
  uVar3 = get_string();
  *(undefined8 *)(lVar1 + 0x28) = uVar3;
  return;
}



void del_pkm(byte param_1)

{
  if (pkms[(int)(uint)param_1] != 0) {
    if ((*(long *)(pkms[(int)(uint)param_1] + 0x28) != 0) &&
       (*(undefined **)(pkms[(int)(uint)param_1] + 0x28) != UNKNOWN)) {
      free(*(void **)(pkms[(int)(uint)param_1] + 0x28));
    }
    free((void *)pkms[(int)(uint)param_1]);
  }
  pkms[(int)(uint)param_1] = 0;
  return;
}



void delete_pkm(void)

{
  undefined uVar1;
  
  puts("[*] Delete PKM!");
  uVar1 = get_pkm();
  del_pkm(uVar1);
  return;
}



void print_moves(byte param_1)

{
  byte local_11;
  undefined8 *lVar1;
  
  lVar1 = (undefined8 *)pkms[(int)(uint)param_1];
  if (lVar1 != (undefined8 *)0x0) {
    for (; local_11 < 10; local_11 = local_11 + 1) {
      if (lVar1[((long)(int)(uint)local_11 + 5) * 2 + 2] != 0) {
        printf("\t(%d) %s\n",(ulong)local_11,lVar1[((long)(int)(uint)local_11 + 5) * 2 + 1]);
      }
    }
  }
  return;
}



long get_move(byte param_1)

{
  long in_FS_OFFSET;
  byte local_11;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  do {
    puts("[*] Choice a Move!");
    print_moves(param_1);
    printf("> ");
    __isoc99_scanf(&DAT_0040209e,&local_11);
    if (*(long *)(((long)(int)(uint)local_11 + 5) * 16 + pkms[(int)(uint)param_1] + 16) != 0) break;
  } while (local_11 < 10);
  if (local_10 != *(long *)(in_FS_OFFSET + 0x28)) {
                    // WARNING: Subroutine does not return
    __stack_chk_fail();
  }
  return ((long)(int)(uint)local_11 + 5) * 16 + pkms[(int)(uint)param_1] + 8;
}



void death_checker(byte param_1)

{
  if (*(long *)(pkms[(int)(uint)param_1] + 16) == 0) {
    printf("[!!!] %s fainted!\n",*(undefined8 *)(pkms[(int)(uint)param_1] + 40));
    del_pkm(param_1);
  }
  return;
}



void fight_pkm(void)

{
  byte bVar1;
  byte bVar2;
  undefined8 *puVar3;
  
  puts("[*] Fight PKMs!");
  bVar1 = get_pkm();
  puVar3 = (undefined8 *)get_move(bVar1);
  bVar2 = get_pkm();
  printf("[%%] %s uses %s on %s!\n",*(undefined8 *)(pkms[(int)(uint)bVar1] + 0x28),*puVar3,
         *(undefined8 *)(pkms[(int)(uint)bVar2] + 0x28));
  (*(code *)puVar3[1])
            (pkms[(int)(uint)bVar1],pkms[(int)(uint)bVar2],pkms[(int)(uint)bVar2],(code *)puVar3[1])
  ;
  death_checker(bVar2);
  return;
}



void info_pkm(void)

{
  undefined8 *puVar1;
  byte bVar2;
  byte local_11;
  
  puts("[*] Info PKMs!");
  bVar2 = get_pkm();
  puVar1 = (undefined8 *)pkms[(int)(uint)bVar2];
  if (puVar1[5] != 0) {
    printf(" *Name: %s\n",puVar1[5]);
  }
  printf(" *ATK:  %d\n",*puVar1);
  printf(" *DEF:  %d\n",puVar1[1]);
  printf(" *HP:   %d/%d\n",puVar1[2],puVar1[3]);
  puts(" *Moves:");
  for (; local_11 < 10; local_11 = local_11 + 1) {
    if (puVar1[((long)(int)(uint)local_11 + 5) * 2 + 2] != 0) {
      printf("  (%d) %s\n",(ulong)local_11,puVar1[((long)(int)(uint)local_11 + 5) * 2 + 1]);
    }
  }
  return;
}



void print_menu(void)

{
  puts("***************");
  puts("0. Add PKM");
  puts("1. Rename PKM");
  puts("2. Kill PKM");
  puts("3. Fight PKM");
  puts("4. Info PKM");
  puts("5. Exit");
  puts("***************");
  return;
}



void main(void)

{
  long in_FS_OFFSET;
  byte local_11;
  undefined8 local_10;
  
  local_10 = *(undefined8 *)(in_FS_OFFSET + 0x28);
  alarm(0xffffffff);
  setvbuf(stdin,(char *)0x0,2,0);
  setvbuf(stdout,(char *)0x0,2,0);
  do {
    print_menu();
    printf("> ");
    __isoc99_scanf(&DAT_0040209e,&local_11);
    switch(local_11) {
    case 0:
      add_pkm();
      break;
    case 1:
      rename_pkm();
      break;
    case 2:
      delete_pkm();
      break;
    case 3:
      fight_pkm();
      break;
    case 4:
      info_pkm();
      break;
    case 5:
                    // WARNING: Subroutine does not return
      exit(0);
    default:
      puts("[!] Wrong choice!");
    }
  } while( true );
}
