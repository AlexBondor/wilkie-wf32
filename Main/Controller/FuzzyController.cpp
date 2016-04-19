#include "FuzzyController.h"

#define PWM_MIN   50
#define PWM_MAX  255
#define PWM_ZERO  70
#define PWM_STEP  10

#define RPM_MIN    0
#define RPM_MAX  250
#define RPM_ZERO 100
#define RPM_STEP   5

/*
 *  Class written by taking the online eFLL library example as model.
 *  The values of the fuzzy sets were determined by doing some physical
 *  experiments.
 */
FuzzyController::FuzzyController()
{
  _fuzzy = new Fuzzy();

  // Input RPM
  FuzzySet*         low_RPM = new FuzzySet(                RPM_MIN,                 RPM_MIN, RPM_ZERO - 2 * RPM_STEP, RPM_ZERO -     RPM_STEP);
  FuzzySet*  little_low_RPM = new FuzzySet(RPM_ZERO - 2 * RPM_STEP, RPM_ZERO -     RPM_STEP, RPM_ZERO -     RPM_STEP,                RPM_ZERO);
  FuzzySet*       right_RPM = new FuzzySet(RPM_ZERO -     RPM_STEP,                RPM_ZERO,                RPM_ZERO, RPM_ZERO +     RPM_STEP);
  FuzzySet* little_high_RPM = new FuzzySet(               RPM_ZERO, RPM_ZERO +     RPM_STEP, RPM_ZERO +     RPM_STEP, RPM_ZERO + 2 * RPM_STEP);
  FuzzySet*        high_RPM = new FuzzySet(RPM_ZERO +     RPM_STEP, RPM_ZERO + 2 * RPM_STEP,                 RPM_MAX,                 RPM_MAX);
  
  FuzzyInput* RPM = new FuzzyInput(1);
  RPM->addFuzzySet(        low_RPM);
  RPM->addFuzzySet( little_low_RPM);
  RPM->addFuzzySet(      right_RPM);
  RPM->addFuzzySet(little_high_RPM);
  RPM->addFuzzySet(       high_RPM);
  
  _fuzzy->addFuzzyInput(RPM);

  // Output PWM
  FuzzySet*         low_PWM = new FuzzySet(                PWM_MIN,                 PWM_MIN, PWM_ZERO - 2 * PWM_STEP, PWM_ZERO -     PWM_STEP);
  FuzzySet*  little_low_PWM = new FuzzySet(PWM_ZERO - 2 * PWM_STEP, PWM_ZERO -     PWM_STEP, PWM_ZERO -     PWM_STEP,                PWM_ZERO);
  FuzzySet*       right_PWM = new FuzzySet(PWM_ZERO -     PWM_STEP,                PWM_ZERO,                PWM_ZERO, PWM_ZERO +     PWM_STEP);
  FuzzySet* little_high_PWM = new FuzzySet(               PWM_ZERO, PWM_ZERO +     PWM_STEP, PWM_ZERO +     PWM_STEP, PWM_ZERO + 2 * PWM_STEP);
  FuzzySet*        high_PWM = new FuzzySet(PWM_ZERO +     PWM_STEP, PWM_ZERO + 2 * PWM_STEP,                 PWM_MAX,                 PWM_MAX);
  
  FuzzyOutput* PWM = new FuzzyOutput(1);
  PWM->addFuzzySet(       high_PWM);
  PWM->addFuzzySet(little_high_PWM);
  PWM->addFuzzySet(      right_PWM);
  PWM->addFuzzySet( little_low_PWM);
  PWM->addFuzzySet(        low_PWM);
  
  _fuzzy->addFuzzyOutput(PWM);

  // Build the rules
  // 1. If         low_RPM then        high_PWM
  // 2. If  little_low_RPM then little_high_PWM
  // 3. If       right_RPM then       right_PWM
  // 4. If little_high_RPM then  little_low_PWM
  // 5. It        high_RPM then         low_PWM

  // Rule no. 1
  FuzzyRuleAntecedent* ifRPMLow = new FuzzyRuleAntecedent();
  ifRPMLow->joinSingle(low_RPM);
  
  FuzzyRuleConsequent* thenPWMHigh = new FuzzyRuleConsequent();
  thenPWMHigh->addOutput(high_PWM);
  
  FuzzyRule* fuzzyRule1 = new FuzzyRule(1, ifRPMLow, thenPWMHigh);  
  _fuzzy->addFuzzyRule(fuzzyRule1);


  // Rule no. 2
  FuzzyRuleAntecedent* ifRPMLittleLow = new FuzzyRuleAntecedent();
  ifRPMLittleLow->joinSingle(little_low_RPM);
  
  FuzzyRuleConsequent* thenPWMLittleHigh = new FuzzyRuleConsequent();
  thenPWMLittleHigh->addOutput(little_high_PWM);
  
  FuzzyRule* fuzzyRule2 = new FuzzyRule(2, ifRPMLittleLow, thenPWMLittleHigh);  
  _fuzzy->addFuzzyRule(fuzzyRule2);


  // Rule no. 3
  FuzzyRuleAntecedent* ifRPMRight = new FuzzyRuleAntecedent();
  ifRPMRight->joinSingle(right_RPM);
  
  FuzzyRuleConsequent* thenPWMRight = new FuzzyRuleConsequent();
  thenPWMRight->addOutput(right_PWM);
  
  FuzzyRule* fuzzyRule3 = new FuzzyRule(3, ifRPMRight, thenPWMRight);  
  _fuzzy->addFuzzyRule(fuzzyRule3);


  // Rule no. 4
  FuzzyRuleAntecedent* ifRPMLittleHigh = new FuzzyRuleAntecedent();
  ifRPMLittleHigh->joinSingle(little_high_RPM);
  
  FuzzyRuleConsequent* thenPWMLittleLow = new FuzzyRuleConsequent();
  thenPWMLittleLow->addOutput(little_low_PWM);
  
  FuzzyRule* fuzzyRule4 = new FuzzyRule(4, ifRPMLittleHigh, thenPWMLittleLow);  
  _fuzzy->addFuzzyRule(fuzzyRule4);


  // Rule no. 5
  FuzzyRuleAntecedent* ifRPMHigh = new FuzzyRuleAntecedent();
  ifRPMHigh->joinSingle(high_RPM);
  
  FuzzyRuleConsequent* thenPWMLow = new FuzzyRuleConsequent();
  thenPWMLow->addOutput(low_PWM);
  
  FuzzyRule* fuzzyRule5 = new FuzzyRule(5, ifRPMHigh, thenPWMLow);  
  _fuzzy->addFuzzyRule(fuzzyRule5);
}

void FuzzyController::setInput(int ruleNumber, int input)
{
  _fuzzy->setInput(ruleNumber, input);
}

void FuzzyController::fuzzify()
{
  _fuzzy->fuzzify();
}

int FuzzyController::defuzzify(int ruleNumber)
{
  return _fuzzy->defuzzify(ruleNumber);
}

