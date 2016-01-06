//
// Created by neverchanje on 11/25/15.
//

#ifndef LEXER_NFA_H
#define LEXER_NFA_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <boost/noncopyable.hpp>
#include <string>

#include "LexerDef.h"
#include "TokenTable.h"

namespace lexer {

class DFA;

typedef std::unordered_set<State> StateSet;

/**
 * Definition of epsilon closure in flex:
 * The epsilon closure is the set of all states reachable by an arbitrary
 * number of epsilon transitions, which themselves do not have epsilon
 * transitions going out, unioned with the set of states which have non-null
 * accepting numbers.
 *
 * Since searching and insertion are the most usual operations in the manipulation
 * of EpsClosure, we use std::unordered_set, which is time-efficient in both of two.
 */
typedef StateSet EpsClosure;

/**
 * Mark the symbol epsilon.
 */
const int SYM_EPSILON = 257;

/**
 * Non-deterministic Finite Automata
 */
class NFA: boost::noncopyable {

// TODO: Optimization: Merge the states which have only epsilon transitions.

 public:

  /**
   * Each Machine is a NFA constructed by a regular expression.
   *
   * The construction of a Machine can be divided into the construction of
   * smaller machines, which are constructed by the constituent subexpressions
   * of the given expression.
   */
  struct Machine {
    // Each Machine contains only single start and final state.
    State start, final;

    Machine(State st, State fn) :
        start(st),
        final(fn) {
    }

    Machine(NFA &nfa) :
        start(nfa.MakeState()),
        final(nfa.MakeState()) {
    }
  };

 public:

  static const State START_STATE = 0;
  static const State FINAL_STATE = -1;

  // NFA is created with a start state and a final state.
  NFA() :
      start_(START_STATE),
      final_(FINAL_STATE),
      maxStateId_(0) {
  }

  // Construct the epsilon closure of the set of NFA states T, and return the
  // result.
  void GetEpsClosure(const std::vector<State> &T, EpsClosure &E) const;

  // Subset construction algorithm.
  DFA ToDFA();

  void AddTrans(State from, Sym sym, State to);

  bool HasTrans(State from, Sym sym) const;

  const std::vector<State> &GetTrans(State, Sym sym) const;

  void AddAccept(State state, int data);

  // Debugging method to write out all of the transitions in the NFA.
  void Dump() const;

  int NumOfStates() const { return maxStateId_ + 2; }

  // Make a new state and add it into the NFA.
  // The index of the first state is 1.
  State MakeState() { return ++maxStateId_; }

  // Convert a machine into a closure.
  // Equivalent with '*' in regex.
  Machine MakeClosure(Machine mach);

  // Make a machine optional.
  // Equivalent with '?' in regex.
  Machine MakeOpt(Machine mach);

  // Equivalent with '|' in regex.
  Machine MakeOr(Machine first, Machine second);

  // Equivalent with '+' in regex.
  Machine MakePosClosure(Machine mach);

  Machine Mach() { return Machine(start_, final_); }

 private:

  // Transition tuple (State from, Sym symbol, State to)
  std::unordered_map<State, std::unordered_map<Sym, std::vector<State> > > trans1_;

  // (State from, State to, Sym symbol)
  std::unordered_map<State, std::unordered_map<State, Sym> > trans2_;

  // It also indicates the number of current states.
  int maxStateId_;

  State start_, final_;

  std::unordered_map<State, int> accepts_;

};

} // namespace lexer

#endif //LEXER_NFA_H