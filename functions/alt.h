#ifndef FUNCTIONS_ALTF_H
#define FUNCTIONS_ALTF_H

// Usage: AltF
// return value: INTEGER
// Returns current_alternative for use in ColorSelect<>, TrSelect<> or IntSelect<>

class AltF {
 public:
    void run(BladeBase* blade) {}
    int getInteger(int led) { return current_alternative; }
};

#endif
