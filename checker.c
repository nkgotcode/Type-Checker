#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "ast.h"
#include "typecheck.h"

extern int yyparse(void);
extern stmt_list* prog;

void parse(string fname) 
{EM_reset(fname);
 if (yyparse() != 0) /* parsing worked */
  fprintf(stderr,"Parsing failed\n");
}

int main(int argc, char **argv) {
 if (argc!=2) {
   printf("Output of string_array_array (should be string[][]): %s\n", ty_to_string(string_array_array()));
   printf("Output of get_inner_type(int[]) (should be int): %s\n", ty_to_string(get_inner_type(ArrayTyNode(IntTyNode()))));
   return 0;
 }
 parse(argv[1]);
// print_tree(prog);
 typecheck(prog);
 printf("Finished typechecking!\n");
 return 0;
}
