//
// Created by Danya Smelskiy on 12.02.2022.
//

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly two input arguments in the following order:\n"
                        "1. Working directory path;\n"
                        "2. listening 'address:port';\n");
        return 1;
    }


}
