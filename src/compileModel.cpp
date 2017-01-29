#include "rwriter.hpp"

#include <fstream>
#include <stan/lang/compiler.hpp>

extern "C"
SEXP
R_generateModelCode(SEXP r_modelFile, SEXP r_modelName, SEXP r_allowUndefined,
               SEXP r_outFileName, SEXP r_msgsAsWarnings)
{
    std::string modelName(CHAR(STRING_ELT(r_modelName, 0)));
    bool allow_undefined = (bool) LOGICAL(r_allowUndefined)[0];

    std::ostringstream msgs;
    std::ifstream in(CHAR(STRING_ELT(r_modelFile, 0)));
    if(!in) {
       PROBLEM "Failed to open model file name %s", CHAR(STRING_ELT(r_modelFile, 0))
          ERROR;
    }

    std::ostream *out_p;
    std::ofstream *fout_p;
    std::ostringstream *outstr_p;
    // if r_outFileName is empty, write to a buffer; otherwise write to the specified file
    if(Rf_length(r_outFileName)) {
        out_p = fout_p = new std::ofstream(CHAR(STRING_ELT(r_outFileName, 0)));
        if(!out_p || !fout_p->is_open()) {
            PROBLEM "Failed to open output file name %s", CHAR(STRING_ELT(r_outFileName, 0))
                ERROR;
        }
    } else
        out_p = outstr_p = new std::ostringstream();
    

    bool ans;
    try {
        ans = stan::lang::compile(&msgs, in, *out_p, modelName, allow_undefined);
    } catch(const std::exception& e) {
        PROBLEM "Stan compiler error:\n %s\n%s", e.what(), msgs.str().data()
            ERROR;        
    }

    in.close();

    if(msgs.str().length()) {
        PROBLEM "Stan compiler warning(s):\n %s", msgs.str().data()
            WARN;
    }

    SEXP r_ans;
    if(Rf_length(r_outFileName))  {
       fout_p->close();
       r_ans = r_outFileName;
    } else {
       out_p->flush();
       r_ans = ScalarString(mkChar(outstr_p->str().c_str()));
    }

    if(!ans) {
        PROBLEM "Failed to parse and generate C++ code"
          ERROR;
    }

    return(r_ans); 
}
