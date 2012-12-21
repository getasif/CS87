FILE(REMOVE_RECURSE
  "./timers_generated_timer.cu.o"
  "libtimers.pdb"
  "libtimers.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/timers.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
