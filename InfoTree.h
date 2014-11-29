#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_INFO_TREE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_INFO_TREE_H

namespace clang {
namespace diff {

enum RTEDTree {
  LEFT,
  RIGHT,
  HEAVY,
  BOTH,
  REVLEFT,
  REVRIGHT,
  REVHEAVY,
};

enum Indeces {
  POST2_SIZE = 0,
  POST2_KR_SUM,
  POST2_REV_KR_SUM,
  POST2_DESC_SUM,
  POST2_PRE,
  POST2_PARENT,
  POST2_LABEL,
  KR,
  POST2_LLD,
  POST2_MIN_KR,
  RKR,
  RPOST2_RLD,
  RPOST2_MIN_RKR,
  RPOST2_POST,
  POST2_STRATEGY,
  PRE2_POST,
};

class LblTree;
class LabelDictionary;

class InfoTree {
public:
    InfoTree(LblTree* aInputTree, LabelDictionary* aLd);

public:
    virtual int getSize();
    virtual bool ifNodeOfType(int postorder, int type);
    virtual bool[] getNodeTypeArray_(int type);
    virtual int getInfo(int infoCode, int nodesPostorder);
    virtual int[] getInfoArray_(int infoCode);
    virtual int[] getNodeRelSubtrees(int pathType, int nodePostorder);
    virtual int[] getPath(int pathType);
    virtual int getCurrentNode();
    virtual void setCurrentNode(int postorder);
    static int[] toIntArray(std::vector<int>& integers);
    virtual void setSwitched(bool value);
    virtual bool isSwitched();

    int info[][];
    bool nodeType[][];

private:
    int gatherInfo(LblTree* aT, int postorder);
    void postTraversalProcessing();

    LblTree* inputTree;
    LabelDictionary* ld;
    int paths[][];
    int relSubtrees[][][];
    int sizeTmp;
    int descSizesTmp;
    int krSizesSumTmp;
    int revkrSizesSumTmp;
    int preorderTmp;
    int currentNode;
    bool switched;
    int leafCount;
    int treeSize;
};

} // namespace diff
} // namespace clang

#endif
