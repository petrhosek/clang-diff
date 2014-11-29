#include "RTED.h"

using namespace llvm;

namespace clang {
namespace diff {

namespace {

enum RTEDTree {
  LEFT,
  RIGHT,
  HEAVY,
  BOTH,
  REVLEFT,
  REVRIGHT,
  REVHEAVY,
};

enum RTEDTree2 {
  POST2_SIZE,
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
}

} // namespace

RTED::RTED(double delCost, double insCost, double matchCost)
  : delCost(delCost), insCost(insCost), matchCost(matchCost), counter(0)
{
  strStat = new int[5];
}

double RTED::nonNormalizedTreeDist(::util::LblTree* t1, ::util::LblTree* t2)
{
    init_(t1, t2);
    STR = new ::intArrayArray(size1);
    computeOptimalStrategy();
    return computeDistUsingStrArray_(it1, it2);
}

double RTED::nonNormalizedTreeDist()
{
    if(it1 == nullptr || it2 == nullptr) {
        ::java::lang::System::err()->println(u"No stored trees to compare."_j);
    }
    if(STR == nullptr) {
        ::java::lang::System::err()->println(u"No strategy to use."_j);
    }
    return computeDistUsingStrArray_(it1, it2);
}

void RTED::init(::util::LblTree* t1, ::util::LblTree* t2)
{
    ld = new ::util::LabelDictionary();
    it1 = new InfoTree(t1, ld);
    it2 = new InfoTree(t2, ld);
    size1 = it1->getSize();
    size2 = it2->getSize();
    IJ = new ::intArrayArray(::java::lang::Math::max(size1, size2));
    delta = new ::doubleArrayArray(size1);
    deltaBit = new ::int8_tArrayArray(size1);
    costV = new ::int64_tArrayArrayArray(3);
    costW = new ::int64_tArrayArray(3);
    auto labels1 = it1->getInfoArray_(POST2_LABEL);
    auto labels2 = it2->getInfoArray_(POST2_LABEL);
    auto sizes1 = it1->getInfoArray_(POST2_SIZE);
    auto sizes2 = it2->getInfoArray_(POST2_SIZE);
    for (auto x = 0; x < sizes1->length; x++) {
        for (auto y = 0; y < sizes2->length; y++) {
            if((*labels1)[x] == (*labels2)[y]) {
                (*(*deltaBit)[x])[y] = 0;
            } else {
                (*(*deltaBit)[x])[y] = 1;
            }
            if((*sizes1)[x] == 1 && (*sizes2)[y] == 1) {
                (*(*delta)[x])[y] = 0;
            } else {
                if((*sizes1)[x] == 1) {
                    (*(*delta)[x])[y] = (*sizes2)[y] - 1;
                }
                if((*sizes2)[y] == 1) {
                    (*(*delta)[x])[y] = (*sizes1)[x] - 1;
                }
            }
        }
    }
}

void RTED::computeOptimalStrategy()
{
    int64_t heavyMin, revHeavyMin, leftMin, revLeftMin, rightMin, revRightMin;
    int64_t min = -1;
    auto strategy = -1;
    auto parent1 = -1;
    auto parent2 = -1;
    auto nodeTypeLeft1 = (*it1->nodeType)[LEFT];
    auto nodeTypeLeft2 = (*it2->nodeType)[LEFT];
    auto nodeTypeRigt1 = (*it1->nodeType)[RIGHT];
    auto nodeTypeRight2 = (*it2->nodeType)[RIGHT];
    auto nodeTypeHeavy1 = (*it1->nodeType)[HEAVY];
    auto nodeTypeHeavy2 = (*it2->nodeType)[HEAVY];
    auto post2size1 = (*it1->info)[POST2_SIZE];
    auto post2size2 = (*it2->info)[POST2_SIZE];
    auto post2descSum1 = (*it1->info)[POST2_DESC_SUM];
    auto post2descSum2 = (*it2->info)[POST2_DESC_SUM];
    auto post2krSum1 = (*it1->info)[POST2_KR_SUM];
    auto post2krSum2 = (*it2->info)[POST2_KR_SUM];
    auto post2revkrSum1 = (*it1->info)[POST2_REV_KR_SUM];
    auto post2revkrSum2 = (*it2->info)[POST2_REV_KR_SUM];
    auto post2parent1 = (*it1->info)[POST2_PARENT];
    auto post2parent2 = (*it2->info)[POST2_PARENT];
    STR = new ::intArrayArray(size1);
    for (auto v = 0; v < size1; v++) {
        ::java::util::Arrays::fill((*costW)[0], static_cast< int64_t >(0));
        ::java::util::Arrays::fill((*costW)[1], static_cast< int64_t >(0));
        ::java::util::Arrays::fill((*costW)[2], static_cast< int64_t >(0));
        for (auto w = 0; w < size2; w++) {
            if((*post2size2)[w] == 1) {
                (*(*costW)[LEFT])[w] = 0;
                (*(*costW)[RIGHT])[w] = 0;
                (*(*costW)[HEAVY])[w] = 0;
            }
            if((*post2size1)[v] == 1) {
                (*(*(*costV)[LEFT])[v])[w] = 0;
                (*(*(*costV)[RIGHT])[v])[w] = 0;
                (*(*(*costV)[HEAVY])[v])[w] = 0;
            }
            heavyMin = static_cast< int64_t >((*post2size1)[v]) * static_cast< int64_t >((*post2descSum2)[w]) + (*(*(*costV)[HEAVY])[v])[w];
            revHeavyMin = static_cast< int64_t >((*post2size2)[w]) * static_cast< int64_t >((*post2descSum1)[v]) + (*(*costW)[HEAVY])[w];
            leftMin = static_cast< int64_t >((*post2size1)[v]) * static_cast< int64_t >((*post2krSum2)[w]) + (*(*(*costV)[LEFT])[v])[w];
            revLeftMin = static_cast< int64_t >((*post2size2)[w]) * static_cast< int64_t >((*post2krSum1)[v]) + (*(*costW)[LEFT])[w];
            rightMin = static_cast< int64_t >((*post2size1)[v]) * static_cast< int64_t >((*post2revkrSum2)[w]) + (*(*(*costV)[RIGHT])[v])[w];
            revRightMin = static_cast< int64_t >((*post2size2)[w]) * static_cast< int64_t >((*post2revkrSum1)[v]) + (*(*costW)[RIGHT])[w];
            auto mins = (new ::int64_tArray({
                leftMin
                , rightMin
                , heavyMin
                , ::java::lang::Long::MAX_VALUE
                , revLeftMin
                , revRightMin
                , revHeavyMin
            }));
            min = leftMin;
            strategy = 0;
            for (auto i = 1; i <= 6; i++) {
                if((*mins)[i] < min) {
                    min = (*mins)[i];
                    strategy = i;
                }
            }
            (*(*STR)[v])[w] = strategy;
            parent1 = (*post2parent1)[v];
            if(parent1 != -1) {
                (*(*(*costV)[HEAVY])[parent1])[w] += (*nodeTypeHeavy1)[v] ? (*(*(*costV)[HEAVY])[v])[w] : min;
                (*(*(*costV)[RIGHT])[parent1])[w] += (*nodeTypeRigt1)[v] ? (*(*(*costV)[RIGHT])[v])[w] : min;
                (*(*(*costV)[LEFT])[parent1])[w] += (*nodeTypeLeft1)[v] ? (*(*(*costV)[LEFT])[v])[w] : min;
            }
            parent2 = (*post2parent2)[w];
            if(parent2 != -1) {
                (*(*costW)[HEAVY])[parent2] += (*nodeTypeHeavy2)[w] ? (*(*costW)[HEAVY])[w] : min;
                (*(*costW)[LEFT])[parent2] += (*nodeTypeLeft2)[w] ? (*(*costW)[LEFT])[w] : min;
                (*(*costW)[RIGHT])[parent2] += (*nodeTypeRight2)[w] ? (*(*costW)[RIGHT])[w] : min;
            }
        }
    }
}

double RTED::computeDistUsingStrArray_(InfoTree* it1, InfoTree* it2)
{
    auto postorder1 = it1->getCurrentNode();
    auto postorder2 = it2->getCurrentNode();
    auto stepStrategy = (*(*STR)[postorder1])[postorder2];
    int tmpPostorder;
    ::intArray* stepPath;
    ::intArray* stepRelSubtrees;
    ::java::util::ArrayList* heavyPath;
    switch (stepStrategy) {
    case LEFT:
        tmpPostorder = postorder1;
        stepPath = it1->getPath(LEFT);
        while ((*stepPath)[postorder1] > -1) {
            stepRelSubtrees = it1->getNodeRelSubtrees(LEFT, postorder1);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it1->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder1 = (*stepPath)[postorder1];
        }
        it1->setCurrentNode(tmpPostorder);
        it1->setSwitched(false);
        it2->setSwitched(false);
        (*strStat)[3]++;
        (*strStat)[LEFT]++;
        return spfL(it1, it2);
    case RIGHT:
        tmpPostorder = postorder1;
        stepPath = it1->getPath(RIGHT);
        while ((*stepPath)[postorder1] > -1) {
            stepRelSubtrees = it1->getNodeRelSubtrees(RIGHT, postorder1);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it1->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder1 = (*stepPath)[postorder1];
        }
        it1->setCurrentNode(tmpPostorder);
        it1->setSwitched(false);
        it2->setSwitched(false);
        (*strStat)[3]++;
        (*strStat)[RIGHT]++;
        return spfR(it1, it2);
    case HEAVY:
        tmpPostorder = postorder1;
        stepPath = it1->getPath(HEAVY);
        heavyPath = new ::java::util::ArrayList();
        heavyPath->add(::java::lang::Integer::valueOf(postorder1));
        while ((*stepPath)[postorder1] > -1) {
            stepRelSubtrees = it1->getNodeRelSubtrees(HEAVY, postorder1);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it1->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder1 = (*stepPath)[postorder1];
            heavyPath->add(::java::lang::Integer::valueOf(postorder1));
        }
        it1->setCurrentNode(tmpPostorder);
        it1->setSwitched(false);
        it2->setSwitched(false);
        (*strStat)[3]++;
        (*strStat)[HEAVY]++;
        return spfH(it1, it2, InfoTree::toIntArray_(heavyPath));
    case REVLEFT:
        tmpPostorder = postorder2;
        stepPath = it2->getPath(LEFT);
        while ((*stepPath)[postorder2] > -1) {
            stepRelSubtrees = it2->getNodeRelSubtrees(LEFT, postorder2);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it2->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder2 = (*stepPath)[postorder2];
        }
        it2->setCurrentNode(tmpPostorder);
        it1->setSwitched(true);
        it2->setSwitched(true);
        (*strStat)[3]++;
        (*strStat)[LEFT]++;
        return spfL(it2, it1);
    case REVRIGHT:
        tmpPostorder = postorder2;
        stepPath = it2->getPath(RIGHT);
        while ((*stepPath)[postorder2] > -1) {
            stepRelSubtrees = it2->getNodeRelSubtrees(RIGHT, postorder2);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it2->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder2 = (*stepPath)[postorder2];
        }
        it2->setCurrentNode(tmpPostorder);
        it1->setSwitched(true);
        it2->setSwitched(true);
        (*strStat)[3]++;
        (*strStat)[RIGHT]++;
        return spfR(it2, it1);
    case REVHEAVY:
        tmpPostorder = postorder2;
        stepPath = it2->getPath(HEAVY);
        heavyPath = new ::java::util::ArrayList();
        heavyPath->add(::java::lang::Integer::valueOf(postorder2));
        while ((*stepPath)[postorder2] > -1) {
            stepRelSubtrees = it2->getNodeRelSubtrees(HEAVY, postorder2);
            if(stepRelSubtrees != nullptr) {
                for(auto rs : *stepRelSubtrees) {
                    it2->setCurrentNode(rs);
                    computeDistUsingStrArray_(it1, it2);
                }
            }
            postorder2 = (*stepPath)[postorder2];
            heavyPath->add(::java::lang::Integer::valueOf(postorder2));
        }
        it2->setCurrentNode(tmpPostorder);
        it1->setSwitched(true);
        it2->setSwitched(true);
        (*strStat)[3]++;
        (*strStat)[HEAVY]++;
        return spfH(it2, it1, InfoTree::toIntArray_(heavyPath));
    default:
        return -1;
    }

}

double RTED::spfL(InfoTree* it1, InfoTree* it2)
{
    auto fPostorder = it1->getCurrentNode();
    auto gPostorder = it2->getCurrentNode();
    auto minKR = (*(*it2->info)[POST2_MIN_KR])[gPostorder];
    auto kr = (*it2->info)[KR];
    if(minKR > -1) {
        for (auto j = minKR; (*kr)[j] < gPostorder; j++) {
            treeEditDist(it1, it2, fPostorder, (*kr)[j]);
        }
    }
    treeEditDist(it1, it2, fPostorder, gPostorder);
    return it1->isSwitched() ? (*(*delta)[gPostorder])[fPostorder] + (*(*deltaBit)[gPostorder])[fPostorder] * costMatch : (*(*delta)[fPostorder])[gPostorder] + (*(*deltaBit)[fPostorder])[gPostorder] * costMatch;
}

void RTED::treeEditDist(InfoTree* it1, InfoTree* it2, int i, int j)
{
    auto m = i - (*(*it1->info)[POST2_LLD])[i] + 2;
    auto n = j - (*(*it2->info)[POST2_LLD])[j] + 2;
    auto forestdist = new ::doubleArrayArray(m);
    auto ioff = (*(*it1->info)[POST2_LLD])[i] - 1;
    auto joff = (*(*it2->info)[POST2_LLD])[j] - 1;
    auto switched = it1->isSwitched();
    (*(*forestdist)[0])[0] = 0;
    for (auto i1 = 1; i1 <= i - ioff; i1++) {
        (*(*forestdist)[i1])[0] = (*(*forestdist)[i1 - 1])[0] + 1;
    }
    for (auto j1 = 1; j1 <= j - joff; j1++) {
        (*(*forestdist)[0])[j1] = (*(*forestdist)[0])[j1 - 1] + 1;
    }
    for (auto i1 = 1; i1 <= i - ioff; i1++) {
        for (auto j1 = 1; j1 <= j - joff; j1++) {
            counter++;
            if(((*(*it1->info)[POST2_LLD])[i1 + ioff] == (*(*it1->info)[POST2_LLD])[i]) && ((*(*it2->info)[POST2_LLD])[j1 + joff] == (*(*it2->info)[POST2_LLD])[j])) {
                double u = 0;
                if((*(*it1->info)[POST2_LABEL])[i1 + ioff] != (*(*it2->info)[POST2_LABEL])[j1 + joff]) {
                    u = costMatch;
                }
                da = (*(*forestdist)[i1 - 1])[j1] + costDel;
                db = (*(*forestdist)[i1])[j1 - 1] + costIns;
                dc = (*(*forestdist)[i1 - 1])[j1 - 1] + u;
                (*(*forestdist)[i1])[j1] = (da < db) ? ((da < dc) ? da : dc) : ((db < dc) ? db : dc);
                setDeltaValue(i1 + ioff, j1 + joff, (*(*forestdist)[i1 - 1])[j1 - 1], switched);
                setDeltaBitValue(i1 + ioff, j1 + joff, static_cast< int8_t >((((*(*forestdist)[i1])[j1] - (*(*forestdist)[i1 - 1])[j1 - 1] > 0) ? 1 : 0)), switched);
            } else {
                double u = 0;
                u = switched ? (*(*deltaBit)[j1 + joff])[i1 + ioff] * costMatch : (*(*deltaBit)[i1 + ioff])[j1 + joff] * costMatch;
                da = (*(*forestdist)[i1 - 1])[j1] + costDel;
                db = (*(*forestdist)[i1])[j1 - 1] + costIns;
                dc = (*(*forestdist)[(*(*it1->info)[POST2_LLD])[i1 + ioff] - 1 - ioff])[(*(*it2->info)[POST2_LLD])[j1 + joff] - 1 - joff] + (switched ? (*(*delta)[j1 + joff])[i1 + ioff] : (*(*delta)[i1 + ioff])[j1 + joff]) + u;
                (*(*forestdist)[i1])[j1] = (da < db) ? ((da < dc) ? da : dc) : ((db < dc) ? db : dc);
            }
        }
    }
}

double RTED::spfR(InfoTree* it1, InfoTree* it2)
{
    auto fReversedPostorder = it1->getSize() - 1 - (*(*it1->info)[POST2_PRE])[it1->getCurrentNode()];
    auto gReversedPostorder = it2->getSize() - 1 - (*(*it2->info)[POST2_PRE])[it2->getCurrentNode()];
    auto minRKR = (*(*it2->info)[RPOST2_MIN_RKR])[gReversedPostorder];
    auto rkr = (*it2->info)[RKR];
    if(minRKR > -1) {
        for (auto j = minRKR; (*rkr)[j] < gReversedPostorder; j++) {
            treeEditDistRev(it1, it2, fReversedPostorder, (*rkr)[j]);
        }
    }
    treeEditDistRev(it1, it2, fReversedPostorder, gReversedPostorder);
    return it1->isSwitched() ? (*(*delta)[it2->getCurrentNode()])[it1->getCurrentNode()] + (*(*deltaBit)[it2->getCurrentNode()])[it1->getCurrentNode()] * costMatch : (*(*delta)[it1->getCurrentNode()])[it2->getCurrentNode()] + (*(*deltaBit)[it1->getCurrentNode()])[it2->getCurrentNode()] * costMatch;
}

void RTED::treeEditDistRev(InfoTree* it1, InfoTree* it2, int i, int j)
{
    auto m = i - (*(*it1->info)[RPOST2_RLD])[i] + 2;
    auto n = j - (*(*it2->info)[RPOST2_RLD])[j] + 2;
    auto forestdist = new ::doubleArrayArray(m);
    auto ioff = (*(*it1->info)[RPOST2_RLD])[i] - 1;
    auto joff = (*(*it2->info)[RPOST2_RLD])[j] - 1;
    auto switched = it1->isSwitched();
    (*(*forestdist)[0])[0] = 0;
    for (auto i1 = 1; i1 <= i - ioff; i1++) {
        (*(*forestdist)[i1])[0] = (*(*forestdist)[i1 - 1])[0] + 1;
    }
    for (auto j1 = 1; j1 <= j - joff; j1++) {
        (*(*forestdist)[0])[j1] = (*(*forestdist)[0])[j1 - 1] + 1;
    }
    for (auto i1 = 1; i1 <= i - ioff; i1++) {
        for (auto j1 = 1; j1 <= j - joff; j1++) {
            counter++;
            if(((*(*it1->info)[RPOST2_RLD])[i1 + ioff] == (*(*it1->info)[RPOST2_RLD])[i]) && ((*(*it2->info)[RPOST2_RLD])[j1 + joff] == (*(*it2->info)[RPOST2_RLD])[j])) {
                double u = 0;
                if((*(*it1->info)[POST2_LABEL])[(*(*it1->info)[RPOST2_POST])[i1 + ioff]] != (*(*it2->info)[POST2_LABEL])[(*(*it2->info)[RPOST2_POST])[j1 + joff]]) {
                    u = costMatch;
                }
                da = (*(*forestdist)[i1 - 1])[j1] + costDel;
                db = (*(*forestdist)[i1])[j1 - 1] + costIns;
                dc = (*(*forestdist)[i1 - 1])[j1 - 1] + u;
                (*(*forestdist)[i1])[j1] = (da < db) ? ((da < dc) ? da : dc) : ((db < dc) ? db : dc);
                setDeltaValue((*(*it1->info)[RPOST2_POST])[i1 + ioff], (*(*it2->info)[RPOST2_POST])[j1 + joff], (*(*forestdist)[i1 - 1])[j1 - 1], switched);
                setDeltaBitValue((*(*it1->info)[RPOST2_POST])[i1 + ioff], (*(*it2->info)[RPOST2_POST])[j1 + joff], static_cast< int8_t >((((*(*forestdist)[i1])[j1] - (*(*forestdist)[i1 - 1])[j1 - 1] > 0) ? 1 : 0)), switched);
            } else {
                double u = 0;
                u = switched ? (*(*deltaBit)[(*(*it2->info)[RPOST2_POST])[j1 + joff]])[(*(*it1->info)[RPOST2_POST])[i1 + ioff]] * costMatch : (*(*deltaBit)[(*(*it1->info)[RPOST2_POST])[i1 + ioff]])[(*(*it2->info)[RPOST2_POST])[j1 + joff]] * costMatch;
                da = (*(*forestdist)[i1 - 1])[j1] + costDel;
                db = (*(*forestdist)[i1])[j1 - 1] + costIns;
                dc = (*(*forestdist)[(*(*it1->info)[RPOST2_RLD])[i1 + ioff] - 1 - ioff])[(*(*it2->info)[RPOST2_RLD])[j1 + joff] - 1 - joff] + (switched ? (*(*delta)[(*(*it2->info)[RPOST2_POST])[j1 + joff]])[(*(*it1->info)[RPOST2_POST])[i1 + ioff]] : (*(*delta)[(*(*it1->info)[RPOST2_POST])[i1 + ioff]])[(*(*it2->info)[RPOST2_POST])[j1 + joff]]) + u;
                (*(*forestdist)[i1])[j1] = (da < db) ? ((da < dc) ? da : dc) : ((db < dc) ? db : dc);
            }
        }
    }
}

double RTED::spfH(InfoTree* it1, InfoTree* it2, ::intArray* heavyPath)
{
    auto fSize = (*(*it1->info)[POST2_SIZE])[it1->getCurrentNode()];
    auto gSize = (*(*it2->info)[POST2_SIZE])[it2->getCurrentNode()];
    auto gRevPre = it2->getSize() - 1 - it2->getCurrentNode();
    auto gPre = (*(*it2->info)[POST2_PRE])[it2->getCurrentNode()];
    auto gTreeSize = it2->getSize();
    int strategy;
    int jOfi;
    t = new ::doubleArrayArray(gSize);
    tCOPY = new ::doubleArrayArray(gSize);
    s = new ::doubleArrayArray(fSize);
    q = new ::doubleArray(fSize);
    auto vp = -1;
    auto nextVp = -1;
    for (auto it = heavyPath->length - 1; it >= 0; it--) {
        vp = (*heavyPath)[it];
        strategy = (*(*it1->info)[POST2_STRATEGY])[vp];
        if(strategy != BOTH) {
            if((*(*it1->info)[POST2_SIZE])[vp] == 1) {
                for (auto i = gSize - 1; i >= 0; i--) {
                    jOfi = jOfI(it2, i, gSize, gRevPre, gPre, strategy, gTreeSize);
                    for (auto j = jOfi; j >= 0; j--) {
                        (*(*t)[i])[j] = (gSize - (i + j)) * costIns;
                    }
                }
                previousStrategy = strategy;
            }
            computePeriod(it1, vp, nextVp, it2, strategy);
        } else {
            if((*(*it1->info)[POST2_SIZE])[vp] == 1) {
                for (auto i = gSize - 1; i >= 0; i--) {
                    jOfi = jOfI(it2, i, gSize, gRevPre, gPre, LEFT, gTreeSize);
                    for (auto j = jOfi; j >= 0; j--) {
                        (*(*t)[i])[j] = (gSize - (i + j)) * costIns;
                    }
                }
                previousStrategy = LEFT;
            }
            computePeriod(it1, vp, nextVp, it2, LEFT);
            if((*(*it1->info)[POST2_SIZE])[vp] == 1) {
                for (auto i = gSize - 1; i >= 0; i--) {
                    jOfi = jOfI(it2, i, gSize, gRevPre, gPre, RIGHT, gTreeSize);
                    for (auto j = jOfi; j >= 0; j--) {
                        (*(*t)[i])[j] = (gSize - (i + j)) * costIns;
                    }
                }
                previousStrategy = RIGHT;
            }
            computePeriod(it1, vp, nextVp, it2, RIGHT);
        }
        nextVp = vp;
    }
    return (*(*t)[0])[0];
}

void RTED::computePeriod(InfoTree* it1, int aVp, int aNextVp, InfoTree* it2, int aStrategy)
{
    auto fTreeSize = it1->getSize();
    auto gTreeSize = it2->getSize();
    auto vpPreorder = (*(*it1->info)[POST2_PRE])[aVp];
    auto vpRevPreorder = fTreeSize - 1 - aVp;
    auto vpSize = (*(*it1->info)[POST2_SIZE])[aVp];
    auto gSize = (*(*it2->info)[POST2_SIZE])[it2->getCurrentNode()];
    auto gPreorder = (*(*it2->info)[POST2_PRE])[it2->getCurrentNode()];
    auto gRevPreorder = gTreeSize - 1 - it2->getCurrentNode();
    auto nextVpPreorder = -1;
    auto nextVpRevPreorder = -1;
    auto nextVpSize = -1;
    int k;
    if(aNextVp != -1) {
        nextVpPreorder = (*(*it1->info)[POST2_PRE])[aNextVp];
        nextVpRevPreorder = fTreeSize - 1 - aNextVp;
        nextVpSize = (*(*it1->info)[POST2_SIZE])[aNextVp];
        k = aStrategy == LEFT ? nextVpPreorder - vpPreorder : nextVpRevPreorder - vpRevPreorder;
        if(aStrategy != previousStrategy) {
            computeIJTable(it2, gPreorder, gRevPreorder, gSize, aStrategy, gTreeSize);
        }
    } else {
        k = 1;
        computeIJTable(it2, gPreorder, gRevPreorder, gSize, aStrategy, gTreeSize);
    }
    auto realStrategy = (*(*it1->info)[POST2_STRATEGY])[aVp];
    auto switched = it1->isSwitched();
    tTMP = tCOPY;
    tCOPY = t;
    t = tTMP;
    if(vpSize - nextVpSize == 1) {
        if(gSize == 1) {
            setDeltaValue((*(*it1->info)[PRE2_POST])[vpPreorder], (*(*it2->info)[PRE2_POST])[gPreorder], vpSize - 1, switched);
        } else {
            setDeltaValue((*(*it1->info)[PRE2_POST])[vpPreorder], (*(*it2->info)[PRE2_POST])[gPreorder], (*(*t)[1])[0], switched);
        }
    }
    int gijForestPreorder;
    int previousI;
    int fForestPreorderKPrime;
    int jPrime;
    int kBis;
    int jOfIminus1;
    int gijOfIMinus1Preorder;
    int jOfI;
    double deleteFromLeft;
    double deleteFromRight;
    double match;
    int fLabel;
    int gLabel;
    for (auto i = gSize - 1; i >= 0; i--) {
        jOfI = this->jOfI(it2, i, gSize, gRevPreorder, gPreorder, aStrategy, gTreeSize);
        counter += realStrategy == BOTH && aStrategy == LEFT ? (k - 1) * (jOfI + 1) : k * (jOfI + 1);
        for (auto kPrime = 1; kPrime <= k; kPrime++) {
            fForestPreorderKPrime = aStrategy == LEFT ? vpPreorder + (k - kPrime) : (*(*it1->info)[POST2_PRE])[fTreeSize - 1 - (vpRevPreorder + (k - kPrime))];
            kBis = kPrime - (*(*it1->info)[POST2_SIZE])[(*(*it1->info)[PRE2_POST])[fForestPreorderKPrime]];
            deleteFromRight = costIns;
            deleteFromLeft = costDel;
            match = 0;
            match += aStrategy == LEFT ? kBis + nextVpSize : vpSize - k + kBis;
            if((i + jOfI) == (gSize - 1)) {
                deleteFromRight += (vpSize - (k - kPrime));
            } else {
                deleteFromRight += (*q)[kPrime - 1];
            }
            fLabel = (*(*it1->info)[POST2_LABEL])[(*(*it1->info)[PRE2_POST])[fForestPreorderKPrime]];
            for (auto j = jOfI; j >= 0; j--) {
                gijForestPreorder = aStrategy == LEFT ? (*(*IJ)[i])[j] : (*(*it2->info)[POST2_PRE])[gTreeSize - 1 - (*(*IJ)[i])[j]];
                if(kPrime == 1) {
                    if(aStrategy != previousStrategy) {
                        if(aStrategy == LEFT) {
                            previousI = gijForestPreorder - gPreorder;
                        } else {
                            previousI = gTreeSize - 1 - (*(*it2->info)[RPOST2_POST])[gTreeSize - 1 - gijForestPreorder]- gRevPreorder;
                        }
                        deleteFromLeft += (*(*tCOPY)[previousI])[i + j - previousI];
                    } else {
                        deleteFromLeft += (*(*tCOPY)[i])[j];
                    }
                } else {
                    deleteFromLeft += (*(*s)[kPrime - 1 - 1])[j];
                }
                match += switched ? (*(*delta)[(*(*it2->info)[PRE2_POST])[gijForestPreorder]])[(*(*it1->info)[PRE2_POST])[fForestPreorderKPrime]] : (*(*delta)[(*(*it1->info)[PRE2_POST])[fForestPreorderKPrime]])[(*(*it2->info)[PRE2_POST])[gijForestPreorder]];
                jPrime = j + (*(*it2->info)[POST2_SIZE])[(*(*it2->info)[PRE2_POST])[gijForestPreorder]];
                gLabel = (*(*it2->info)[POST2_LABEL])[(*(*it2->info)[PRE2_POST])[gijForestPreorder]];
                if(fLabel != gLabel) {
                    match += costMatch;
                }
                if(j != jOfI) {
                    deleteFromRight += (*(*s)[kPrime - 1])[j + 1];
                    if(kBis == 0) {
                        if(aStrategy != previousStrategy) {
                            previousI = aStrategy == LEFT ? (*(*IJ)[i])[jPrime] - gPreorder : (*(*IJ)[i])[jPrime] - gRevPreorder;
                            match += (*(*tCOPY)[previousI])[i + jPrime - previousI];
                        } else {
                            match += (*(*tCOPY)[i])[jPrime];
                        }
                    } else if(kBis > 0) {
                        match += (*(*s)[kBis - 1])[jPrime];
                    } else {
                        match += gSize - (i + jPrime);
                    }
                }
                (*(*s)[kPrime - 1])[j] = (deleteFromLeft < deleteFromRight) ? ((deleteFromLeft < match) ? deleteFromLeft : match) : ((deleteFromRight < match) ? deleteFromRight : match);
                deleteFromRight = costIns;
                deleteFromLeft = costDel;
                match = 0;
            }
        }
        t->set(i, (realStrategy == BOTH && aStrategy == LEFT) ? (*s[k - 1 - 1])->clone() : (*s[k - 1])->clone());
        if(i > 0) {
            jOfIminus1 = this->jOfI(it2, i - 1, gSize, gRevPreorder, gPreorder, aStrategy, gTreeSize);
            if(jOfIminus1 <= jOfI) {
                for (auto x = 0; x < k; x++) {
                    (*q)[x] = (*(*s)[x])[jOfIminus1];
                }
            }
            if(i + jOfIminus1 < gSize) {
                gijOfIMinus1Preorder = aStrategy == LEFT ? (*(*it2->info)[POST2_PRE])[gTreeSize - 1 - (gRevPreorder + (i - 1))] : gPreorder + (i - 1);
                if(k - 1 - 1 < 0) {
                    if(aStrategy != previousStrategy) {
                        previousI = aStrategy == LEFT ? (*(*IJ)[i])[jOfIminus1] - gPreorder : (*(*IJ)[i])[jOfIminus1] - gRevPreorder;
                        setDeltaValue((*(*it1->info)[PRE2_POST])[vpPreorder], (*(*it2->info)[PRE2_POST])[gijOfIMinus1Preorder], (*(*tCOPY)[previousI])[i + jOfIminus1 - previousI], switched);
                    } else {
                        setDeltaValue((*(*it1->info)[PRE2_POST])[vpPreorder], (*(*it2->info)[PRE2_POST])[gijOfIMinus1Preorder], (*(*tCOPY)[i])[jOfIminus1], switched);
                    }
                } else {
                    setDeltaValue((*(*it1->info)[PRE2_POST])[vpPreorder], (*(*it2->info)[PRE2_POST])[gijOfIMinus1Preorder], (*(*s)[k - 1 - 1])[jOfIminus1], switched);
                }
            }
        }
    }
    previousStrategy = aStrategy;
}

void RTED::computeIJTable(InfoTree* it, int subtreePreorder, int subtreeRevPreorder, int subtreeSize, int aStrategy, int treeSize)
{
    int change;
    auto post2pre = (*it->info)[POST2_PRE];
    auto rpost2post = (*it->info)[RPOST2_POST];
    if(aStrategy == LEFT) {
        for (auto x = 0; x < subtreeSize; x++) {
            (*(*IJ)[0])[x] = x + subtreePreorder;
        }
        for (auto x = 1; x < subtreeSize; x++) {
            change = (*post2pre)[(treeSize - 1 - (x - 1 + subtreeRevPreorder))];
            for (auto z = 0; z < subtreeSize; z++) {
                if((*(*IJ)[x - 1])[z] >= change) {
                    (*(*IJ)[x])[z] = (*(*IJ)[x - 1])[z] + 1;
                } else {
                    (*(*IJ)[x])[z] = (*(*IJ)[x - 1])[z];
                }
            }
        }
    } else {
        for (auto x = 0; x < subtreeSize; x++) {
            (*(*IJ)[0])[x] = x + subtreeRevPreorder;
        }
        for (auto x = 1; x < subtreeSize; x++) {
            change = treeSize - 1 - (*rpost2post)[(treeSize - 1 - (x - 1 + subtreePreorder))];
            for (auto z = 0; z < subtreeSize; z++) {
                if((*(*IJ)[x - 1])[z] >= change) {
                    (*(*IJ)[x])[z] = (*(*IJ)[x - 1])[z] + 1;
                } else {
                    (*(*IJ)[x])[z] = (*(*IJ)[x - 1])[z];
                }
            }
        }
    }
}

int RTED::jOfI(InfoTree* it, int aI, int aSubtreeWeight, int aSubtreeRevPre, int aSubtreePre, int aStrategy, int treeSize)
{
    return aStrategy == LEFT ? aSubtreeWeight - aI - (*(*it->info)[POST2_SIZE])[treeSize - 1 - (aSubtreeRevPre + aI)] : aSubtreeWeight - aI - (*(*it->info)[POST2_SIZE])[(*(*it->info)[RPOST2_POST])[treeSize - 1 - (aSubtreePre + aI)]];
}

void RTED::setDeltaValue(int a, int b, double value, bool switched)
{
    if(switched) {
        (*(*delta)[b])[a] = value;
    } else {
        (*(*delta)[a])[b] = value;
    }
}

void RTED::setDeltaBitValue(int a, int b, int8_t value, bool switched)
{
    if(switched) {
        (*(*deltaBit)[b])[a] = value;
    } else {
        (*(*deltaBit)[a])[b] = value;
    }
}

void RTED::setCustomCosts(double costDel, double costIns, double costMatch)
{
    this->costDel = costDel;
    this->costIns = costIns;
    this->costMatch = costMatch;
}

void RTED::setCustomStrategy(::intArrayArray* strategyArray_)
{
    STR = strategyArray_;
}

void RTED::setCustomStrategy(int strategy, bool ifSwitch)
{
    STR = new ::intArrayArray(size1);
    if(ifSwitch) {
        for (auto i = 0; i < size1; i++) {
            for (auto j = 0; j < size2; j++) {
                (*(*STR)[i])[j] = (*(*it1->info)[POST2_SIZE])[i] >= (*(*it2->info)[POST2_SIZE])[j] ? strategy : strategy + 4;
            }
        }
    } else {
        for (auto i = 0; i < size1; i++) {
            ::java::util::Arrays::fill((*STR)[i], strategy);
        }
    }
}

java::util::LinkedList* RTED::computeEditMapping()
{
    auto treedist = new ::doubleArrayArray(size1 + 1);
    auto forestdist = new ::doubleArrayArray(size1 + 1);
    auto rootNodePair = true;
    for (auto i = 0; i < size1; i++) {
        (*(*treedist)[i])[0] = i;
    }
    for (auto j = 0; j < size2; j++) {
        (*(*treedist)[0])[j] = j;
    }
    for (auto i = 1; i <= size1; i++) {
        for (auto j = 1; j <= size2; j++) {
            (*(*treedist)[i])[j] = (*(*delta)[i - 1])[j - 1] + (*(*deltaBit)[i - 1])[j - 1];
        }
    }
    forestDist(it1, it2, size1, size2, treedist, forestdist);
    auto editMapping = new ::java::util::LinkedList();
    auto treePairs = new ::java::util::LinkedList();
    treePairs->push(static_cast< ::java::lang::Object* >(new ::intArray({
        size1
        , size2
    })));
    while (!treePairs->isEmpty()) {
        auto treePair = java_cast< ::intArray* >(treePairs->pop());
        auto lastRow = (*treePair)[0];
        auto lastCol = (*treePair)[1];
        if(!rootNodePair) {
            forestDist(it1, it2, lastRow, lastCol, treedist, forestdist);
        }
        rootNodePair = false;
        auto firstRow = it1->getInfo(POST2_LLD, lastRow - 1) + 1 - 1;
        auto firstCol = it2->getInfo(POST2_LLD, lastCol - 1) + 1 - 1;
        auto row = lastRow;
        auto col = lastCol;
        while ((row > firstRow) || (col > firstCol)) {
            if((row > firstRow) && ((*(*forestdist)[row - 1])[col] + costDel == (*(*forestdist)[row])[col])) {
                editMapping->push(static_cast< ::java::lang::Object* >(new ::intArray({
                    row
                    , 0
                })));
                row--;
            } else if((col > firstCol) && ((*(*forestdist)[row])[col - 1] + costIns == (*(*forestdist)[row])[col])) {
                editMapping->push(static_cast< ::java::lang::Object* >(new ::intArray({
                    0
                    , col
                })));
                col--;
            } else {
                if((it1->getInfo(POST2_LLD, row - 1) == it1->getInfo(POST2_LLD, lastRow - 1)) && (it2->getInfo(POST2_LLD, col - 1) == it2->getInfo(POST2_LLD, lastCol - 1))) {
                    editMapping->push(static_cast< ::java::lang::Object* >(new ::intArray({
                        row
                        , col
                    })));
                    row--;
                    col--;
                } else {
                    treePairs->push(static_cast< ::java::lang::Object* >(new ::intArray({
                        row
                        , col
                    })));
                    row = it1->getInfo(POST2_LLD, row - 1) + 1 - 1;
                    col = it2->getInfo(POST2_LLD, col - 1) + 1 - 1;
                }
            }
        }
    }
    return editMapping;
}

void RTED::forestDist(InfoTree* ted1, InfoTree* ted2, int i, int j, ::doubleArrayArray* treedist, ::doubleArrayArray* forestdist)
{
    (*(*forestdist)[ted1->getInfo(POST2_LLD, i - 1) + 1 - 1])[ted2->getInfo(POST2_LLD, j - 1) + 1 - 1] = 0;
    for (auto di = ted1->getInfo(POST2_LLD, i - 1) + 1; di <= i; di++) {
        (*(*forestdist)[di])[ted2->getInfo(POST2_LLD, j - 1) + 1 - 1] = (*(*forestdist)[di - 1])[ted2->getInfo(POST2_LLD, j - 1) + 1 - 1] + costDel;
        for (auto dj = ted2->getInfo(POST2_LLD, j - 1) + 1; dj <= j; dj++) {
            (*(*forestdist)[ted1->getInfo(POST2_LLD, i - 1) + 1 - 1])[dj] = (*(*forestdist)[ted1->getInfo(POST2_LLD, i - 1) + 1 - 1])[dj - 1] + costIns;
            if((ted1->getInfo(POST2_LLD, di - 1) == ted1->getInfo(POST2_LLD, i - 1)) && (ted2->getInfo(POST2_LLD, dj - 1) == ted2->getInfo(POST2_LLD, j - 1))) {
                double costRen = 0;
                if(!(ted1->getInfo(POST2_LABEL, di - 1) == ted2->getInfo(POST2_LABEL, dj - 1))) {
                    costRen = costMatch;
                }
                (*(*forestdist)[di])[dj] = ::java::lang::Math::min(::java::lang::Math::min((*(*forestdist)[di - 1])[dj] + costDel, (*(*forestdist)[di])[dj - 1] + costIns), (*(*forestdist)[di - 1])[dj - 1] + costRen);
                (*(*treedist)[di])[dj] = (*(*forestdist)[di])[dj];
            } else {
                (*(*forestdist)[di])[dj] = ::java::lang::Math::min(::java::lang::Math::min((*(*forestdist)[di - 1])[dj] + costDel, (*(*forestdist)[di])[dj - 1] + costIns), (*(*forestdist)[ted1->getInfo(POST2_LLD, di - 1) + 1 - 1])[ted2->getInfo(POST2_LLD, dj - 1) + 1 - 1] + (*(*treedist)[di])[dj]);
            }
        }
    }
}

} // namespace query
} // namespace clang
