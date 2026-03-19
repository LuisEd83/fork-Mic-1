/**
 * Módulo: Implementação do registrador
 * 
 * Objetivo:
 *  -> Com as funções definidas em register.hpp
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
                const char* name;

        public:
                
};

class Reg8 {
        public:
                char data;
};