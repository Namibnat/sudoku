/* solve a sudoku puzzel */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "sudoku.h"
char origp[PZSZ];                   /* the origional puzzle */
char n[10];                         /* each number */
int vld_puzzle(char *given);
int solveit(char *working, int nz);
void fill_struct(char *working);
int workonblock(int p, int workwhat);
void seta(int v, int p);
void help();
int guess(int n);
int main(int argc, char **argv)
{
  int i, k=0;
  /* get val of entered puzzle */
  (argc == 2)?k = vld_puzzle(argv[1]):k++;
  /* Decide to kill or help */
  if(k == KILL){
    printf("The puzzle entered is not valid!\n");
  }
  if(k == HELP)
    help();
  /* if a valid puzzle wasn't entered, end it */
  if(k)
    return 0;
  /* try to solve it */
  if(!solveit(origp, n[0])){
    guess(n[0]);
    printf("\nNot solved\n");
  }
  return 0;
}

char slvpz[PZSZ];      /* the solved puzzle */
struct cellval{        /* struct of blocks */
  char val;            /* value of each block */
  char posval[LENG];   /* working possible values */
  int hl;              /* to which horizontal line does it belong */
  int vl;              /* to which vertical line does it belong */
  int sq;              /* to which square does it belong */
} sa[PZSZ];            /* the array of struct of squares for the whole puzzle */

int solveit(char *working, int nz)
{
  strcpy(slvpz, working);
  int i, ii, tmp, solved = 0, changed = 1, inchanged = 1, rounds = 0, test;
  int workfirst = 0, worksecond = 1;
  /* Get all the working values into struct bl sa */
  fill_struct(working);
  /* go through all the squares, checking for empty cells to work on */
  changed = 1;
  while(changed){
    changed = 0;
    inchanged = 1;
    while(inchanged && nz){
      inchanged = 0;
      rounds++;
      for(i = tmp = 0; i < PZSZ; i++){
	test = tmp;
	if(sa[i].val == BASEVAL){
	  /* work on it */
	  tmp += workonblock(i, workfirst);
	  if(test < tmp){
	    nz--;
	  }
	}
      }
      if(tmp){
	changed++;
	inchanged++;
      }
    }
    inchanged = 1;
    while(inchanged){
      inchanged = 0;
      rounds++;
      for(i = tmp = 0; i < PZSZ; i++){
	test = tmp;
	if(sa[i].val == BASEVAL){
	  /* work on it */
	  tmp += workonblock(i, worksecond);
	  if(test < tmp){
	    nz--;
	  }
	}
      }
      if(tmp){
	changed++;
	inchanged++;
      }
    }
    if(nz == 0){
      changed == 0;
    }
  }
  if(nz == 0){
    solved++;
  }
  /* print the origional and the result */
  printf("\nOrigional:  ");
  printf("%s", origp);
  printf("\nSolution:   ");
  printf("%s\n\n", slvpz);
  printf("  ");
  for(ii = 0; ii < PZSZ; ii++){
    if((ii != 0) && (ii%3 == 0) && (ii%9 != 0)){
      printf(" | ");
    }
    if((ii != 0) && (ii%9 == 0) && (ii%27 != 0)){
      printf("\n  ");
    }
    if((ii != 0) && (ii%27 == 0)){
      printf("\n  ________________________\n\n  ");
    }
    printf("%2c", slvpz[ii]);
  }
  printf("\n\n");
  printf("%d rounds\n\n", rounds);
  return solved;
}

