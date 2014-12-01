#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_LBL_TREE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_LBL_TREE_H

#include <string>

namespace clang {
namespace diff {

enum TreeType {
  HIDE_NOTHING = 0,
  HIDE_ROOT_LABEL,
  RENAME_LABELS_TO_LEVEL,
  HIDE_ALL_LABELS,
  RANDOM_ROOT_LABEL,
  NO_NODE = -1,
  NO_TREE_ID = -1
};

class LblTree
{
public:
    LblTree(std::string* label, int treeID);
    LblTree();

    std::string* getLabel() const { return label; }
    void setLabel(std::string* label) {
      Label = label;
    }

    int getTreeID() const {
      if(isRoot()) {
        return treeID;
      } else {
        return (java_cast< LblTree* >(getRoot()))->getTreeID();
      }
    }
    void setTreeID(int treeID) {
    if(isRoot()) {
      this->treeID = treeID;
      } else {
        (java_cast< LblTree* >(getRoot()))->setTreeID(treeID);
      }
    }

    void* getTmpData() const { return TmpData; }
    void setTmpData(void* tmpData) {
      TmpData = tmpData;
    }

    void prettyPrint();
    void prettyPrint(bool printTmpData);
    int getNodeCount();

    static LblTree* fromString(const std::string& s);
    std::string toString() override;
    virtual void clearTmpData();

private:
    int TreeID;
    std::string* Label;
    void* TmpData;
    int NodeID;
};

} // namespace diff
} // namespace clang

#endif
