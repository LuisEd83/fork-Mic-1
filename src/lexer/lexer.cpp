#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_map>

// Mapa de palavras-chave: registradores e instruções IJVM
static const std::unordered_map<std::string, Tipo> keywords = {
    {"mar",    Tipo::R_MAR},
    {"mdr",    Tipo::R_MDR},
    {"pc",     Tipo::R_PC},
    {"mbr",    Tipo::R_MBR},
    {"sp",     Tipo::R_SP},
    {"lv",     Tipo::R_LV},
    {"cpp",    Tipo::R_CPP},
    {"tos",    Tipo::R_TOS},
    {"opc",    Tipo::R_OPC},
    {"h",      Tipo::R_H},
    {"BIPUSH", Tipo::BIPUSH},
    {"DUP",    Tipo::DUP},
    {"ILOAD",  Tipo::ILOAD},
};

// Mapa de símbolos de um único caractere
static const std::unordered_map<char, Tipo> symbols = {
    {'=', Tipo::Attr}
};

// Lê o arquivo inteiro e retorna como string
std::string getExpression(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[FILE Error] Erro ao abrir o arquivo: " << fileName << "\n";
        exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;
    int i            = 0;
    int iCumulative  = 0; // Desconta comentários na posição reportada
    int n            = static_cast<int>(content.size());

    while (i < n) {

        // Ignora espaços em branco
        if (std::isspace(content[i])) {
            i++;
            continue;
        }

        // Comentários delimitados por #...#
        if (content[i] == '#') {
            int start = i++;
            while (i < n && content[i] != '#') i++;
            if (i >= n) {
                std::cerr << "[LEXICAL Error] Comentário não fechado\n";
                exit(1);
            }
            i++;
            iCumulative += (i - start);
            continue;
        }

        Token tk;
        tk.posicao = i - iCumulative;

        // Números (decimais para ILOAD x, binários para BIPUSH byte)
        if (std::isdigit(content[i])) {
            int start = i;
            while (i < n && std::isdigit(content[i])) i++;
            tk.lexema = content.substr(start, i - start);
            tk.tipo   = Tipo::Numero;
            tokens.push_back(tk);
            continue;
        }

        // Símbolos de um caractere
        auto symIt = symbols.find(content[i]);
        if (symIt != symbols.end()){
            tk.lexema = std::string(1, content[i]); // Converte char → string
            tk.tipo = symIt->second;
            tokens.push_back(tk);
            i++;
            continue;
        }

        // Identificadores: registradores e instruções IJVM
        if (std::isalpha(content[i]) || content[i] == '_') {
            int start = i;
            while (i < n && (std::isalnum(content[i]) || content[i] == '_')) i++;
            tk.lexema = content.substr(start, i - start);

            auto it = keywords.find(tk.lexema);
            if (it != keywords.end()) {
                tk.tipo = it->second;
            } else {
                tk.tipo = Tipo::R_Geral;
                /*
                std::cerr << "[LEXICAL Error] Token desconhecido: \""
                          << tk.lexema << "\", posição: " << tk.posicao << "\n";
                exit(1);
                */
            }
            tokens.push_back(tk);
            continue;
        }

        // Caractere inválido
        std::cerr << "[LEXICAL Error] Caractere inválido: '" << content[i]
                  << "', posição: " << tk.posicao << "\n";
        exit(1);
    }

    tokens.push_back({Tipo::EOF_TOKEN, "", -1});
    return tokens;
}

void printTokens(const std::vector<Token>& tokens) {
    auto tipoStr = [](Tipo t) -> std::string {
        switch (t) {
            case Tipo::R_MAR:     return "R_MAR";
            case Tipo::R_MDR:     return "R_MDR";
            case Tipo::R_PC:      return "R_PC";
            case Tipo::R_MBR:     return "R_MBR";
            case Tipo::R_SP:      return "R_SP";
            case Tipo::R_LV:      return "R_LV";
            case Tipo::R_CPP:     return "R_CPP";
            case Tipo::R_TOS:     return "R_TOS";
            case Tipo::R_OPC:     return "R_OPC";
            case Tipo::R_H:       return "R_H";
            case Tipo::R_Geral:   return "R_Geral";
            case Tipo::Attr:      return "Attr";
            case Tipo::BIPUSH:    return "BIPUSH";
            case Tipo::DUP:       return "DUP";
            case Tipo::ILOAD:     return "ILOAD";
            case Tipo::Numero:    return "Numero";
            case Tipo::EOF_TOKEN: return "EOF_TOKEN";
            default:              return "Desconhecido";
        }
    };

    for (const auto& tk : tokens) {
        std::cout << "< " << tipoStr(tk.tipo)
                  << ", \""  << tk.lexema << "\""
                  << ", "    << tk.posicao << " >\n";
    }
}