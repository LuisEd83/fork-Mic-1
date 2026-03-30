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

// Aplica ENA e ENB nas entradas antes de passar para a ULA
ULA_input aplicaEnable(ULA_input& entrada, ULA_control& co){
    ULA_input resultado = entrada;

    // ENA = control[3], ENB = control[2]
    if(!co.control[3]) resultado.A.fill(0); // ENA desligado -> A = 0
    if(!co.control[2]) resultado.B.fill(0); // ENB desligado -> B = 0

    return resultado;
}

void converteInt32(std::string valor, std::array<bool, 32>& data){
    for(int i = 31; i >= 0; i++){
        data[i] = (valor[0] == '1');
    }
}
void converteInt8(std::string valor, std::array<bool, 8>& data){
    for(int i = 7; i >= 0; i++){
        data[i] = (valor[0] == '1');
    }
}

void imprimeArray(std::ofstream& log, std::array<bool, 32> arr){
    for(int i = 0; i < 32; i++){log << arr[i];} // MSB primeiro, naturalmente
    log << "\n";
}

void imprimeArray8(std::ofstream& log, std::array<bool, 8> arr){
    for(int i = 0; i < 8; i++){log << arr[i];} // MSB primeiro, naturalmente
    log << "\n";
}

void imprimeRegistradores(std::ofstream& log,
                           Reg32_memory& MAR, Reg32_memory& MDR, Reg32_memory& PC,
                           Reg8& MBR, Reg32& SP, Reg32& LV, Reg32& CPP,
                           Reg32& TOS, Reg32& OPC, Reg32& H){
    log << "mar = "; imprimeArray(log, MAR.data);
    log << "mdr = "; imprimeArray(log, MDR.data);
    log << "pc = ";  imprimeArray(log, PC.data);
    log << "mbr = "; imprimeArray8(log, MBR.data);
    log << "sp = ";  imprimeArray(log, SP.data);
    log << "lv = ";  imprimeArray(log, LV.data);
    log << "cpp = "; imprimeArray(log, CPP.data);
    log << "tos = "; imprimeArray(log, TOS.data);
    log << "opc = "; imprimeArray(log, OPC.data);
    log << "h = ";   imprimeArray(log, H.data);
}

// Nomes dos registradores do barramento B
std::string nomeBarB(uint8_t codigo){
    switch(codigo){
        case 0: return "mdr";
        case 1: return "pc";
        case 2: return "mbr";
        case 3: return "mbru";
        case 4: return "sp";
        case 5: return "lv";
        case 6: return "cpp";
        case 7: return "tos";
        case 8: return "opc";
        default: return "?";
    }
}

// Nomes dos registradores habilitados no barramento C
std::string nomeBarC(uint16_t mascara){
    std::string resultado = "";
    // Bit: 8=H 7=OPC 6=TOS 5=CPP 4=LV 3=SP 2=PC 1=MDR 0=MAR
    if((mascara >> 8) & 1) resultado += "h, ";
    if((mascara >> 7) & 1) resultado += "opc, ";
    if((mascara >> 6) & 1) resultado += "tos, ";
    if((mascara >> 5) & 1) resultado += "cpp, ";
    if((mascara >> 4) & 1) resultado += "lv, ";
    if((mascara >> 3) & 1) resultado += "sp, ";
    if((mascara >> 2) & 1) resultado += "pc, ";
    if((mascara >> 1) & 1) resultado += "mdr, ";
    if((mascara >> 0) & 1) resultado += "mar, ";
    // Remove a vírgula e espaço finais
    if(!resultado.empty()) resultado = resultado.substr(0, resultado.size() - 2);
    return resultado;
}

