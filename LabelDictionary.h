#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_LABEL_DICTIONARY_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_LABEL_DICTIONARY_H

#include <map>
#include <string>

namespace clang {
namespace diff {

class LabelDictionary
{
public:
    LabelDictionary();

public:
    int store(const std::string& label);
    std::string read(int labelID);
    bool isNewLabelsAllowed() const { return NewLabelsAllowed; };
    void setNewLabelsAllowed(bool newLabelsAllowed);

    typedef std::map<std::string, int> StrIntMap;
    typedef std::map<int, std::string> IntStrMap;

private:
    int Count;
    StrIntMap StrInt;
    IntStrMap IntStr;
    bool NewLabelsAllowed;
};

} // namespace diff
} // namespace clang

#endif
