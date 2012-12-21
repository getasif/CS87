FILE(REMOVE_RECURSE
  "./hello_world_generated_hello_world.cu.o"
  "hello_world.pdb"
  "hello_world"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/hello_world.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
