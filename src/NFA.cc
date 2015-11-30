//
// Created by neverchanje on 11/25/15.
//

#include "NFA.h"

using namespace lexer;

typedef NFA::Machine Machine;

EpsClosure NFA::GetEpsClosure(const std::vector<State> &T) {
  // Since retrieving from unordered_map requires modification of the map,
  // this method cannot mark const.
  std::vector<State> S = T;

  EpsClosure E;

  while (!S.empty()) {
    State u = S.back();
    S.pop_back();

    auto &tos = trans1_[u][SYM_EPSILON];
    State v;
    for (auto it = tos.begin(); it != tos.end(); it++) {
      v = *it;
      if (E.insert(v).second == true) { // v is not in epsilon closure.
        S.push_back(v);
      }
    }
  }
  return E;
}

void NFA::AddTrans(State from, int sym, State to) {
  trans1_[from][sym].push_back(to);
  trans2_[from][to] = sym;
}

void NFA::Dump() const {
  fprintf(stderr, "------- Begining of dumping the NFA. -------");
  for (auto t1 = trans2_.begin(); t1 != trans2_.end(); t1++) {
    for (auto t2 = (*t1).second.begin(); t2 != (*t1).second.end(); t2++) {
      fprintf(stderr, "<from:%d, sym:%d, to:%d>\n",
              (*t1).first, (*t2).first, (*t2).second);
    }
  }
  fprintf(stderr, "------- Ending of dumping the NFA. -------\n");
}

State NFA::MakeState() {
  State stateId = maxStateId_;
  maxStateId_++;
  return stateId;
}

Machine NFA::MakeOpt(Machine mach) {
  State start = MakeState();
  State final = MakeState();
  AddTrans(start, SYM_EPSILON, mach.Start);
  AddTrans(mach.Final, SYM_EPSILON, final);
  AddTrans(start, SYM_EPSILON, final);
  return Machine(start, final);
}

Machine NFA::MakeOr(Machine first, Machine second) {
  State start = MakeState();
  State final = MakeState();
  AddTrans(start, SYM_EPSILON, first.Start);
  AddTrans(start, SYM_EPSILON, second.Start);
  AddTrans(first.Final, SYM_EPSILON, final);
  AddTrans(second.Final, SYM_EPSILON, final);
  return Machine(start, final);
}

int NFA::NumOfStates() const {
  return maxStateId_;
}

State NFA::AddAccept(State state, const AcceptData &data) {
  State accept = MakeState();
  AddTrans(state, SYM_EPSILON, accept);
  accepts_[accept] = data;
  return accept;
}

Machine NFA::MakeClosure(Machine mach) {
  return MakeOpt(MakePosClosure(mach));
}

Machine NFA::MakePosClosure(Machine mach) {
  AddTrans(mach.Final, SYM_EPSILON, mach.Start);
  return mach;
}