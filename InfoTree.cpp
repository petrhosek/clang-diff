#include "InfoTree.h"

using namespace llvm;

namespace clang {
namespace diff {
  
InfoTree::InfoTree(LblTree* aInputTree, LabelDictionary* aLd)
  : sizeTmp(0),
    descSizesTmp(0),
    krSizesSumTmp(0),
    revkrSizesSumTmp(0),
    preorderTmp(0),
    currentNode(-1),
    switched(false),
    leafCount(0),
    treeSize(0)
{
    this->inputTree = aInputTree;
    treeSize = inputTree->getNodeCount();
    this->info = new ::intArrayArray(16);
    ::java::util::Arrays::fill((*info)[POST2_PARENT], -1);
    ::java::util::Arrays::fill((*info)[POST2_MIN_KR], -1);
    ::java::util::Arrays::fill((*info)[RPOST2_MIN_RKR], -1);
    ::java::util::Arrays::fill((*info)[POST2_STRATEGY], -1);
    this->paths = new ::intArrayArray(3);
    ::java::util::Arrays::fill((*paths)[LEFT], -1);
    ::java::util::Arrays::fill((*paths)[RIGHT], -1);
    ::java::util::Arrays::fill((*paths)[HEAVY], -1);
    this->relSubtrees = new ::intArrayArrayArray(3);
    this->nodeType = new ::boolArrayArray(3);
    this->ld = aLd;
    this->currentNode = treeSize - 1;
    gatherInfo(inputTree, -1);
    postTraversalProcessing();
}

int InfoTree::getSize()
{
    return treeSize;
}

bool InfoTree::ifNodeOfType(int postorder, int type)
{
    return (*(*nodeType)[type])[postorder];
}

boolArray* InfoTree::getNodeTypeArray_(int type)
{
    return (*nodeType)[type];
}

int InfoTree::getInfo(int infoCode, int nodesPostorder)
{
    return (*(*info)[infoCode])[nodesPostorder];
}

intArray* InfoTree::getInfoArray_(int infoCode)
{
    return (*info)[infoCode];
}

intArray* InfoTree::getNodeRelSubtrees(int pathType, int nodePostorder)
{
    return (*(*relSubtrees)[pathType])[nodePostorder];
}

intArray* InfoTree::getPath(int pathType)
{
    return (*paths)[pathType];
}

int InfoTree::getCurrentNode()
{
    return currentNode;
}

void InfoTree::setCurrentNode(int postorder)
{
    currentNode = postorder;
}

int InfoTree::gatherInfo(::util::LblTree* aT, int postorder)
{
    auto currentSize = 0;
    auto childrenCount = 0;
    auto descSizes = 0;
    auto krSizesSum = 0;
    auto revkrSizesSum = 0;
    auto preorder = preorderTmp;
    auto heavyChild = -1;
    auto leftChild = -1;
    auto rightChild = -1;
    auto weight = -1;
    auto maxWeight = -1;
    auto currentPostorder = -1;
    auto oldHeavyChild = -1;
    auto heavyRelSubtreesTmp = new ::java::util::ArrayList();
    auto leftRelSubtreesTmp = new ::java::util::ArrayList();
    auto rightRelSubtreesTmp = new ::java::util::ArrayList();
    auto childrenPostorders = new ::java::util::ArrayList();
    preorderTmp++;
    for (auto *e = aT->children(); e->hasMoreElements(); ) {
        childrenCount++;
        postorder = gatherInfo(java_cast< ::util::LblTree* >(java_cast< ::java::lang::Object* >(e->nextElement())), postorder);
        childrenPostorders->add(::java::lang::Integer::valueOf(postorder));
        currentPostorder = postorder;
        weight = sizeTmp + 1;
        if(weight >= maxWeight) {
            maxWeight = weight;
            oldHeavyChild = heavyChild;
            heavyChild = currentPostorder;
        } else {
            heavyRelSubtreesTmp->add(::java::lang::Integer::valueOf(currentPostorder));
        }
        if(oldHeavyChild != -1) {
            heavyRelSubtreesTmp->add(::java::lang::Integer::valueOf(oldHeavyChild));
            oldHeavyChild = -1;
        }
        if(childrenCount == 1) {
            leftChild = currentPostorder;
        } else {
            leftRelSubtreesTmp->add(::java::lang::Integer::valueOf(currentPostorder));
        }
        rightChild = currentPostorder;
        if(e->hasMoreElements()) {
            rightRelSubtreesTmp->add(::java::lang::Integer::valueOf(currentPostorder));
        }
        currentSize += 1 + sizeTmp;
        descSizes += descSizesTmp;
        if(childrenCount > 1) {
            krSizesSum += krSizesSumTmp + sizeTmp + 1;
        } else {
            krSizesSum += krSizesSumTmp;
            (*(*nodeType)[LEFT])[currentPostorder] = true;
        }
        if(e->hasMoreElements()) {
            revkrSizesSum += revkrSizesSumTmp + sizeTmp + 1;
        } else {
            revkrSizesSum += revkrSizesSumTmp;
            (*(*nodeType)[RIGHT])[currentPostorder] = true;
        }
    }
    postorder++;
    aT->setTmpData(::java::lang::Integer::valueOf(postorder));
    auto currentDescSizes = descSizes + currentSize + 1;
    (*(*info)[POST2_DESC_SUM])[postorder] = (currentSize + 1) * (currentSize + 1 + 3) / 2 - currentDescSizes;
    (*(*info)[POST2_KR_SUM])[postorder] = krSizesSum + currentSize + 1;
    (*(*info)[POST2_REV_KR_SUM])[postorder] = revkrSizesSum + currentSize + 1;
    (*(*info)[POST2_LABEL])[postorder] = ld->store(aT->getLabel());
    for (auto _i = childrenPostorders->iterator(); _i->hasNext(); ) {
        ::java::lang::Integer* i = java_cast< ::java::lang::Integer* >(_i->next());
        {
            (*(*info)[POST2_PARENT])[(i)->intValue()] = postorder;
        }
    }
    (*(*info)[POST2_SIZE])[postorder] = currentSize + 1;
    if(currentSize == 0) {
        leafCount++;
    }
    (*(*info)[POST2_PRE])[postorder] = preorder;
    (*(*info)[PRE2_POST])[preorder] = postorder;
    (*(*info)[RPOST2_POST])[treeSize - 1 - preorder] = postorder;
    if(heavyChild != -1) {
        (*(*paths)[HEAVY])[postorder] = heavyChild;
        (*(*nodeType)[HEAVY])[heavyChild] = true;
        if(leftChild < heavyChild && heavyChild < rightChild) {
            (*(*info)[POST2_STRATEGY])[postorder] = BOTH;
        } else if(heavyChild == leftChild) {
            (*(*info)[POST2_STRATEGY])[postorder] = RIGHT;
        } else if(heavyChild == rightChild) {
            (*(*info)[POST2_STRATEGY])[postorder] = LEFT;
        }
    } else {
        (*(*info)[POST2_STRATEGY])[postorder] = RIGHT;
    }
    if(leftChild != -1) {
        (*(*paths)[LEFT])[postorder] = leftChild;
    }
    if(rightChild != -1) {
        (*(*paths)[RIGHT])[postorder] = rightChild;
    }
    (*relSubtrees)[HEAVY]->set(postorder, toIntArray_(heavyRelSubtreesTmp));
    (*relSubtrees)[RIGHT]->set(postorder, toIntArray_(rightRelSubtreesTmp));
    (*relSubtrees)[LEFT]->set(postorder, toIntArray_(leftRelSubtreesTmp));
    descSizesTmp = currentDescSizes;
    sizeTmp = currentSize;
    krSizesSumTmp = krSizesSum;
    revkrSizesSumTmp = revkrSizesSum;
    return postorder;
}

void InfoTree::postTraversalProcessing()
{
    auto nc1 = treeSize;
    info->set(KR, new ::intArray(leafCount));
    info->set(RKR, new ::intArray(leafCount));
    auto nc = nc1;
    auto lc = leafCount;
    auto i = 0;
    for (i = 0; i < treeSize; i++) {
        if((*(*paths)[LEFT])[i] == -1) {
            (*(*info)[POST2_LLD])[i] = i;
        } else {
            (*(*info)[POST2_LLD])[i] = (*(*info)[POST2_LLD])[(*(*paths)[LEFT])[i]];
        }
        if((*(*paths)[RIGHT])[i] == -1) {
            (*(*info)[RPOST2_RLD])[treeSize - 1 - (*(*info)[POST2_PRE])[i]] = (treeSize - 1 - (*(*info)[POST2_PRE])[i]);
        } else {
            (*(*info)[RPOST2_RLD])[treeSize - 1 - (*(*info)[POST2_PRE])[i]] = (*(*info)[RPOST2_RLD])[treeSize - 1 - (*(*info)[POST2_PRE])[(*(*paths)[RIGHT])[i]]];
        }
    }
    auto visited = new ::boolArray(nc);
    auto visitedR = new ::boolArray(nc);
    ::java::util::Arrays::fill(visited, false);
    auto k = lc - 1;
    auto kR = lc - 1;
    for (i = nc - 1; i >= 0; i--) {
        if(!(*visited)[(*(*info)[POST2_LLD])[i]]) {
            (*(*info)[KR])[k] = i;
            (*visited)[(*(*info)[POST2_LLD])[i]] = true;
            k--;
        }
        if(!(*visitedR)[(*(*info)[RPOST2_RLD])[i]]) {
            (*(*info)[RKR])[kR] = i;
            (*visitedR)[(*(*info)[RPOST2_RLD])[i]] = true;
            kR--;
        }
    }
    auto parent = -1;
    auto parentR = -1;
    for (i = 0; i < leafCount; i++) {
        parent = (*(*info)[KR])[i];
        while (parent > -1 && (*(*info)[POST2_MIN_KR])[parent] == -1) {
            (*(*info)[POST2_MIN_KR])[parent] = i;
            parent = (*(*info)[POST2_PARENT])[parent];
        }
        parentR = (*(*info)[RKR])[i];
        while (parentR > -1 && (*(*info)[RPOST2_MIN_RKR])[parentR] == -1) {
            (*(*info)[RPOST2_MIN_RKR])[parentR] = i;
            parentR = (*(*info)[POST2_PARENT])[(*(*info)[RPOST2_POST])[parentR]];
            if(parentR > -1) {
                parentR = treeSize - 1 - (*(*info)[POST2_PRE])[parentR];
            }
        }
    }
}

intArray* InfoTree::toIntArray_(::java::util::List* integers)
{
    clinit();
    auto ints = new ::intArray(integers->size());
    auto i = 0;
    for (auto _i = integers->iterator(); _i->hasNext(); ) {
        ::java::lang::Integer* n = java_cast< ::java::lang::Integer* >(_i->next());
        {
            (*ints)[i++] = (n)->intValue();
        }
    }
    return ints;
}

void InfoTree::setSwitched(bool value)
{
    switched = value;
}

bool InfoTree::isSwitched()
{
    return switched;
}

} // namespace query
} // namespace clang
