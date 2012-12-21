FILE(REMOVE_RECURSE
  "./simple_kernel_generated_simple_kernel.cu.o"
  "simple_kernel.pdb"
  "simple_kernel"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/simple_kernel.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