/* Given cell p... */
int workonblock(int p, int workwhat)
{ 
  int i, ii, ic, il, is, ihv, ihvi;
  int hv, vv, sv;
  int hv1, hv2, vv1, vv2, h1f, h2f, v1f, v2f;
  int gotval = MISSEDVAL, negs, maybevalue, holdvalue;
  int count, skip, test, testh, testv, testh1, testh2, testv1, testv2, good;
  int lfh1, lfh2, lfv1, lfv2;
  /* Determine which lines and squares are relevant while solving */
  struct cellval *sp, *elv, *ts; /* for square, elv and test square */
  sp = &sa[p];
  hv = (*sp).hl;
  vv = (*sp).vl;
  sv = (*sp).sq;
  switch(hv % 3){
  case(0):
    hv1 = hv + 1;
    hv2 = hv + 2;
    break;
  case(1):
    hv1 = hv - 1;
    hv2 = hv + 1;
    break;
  case(2):
    hv1 = hv - 2;
    hv2 = hv - 1;
    break;
  }
  switch(vv % 3){
  case(0):
    vv1 = vv + 1;
    vv2 = vv + 2;
    break;
  case(1):
    vv1 = vv - 1;
    vv2 = vv + 1;
    break;
  case(2):
    vv1 = vv -2;
    vv2 = vv -1;
    break;
  }
  /* set arrays for the relevant lines for solving */
  int ins[8], pins = 0;
  int inh[8], pinh = 0;
  int inh1[9], pinh1 = 0;
  int inh2[9], pinh2 = 0;
  int inv[8], pinv = 0;
  int inv1[9], pinv1 = 0;
  int inv2[9], pinv2 = 0;
  /* fill them with the reference of the cell */
  for(i = 0; i < PZSZ; i++){
    if(i != p){
      if(sa[i].sq == (*sp).sq)
	ins[pins++] = i;
      if(sa[i].hl == hv)
	inh[pinh++] = i;
      if(sa[i].hl == hv1)
	inh1[pinh1++] = i;
      if(sa[i].hl == hv2)
	inh2[pinh2++] = i;
      if(sa[i].vl == vv)
	inv[pinv++] = i;
      if(sa[i].vl == vv1)
	inv1[pinv1++] = i;
      if(sa[i].vl == vv2)
	inv2[pinv2++] = i;
    }
  }
  /* fill the possible values for the square we're working on */
  for(i = 0; i < LENG; i++){
    if((*sp).posval[i] != -1)
      (*sp).posval[i] = i + BASEVAL;
  }
  /* ********************* FIRST LEVEL *********************** */
  if(!workwhat){
    /* cycle through possible values */
    for(i = 1; i < LENG; i++){
      /* check if the value is in the square */
      for(ic = 0; ic < 8; ic++){
	elv = &sa[ins[ic]];
        if((*elv).val == (i + BASEVAL)){
	  (*sp).posval[i] = -1;
	}
      }
      /* check if the value is in the lines */
      for(ic = 0; ic < 8; ic++){
	elv = &sa[inh[ic]];
	if((*elv).val == (i + BASEVAL)){
	  (*sp).posval[i] = -1;
	}
      }
      for(ic = 0; ic < 8; ic++){
	elv = &sa[inv[ic]];
	if((*elv).val == (i + BASEVAL)){
	  (*sp).posval[i] = -1;
	}
      }
    }
    /* run through posval to see if we got a hit */
    for(i = 1, negs = maybevalue = 0; i < LENG; i++){
      if((*sp).posval[i] == -1){
	negs++;
      }
      else{
	maybevalue = (*sp).posval[i];
      }
    }
    if(negs == 8){
      slvpz[p] = (*sp).val = maybevalue;
      gotval = GOTVAL;
      return gotval;
    }
  }
  /* ********************* SECOND LEVEL *********************** */
  if(workwhat == 1){
    int tsth1, tsth2, tstv1, tstv2;
    int sh1v1, sh1v, sh1v2, shv1, shv2, sh2v1, sh2v, sh2v2;
    sh1v1 = sh1v = sh1v2 = shv1 = shv2 = sh2v1 = sh2v = sh2v2 = 0;
    for(i = 0; i < 8; i++){
      ts = &sa[ins[i]];
      if((*ts).val != BASEVAL){
	if(((*ts).hl == hv1) && ((*ts).vl == vv1))
	  sh1v1++;
	else if(((*ts).hl == hv1) && ((*ts).vl == vv))
	  sh1v++;
	else if(((*ts).hl == hv1) && ((*ts).vl == vv2))
	  sh1v2++;
	else if(((*ts).hl == hv)  && ((*ts).vl == vv1))
	  shv1++;
	else if(((*ts).hl == hv)  && ((*ts).vl == vv2))
	  shv2++;
	else if(((*ts).hl == hv2) && ((*ts).vl == vv1))
	  sh2v1++;
	else if(((*ts).hl == hv2) && ((*ts).vl == vv))
	  sh2v++;
	else if(((*ts).hl == hv2) && ((*ts).vl == vv2))
	  sh2v2++;
	else
	  ; /* shouldn't get here, left in for testing only */
      }
    }
    for(i = 1; i < LENG; i++){
      holdvalue = (BASEVAL + i);
      good = 0;
      /* see if the value is already in the square */
      for(ii = 0; ii < 8; ii++){
	if(sa[ins[ii]].val == holdvalue){
	  ii = -1;
	  break;
	}
      }
      if(ii == -1){
	continue;
      }
      /* if not, cycle through h1, h2, v1 & v2 to see which ones have the value */
      tsth1 = tsth2 = tstv1 = tstv2 = 0;
      for(ii = 0; ii < 9; ii++){
	if(sa[inh1[ii]].val == holdvalue)
	   tsth1++;
	if(sa[inh2[ii]].val == holdvalue)
	  tsth2++;
	if(sa[inv1[ii]].val == holdvalue)
	  tstv1++;
	if(sa[inv2[ii]].val == holdvalue)
	  tstv2++;
      }
      if((sh1v1 || (!sh1v1 && (tsth1 || tstv1))))
	good++;
      if((sh1v || (!sh1v && tsth1)))
	good++;
      if((sh1v2 || (!sh1v2 && (tsth1 || tstv2))))
	good++;
      if((shv1 || (!shv1 && tstv1)))
	good++;
      if((shv2 || (!shv2 && tstv2)))
	good++;
      if((sh2v1 || (!sh2v1 && (tstv1 || tsth2))))
	good++;
      if((sh2v || (!sh2v && tsth2)))
	good++;
      if((sh2v2 || (!sh2v2 && (tstv2 || tsth2))))
	good++;
      if(good == 8){
	slvpz[p] = (*sp).val = holdvalue;
	gotval = GOTVAL;
	for(is = 0; is < LENG; is++){
	  if((*sp).posval[is] != holdvalue){
	    (*sp).posval[is] = -1;
	  }
	}
	return gotval;
      }
    } /* This ends cycling through each possible value */
  } /* This ends if(workwhat == 1) */
  gotval = MISSEDVAL;
  return gotval;
}

