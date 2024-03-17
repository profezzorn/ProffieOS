#ifndef MODES_BOOL_SETTING_H
#define MODES_BOOL_SETTING_H

namespace mode {

struct BoolSetting {
  virtual bool get() = 0;
  virtual bool set() = 0;
  virtual bool say() = 0;
};

template<class SPEC, class BoolSetting>
class DirectBoolEntry : public MenuEntry {
public:  
  void say(int entry) override {
    getPtr<BoolSetting>()->say();
    getSL<SPEC>()->SayBool(getPtr<BoolSetting>()->get());
  }
  static void select(int entry) {
    getPtr<BoolSetting>()->set(getPtr<BoolSetting>()->get());
    say(entry);
  }
};

// Represents two menu entries.
// This is used by the class below and should generally not be used directly.
template<class SPEC, class BoolSetting>
struct IndirectBoolEntries : public MenuEntry {
  static const int size = 2;
  static void say(int entry) override {
    getPtr<BoolSetting>()->say();
    getSL<SPEC>()->SayBool(entry == 1);
  }
  static void select(int entry) override {
    getPtr<BoolSetting>()->set(entry == 1);
    say(entry);
  }
};

template<class SPEC, class BoolSetting>
class IndirectBoolEntry : public MenuEntry {
public:  
  static const int size = 1;
  void say(int entry) override {
    getPtr<BoolSetting>()->say();
    getSL<SPEC>()->SayBool(getPtr<BoolSetting>()->get());
  }
  typedef typename SPEC::template IndirectBoolEntries<BoolSetting> ENTRIES;
  typedef typename SPEC::template MenuEntryMenu<SPEC, ENTRIES> MENU;
  static void select(int entry) {
    pushMode<MENU>();
  }
};

}  // namespace mode

#endif // MODES_BOOL_SETTING_H
