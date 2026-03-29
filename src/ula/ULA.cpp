/**
 * Módulo: ULA
 * 
 * Obejtivo:
 *  -> Desenvolver uma implementação tal que simule uma ULA (Unidade Lógica e Aritmética);
 *  -> Utilizar os registradores desenvolvidos no arquivo Registrador.cpp para leitura e 
 *  armazenamento de dados.
 */

#include <iostream>
#include <array>

//#include "register.hpp"   //Para desenvolver depois
#include "ULA.hpp"

//Esta função realiza o +1 nas entradas
void add1(std::array<bool, 32> X, ULA_output& out){
    int c_out = 1;
    int c_out_1, c_out_0; // índices 1 e 0 para overflow
    for(int i = 31; i >= 0; i--){
        if(c_out == 1){
            if(X[i] == 1){ out.s[i] = 0; c_out = 1; }
            else          { out.s[i] = 1; c_out = 0; }
        } else {
            out.s[i] = X[i];
        }
        if(i == 1) c_out_1 = c_out;
        if(i == 0) c_out_0 = c_out;
    }
    if(c_out_0 ^ c_out_1) {out.carry_out = 1;}
}

// addXY: mesma lógica
void addXY(std::array<bool, 32> X, std::array<bool, 32> Y, ULA_output& out){
    int c_out = 0;
    int c_out_1, c_out_0;
    for(int i = 31; i >= 0; i--){
        out.s[i] = X[i] ^ Y[i] ^ c_out;
        c_out = (X[i] & Y[i]) | ((X[i] ^ Y[i]) & c_out);
        if(i == 1) c_out_1 = c_out;
        if(i == 0) c_out_0 = c_out;
    }
    out.carry_out = c_out_0;
}

void ULA::deslocador(ULA_output& out){
    // SLL8: desloca 8 bits para a esquerda (lógico — preenche com 0)
    if(co.control[0]) { // SLL8
        std::array<bool, 32> shifted = {};
        for(int i = 0; i < 24; i++){shifted[i] = out.s[i + 9];}
        out.s = shifted;
    }
    // SRA1: desloca 1 bit para a direita (aritmético — preserva bit de sinal)
    if(co.control[1]) { // SRA1
        std::array<bool, 32> shifted = out.s; // cópia do estado original
        bool sinal = shifted[0];
        for(int i = 31; i >= 0; i--) out.s[i] = shifted[i - 1];
        out.s[0] = sinal; // preserva o sinal
    }

    // N e Z sempre calculados sobre o Sd final (pós-deslocamento)
    out.N = out.s[0]; //Código que resolve o problema: out.N = out.s[31];
    out.Z = true;
    for(int i = 0; i < 32; i++){
        if(out.s[i]){ out.Z = false; break;}
    }
}

ULA_output ULA::output(){
    bool F0 = co.control[2], F1 = co.control[3]; //Entrada do decodificador
    ULA_output out = {};        //Declaração do resultado. TODOS os campos inicializam em zero ou false
    ULA_input input_local = in; //Armazena uma cópia do input

    //Converte o vetor [ENA, ENB, INVA, INC] em um inteiro (A mágica ocorre por conta do OR | ) 
    int operacao = (co.control[4] << 3) | (co.control[5] << 2) | (co.control[6] << 1) | (co.control[7] << 0);

    if(!co.control[4]){input_local.A.fill(0);} //Zera todo o campo de A
    if(!co.control[5]){input_local.B.fill(0);} //Zera todo o campo de B

    /**
     * O bloco a seguir, primeiramente, utiliza dois switch case para simular um decodificador.
     * Após o decodificador, há a operação (lógica ou aritmética).
    */
    switch((int)F0){
        case 0:
            switch((int)F1){ 
                case 0:
                    /*____Operações Aritméticas____*/
                            /*____Não possui____*/
                    
                    /*____Operações Lógicas____*/
                    if(operacao == 12){ //Return A and B
                        for(int i = 0; i < 32; i++){
                            out.s[i] = input_local.A[i] & input_local.B[i];
                        }
                    }
                break;
                case 1:
                    /*____Operações Aritméticas____*/
                            /*____Não possui____*/

                    /*____Operações Lógicas____*/
                    if(operacao == 0){    /*Return 0*/    }

                    if(operacao == 4){ //Return B
                        for(int i = 0; i < 32; i++){
                            out.s[i] = input_local.B[i];
                        }
                    }

                    if(operacao == 8){ //Return A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = input_local.A[i];
                        }
                    }

                    if(operacao == 10){ //Return not A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !input_local.A[i];
                        }
                    }

                    if(operacao == 12){ //Return A or B
                        for(int i = 0; i < 32; i++){
                            out.s[i] = input_local.A[i] | input_local.B[i];
                        }
                    }

                break;
                default:
                    std::cout << "[ERROR] - Corrupção de dados detectada nos sinais de entrada correspondente aos [0, F1]" << std::endl;
                    break;
            }
            break;
        case 1:
            switch((int)F1){
                case 0:
                    /*____Operações Aritméticas____*/
                            /*____Não possui____*/
                    
                    /*____Operações Lógicas____*/
                    if(operacao == 12){ //Return not B
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !input_local.B[i];
                        }
                    }
                break;
                case 1:
                    /*____Operações Aritméticas____*/
                    if(operacao == 1){ //Return 1
                        out.s[31] = 1;           //Só o primeiro elemento do array precisa ser 1 (00000 [...] 00001)
                    }

                    if(operacao == 2){ //Return -1
                        for(int i = 0; i < 32; i++){
                            out.s[i] = 1;       //O -1 é dado como todos os valores do array como 1
                        }
                    } 

                    if(operacao == 5){ //Return B + 1
                        add1(input_local.B, out);        //Incremento no B
                    }

                    if(operacao == 6){ //Return B - 1
                        std::array<bool, 32> x;
                        for(int i = 0; i < 32; i++){
                            x[i] = 1;
                        }
                        addXY(input_local.B, x, out);    //Decremento no B
                    }

                    if(operacao == 9){ //Return A + 1
                        add1(input_local.A, out);        //Incremento no A
                    }

                    if(operacao == 11){ //Return -A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !input_local.A[i];
                        }
                        add1(out.s, out);       //Realiza o complemento de dois
                    }

                    if(operacao == 12){ //Return A + B
                        addXY(input_local.A, input_local.B, out); //Adiciono A ao B
                    }

                    if(operacao == 13){ //Return A + B + 1
                        addXY(input_local.A, input_local.B, out); //Armazeno A+B no output
                        add1(out.s, out);       //Adiciono 1 no output
                    }

                    if(operacao == 15){ //Return B - A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !input_local.A[i];
                        }
                        add1(out.s, out);       //Realizo o comp. de dois e armazeno no out 
                        addXY(input_local.B, out.s, out);//Adiciono B ao (!A + 1) que estava armazenado no out
                    }

                    /*____Operações Lógicas____*/
                        /*____Não possui____*/
                break;
                default:
                    std::cout << "[ERROR] - Corrupção de dados detectada nos sinais de entrada correspondentes ao [1, F1]" << std::endl;
                    break;
            }
        break;
        default: //Basicamente uma flag
            std::cout << "[ERROR] - Corrupção de dados detectada no sinal de entrada correspondente ao F0" << std::endl;
            break;
    }

    return out;
}