void fill_struct(char *working)
{
  int i, ii, r;
  for(i = 0; i < PZSZ; i++){
    sa[i].val = working[i];
    /* if val is still zero, fill the possible values */
    if(sa[i].val == BASEVAL){
      sa[i].posval[0] == -1;
      for(ii = 1; ii < LENG; ii++){
	sa[i].posval[ii] = BASEVAL + ii;
      }
    }
    sa[i].hl = i/9;
    for(ii = i; ii > 8; ii -= 9)
      ;
    sa[i].vl = ii;
    /* Options for squares: 0, 3, 6, 27, 30, 33, 54, 57, 60    */
    for(ii = r = 0; r < LENG; r++){
      if((i >= ii && i <= (ii+2)) || (i >= (ii+9) && i <= (ii+11)) || (i >= (ii+18) && i <= (ii+20))){
	sa[i].sq = ii;
      }
      ii += ((r + 1) % 3)?3:21;
    }
  }
}

void seta(int v, int p)
{
  int i;
  for(i = 0; i < LENG; i++){
    if(sa[p].posval[i] != v){
      sa[p].posval[i] = -1;
    }
  }
}

/* check the puzzle is valid */
int vld_puzzle(char *given)
{
  int i, k = 0;
  /* initialize to zero (just to be safe) */
  for(i = 0; i < 10; n[i] = 0, i++)
    ;
  for(i = 0; i < PZSZ; i++){
    /* allow underscore as input */
    if(given[i] == '_'){
      given[i] = BASEVAL;
    }
    /* check if the given arg is for help */
    if(!isdigit(given[i])){
      while(given[i] == '-')
	i++;
      if((given[i] == 'h') || (given[i] == 'H')){
	k = 2;
	break;
      }
      k++;
      break;
    }
  }
  /* check that not too many vals were given */
  if(i == PZSZ && isdigit(given[i]) && given[i] > 0){
    printf("\nToo big...\n");
    k++;
  }
  /* check that there are not too many of each number value */
  if(k == 0){
    for(i = 0; i < PZSZ; i++)
      n[(given[i] - BASEVAL)]++;
    for(i = 0; i < LENG; i++){
      if(i && n[i] > 9){
	printf("Too many %ds\n", i);
	k++;
	break;
      }
    }
  }
  /* copy the given puzzle vals into the working puzzle */
  strcpy(origp, given);
  return k;
}

/* print help */
void help()
{
  printf("Print the help\n");
}

int guess(int n)
{
  /* right now it just works with one guess
     what it needs to do is make a whole array of
     possible guesses.  Then we set a holding array
     to the old value, and do guessing for each possible 
     guess value, resetting the origional array after each unsuccessful
     guess, and moving on to the next one until
     we either go through the whole guessing array
     or we fill the puzzle.

    potentially we could go on to doing two or more guesses
    at the same time.  We could carry on guessing until we've practically
    filled the whole puzzle.
  */
  char guessp[PZSZ];
  int p, i, count, lowest, lp, hold;
  lowest = 8;
  for(p = 0; p < PZSZ; p++){
    count = 0;
    if(sa[p].val == BASEVAL){
      for(i = 1; i < LENG; i++){
	if(sa[p].posval[i] != -1){
	  count++;
	}
      }
      if(count < lowest){
	lowest = count;
	lp = p;
      }
    }
  }
  printf("The lowest was %d at %d\n", lowest, lp);
  for(p = hold = 0; p < PZSZ; p++){
    if(p == lp){
      for(i = 1; i < LENG; i++){
	/* find the first val it could be */
	if(sa[p].posval[i] != -1){
	  guessp[p] = sa[p].posval[i];
	  break;
	}
      }
    }
    else{
      guessp[p] = slvpz[p];
    }
  }
  if(!solveit(guessp, --n)){
    printf("Not solved with a guess");
  }
  return lp;
}
