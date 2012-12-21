FILE(REMOVE_RECURSE
  "./maxval_generated_maxval.cu.o"
  "maxval.pdb"
  "maxval"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/maxval.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
