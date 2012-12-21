FILE(REMOVE_RECURSE
  "./enum_gpu_generated_enum_gpu.cu.o"
  "enum_gpu.pdb"
  "enum_gpu"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/enum_gpu.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
