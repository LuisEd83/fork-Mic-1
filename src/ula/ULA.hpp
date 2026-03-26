#ifndef ULA_HPP
#define ULA_HPP

/**
 * Módulo: ULA Header
 * 
 * Objetivos:
 *  -> Definir as funções, classes (atributos ou métodos) e estruturas que serão utilizadas
 *  no arquivo ULA.cpp
 * 
 * Com isto feito, será possível linkar os arquivos
*/

#include <iostream>
#include <array>

/*Estruturas necesárias*/
struct ULA_control{
    std::array<bool, 6> control;
};

struct ULA_input{
    std::array<bool, 32> A; 
    std::array<bool, 32> B;
};

struct ULA_output{
    std::array<bool, 32> s;
    bool carry_out;
    //bool N, Z (Por enquanto não serão necessários)
};

/*Funções de soma*/
void add1(std::array<bool, 32> X, ULA_output& out);
void addXY(std::array<bool, 32> X, std::array<bool, 32> Y, ULA_output& out);

/*Classe*/
class ULA{
    public:
        ULA_output output(ULA_input& in, ULA_control& co);
        void log();
};

/*Função da ULA completa (com os registradores, mas sem o deslocador) */
std::array<bool, 32> ULA_completa(const std::string nome_arq); //Retorno: array com 32 booleanos
/*
Observação, a variável booleana que deve receber o retorno da ULA_completa deve ser do tipo:
std::array<bool, 32>
*/


#endif