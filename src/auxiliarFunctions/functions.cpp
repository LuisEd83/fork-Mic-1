#include "auxiliarFunctions/functions.hpp"

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

void imprimeMemoria(std::ofstream& log,
                    std::array<std::array<bool,32>, 16>& mem){
    log << "*******************************\n";
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 32; j++) log << mem[i][j];
        log << "\n";
    }
    log << "*******************************\n";
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