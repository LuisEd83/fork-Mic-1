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
    std::array<bool, 8> control;
};

struct ULA_input{
    std::array<bool, 32> A; 
    std::array<bool, 32> B;
};

struct ULA_output{
    std::array<bool, 32> s;
    bool carry_out;
    bool N, Z;
};

/*Funções de soma*/
void add1(std::array<bool, 32> X, ULA_output& out);
void addXY(std::array<bool, 32> X, std::array<bool, 32> Y, ULA_output& out);

/*Classe*/
class ULA{
    private:
        ULA_input in;
        ULA_control co;
    public:
        ULA_output output();  
};

/*
class ULA{
    public:
        ULA_output output(ULA_input& in, ULA_control& co);
};
*/

class Deslocador{
    public:
        void deslocador(ULA_output& out, ULA_control& co);
};

#endif