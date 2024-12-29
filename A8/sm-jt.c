#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int           reg[8];
unsigned char mem[10000*4];
int           pc = -1;
unsigned char insOpCode;
unsigned char insOp0;
unsigned char insOp1;
unsigned char insOp2;
char          insOpImm;
int           insOpExt;

void fetch() {

  void* fetch_jumptable[] = {
    &&fetch_0b, &&fetch_default, &&fetch_default, &&fetch_default, &&fetch_default, &&fetch_default,
    &&fetch_default, &&fetch_default, &&fetch_default, &&fetch_default, &&fetch_default,
    &&fetch_0b, &&fetch_default, &&fetch_default, &&fetch_default, &&fetch_default,
  };

  insOpCode = mem [pc] >> 4;
  insOp0    = mem [pc] & 0xf;
  insOp1    = mem [pc+1] >> 4;
  insOp2    = mem [pc+1] & 0xf;
  insOpImm  = mem [pc+1];
  pc += 2;

  goto *fetch_jumptable[insOpCode];

      fetch_0b:
        insOpExt = mem [pc] << 24 | mem [pc+1] << 16 | mem [pc+2] << 8 | mem [pc+3];
        pc += 4;
        goto fetch_end;
      fetch_default:
        goto fetch_end;
      
      fetch_end:
        return;
}

int exec() {
  int cont = 1;
  int addr, val;

  static void* jumpTable[] = {
    &&case_0, &&case_1, &&case_2, &&case_3, &&case_4, &&case_default,
    &&case_6, &&case_7, &&case_8, &&case_9, &&case_a,
    &&case_b, &&case_c, &&case_default, &&case_default, &&case_f,
  };

  goto *jumpTable[insOpCode];
  
      case_0: // ld $i, d .............. 0d-- iiii iiii
        reg [insOp0] = insOpExt;
        goto end;

      case_1: // ld o(rs), rd .......... 1osd
        addr = (insOp0 << 2) + reg [insOp1];
        reg [insOp2] = mem [addr] << 24 | mem [addr+1] << 16 | mem [addr+2] << 8 | mem [addr+3];
        goto end;

      case_2: // ld (rs, ri, 2), rd .... 2sid
        addr = reg [insOp0] + (reg [insOp1] << 2);
        reg [insOp2] = mem [addr] << 24 | mem [addr+1] << 16 | mem [addr+2] << 8 | mem [addr+3];
        goto end;

      case_3: // st rs, o(rd) .......... 3sod
        addr = (insOp1 << 2) + reg [insOp2];
        val  = reg [insOp0];
        mem [addr]   = val >> 24 & 0xff;
        mem [addr+1] = val >> 16 & 0xff;
        mem [addr+2] = val >>  8 & 0xff;
        mem [addr+3] = val       & 0xff;
        goto end;

      case_4: // st rs, (rd, ri, 4) .... 4sdi
        addr = reg [insOp1] + (reg [insOp2] << 2);
        val  = reg [insOp0];
        mem [addr]   = val >> 24 & 0xff;
        mem [addr+1] = val >> 16 & 0xff;
        mem [addr+2] = val >>  8 & 0xff;
        mem [addr+3] = val       & 0xff;
        goto end;

      case_6: // ALU ................... 6-sd
        static void* jumpTable6[] = {
          &&case6_0, &&case6_1, &&case6_2, &&case6_3, &&case6_4, &&case6_5,
          &&case6_6, &&case6_7, &&case6_default, &&case6_default, &&case6_default,
          &&case6_default, &&case6_default, &&case6_default, &&case6_default, &&case6_f,
        };

        goto *jumpTable6[insOp0];

            case6_0: // mov rs, rd ........ 60sd
              reg [insOp2] = reg [insOp1];
              goto end_6;
            case6_1: // add rs, rd ........ 61sd
              reg [insOp2] = reg [insOp1] + reg [insOp2];
              goto end_6;
            case6_2: // and rs, rd ........ 62sd
              reg [insOp2] = reg [insOp1] & reg [insOp2];
              goto end_6;
            case6_3: // inc rr ............ 63-r
              reg [insOp2] = reg [insOp2] + 1;
              goto end_6;
            case6_4: // inca rr ........... 64-r
              reg [insOp2] = reg [insOp2] + 4;
              goto end_6;
            case6_5: // dec rr ............ 65-r
              reg [insOp2] = reg [insOp2] - 1;
              goto end_6;
            case6_6: // deca rr ........... 66-r
              reg [insOp2] = reg [insOp2] -4;
              goto end_6;
            case6_7: // not ............... 67-r
              reg [insOp2] = ~ reg [insOp2];
              goto end_6;
            case6_f: // gpc ............... 6f-r
              reg [insOp2] = pc + (insOp1 << 1);
              goto end_6;
            case6_default:
              printf ("Illegal ALU instruction: pc=0x%x fun=0x%x\n", pc, insOp0);
        end_6:
          goto end;

      case_7: // sh? $i,rd ............. 7dii
        if (insOpImm > 0)
          reg [insOp0] = reg [insOp0] << insOpImm;
        else
          reg [insOp0] = reg [insOp0] >> -insOpImm;
        goto end;

      case_8: // br o .................. 8-oo
        pc += insOpImm << 1;
        goto end;

      case_9: // beq rs, o ............. 9roo
        if (reg [insOp0] == 0)
          pc += insOpImm << 1;
        goto end;

      case_a: // bgt rs, o .............. aroo
        if (reg [insOp0] > 0)
          pc += insOpImm << 1;
        goto end;

      case_b: // j i ................... b--- iiii iiii
        pc = insOpExt;
        goto end;

      case_c: // j o(rr) ............... croo
        pc = (((unsigned short) insOpImm) << 1) + reg [insOp0];
        goto end;

      case_f: // halt or nop ............. f?--
        if (insOp0 == 0)
          cont = 0;
        else if (insOp0 == 1) {
          // sys i                           f1ii
          if (insOpImm == 0) {
            // sys read
            reg[0] = read(reg[0], &mem[reg[1]], reg[2]);
          } else if (insOpImm == 1) {
            // sys write
            reg[0] = write(reg[0], &mem[reg[1]], reg[2]);
          } else if (insOpImm == 2) {
            // sys exec
            char *unsafe = getenv("SIMPLE_MACHINE_ALLOW_EXEC");
            if(unsafe && strcmp(unsafe, "1") == 0) {
              int size = reg[1];
              char *str = malloc(size + 1);
              memcpy(str, &mem[reg[0]], size);
              str[size] = 0;
              reg[0] = system(str);
              free(str);
            } else {
              printf("<<<WOULD EXECUTE %.*s>>>\n", reg[1], &mem[reg[0]]);
            }
          }
        }
        goto end;

      case_default:
        printf ("Illegal  instruction: pc=0x%x opCode=0x%x\n", pc, insOpCode);
  
      end:
        return cont;
}

