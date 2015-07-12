#define DIMENSION 4UL

#define MICRO 1000000UL
#define MILI 1000UL
#define SEC 1UL

#define FNAME "file.out"

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <memory.h>

unsigned magicSum;
unsigned square;
unsigned limit;
unsigned nrRes;
unsigned magicMatrix[DIMENSION][DIMENSION];
unsigned *usedNumbers;
unsigned long tStart;
unsigned long tStop;
unsigned long timeDiff;
FILE *out;

void backtrack(unsigned, unsigned, unsigned);
void backtrackLastRow(unsigned);
void lastDiag();
void createMainDiag(unsigned, unsigned);
void init ();

void writeOneResult();
void writeNrOfResults();
void measureTime(unsigned long *microsec);

unsigned checkMainDiag();
unsigned checkSecDiag();
unsigned checkRow(unsigned);
unsigned checkColumn(unsigned);
unsigned sumRow(unsigned);
unsigned sumColumn(unsigned);
unsigned sumMainDiag(unsigned);
unsigned isInMainDiag(unsigned, unsigned);

int main(void)
{
    init ();

    measureTime(&tStart);

    createMainDiag(0, 0);

    measureTime(&tStop);
    timeDiff = (unsigned long)((tStop - tStart)) / MILI;
    writeNrOfResults();

    free (usedNumbers);
    fclose(out);
    return 0;
}

void init()
{
    nrRes = 0;
    square = DIMENSION * DIMENSION;
    magicSum = DIMENSION * (square +1) >> 1;
    limit = square - DIMENSION - 1;
    usedNumbers = (unsigned*)malloc ((square + 1) * sizeof (unsigned));
    memset(usedNumbers, 0, (square + 1) * sizeof(unsigned));

    for(unsigned i = 0; i < DIMENSION; ++i) {
        memset (magicMatrix[i], 0, DIMENSION * sizeof (unsigned));
    }

    out = fopen(FNAME, "w");
}

void backtrack(unsigned p, unsigned q, unsigned sum){

    if(p == DIMENSION - 1 && !q){
        lastDiag();
        return;
    }

    if(isInMainDiag(p,q)){
        backtrack(p, q + 1, sum + magicMatrix[p][q]);
        return;
    }

    unsigned s;

    for(unsigned j = 0; j < square; ++j){
        if(usedNumbers[j]) continue;
        usedNumbers[j] = 1;
        magicMatrix[p][q] = j + 1;

        if((s = sum + j + 1) > magicSum){
            usedNumbers[j] = 0;
            continue;
        }

        if(q == DIMENSION - 1)
            if(checkRow(p)){
                backtrack(p + 1, 0, 0);
                usedNumbers[j] = 0;
            }
            else{
                usedNumbers[j] = 0;
                continue;
            }
        else{
            backtrack(p, q + 1, s);
            usedNumbers[j] = 0;
        }
    }
}

void lastDiag(){
    for(unsigned i = 0; i < square; ++i){
        if(usedNumbers[i]) continue;
        usedNumbers[i] = 1;
        magicMatrix[DIMENSION - 1][0] = i + 1;
        if(checkSecDiag() && checkColumn(0))
            backtrackLastRow(1);
        usedNumbers[i] = 0;
    }
}

void backtrackLastRow(unsigned i){
    for(unsigned j = 0; j < square; ++j){
        if(usedNumbers[j]) continue;
        usedNumbers[j] = 1;
        magicMatrix[DIMENSION - 1][i] = j + 1;
        if(!checkColumn(i)){
            usedNumbers[j] = 0;
            continue;
        }
        if(i == DIMENSION - 2)
            if(checkRow(DIMENSION - 1)){
                nrRes++;
#ifdef RES
                writeOneResult();
#endif
                usedNumbers[j] = 0;
                continue;
            }
            else{
                usedNumbers[j] = 0;
                continue;
            }
        else{
            backtrackLastRow(i + 1);
            usedNumbers[j] = 0;
            continue;
        }
    }
}

void createMainDiag(unsigned i, unsigned sum){
    unsigned s;
    for(unsigned j = 0; j < square; ++j){
        if(usedNumbers[j]) continue;
        magicMatrix[i][i] = j + 1;
        usedNumbers[j] = 1;
        if(i == DIMENSION - 1){
            if(checkMainDiag()) backtrack(0, 0, 0);
            usedNumbers[j] = 0;
        }
        else{
            if((s = sum + j + 1) > magicSum){
                usedNumbers[j] = 0;
                continue;
            }
            createMainDiag(i + 1, s);
            usedNumbers[j] = 0;
        }
    }
}

void writeOneResult(){
    for(unsigned i = 0; i < DIMENSION; ++i){
        fprintf (out, "\n");
        for(unsigned j = 0; j < DIMENSION; ++j)
            fprintf(out, "%4d", magicMatrix[i][j]);
    }
    fprintf (out, "\n");
}

void writeNrOfResults(){
    printf ("\nido: %lu microsec\nmegoldasok szama: %d\n", timeDiff, nrRes);
}

unsigned checkMainDiag(){
    unsigned sum = 0;
    for(unsigned i = 0; i < DIMENSION; ++i)
        sum+=magicMatrix[i][i];
    return sum == magicSum;
}

unsigned checkSecDiag(){
    unsigned sum = 0;
    for(unsigned i = 0; i < DIMENSION; ++i)
        sum+=magicMatrix[DIMENSION - i - 1][i];
    return sum == magicSum;
}

unsigned checkRow(unsigned row){
    unsigned sum = 0;
    for(unsigned i = 0; i < DIMENSION; ++i)
        sum+=magicMatrix[row][i];
    return sum == magicSum;
}

unsigned checkColumn(unsigned column){
    unsigned sum = 0;
    for(unsigned i = 0; i < DIMENSION; ++i)
        sum+=magicMatrix[i][column];
    return sum == magicSum;
}

unsigned isInMainDiag(unsigned row, unsigned column){
    return row == column;
}

void measureTime(unsigned long *microsec)
{
    struct timeval *time = (struct timeval*) malloc (sizeof (struct timeval));
    gettimeofday(time, 0);
    *microsec = 1000000 * time->tv_sec + time->tv_usec;
    free (time);
}
