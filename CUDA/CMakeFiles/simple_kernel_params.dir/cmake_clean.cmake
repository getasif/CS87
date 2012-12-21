FILE(REMOVE_RECURSE
  "./simple_kernel_params_generated_simple_kernel_params.cu.o"
  "simple_kernel_params.pdb"
  "simple_kernel_params"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/simple_kernel_params.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
