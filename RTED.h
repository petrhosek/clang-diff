#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_RTED_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_DIFF_RTED_H

namespace clang {
namespace diff {

class LblTree;
class LabelDictionary;

class RTED {
public:
    RTED(double delCost, double insCost, double matchCost);

private:
    InfoTree* it1;
    InfoTree* it2;
    int size1;
    int size2;
    ::util::LabelDictionary* ld;
    ::intArrayArray* STR;
    ::doubleArrayArray* delta;
    ::int8_tArrayArray* deltaBit};
    ::intArrayArray* IJ;
    ::int64_tArrayArrayArray* costV;
    ::int64_tArrayArray* costW;
    ::doubleArrayArray* t;
    ::doubleArrayArray* tCOPY;
    ::doubleArrayArray* tTMP;
    ::doubleArrayArray* s;
    ::doubleArray* q;

public:
    int64_t counter;

private:
    double da, db, dc;
    int previousStrategy;

public:
    ::intArray* strStat;

public:
    virtual double nonNormalizedTreeDist(::util::LblTree* t1, ::util::LblTree* t2);
    virtual double nonNormalizedTreeDist();
    virtual void init_(::util::LblTree* t1, ::util::LblTree* t2);
    virtual void computeOptimalStrategy();

private:
    double computeDistUsingStrArray_(InfoTree* it1, InfoTree* it2);
    double spfL(InfoTree* it1, InfoTree* it2);
    void treeEditDist(InfoTree* it1, InfoTree* it2, int i, int j);
    double spfR(InfoTree* it1, InfoTree* it2);
    void treeEditDistRev(InfoTree* it1, InfoTree* it2, int i, int j);
    double spfH(InfoTree* it1, InfoTree* it2, ::intArray* heavyPath);
    void computePeriod(InfoTree* it1, int aVp, int aNextVp, InfoTree* it2, int aStrategy);
    void computeIJTable(InfoTree* it, int subtreePreorder, int subtreeRevPreorder, int subtreeSize, int aStrategy, int treeSize);
    int jOfI(InfoTree* it, int aI, int aSubtreeWeight, int aSubtreeRevPre, int aSubtreePre, int aStrategy, int treeSize);
    void setDeltaValue(int a, int b, double value, bool switched);
    void setDeltaBitValue(int a, int b, int8_t value, bool switched);

public:
    virtual void setCustomCosts(double costDel, double costIns, double costMatch);
    virtual void setCustomStrategy(::intArrayArray* strategyArray_);
    virtual void setCustomStrategy(int strategy, bool ifSwitch);
    virtual ::java::util::LinkedList* computeEditMapping();

private:
    void forestDist(InfoTree* ted1, InfoTree* ted2, int i, int j, double** treedist, double** forestdist);

private:
    double costDel;
    double costIns;
    double costMatch;
};

} // namespace diff
} // namespace clang

#endif
