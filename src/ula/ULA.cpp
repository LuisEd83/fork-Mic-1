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
void addXY(std::array<bool, 32> X, std::array<bool, 32> Y, ULA_output& out){
    int c_out = 0; //Diferente do add1, o c_out inicializa em 0
    int c_out_30, c_out_31;

    for(int i = 0; i < 32; i++){
        out.s[i] = X[i] ^ Y[i] ^ c_out;                     //Resultado da saída do somador
        c_out = (X[i] & Y[i]) | ((X[i] ^ Y[i]) & c_out);    //Realimenta o somador 

        //Por estarmos trabalhando com um array de bits que formam um número signed (com sinal):
        if(i == 30){c_out_30 = c_out;} //Armazena o penúltimo c_out em tempos distintos
        if(i == 31){c_out_31 = c_out;} //Armazena o último c_out em tempos distintos
    }

    //if(c_out_31 ^ c_out_30){
    //    out.carry_out = 1;x
    //}
    out.carry_out = c_out_31;
}

ULA_output ULA::output(){
    bool F0 = co.control[5], F1 = co.control[4]; //Entrada do decodificador
    ULA_output out = {}; //Declaração do resultado. TODOS os campos inicializam em zero ou false

    //Converte o vetor [ENA, ENB, INVA, INC] em um inteiro (A mágica ocorre por conta do OR | ) 
    int operacao = (co.control[3] << 3) | (co.control[2] << 2) | (co.control[1] << 1) | (co.control[0] << 0);

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
            switch((int)F1){
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
                        out.s[0] = 1;           //Só o primeiro elemento do array precisa ser 1 (00000 [...] 00001)
                    }

                    if(operacao == 2){ //Return -1
                        for(int i = 0; i < 32; i++){
                            out.s[i] = 1;       //O -1 é dado como todos os valores do array como 1
                        }
                    } 

                    if(operacao == 5){ //Return B + 1
                        add1(in.B, out);        //Incremento no B
                    }

                    if(operacao == 6){ //Return B - 1
                        std::array<bool, 32> x;
                        for(int i = 0; i < 32; i++){
                            x[i] = 1;
                        }
                        addXY(in.B, x, out);    //Decremento no B
                    }

                    if(operacao == 9){ //Return A + 1
                        add1(in.A, out);        //Incremento no A
                    }

                    if(operacao == 11){ //Return -A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !in.A[i];
                        }
                        add1(out.s, out);       //Realiza o complemento de dois
                    }

                    if(operacao == 12){ //Return A + B
                        addXY(in.A, in.B, out); //Adiciono A ao B
                    }

                    if(operacao == 13){ //Return A + B + 1
                        addXY(in.A, in.B, out); //Armazeno A+B no output
                        add1(out.s, out);       //Adiciono 1 no output
                    }

                    if(operacao == 15){ //Return B - A
                        for(int i = 0; i < 32; i++){
                            out.s[i] = !in.A[i];
                        }
                        add1(out.s, out);       //Realizo o comp. de dois e armazeno no out 
                        addXY(in.B, out.s, out);//Adiciono B ao (!A + 1) que estava armazenado no out
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

void Deslocador::deslocador(ULA_output& out, ULA_control& co){ 
    // SLL8: desloca 8 bits para a esquerda (lógico — preenche com 0)
    if(co.control[7]) { // SLL8
        std::array<bool, 32> shifted = {};
        for(int i = 8; i < 32; i++){shifted[i] = out.s[i - 8];}
        out.s = shifted;
    }
    // SRA1: desloca 1 bit para a direita (aritmético — preserva bit de sinal)
    if(co.control[6]) { // SRA1
        std::array<bool, 32> shifted = out.s; // cópia do estado original
        bool sinal = shifted[31];
        for(int i = 0; i < 31; i++){out.s[i] = shifted[i + 1];}
        out.s[31] = sinal; // preserva o sinal
    }

    // N e Z sempre calculados sobre o Sd final (pós-deslocamento)
    out.N = out.s[31];
    out.Z = true;
    for(int i = 0; i < 32; i++){
        if(out.s[i]){ out.Z = false; break;}
    }
}

//Chat:
// L: Tá aparecendo um erro fantasma KKKKKKKK
// L: Tá dizendo que a declaração está errada, mas elas está correta, varei
// L: Pronto, deslocador feito (ele calcula também as flags N e Z) 
// T: é o L né, mano?
// L: Agora a gente tem que implementar lá no código da Mic e testar se deu certo
// T: Um novo código? Ou alterar o antigo?
// L: Eu tô pensando em alterar. Aquilo etm muita gambiarra (ao menos senti isso)
// T: Entendi 
// T: Eu acho que dá pra unificar a ULA, colocando ULA_control, ULA_input, etc como parâmetro da classe ULA.
// Percebe que tu declarou ULA_control dentro da  main? Sendo que há uma instância de uma ULA fora da função?
// Acredito que dava pra deixar tudo junto na mesma classe e operar uma só variável
// L: Ahh, faz sentido, o Claude disse pra não fazer isso ()mas ele fez, não sei pq, pois ele disse que eu 
// T: "o Claude disse pra não fazer isso", pq?
