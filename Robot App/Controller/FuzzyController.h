// Ensure this library description is only included once
#ifndef FuzzyController_h
#define FuzzyController_h

#include "Arduino.h"

// Fuzzy controller specific library
#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyOutput.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>

class FuzzyController
{
  public:
    FuzzyController();
    void setInput(int, int);
    void fuzzify();
    int defuzzify(int);

  private:
    Fuzzy* _fuzzy;
};
#endif
