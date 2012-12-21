FILE(REMOVE_RECURSE
  "./book_generated_book.cu.o"
  "libbook.pdb"
  "libbook.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/book.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
