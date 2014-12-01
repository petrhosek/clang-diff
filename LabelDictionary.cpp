#include "LabelDictionary.h"

namespace clang {
namespace diff {

namespace {
const int KEY_DUMMY_LABEL = -1;
}

LabelDictionary::LabelDictionary()
  : Count(0), NewLabelsAllowed(true) { }

int LabelDictionary::store(const std::string& label) {
  auto key = StrInt.find(label);
  if (key != StrInt.end()) {
    return key->second;
  } else if (NewLabelsAllowed) {
    auto intKey = Count++;
    StrInt.insert(StrIntMap::value_type(label, intKey));
    IntStr.insert(IntStrMap::value_type(intKey, label));
    return intKey;
  }
  return KEY_DUMMY_LABEL;
}

std::string LabelDictionary::read(int labelID) {
  return IntStr[labelID];
}

void LabelDictionary::setNewLabelsAllowed(bool newLabelsAllowed) {
  NewLabelsAllowed = newLabelsAllowed;
}

} // namespace query
} // namespace clang
