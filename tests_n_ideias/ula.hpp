#ifndef ULA_HPP
#define ULA_HPP

#include <string>

struct Output {
    unsigned int s;
    int carry_out;
};

class ULA{
    private:
        unsigned int A, B;
        int ENA, ENB, INVA, INC;
        int F0, F1;

        struct Inputs {
            unsigned int A;
            unsigned int B;
            int carry_in;
        };

        Inputs preparing_inputs() const;
        Output calculate_output(const Inputs& input) const;

    public:
        ULA(unsigned int A, unsigned int B);
        void instructions(std::string& instruction);
        Output execute();
};

#endif