#include <Rdefines.h>

#ifdef length
#undef length
#endif


#include <stdlib.h>

#include <stan/callbacks/writer.hpp>

/*
 Other classes:
   specify a stride so that we only collect the values from every k-th iteration.
   an R function that gets called every iteration.

 */

class Rwriter : public stan::callbacks::writer {

protected:
    SEXP  r_arrays;
    double **arrays;
    int iteration;
    int size;

public:
    Rwriter(){}
    ~Rwriter() {
        if(r_arrays)
            R_ReleaseObject(r_arrays);
        if(arrays)
            free(arrays);
    }

    SEXP getR() { 
        if(size > iteration) {
             // shrink the arrays to the number of iterations, if necessary
            int i, nvars = Rf_length(r_arrays);
            for(i = 0; i < nvars; i++) {
                SEXP tmp;
                tmp = VECTOR_ELT(r_arrays, i);
                SETLENGTH(tmp, iteration);
            }
        }

        return(r_arrays);
    }

    Rwriter(int numVars) {
        alloc(numVars);
    }
 
    virtual void operator()(const std::vector<std::string>& names) {
        int i;
        size_t len = names.size();
        SEXP r_names;
        if(!r_arrays)
            alloc(len);

        PROTECT(r_names = NEW_CHARACTER(len));
        for(i = 0; i < len; i++) {        
            SET_STRING_ELT(r_names, i, mkChar(names[i].c_str()));
        }
        SET_NAMES(r_arrays, r_names);
        UNPROTECT(1);
    }    


    virtual void operator()(const std::vector<double>& state) {
        size_t len = state.size();
        if(iteration == size)
            extend(2*size);

        int i;
        for(i = 0; i < len; i++) {
            arrays[i][iteration] = state[i];
        }
        iteration++;
    }

    protected:
    void alloc(int numVars, int numIter = 2000) {
        iteration = 0;

        R_PreserveObject(r_arrays = NEW_LIST(numVars));

        arrays = (double **) malloc(numVars * sizeof(double *));
        if(!arrays) {
            PROBLEM "can't allocate %d pointers", numVars
                ERROR;
        }

        int i;
        for(i = 0; i < numVars; i++) {
            SEXP tmp;
            SET_VECTOR_ELT(r_arrays, i, tmp = NEW_NUMERIC(numIter));
            arrays[i] = REAL(tmp);
        }
     }



    void extend(int newSize) {
        int i;
        int numVars = Rf_length(r_arrays);
        for(i = 0; i < numVars; i++) {
            SEXP tmp;
            tmp = VECTOR_ELT(r_arrays, i);
            // do we have to put this back into r_arrays (a new R object)
            // or have we changed the existing object directly.
            SETLENGTH(tmp, newSize);
            arrays[i] = REAL(tmp);
        }
    }
};

