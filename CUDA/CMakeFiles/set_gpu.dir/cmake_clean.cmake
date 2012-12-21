FILE(REMOVE_RECURSE
  "./set_gpu_generated_set_gpu.cu.o"
  "set_gpu.pdb"
  "set_gpu"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/set_gpu.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
