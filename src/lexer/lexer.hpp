#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

enum class Tipo {
    // Registradores
    R_MAR, R_MDR, R_PC, R_MBR, R_SP,
    R_LV,  R_CPP, R_TOS, R_OPC, R_H,
    R_Geral,
    // Instruções IJVM
    BIPUSH, DUP, ILOAD,
    // Literais
    Numero,
    // Atribuição
    Attr,
    // Controle
    EOF_TOKEN,
    Desconhecido
};

struct Token {
    Tipo        tipo;
    std::string lexema;
    int         posicao;
};

std::string          getExpression(const std::string& fileName);
std::vector<Token>   tokenize(const std::string& content);
void                 printTokens(const std::vector<Token>& tokens);

#endif