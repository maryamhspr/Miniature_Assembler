#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
//the number of R, I and J type instructions
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2
/* this structure is defined to hold ever entity of symbol table    *
 * which, after finding the length, will become an array of symbols */
struct symbolTable {
    int value;
    char *symbol;
};
/* every time an instruction is read, it will be tokenized and each   *
 * token will reside in one element of this structure               */
struct instruction {
    size_t instType; // 0 means r type, 1 means i type and 2 means j type
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};

/* the follwing function, findSymTabLen, will scan the assembly    *
 * code and count the number of symbols and return it              */
int findSymTabLen(FILE *);

/* this function fills up the symbol table                         */
void fillSymTab(struct symbolTable *, FILE *);

/* and the following functions are other ancillary functions        */
void formInst(struct instruction *, int, char *, int, struct symbolTable *);

int hex2int(char *);

void int2hex16(char *, int);

void completeImm(char *, int, struct instruction *, struct symbolTable *);

void completeDirective(char *, int, struct instruction *, struct symbolTable *);