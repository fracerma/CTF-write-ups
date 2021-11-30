typedef long unsigned int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;


struct move {
     char *name;
     void (*fun)(struct pkm *, struct pkm *);
 } M_EMPTY;

  struct move {
     char *name;
     void (*fun)(struct pkm *, struct pkm *);
 } TACKLE;

typedef struct move {
     char *name;
     void (*fun)(struct pkm *, struct pkm *);
 } move;

  
struct pkm {
    uint64_t atk;
    uint64_t def;
    uint64_t hp;
    uint64_t max_hp;
    uint8_t status;
    char *name;
    uint64_t IVs[5];
    move moves[10];
} pkms;