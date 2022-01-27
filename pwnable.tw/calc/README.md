# Calc challenge pwnable.tw
The challenge is a simulation of a calculator, the binary is a 32bit statically linked and trying with some random input I see that:
```
=== Welcome to SECPROG calculator ===
+10000
[1]    1105 segmentation fault  ./calc
```

Disassembling with ghidra we got, the main funciton:
```C
void main(void)

{
  signal(14,timeout);
  alarm(60);
  puts("=== Welcome to SECPROG calculator ===");
  fflush((FILE *)stdout);
  calc();
  puts("Merry Christmas!");
  return;
}
```

We got an alarm signal after 60 seconds that make the program exit. The first step I did is patch the binary to emit this signal after a lot longer.
The calc function is where is located the main part of the program:
```C
void calc(void)

{
  int num_char_read;
  int in_GS_OFFSET;
  int struct_index;
  undefined4 buffer_integer [100];
  char buffer [1024];
  int canary;
  
  canary = *(int *)(in_GS_OFFSET + 0x14);
  while( true ) {
    bzero(buffer,1024);
    num_char_read = get_expr(buffer,1024);
    if (num_char_read == 0) break;
    init_pool(&struct_index);
    num_char_read = parse_expr(buffer,&struct_index);
    if (num_char_read != 0) {
      printf("%d\n",buffer_integer[struct_index + -1]);
      fflush((FILE *)stdout);
    }
  }
  if (canary == *(int *)(in_GS_OFFSET + 0x14)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}
```
And `get_expr` and `parse_expr`:
```C

int get_expr(char *buffer,int buffer_size)

{
  ssize_t sVar1;
  char char_read;
  int index;
  
  index = 0;
  while (index < buffer_size) {
    sVar1 = read(0,&char_read,1);
    if ((sVar1 == -1) || (char_read == '\n')) break;
    if ((((char_read == '+') || (((char_read == '-' || (char_read == '*')) || (char_read == '/'))))
        || (char_read == '%')) || (('/' < char_read && (char_read < ':')))) {
      buffer[index] = char_read;
      index = index + 1;
    }
  }
  buffer[index] = '\0';
  return index;
}

int parse_expr(char *buffer,struct_result *struct)

{
  int iVar1;
  char *dest_tmp;
  int iVar2;
  char *index_copy;
  int in_GS_OFFSET;
  char *buffer_addr_copy;
  int index;
  int index_2;
  char buffer_operator [100];
  int canary;
  
  canary = *(int *)(in_GS_OFFSET + 0x14);
  buffer_addr_copy = buffer;
  index_2 = 0;
  bzero(buffer_operator,100);
  index = 0;
  do {
    if (9 < (int)buffer[index] - 48U) {
      index_copy = buffer + (index - (int)buffer_addr_copy);
      dest_tmp = (char *)malloc((size_t)(index_copy + 1));
      memcpy(dest_tmp,buffer_addr_copy,(size_t)index_copy);
      dest_tmp[(int)index_copy] = '\0';
      iVar2 = strcmp(dest_tmp,"0");
      if (iVar2 == 0) {
        puts("prevent division by zero");
        fflush((FILE *)stdout);
        iVar2 = 0;
        goto LAB_0804935f;
      }
      iVar2 = atoi(dest_tmp);
      if (0 < iVar2) {
        iVar1 = struct->index;
        struct->index = iVar1 + 1;
        struct->buffer_integer[iVar1] = iVar2;
      }
      if ((buffer[index] != '\0') && (9 < (int)buffer[index + 1] - 48U)) {
        puts("expression error!");
        fflush((FILE *)stdout);
        iVar2 = 0;
        goto LAB_0804935f;
      }
      buffer_addr_copy = buffer + index + 1;
      if (buffer_operator[index_2] == '\0') {
        buffer_operator[index_2] = buffer[index];
      }
      else {
        switch(buffer[index]) {
        case '%':
        case '*':
        case '/':
          if ((buffer_operator[index_2] == '+') || (buffer_operator[index_2] == '-')) {
            buffer_operator[index_2 + 1] = buffer[index];
            index_2 = index_2 + 1;
          }
          else {
            eval(struct,buffer_operator[index_2]);
            buffer_operator[index_2] = buffer[index];
          }
          break;
        default:
          eval(struct,buffer_operator[index_2]);
          index_2 = index_2 + -1;
          break;
        case '+':
        case '-':
          eval(struct,buffer_operator[index_2]);
          buffer_operator[index_2] = buffer[index];
        }
      }
      if (buffer[index] == '\0') {
        for (; -1 < index_2; index_2 = index_2 + -1) {
          eval(struct,buffer_operator[index_2]);
        }
        iVar2 = 1;
LAB_0804935f:
        if (canary != *(int *)(in_GS_OFFSET + 0x14)) {
                    /* WARNING: Subroutine does not return */
          __stack_chk_fail();
        }
        return iVar2;
      }
    }
    index = index + 1;
  } while( true );
}
```
And `eval`:
```C

void eval(struct_result *result,char param_2)

{
  if (param_2 == '+') {
    result->buffer_integer[result->index + -2] =
         result->buffer_integer[result->index + -2] + result->buffer_integer[result->index + -1];
  }
  else {
    if (param_2 < ',') {
      if (param_2 == '*') {
        result->buffer_integer[result->index + -2] =
             result->buffer_integer[result->index + -2] * result->buffer_integer[result->index + -1]
        ;
      }
    }
    else {
      if (param_2 == '-') {
        result->buffer_integer[result->index + -2] =
             result->buffer_integer[result->index + -2] - result->buffer_integer[result->index + -1]
        ;
      }
      else {
        if (param_2 == '/') {
          result->buffer_integer[result->index + -2] =
               result->buffer_integer[result->index + -2] /
               result->buffer_integer[result->index + -1];
        }
      }
    }
  }
  result->index = result->index + -1;
  return;
}
```
After some reversing and dynamical analysis understand that there is a structure composed by a index and an array of int, the index points always to the last free element of the array and at the end in the first position of the array there will be the result.
We have also another array of chars that keep in memory the operators.
EXAMPLE 2+5+7-2 :
	buffer_integer=[2,3,7,2]
	buffer_operator=["+","+","-"]

The while loop in parse_expr is executed and it read untill it reach an operator, then it copy the number in the buffer_integer and the operator in the buffer_operator and execute the eval function depending on the type of operator.
# Vuln
When we write +10000 in the input, the program put the plus in the buffer_operator and 10000 in the buffer_integer and then call the eval function.
But when we do so the program is expecting the presence of two integer instead of just one. In this manner we can overwrite the index.
Writing +10000 in the input, will add 10000 to the index, as you can see:
```GDB
				index	     buffer_integer[0]   ...
0xffffc4d8:     0x00002710      0x00002710      0x00000000      0x00000000
```
If we write +10000+10 we can write 10 in buffer_integer[10000]. 
And that's let us write exactly over the return addres of the `calc` function.
The exact exploit is in the `solve.py` file.







