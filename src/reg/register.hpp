/**
 * Módulo: TAD Registrador
 * 
 * Objetivos:
 *  -> Definir os atributos, métodos e funções gerais dos registradores de acordo
 *  com os textos (slides e livro);
 *  -> Definir um inicializador geral para o registrador;
 * 
 * Observações:
 *  -> O registrador DEVE possuir o modo de Leitura e Escrita;
 *  -> O registrador DEVE converter o resultado de string para booleano (os que acessam a memória)
 *  -> Retorno: DEVE retornar ou um bool array[6] (o que conecta com a ULA)
 * 
 * Com os objetivos concluídos, será possível simular um registrador de N bits que
 * será utilizado no desenvolvimento da ULA.
*/

#include <iostream>
#include <array> //Esta biblioteca vai ajudar.

/*O que um registrador deve possuir em geral? No caso do projeto, é claro*/
/**
 *  -> Nome;
 *  -> Vetor booleano (N bits);
 *  -> Duas variáveis (Usadas para definir as operações - leitura ou escrita);
 *  -> Forma de acessar o barramento B;
 *  -> Forma do barramento C acessar o registrador
 *  -> Forma de acessar a memória (em alguns casos).
 */

/*Aqui está uma estrutura para um registrador geral*/
/**
 * Note o seguinte: as entradas (input 1 e 2) não necessariamente existem para todos os registradores, vide o  registra-
 * dor H (holder register). Nestes casos onde não há uma das entradas, vamos inicializar este registrador com o valor 
 * da porta não existente como FALSE, i.e, supondo o H, o input 2 seria FALSE, podendo só o input_1 variar.
 */
class Reg{
    protected:
        const std::string nome;    //Nome do registrador
        bool input_1;              //Entrada 1 ESQUERDA => Na imagem do caminho de dados, este imput é o do lado esquerdo
        bool input_2;              // Entrada 2 DIREITA  => Na imagem do caminho de dados, este imput é o do lado direito

    public:

        virtual bool transf(/*No futuro isto terá parâmetros*/);                //Transfere os dados para o barramento B
        virtual std::array<bool, 32> recebe(/*No futuro isto terá parâmetros*/);   
};

/* Classe Reg32, contém todos os parâmetros da classe Reg, mas com alterações na implementação das funções transf() e recebe()*/
class Reg32 : public Reg{
    protected:
        std::array<bool, 32> data; //Informação do registrador                             //Os dados do barramento C são passados para o registrador
        
    public:
        bool transf(/*No futuro isto terá parâmetros*/) override; // Transfere os dados para o barramento B
        std::array<bool, 32> recebe(/*No futuro isto terá parâmetros*/) override;
};

/*Esta classe é responsável pela interação com a memória (relacionados ao registradores de 32 bits, é claro)*/
class Reg32_memory : Reg32{
    public:
        std::array<bool, 32> leituraMemory(std::string arquivo); //Realiza a leitura DA memória (arquivo txt)
        bool escritaMemory(std::string arquivo);                 //Realiza a escrita NA memória (arquivo txt)
};

/*Está classe é mais "tranquila", uma vez que só há o MBR com 8 bits*/
//Está classe tem interação com a memória
class Reg8 : public Reg{ //Toda informação do Reg32 pode ser aplicado aqui..
    private:
        std::array<bool, 8> data;

    public:
        /*Por que 32 bits? Simplesmente Extensão de sinal no barramento B*/
        bool transf(/*No futuro isto terá parâmetros*/) override;
        std::array<bool, 32> recebe(/*No futuro isto terá parâmetros*/) override;
};