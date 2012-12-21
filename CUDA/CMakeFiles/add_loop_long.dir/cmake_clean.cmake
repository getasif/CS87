FILE(REMOVE_RECURSE
  "./add_loop_long_generated_add_loop_long.cu.o"
  "add_loop_long.pdb"
  "add_loop_long"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/add_loop_long.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
