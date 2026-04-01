/**
 * Módulo FINAL: Implementação COMPLETA da microarquitetura MIC-1
 * 
 *  Objetivo:
 *  -> Unificar TODOS os arquivos;
 *  -> Implementar a MIC-1 de forma coerente e consisa.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <array>

#include "ula/ULA.hpp"
#include "reg/register.hpp"
#include "lexer/lexer.hpp"
#include "auxiliarFunctions/functions.hpp"

/*Reformulação da main*/
int main(){
    /*Abertura de leitura e escrita de arquivos:*/
    std::ifstream arqDados("src/memory/memory_Data.txt");           //Arquivo que contém os dados (memória)
    std::ifstream programa("src/memory/memory_Instructions.txt");   //Arquivo que contém o programa
    std::ofstream log("resultados/log_execucao.txt");               //Arquivo que armazena a saída
    
    if(!arqDados.is_open()){ std::cerr << "[FILE Error] memory_Data.txt\n";         return 1; }
    if(!programa.is_open()){ std::cerr << "[FILE Error] memory_Instructions.txt\n"; return 1; }
    if(!log.is_open())     { std::cerr << "[FILE Error] log_execucao.txt\n";        return 1; }
    
    /*Variáveis de Estado do caminho de dados*/
    ULA ula = {};            //Instanciação da ULA
    Reg8 MBR = {};           //Instanciação do registrador de 8 bits que possui relação com a memória
    Reg32_memory MAR = {};   //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32_memory MDR = {};   //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32_memory PC = {};    //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32 SP = {};           //Instanciação do registrador de 32 bits        
    Reg32 LV = {};           //Instanciação do registrador de 32 bits        
    Reg32 CPP = {};          //Instanciação do registrador de 32 bits        
    Reg32 TOS = {};          //Instanciação do registrador de 32 bits        
    Reg32 OPC = {};          //Instanciação do registrador de 32 bits        
    Reg32 H = {};            //Instanciação do registrador de 32 bits        

    /*Memória de dados — 16 palavras de 32 bits*/
    std::array<std::array<bool, 32>, 16> memoria = {};

     /*Carrega memória de dados*/
    {
        std::string linhaM;
        int idx = 0;
        while(std::getline(arqDados, linhaM) && idx < 16){
            if(!linhaM.empty() && linhaM.back() == '\r'){linhaM.pop_back();}
            if(linhaM.empty()){continue;}

            for(int j = 0; j < 32; j++){
                memoria[idx][j] = (linhaM[j] == '1');
            }

            idx++;
        }
    }

    /*Atribui os valores para os registradores*/
    std::string conteudo = getExpression("tests/registradores_etapa3_tarefa1.txt");
    std::vector<Token> tokens = tokenize(conteudo);

    //Percorre os tokens: padrão é REGISTRADOR = NUMERO
    for(int i = 0; i < tokens.size() - 1; ){
        if(tokens[i].tipo == Tipo::EOF_TOKEN) break;

        //Espera: [Registrador] [=] [Numero]
        Token regTok  = tokens[i];
        Token attrTok = tokens[i+1];
        Token valTok  = tokens[i+2];

        if(attrTok.tipo != Tipo::Attr || valTok.tipo != Tipo::Numero){
            std::cerr << "[PARSE Error] Formato inválido no arquivo de registradores\n";
            exit(1);
        }

        std::string valor = valTok.lexema; // string de 32 bits ex: "00000000..."

        auto preenche32 = [&](Reg32& r){
            for(int j = 0; j < 32; j++) r.data[j] = (valor[j] == '1');
        };
        auto preenche8 = [&](Reg8& r){
            for(int j = 0; j < 8; j++) r.data[j] = (valor[j] == '1');
        };

        switch(regTok.tipo){
            case Tipo::R_MAR: preenche32(MAR); break;
            case Tipo::R_MDR: preenche32(MDR); break;
            case Tipo::R_PC:  preenche32(PC);  break;
            case Tipo::R_MBR: preenche8(MBR);  break;
            case Tipo::R_SP:  preenche32(SP);  break;
            case Tipo::R_LV:  preenche32(LV);  break;
            case Tipo::R_CPP: preenche32(CPP); break;
            case Tipo::R_TOS: preenche32(TOS); break;
            case Tipo::R_OPC: preenche32(OPC); break;
            case Tipo::R_H:   preenche32(H);   break;
            default:
                std::cerr << "[PARSE Error] Token inesperado: " << regTok.lexema << "\n";
                exit(1);
        }

        i += 3; // avança [reg] [=] [valor]
    }

    /*Cabeçalho inicial*/
    log << "============================================================\n";
    log << "Initial memory state\n";
    imprimeMemoria(log, memoria);
    log << "Initial register state\n";
    imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);
    log << "============================================================\n";
    log << "Start of Program\n";
    log << "============================================================\n";

    std::string linha;
    int ciclo = 0;

    while(std::getline(programa, linha)){
        if(!linha.empty() && linha.back() == '\r') linha.pop_back();
        if(linha.empty()) break;

        ciclo++;

        //Parse dos 21 bits
        //instrução: [0-7]=ULA [8-16]=C [17-18]=MEM [19-22]=B
        for(int i = 0; i < 8; i++)
            ula.co.control[i] = (linha[i] == '1');

        uint16_t mascaraC = 0;
        for(int i = 8; i <= 16; i++)
            mascaraC = (mascaraC << 1) | (linha[i] == '1');

        bool bitWRITE = (linha[17] == '1');
        bool bitREAD  = (linha[18] == '1');

        uint8_t codigoB = 0;
        for(int i = 19; i <= 22; i++)
            codigoB = (codigoB << 1) | (linha[i] == '1');

        //Cabeçalho do ciclo — formata IR com espaços como na saída esperada
        log << "Cycle " << ciclo << "\n";
        log << "ir = "
            << linha.substr(0,  8) << " "
            << linha.substr(8,  9) << " "
            << linha.substr(17, 2) << " "
            << linha.substr(19, 4) << "\n";
        log << "b = " << nomeBarB(codigoB) << "\n";
        log << "c = " << nomeBarC(mascaraC) << "\n";
        log << "\n";

        //Estado antes
        log << "> Registers before instruction\n";
        log << "*******************************\n";
        imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);

        //Monta entradas da ULA
        ula.in.A = H.recebe();
        ula.in.B = decodificador(codigoB, OPC, TOS, CPP, LV, SP, MBR, PC, MDR);

        //Executa ULA e deslocador
        ULA_output out = ula.output();
        ula.deslocador(out);

        //Escreve nos registradores habilitados pelo barramento C
        seletor(mascaraC, out.s, H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR);

        /*Executo a LEITURA ou ESCRITA na memória*/
        if(bitWRITE && !bitREAD){
            uint32_t endereco = 0;
            for(int i = 0; i < 32; i++){
                endereco = (endereco << 1) | MAR.data[i];
            }
            if(endereco < 16){memoria[endereco] = MDR.data;}
        }
        else if(bitREAD && !bitWRITE){
            uint32_t endereco = 0;
            for(int i = 0; i < 32; i++){
                endereco = (endereco << 1) | MAR.data[i];
            }
            if(endereco < 16){MDR.data = memoria[endereco];}
        }

        //Estado depois
        log << "\n";
        log << "> Registers after instruction\n";
        log << "*******************************\n";
        imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);

        /*Memória após a instrução*/
        log << "\n";
        log << "> Memory after instruction\n";
        imprimeMemoria(log, memoria);
        log << "============================================================\n";

    }

    //EOP
    ciclo++;
    log << "Cycle " << ciclo << "\n";
    log << "No more lines, EOP.\n";

    programa.close();
    log.close();
    return 0;
}