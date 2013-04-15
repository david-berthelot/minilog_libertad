// Verilog netlist reader
// Author: David Berthelot

#include <stdio.h>
#include <string.h>
#include "vlogobjects.hxx"

int main(int argc,char **argv)
{
    pair<bool,VLP::Design*> g = make_pair(true,static_cast<VLP::Design*>(0));
    const bool print = (argc > 1) ? strcmp(argv[1],"-p") == 0 : false;

    if ( argc > 1 ) {
        printf("Reading verilog file %s\n",argv[argc-1]);
        g = VLP::parse_vlog_file(argv[argc-1]);
    } else {
        g = VLP::parse_vlog_file(0);
    }
    printf("Finished\n");
    fflush(stdin);

    if (0) {
        printf("---------------------------------------- Lexeme Table\n");
        //LEXEMES->print();
        printf("----------------------------------------\n");
    }
    if (print) {
        g.second->print();
    }
    delete g.second;
    return g.first ? 0 : 1;
}
