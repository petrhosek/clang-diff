#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <string>

using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory ClangDiffCategory("clang-diff options");

static cl::opt<std::string> LeftFilename(cl::Positional,
                                         cl::desc("<first file>"),
                                         cl::Required);
static cl::opt<std::string> RightFilename(cl::Positional,
                                          cl::desc("<second file>"),
                                          cl::Required);

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, ClangDiffCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  //DiffConsumer Consumer;
  //DifferenceEngine Engine(Consumer);

  //Engine.diff(LAST.get(), RAST.get());

  //Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
  //clang::tooling::runToolOnCode(new FindNamedClassAction, argv[1]);

  //return Consumer.hadDifferences();

  return 0;
}
