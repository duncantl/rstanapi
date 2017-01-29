
genCppCode =
function(model, modelName, outfile = character(), allowUndefined = FALSE)
{
   if(!is(model, "AsIs")) {
      if(!file.exists(model))
         stop("no such file for stan model: ", model)
      path.expand = model
   }

   if(length(outfile)) {
      outfile = path.expand(outfile)
      if(!file.exists(dirname(outfile)))
         stop("the directory ", dirname(outfile), " doesn't exist")
   }
   
   .Call("R_generateModelCode", as.character(model), as.character(modelName),
                                as.logical(allowUndefined), as.character(outfile), TRUE)
}
