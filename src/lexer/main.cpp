#include <iostream>
#include <string>
#include "lexer.hpp"

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cout << "[ERRO] Número de argumentos inválido\n";
        return 1;
    }
    std::string file_name = argv[1];
    std::string exp = getExpression(file_name);

    std::vector<Token> tokens = tokenize(exp);
    printTokens(tokens);
    return 0;
}