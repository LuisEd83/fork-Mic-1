/**
 * (ISSO É PARA O TAD)
 * Módulo: TAD Registrador
 * 
 * Objetivos:
 *  -> Definir os atributos, métodos e funções gerais dos regitradores de acordo
 *  com os textos (slides e livro);
 *  -> Definir um inicializador geral para o registrador;
 * 
 * Observações:
 *  -> O registrador DEVE possuir o modo de Leitura e Escrita;
 * 
 * Com os objetivos concluídos, será possível simular um registrador de N bits que
 * será utilizado no desenvolvimento da ULA.
*/

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#define SIZE_32 32
#define SIZE_8 8

class Register {

};

class Reg32 : public Register {
        private:
                int size;
                const std::string name;
                std::unique_ptr<bool> data;

        public:

                Reg32(std::string n) : size(SIZE_32), name(n) {
                        data = std::make_unique<bool>(SIZE_32);
                }

                
};


class Reg32 {
        private:
                const char* name
                int 

        public:
                
};

class Reg8 {
        public:
                char data;
}