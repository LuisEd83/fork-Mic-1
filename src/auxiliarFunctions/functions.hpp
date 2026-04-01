
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <fstream>
#include <array>
#include <string>
#include <vector>
#include <map>

#include "reg/register.hpp"
#include "lexer/lexer.hpp"

using Microinstrucao = std::array<bool, 23>;
using Programa       = std::vector<Microinstrucao>;
using TabelaMicro    = std::map<std::string, Microinstrucao>;

void imprimeArray(std::ofstream& log, std::array<bool, 32> arr);

void imprimeRegistradores(std::ofstream& log,

                           Reg32_memory& MAR, Reg32_memory& MDR, Reg32_memory& PC,
                           Reg8& MBR, Reg32& SP, Reg32& LV, Reg32& CPP,
                           Reg32& TOS, Reg32& OPC, Reg32& H);

void imprimeMemoria(std::ofstream& log,
                    std::array<std::array<bool,32>, 16>& mem);

std::string nomeBarB(uint8_t codigo);

std::string nomeBarC(uint16_t mascara);


Microinstrucao fromString(const std::string& s);
TabelaMicro buildTabela();
Programa traduzir(const std::vector<Token>& tokens, int& pos,
                  const TabelaMicro& tab);

#endif