#include <iostream>
#include <string.h>
#include "ula.hpp"

ULA::ULA(unsigned int A, unsigned int B) : A(A), B(B), ENA(0), ENB(0), INVA(0), INC(0), F0(0), F1(0) {}

void ULA::instructions(std::string& instruction) {
    // Decodificando a instrução e convertendo os caracteres já para inteiros:
    F0 = instruction[0] - '0';
    F1 = instruction[1] - '0';
    ENA = instruction[2] - '0';
    ENB = instruction[3] - '0';
    INVA = instruction[4] - '0';
    INC = instruction[5] - '0';
}

Output ULA::execute(){
    // ...
}

// Função para preparar os sinais de entrada para a ULA:
ULA::Inputs ULA::preparing_inputs() const {
    Inputs input;

    if (ENA) {
        input.A = A; // Se ENA for 1, então A é habilitado
    } else {
        input.A = 0; 
    }

    if (INVA) {
        input.A = ~input.A; // Se INVA for 1, então A é invertido
    }

    if (ENB) {
        input.B = B; // Se ENB for 1, então B é habilitado
    } else {
        input.B = 0; 
    }

    input.carry_in = INC; // Carry-in é determinado por INC

    return input;
}

// Função para calcular o valor S e o carry-out (vai-um): 
Output ULA::calculate_output(const Inputs& input) const {
    Output output;
    
    // Tabela de operações:
    // F1=0 F0=0 → AND
    // F1=0 F0=1 → OR
    // F1=1 F0=0 → NOT B
    // F1=1 F0=1 → A + B (aritmético)

    return output;
}