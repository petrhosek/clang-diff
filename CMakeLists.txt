set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangDiff
  InfoTree.cpp
  LabelDictionary.cpp
  LblTree.cpp
  RTED.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangTooling
  )

add_subdirectory(tool)
