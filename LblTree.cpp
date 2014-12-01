#include "LblTree.h"

using namespace llvm;

namespace clang {
namespace diff {

namespace {
const char *TAB_STRING_ = "    ";
const char *ROOT_STRING_ = "*---+";
const char *BRANCH_STRING_ = "+---+";
const char *OPEN_BRACKET_ = "{";
const char *CLOSE_BRACKET_ = "}";
const char *ID_SEPARATOR_ = ":";
}

LblTree::LblTree(std::string* label, int treeID)
  : TreeID(treeID), NodeID(NO_NODE), Label(label), TmpData(nullptr) { }

LblTree::LblTree()
  : LblTree(nullptr, NO_TREE_ID) { }

void LblTree::prettyPrint() {
    prettyPrint(false);
}

void LblTree::prettyPrint(bool printTmpData) {
    for (auto i = 0; i < getLevel(); i++) {
        ::java::lang::System::out()->print(TAB_STRING_);
    }
    if(!isRoot()) {
        ::java::lang::System::out()->print(BRANCH_STRING_);
    } else {
        if(getTreeID() != NO_TREE_ID) {
            ::java::lang::System::out()->println(::java::lang::StringBuilder().append(u"treeID: "_j)->append(getTreeID())->toString());
        }
        ::java::lang::System::out()->print(ROOT_STRING_);
    }
    ::java::lang::System::out()->print(::java::lang::StringBuilder().append(u" '"_j)->append(this->getLabel())
        ->append(u"' "_j)->toString());
    if(printTmpData) {
        ::java::lang::System::out()->println(getTmpData());
    } else {
        ::java::lang::System::out()->println();
    }
    for (auto *e = children(); e->hasMoreElements(); ) {
        (java_cast< LblTree* >(java_cast< ::java::lang::Object* >(e->nextElement())))->prettyPrint(printTmpData);
    }
}

int LblTree::getNodeCount()
{
    auto sum = 1;
    for (auto *e = children(); e->hasMoreElements(); ) {
        sum += (java_cast< LblTree* >(java_cast< ::java::lang::Object* >(e->nextElement())))->getNodeCount();
    }
    return sum;
}

LblTree* LblTree::fromString(::java::lang::String* s)
{
    clinit();
    auto treeID = FormatUtilities::getTreeID(s);
    s = s->substring(s->indexOf(OPEN_BRACKET_), s->lastIndexOf(CLOSE_BRACKET_) + 1);
    auto node = new LblTree(FormatUtilities::getRoot(s), treeID);
    auto c = FormatUtilities::getChildren(s);
    for (auto i = 0; i < c->size(); i++) {
        node->add(fromString(java_cast< ::java::lang::String* >(java_cast< ::java::lang::Object* >(c->elementAt(i)))));
    }
    return node;
}

std::string LblTree::toString()
{
    auto res = ::java::lang::StringBuilder().append(OPEN_BRACKET_)->append(getLabel())->toString();
    if((getTreeID() >= 0) && (isRoot())) {
        res = ::java::lang::StringBuilder().append(getTreeID())->append(ID_SEPARATOR_)
            ->append(res)->toString();
    }
    for (auto *e = children(); e->hasMoreElements(); ) {
        res = ::java::lang::StringBuilder(res).append((java_cast< LblTree* >(java_cast< ::java::lang::Object* >(e->nextElement())))->toString())->toString();
    }
    res = ::java::lang::StringBuilder(res).append(CLOSE_BRACKET_)->toString();
    return res;
}

int LblTree::compareTo(::java::lang::Object* o)
{
    return getLabel()->compareTo((java_cast< LblTree* >(o))->getLabel());
}

void LblTree::clearTmpData()
{
    for (auto *e = breadthFirstEnumeration(); e->hasMoreElements(); ) {
        (java_cast< LblTree* >(java_cast< ::java::lang::Object* >(e->nextElement())))->setTmpData(nullptr);
    }
}

} // namespace query
} // namespace clang
