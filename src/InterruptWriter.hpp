#include "Rstanwriter.hpp"

public class RInterruptWriter : public stan::callbacks::interrupt 
{
  public:
    RInterruptWriter() {}
    void operator() {
        PROBLEM "stan engine interrupted"
            ERROR;
    }
};

