/**
 * Módulo: Implementação do registrador
 * 
 * Objetivo:
 *  -> Com as funções definidas em register.hpp
*/

#include "register.hpp"

// ====== Reg32 ======
uint32_t Reg32::toUint(){
    uint32_t valor = 0;
    for (int i = 0; i < 32; i++){
        valor = (valor << 1) | data[i];
    }
    return valor;
}

bool Reg32::transf(){
    
}

std::array<bool, 32> Reg32::recebe(){
    
}

// ====== Reg8 ======
uint32_t Reg8::toUint(){
    uint8_t valor = 0;
    for (int i = 0; i < 8; i++){
        valor = (valor << 1) | data[i];
    }
    return static_cast<uint32_t>(valor); // Extensão de zero para 32 bits
}

bool Reg8::transf(){

}

std::array<bool, 32> Reg8::recebe(){
    
}