/*Reformulação da main*/
int main(){
    /*Abertura de leitura e escrita de arquivos:*/
    std::ifstream programa("tests/programa_etapa2_tarefa2.txt");            //Arquivo que contém o programa
    std::ofstream log("resultados/log_execucao.txt");                       //Arquivo de log
    
    /*Variáveis de Estado do caminho de dados*/
    ULA ula;            //Instanciação da ULA
    Reg8 MBR;           //Instanciação do registrador de 8 bits que possui relação com a memória
    Reg32_memory MAR;   //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32_memory MDR;   //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32_memory PC;    //Instanciação do registrador de 32 bits que possui relação com a memória       
    Reg32 SP;           //Instanciação do registrador de 32 bits        
    Reg32 LV;           //Instanciação do registrador de 32 bits        
    Reg32 CPP;          //Instanciação do registrador de 32 bits        
    Reg32 TOS;          //Instanciação do registrador de 32 bits        
    Reg32 OPC;          //Instanciação do registrador de 32 bits        
    Reg32 H;            //Instanciação do registrador de 32 bits        

    /*Atribui os valores para os registradores*/
    std::string conteudo = getExpression("tests/registradores_etapa2_tarefa2.txt");
    std::vector<Token> tokens = tokenize(conteudo);

    // Percorre os tokens: padrão é REGISTRADOR = NUMERO
    for(int i = 0; i < tokens.size() - 1; ){
        if(tokens[i].tipo == Tipo::EOF_TOKEN) break;

        // Espera: [Registrador] [=] [Numero]
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

    //Cabeçalho com estado inicial
    log << "=====================================================\n";
    log << "> Initial register states\n";
    log << "=====================================================\n";
    imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);
    log << "=====================================================\n";
    log << "Start of program\n";
    log << "=====================================================\n";

    std::string linha;
    int ciclo = 0;

    while(std::getline(programa, linha)){
        if(!linha.empty() && linha.back() == '\r') linha.pop_back();
        if(linha.empty()) break;

        ciclo++;

        //Parse dos 21 bits
        //[0-7] = ULA control, [8-16] = barramento C, [17-20] = barramento B
        for(int i = 0; i < 8; i++)
            ula.co.control[i] = (linha[i] == '1');

        uint16_t mascaraC = 0;
        for(int i = 8; i <= 16; i++)
            mascaraC = (mascaraC << 1) | (linha[i] == '1');

        uint8_t codigoB = 0;
        for(int i = 17; i <= 20; i++)
            codigoB = (codigoB << 1) | (linha[i] == '1');

        // Cabeçalho do ciclo — formata IR com espaços como na saída esperada
        log << "Cycle " << ciclo << "\n";
        log << "ir = " << linha.substr(0,8) << " " 
                       << linha.substr(8,9) << " " 
                       << linha.substr(17,4) << "\n";
        log << "b_bus = " << nomeBarB(codigoB) << "\n";
        log << "c_bus = " << nomeBarC(mascaraC) << "\n";

        // Estado antes
        log << "> Registers before instruction\n";
        imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);

        //Monta entradas da ULA
        ula.in.A = H.recebe();
        ula.in.B = decodificador(codigoB, OPC, TOS, CPP, LV, SP, MBR, PC, MDR);
        log << "[DEBUG] ula.in.B = "; imprimeArray(log, ula.in.B);
        log << "[DEBUG] ula.in.A = "; imprimeArray(log, ula.in.A);

        // Executa ULA e deslocador
        ULA_output out = ula.output();
        ula.deslocador(out);

        log << "[DEBUG] mascaraC = " << mascaraC << "\n";
        log << "[DEBUG] out.s = "; imprimeArray(log, out.s);
        ula.deslocador(out);
        log << "[DEBUG] out.s pós deslocador = "; imprimeArray(log, out.s);

        // Escreve nos registradores habilitados pelo barramento C
        seletor(mascaraC, out.s, H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR);

        log << "[DEBUG] sp após seletor: "; imprimeArray(log, SP.data);
        log << "[DEBUG] lv após seletor: "; imprimeArray(log, LV.data);

        // Estado depois
        log << "> Registers after instruction\n";
        imprimeRegistradores(log, MAR, MDR, PC, MBR, SP, LV, CPP, TOS, OPC, H);
        log << "=====================================================\n";
    }

    // EOP
    ciclo++;
    log << "Cycle " << ciclo << "\n";
    log << "No more lines, EOP.\n";

    programa.close();
    log.close();
    return 0;
}