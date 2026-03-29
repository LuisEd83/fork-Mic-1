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

// Aplica ENA e ENB nas entradas antes de passar para a ULA
ULA_input aplicaEnable(ULA_input& entrada, ULA_control& co){
    ULA_input resultado = entrada;

    // ENA = control[3], ENB = control[2]
    if(!co.control[3]) resultado.A.fill(0); // ENA desligado -> A = 0
    if(!co.control[2]) resultado.B.fill(0); // ENB desligado -> B = 0

    return resultado;
}

void imprimeArray(std::ofstream& log, std::array<bool, 32> arr){
    for(int i = 0; i < 32; i++) log << arr[i]; // MSB primeiro, naturalmente
    log << "\n";
}

/*Reformulação da main*/
int main(){
    /*Abertura de leitura e escrita de arquivos:*/
    std::ifstream programa("tests/programa_etapa2_tarefa1.txt");
    std::ofstream log("resultados/log_execucao.txt");

    /*Inicialização de variáveis*/
    std::string linha;  //Armazena a linha de execução
    int PC = 0;         //Junto com a variável acima, simulam o registrador PC
    ULA ula;            //Instanciação da ULA

    /*Constantes dadas pelo professor*/
    ula.in.A.fill(0);
    ula.in.B.fill(0);
    ula.in.A[31] = 1;  // LSB = índice 31
    ula.in.B[0]  = 1;  // MSB = índice 0

    //Cabeçalho
    log << "b = "; imprimeArray(log, ula.in.B);
    log << "a = "; imprimeArray(log, ula.in.A);
    log << "Start of Program\n";
    log << "============================================================\n";

    while(std::getline(programa, linha)){
        PC++;       //Incrementa o PC

        if(!linha.empty() && linha.back() == '\r'){linha.pop_back();}

        /*Verifica se a linha é vazia*/
        if(linha.empty()){
            log << "Cycle " << PC << "\n";
            log << "PC = " << PC << "\n";
            log << "> Line is empty, EOP.\n";
            break;
        }

        log << "Cycle " << PC << "\n";
        log << "PC = " << PC << "\n";
        log << "IR = " << linha << "\n";
        
        /*Extraio a instrução da linha*/
        size_t tam_linha = linha.size();
        for(int i = 0; i < tam_linha; i++){ /*PARTE IMPORTANTE! NÃO RETIRE*/
            ula.co.control[i] = (linha[i] == '1'); 
        }

        /*Chamo o deslocador*/
        // SLL8 = control[0], SRA1 = control[1]
        if(ula.co.control[0] && ula.co.control[1]){
            log << "> Error, invalid control signals.\n";
            log << "============================================================\n";
            continue; //pula para a próxima instrução
        }
        log << "b = "; imprimeArray(log, ula.in.B);
        log << "a = "; imprimeArray(log, ula.in.A); 
        
        /*Chamo ULA para cálculo*/
        ULA_output out; //Variável para armazenar a saída da ULA
        out = ula.output();
        /*Print da saída da ULA*/
        log << "s = "; imprimeArray(log, out.s);
        
        /*Chama deslocador*/
        ula.deslocador(out);
        /*Print da saída após o deslocador*/
        log << "sd = "; imprimeArray(log, out.s); 
        
        
        log << "n = " << out.N << "\n";
        log << "z = " << out.Z << "\n";
        log << "co = " << out.carry_out << "\n";
        log << "============================================================\n";
    }

    /*Verifica se chegou ao final do arquivo*/
    if(programa.eof()){
        PC++;
        log << "Cycle " << PC << "\n";
        log << "PC = " << PC << "\n";
        log << "> Line is empty, EOP.\n";
    }

    /*Fechando arquivo*/
    programa.close();
    log.close();
    return 0;
}



/*
int main(){
    // Valores iniciais de A e B
    ULA_input entrada;
    entrada.A.fill(1);          // A = 11111...1
    entrada.B.fill(0);
    entrada.B[0] = 1;           // B = 00000...1
    bool ena_anterior = true; 

    std::ifstream programa("tests/programa_etapa1.txt");
    std::ofstream log("resultados/log_execucao.txt");

    // Cabeçalho
    log << "b = "; imprimeArray(log, entrada.B);
    log << "a = "; imprimeArray(log, entrada.A);
    log << "Start of Program\n";
    log << "============================================================\n";

    std::string linha;
    int PC = 0;
    ULA ula;

    while(std::getline(programa, linha)){
        PC++;

        // Remove \r caso exista
        if(!linha.empty() && linha.back() == '\r'){linha.pop_back();}

        // Linha vazia = EOP
        if(linha.empty()){
            log << "Cycle " << PC << "\n";
            log << "PC = " << PC << "\n";
            log << "> Line is empty, EOP.\n";
            break;
        }

        // Monta ULA_control
        ULA_control co;
        co.control[5] = (linha[0] == '1'); // F0
        co.control[4] = (linha[1] == '1'); // F1
        co.control[3] = (linha[2] == '1'); // ENA
        co.control[2] = (linha[3] == '1'); // ENB
        co.control[1] = (linha[4] == '1'); // INVA
        co.control[0] = (linha[5] == '1'); // INC

        if(co.control[3]) entrada.A.fill(1);       // ENA atual = 1, restaura
        else if(!ena_anterior) entrada.A.fill(0);  // ENA anterior = 0, zera
        
        // Aplica enable para exibir no log o que a ULA enxerga
        ULA_input visivel = aplicaEnable(entrada, co);

        ULA_output resultado = ula.output(visivel, co); //Aqui?
        
        // Log do ciclo
        log << "Cycle " << PC << "\n";
        log << "PC = " << PC << "\n";
        log << "IR = " << linha << "\n";
        log << "b = "; imprimeArray(log, entrada.B);
        log << "a = "; imprimeArray(log, entrada.A);
        log << "s = "; imprimeArray(log, resultado.s);
        log << "co = " << resultado.carry_out << "\n";
        log << "============================================================\n";
        
        ena_anterior = co.control[3];
    }

    if(programa.eof()){
        PC++;
        log << "Cycle " << PC << "\n";
        log << "PC = " << PC << "\n";
        log << "> Line is empty, EOP.\n";
    }   


    programa.close();
    log.close();
    return 0;
}
*/