int loadFile (char* filename) {
  char*  line = NULL;
  size_t len  = 0;
  FILE* f = fopen (filename, "r");
  if (f == NULL)
    return 0;
  while (getline (&line, &len, f) > 0) {
    char* sep = line;
    while (*sep != ':')
      sep++;
    *sep = 0;
    int addr = strtol (line, 0, 16);
    if (pc < 0)
      pc = addr;
    line = sep + 1;
    char v[3] = {0,0,0};
    while (1) {
      while (*line && *line == ' ')
        line ++;
      if (*line == 0 || *(line+1) == 0)
        break;
      v[0] = line[0];
      v[1] = line[1];
      mem [addr] = strtol (v, 0, 16);
      line += 2;
      addr += 1;
    }
    line = NULL;
  }
  fclose (f);
  return 1;
}

void showMem (int start, int count) {
  int i;
  for (i=0; i<count*4; i+=4)
    printf ("0x%08x: %02x %02x %02x %02x\n", start+i, mem [start+i], mem [start+i+1], mem [start+i+2], mem [start+i+3]);
}

void compute() {
  int cont = 0;
  do {
    fetch();
    cont = exec();
  } while (cont);
}

struct showMemList {
  int  len;
  int* addr;
  int* count;
} showMemList;

int showReg;

int parseArgs (int argc, char** argv) {
  if (argc < 2)
    return 0;
  for (int i=1; i<argc; i++)
    if (argv[i][0] == '-') {
      char* op = argv[i] + 1;
      if (strcmp (op, "p") == 0) {
        i += 1;
        pc = strtol (argv[i], 0, 16);
      } else if (strcmp (op, "m") == 0) {
        i += 1;
        char* sep = argv[i];
        while (*sep !=':')
          sep++;
        *sep = 0;
        showMemList.len++;
        showMemList.addr = realloc (showMemList.addr, showMemList.len * sizeof (*showMemList.addr));
        showMemList.addr [showMemList.len-1] = strtol (argv[i], 0, 16);
        showMemList.count = realloc (showMemList.count, showMemList.len * sizeof (*showMemList.count));
        showMemList.count [showMemList.len-1] = strtol (sep+1, 0, 16);
      } else if (strcmp (op, "r") == 0)
        showReg = 1;
      else
        return 0;
    } else if (i != argc-1)
      return 0;
  return 1;
}

int main (int argc, char** argv) {
  if (! parseArgs (argc, argv)) {
    fprintf (stderr, "usage: sm [-p startPC] [-m addr:count]* [-r] filename\n");
    return EXIT_FAILURE;
  }
  int ok = loadFile (argv[argc-1]);
  if (! ok) {
    fprintf (stderr, "error reading input file\n");
    return EXIT_FAILURE;
  }
  compute ();
  for (int i=0; i<showMemList.len; i++)
    showMem (showMemList.addr[i], showMemList.count[i]);
  if (showReg)
    for (int i=0; i<8; i++)
      printf ("r%d: 0x%08x (%d)\n", i, reg[i], reg[i]);
}
