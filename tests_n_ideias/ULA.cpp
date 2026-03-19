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

//#include "register.hpp"   //Para desenvolver depois
//#include "ULA.hpp"        //Para desenvolver depois

//Struct que armazena os sinais de entrada
typedef struct ULA_control{
    bool control[6];
    //int32_t control
};

//Struct que armazena os dados de entrada da ULA, i.e, os dados dos barramentos A e B.
typedef struct ULA_input{
    bool A[32]; 
    bool B[32];
};

//Struct que armazena as saídas da ULA, desde as flags até o vetor de 32 bits.
typedef struct ULA_output{
    bool s[32];
    bool carry_out;
    //bool N, Z (Por enquanto não serão necessários)
};

//Esta função realiza o +1 nas entradas
void add1(bool X[], ULA_output& out){
    int c_out = 1;
    int c_out_30, c_out_31;
    for(int i = 0; i < 32; i++){
        if(c_out == 1){
            if(X[i] == 1){
                out.s[i] = 0;
                c_out = 1;
            }else{ //X[i] == 0
                out.s[i] = 1;
                c_out = 0;
            }
        }else{ //c_out == 0
            out.s[i] = X[i];
        }

        //Por estarmos trabalhando com um array de bits que formam um número signed (com sinal):
        if(i == 30){c_out_30 = c_out;} //Armazena o penúltimo c_out em tempos distintos
        if(i == 31){c_out_31 = c_out;} //Armazena o último c_out em tempos distintos
    }
    if(c_out_31 ^ c_out_30){
        out.carry_out = 1;
    }
}

//Esta função soma dois vetores de bits
void addXY(bool X[], bool Y[], ULA_output& out){
    int c_out = 0; //Diferente do add1, o c_out inicializa em 0
    int c_out_30, c_out_31;

    for(int i = 0; i < 32; i++){
        out.s[i] = X[i] ^ Y[i] ^ c_out;                     //Resultado da saída do somador
        c_out = (X[i] & Y[i]) | ((X[i] ^ Y[i]) & c_out);    //Realimenta o somador 

        //Por estarmos trabalhando com um array de bits que formam um número signed (com sinal):
        if(i == 30){c_out_30 = c_out;} //Armazena o penúltimo c_out em tempos distintos
        if(i == 31){c_out_31 = c_out;} //Armazena o último c_out em tempos distintos
    }

    if(c_out_31 ^ c_out_30){
        out.carry_out = 1;
    }
}

class ULA{
    public:
        ULA_output output(ULA_input& in, ULA_control& co){
            bool F0 = co.control[5], F1 = co.control[4]; //Entrada do decodificador
            ULA_output out = {}; //Declaração do resultado. TODOS os campos inicializam em zero ou false

            //Converte o vetor [ENA, ENB, INVA, INC] em um inteiro (A mágica ocorre por conta do OR | ) 
            int operacao = (co.control[3] << 3) | (co.control[2] << 2) | (co.control[1] << 1) | (co.control[0] << 0);

            /**
             * O bloco a seguir, primeiramente, utiliza dois switch case para simular um decodificador.
             * Após o decodificador, há a operação (lógica ou aritmética).
            */
            switch(F0){
                case 0:
                    switch(F1){ 
                        case 0:
                            /*____Operações Aritméticas____*/
                                 /*____Não possui____*/
                            
                            /*____Operações Lógicas____*/
                            if(operacao == 12){ //Return A and B
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = in.A[i] & in.B[i];
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
                                    out.s[i] = in.B[i];
                                }
                            }

                            if(operacao == 8){ //Return A
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = in.A[i];
                                }
                            }

                            if(operacao == 10){ //Return not A
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = !in.A[i];
                                }
                            }

                            if(operacao == 12){ //Return A or B
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = in.A[i] | in.B[i];
                                }
                            }

                        break;
                        default:
                            std::cout << "[ERROR] - Corrupção de dados detectada nos sinais de entrada correspondente aos [0, F1]" << std::endl;
                            break;
                    }
                    break;
                case 1:
                    switch(F1){
                        case 0:
                            /*____Operações Aritméticas____*/
                                  /*____Não possui____*/
                            
                            /*____Operações Lógicas____*/
                            if(operacao == 12){ //Return not B
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = !in.B[i];
                                }
                            }
                        break;
                        case 1:
                            /*____Operações Aritméticas____*/
                            if(operacao == 1){ //Return 1
                                out.s[0] = 1;
                            }

                            if(operacao == 2){ //Return -1
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = 1;
                                }
                            } 

                            if(operacao == 5){ //Return B + 1
                                add1(in.B, out);
                            }

                            if(operacao == 6){ //Return B - 1
                                bool x[32];
                                for(int i = 0; i < 32; i++){
                                    x[i] = 1;
                                }
                                addXY(in.B, x, out);
                            }

                            if(operacao == 9){ //Return A + 1
                                add1(in.A, out);
                            }

                            if(operacao == 11){ //Return -A
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = !in.A[i];
                                }
                                add1(out.s, out);
                            }

                            if(operacao == 12){ //Return A + B
                                addXY(in.A, in.B, out);
                            }

                            if(operacao == 13){ //Return A + B + 1
                                addXY(in.A, in.B, out); //Armazeno A+B no output
                                add1(out.s, out);       //Adiciono 1 no output
                            }

                            if(operacao == 15){ //Return B - A
                                for(int i = 0; i < 32; i++){
                                    out.s[i] = !in.A[i];
                                }
                                add1(out.s, out);
                                addXY(in.B, out.s, out);
                            }

                            /*____Operações Lógicas____*/
                                /*____Não possui____*/
                        break;
                        default:
                            std::cout << "[ERROR] - Corrupção de dados detectada nos sinais de entrada correspondentes ao [1, F1]" << std::endl;
                            break;
                    }

                default: //Basicamente uma flag
                    std::cout << "[ERROR] - Corrupção de dados detectada no sinal de entrada correspondente ao F0" << std::endl;
                    break;
            }

            return out;
        }

        void log(){} //Implementar depois
};
