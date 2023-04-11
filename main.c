#include "assemble.h"

void main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();
    struct symbolTable *pSymTab;
    int symTabLen, instCnt = 0, lineSize = 72;
    bool found, foundDirective;
    struct instruction *currInst;
    char *line, *token;
    char *instructions[] = {"add", "sub", "slt", "or", "nand",
                            "addi", "slti", "ori", "lui", "lw", "sw", "beq", "jalr",
                            "j", "halt"};
    currInst = (struct instruction *) malloc(sizeof(struct instruction));
    if (argc < 3) {
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if ((assp = fopen(argv[1], "r")) == NULL) {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((machp = fopen(argv[2], "w")) == NULL) {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }
    //**************
    symTabLen = findSymTabLen(assp);
    pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++) {
        pSymTab[i].symbol = (char *) malloc(6);
    }
    fillSymTab(pSymTab, assp);
    line = (char *) malloc(72);
    while (fgets(line, lineSize, assp) != NULL) {
        found = foundDirective = false;
        token = strtok(line, "\t, \n");
        for (int i = 0; i < symTabLen; i++) {
            if (strcmp(token, pSymTab[i].symbol) == 0) {
                token = strtok(NULL, "\t, \n"); //token pointer moves forward one house.
            }
        }
        currInst->PC = instCnt++;
        for (int i = 0; i < 15; i++) {
            if (strcmp(token, instructions[i]) == 0) {
                found = true;
                formInst(currInst, i, token, symTabLen, pSymTab);
            }
        }
        if ((strcmp(token, ".fill") == 0) || (strcmp(token, ".space") == 0)) {
            found = true;
            foundDirective = true;
            token = strtok(NULL, "\t, \n");
            completeDirective(token, symTabLen, currInst, pSymTab);
        }
        if (!found) {
            printf("Opcode \"%s\" is invalid!", token);
            exit(1);
        }
        if (!foundDirective)
            currInst->intInst = hex2int(currInst->inst);
        fprintf(machp, "%d\n", currInst->intInst);
    }
    fclose(assp);
    fclose(machp);
}

int findSymTabLen(FILE *inputFile) {
    int count = 0;
    size_t lineSize;
    char *line;
    line = (char *) malloc(72);
    while (fgets(line, lineSize, inputFile) != NULL) {
        if ((line[0] == ' ') || (line[0] == '\t'));
        else
            count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}

void fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    int lineSize = 72;
    char *line;
    int i = 0;
    char *token;
    line = (char *) malloc(72);
    while (fgets(line, lineSize, inputFile) != NULL) {
        if ((line[0] == ' ') || (line[0] == '\t'));
        else {
            token = strtok(line, "\t, \n");
            for (int j = 0; j < i; j++) {
                if (strcmp(symT[j].symbol, token) == 0) {
                    printf("Label \"%s\" has already exist!", token);
                    exit(1);
                }
            }
            strcpy(symT[i].symbol, token);
            symT[i].value = lineNo;
            i++;
        }
        lineNo++;
    }
    rewind(inputFile);
    free(line);
}

void formInst(struct instruction *currInst, int i, char *token, int symTabLen, struct symbolTable *pSymTab) {
    char hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    currInst->mnemonic = (char *) malloc(5);
    strcpy(currInst->mnemonic, token);
    currInst->inst[0] = '0';
    currInst->inst[1] = hexTable[i];
    if (i < 5) {
        currInst->instType = RTYPE;
        currInst->rd = atoi(strtok(NULL, "\t, \n"));
        currInst->rs = atoi(strtok(NULL, "\t, \n"));
        currInst->rt = atoi(strtok(NULL, "\t, \n"));
        currInst->inst[2] = hexTable[currInst->rs];
        currInst->inst[3] = hexTable[currInst->rt];
        currInst->inst[4] = hexTable[currInst->rd];
        for (int j = 5; j < 8; j++)
            currInst->inst[j] = '0';
    } else if (i < 13) {
        currInst->instType = ITYPE;
        currInst->rt = atoi(strtok(NULL, "\t, \n"));
        if (i == 8)
            currInst->rs = 0;
        else
            currInst->rs = atoi(strtok(NULL, "\t, \n"));
        if (i == 12)
            currInst->imm = 0;
        else {
            token = strtok(NULL, "\t, \n");
            completeImm(token, symTabLen, currInst, pSymTab);
        }
        if (i == 11) {
            currInst->imm -= (currInst->PC + 1);
            currInst->inst[2] = hexTable[currInst->rt];
            currInst->inst[3] = hexTable[currInst->rs];
        } else {
            currInst->inst[2] = hexTable[currInst->rs];
            currInst->inst[3] = hexTable[currInst->rt];
        }
        int2hex16(&currInst->inst[4], currInst->imm);
    } else {
        currInst->instType = JTYPE;
        currInst->inst[2] = '0';
        currInst->inst[3] = '0';
        if (i == 14)
            currInst->imm = 0;
        else {
            token = strtok(NULL, "\t, \n");
            completeImm(token, symTabLen, currInst, pSymTab);
        }
        int2hex16(&currInst->inst[4], currInst->imm);
    }
}

int hex2int(char *hex) {
    int result = 0;
    while ((*hex) != '\0') {
        if (('0' <= (*hex)) && ((*hex) <= '9'))
            result = result * 16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <= 'f'))
            result = result * 16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <= 'F'))
            result = result * 16 + (*hex) - 'A' + 10;
        hex++;
    }
    return (result);
}

void int2hex16(char *lower, int a) {
    sprintf(lower, "%X", a);
    if (a < 0x10) {
        lower[4] = '\0';
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x100) {
        lower[4] = '\0';
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x1000) {
        lower[4] = '\0';
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
}

void completeImm(char *token, int symTabLen, struct instruction *currInst, struct symbolTable *pSymTab) {
    bool foundSym = false;
    if (!isdigit(token[0])) {
        for (int i = 0; i < symTabLen; i++) {
            if (strcmp(pSymTab[i].symbol, token) == 0) {
                foundSym = true;
                currInst->imm = pSymTab[i].value;
            }
        }
        if (!foundSym) {
            printf("Label \"%s\" does NOT exist!", token);
            exit(1);
        }
    } else
        currInst->imm = atoi(token);
    if (currInst->imm > pow(2, 16)) {
        printf("Offset \"%s\" is invali! Because it is greater than 65535.", token);
        exit(1);
    }
}

void completeDirective(char *token, int symTabLen, struct instruction *currInst, struct symbolTable *pSymTab) {
    bool foundSym = false;
    if (!isdigit(token[0]) && (token[0] != '-')) {
        for (int i = 0; i < symTabLen; i++) {
            if (strcmp(token, pSymTab[i].symbol) == 0) {
                foundSym = true;
                currInst->intInst = pSymTab[i].value;
            }
        }
        if (!foundSym) {
            printf("Label \"%s\" does NOT exist!", token);
            exit(1);
        }
    } else
        currInst->intInst = atoi(token);
}