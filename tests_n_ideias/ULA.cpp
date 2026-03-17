/**
 * Módulo: ULA
 * 
 * Obejtivo:
 *  -> Desenvolver uma implementação tal que simule uma ULA (Unidade Lógica e Aritmética);
 *  -> Utilizar os registradores desenvolvidos no arquivo Registrador.cpp para leitura e 
 *  armazenamento de dados.
 */

#include <iostream>
#include <memory>

//Struct que armazena os sinais de entrada
typedef struct ULA_control{
    int control[6];
};

//Struct que armazena os dados de entrada da ULA, i.e, os dados dos barramentos A e B.
typedef struct ULA_input{
    unsigned int A[32]; 
    unsigned int B[32];
};

//Struct que armazena as saídas da ULA, desde as flags até o vetor de 32 bits.
typedef struct ULA_output{
    unsigned int s[32];
    int N, Z, carry_out;
};

class ULA{
    private:
        std::unique_ptr<ULA_control> controle;  //Ponteiro para os dados de controle: [F0, F1, ENA, ENB, INVA, INC]
        std::unique_ptr<ULA_input> data;        //Ponteiro para os dados de entrada.
        std::unique_ptr<ULA_output> resultado;  //Ponteiro para a saída da ULA
    public:
        
};

