FILE(REMOVE_RECURSE
  "./simple_device_call_generated_simple_device_call.cu.o"
  "simple_device_call.pdb"
  "simple_device_call"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/simple_device_call.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
