FILE(REMOVE_RECURSE
  "./add_loop_gpu_generated_add_loop_gpu.cu.o"
  "add_loop_gpu.pdb"
  "add_loop_gpu"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/add_loop_gpu.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
