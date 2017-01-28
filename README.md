This is an experiment to use the stan refactored API in R.
We will mimic what cmdstan does to 
- parse the model, 
- read the data,
- compile the model
- sample from the C++ model code

We're focusing first on collecting the values from the sampling.
We implement a writer for collecting these values from the MCMC iterations. 
See src/rwriter.hpp.
This class extends the stan::callbacks::writer and collects the values
in numeric vectors, with one vector for each parameter in the model.
This is called first and once with the names of the parameters.
Then it is called at each iteration with a std::vector of the parameter
values, presumably in the same order as the names.
We insert these values into the relevant R numeric vectors.
When the sampling is complete, we retrieve the vectors as a list.


