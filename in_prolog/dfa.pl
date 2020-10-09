% Running swipl from the directory in which this file is located, 
% this module was tested by doing the following (with inputs ouputs listed)
% 
% ?- [dfa].
% 
% ?- dfaAccept(dfa1, '1.0').
% true.
% ?- dfaAccept(dfa1, '.0').
% true.
% ?- dfaAccept(dfa1, '1.0.1').
% false.
% ?- dfaAccept(dfa1, '.').
% false.
% ?- dfaAccept(dfa1, '..100').
% false.
% ?- dfaAccept(dfa1, '.100').
% true.
% 


% Facts about the dfa1
dfa_start(dfa1, q0).
dfa_final(dfa1, q3).
dfa_transition(dfa1, q0, '0', q1).
dfa_transition(dfa1, q0, '1', q1).
dfa_transition(dfa1, q0, '.', q2).
dfa_transition(dfa1, q1, '0', q1).
dfa_transition(dfa1, q1, '1', q1).
dfa_transition(dfa1, q1, '.', q3).
dfa_transition(dfa1, q2, '0', q3).
dfa_transition(dfa1, q2, '1', q3).
dfa_transition(dfa1, q2, '.', q4).
dfa_transition(dfa1, q3, '0', q3).
dfa_transition(dfa1, q3, '1', q3).
dfa_transition(dfa1, q3, '.', q4).
dfa_transition(dfa1, q4, '0', q4).
dfa_transition(dfa1, q4, '1', q4).
dfa_transition(dfa1, q4, '.', q4).


% Returns a list containing all the states in the given DFA 
allStates(DFA, States) :-
    findall([F,T], dfa_transition(DFA, F, _, T), Transitions), 
    flatten(Transitions, FlatTransitions),
    list_to_set(FlatTransitions, States).


% Returns a boolean indicatingif the given input string is accepted by the
% given DFA
dfaAccept(DFA, InputStr) :-
    dfa_start(dfa1, CurrentState),
    string_to_list(InputStr, StrList),
    dfaAccept(DFA, StrList, CurrentState),
    !.
dfaAccept(DFA, [H|T], CurrentState) :-
    char_code(Letter, H),
    dfa_transition(DFA, CurrentState, Letter, NextState),
    dfaAccept(DFA, T, NextState).
dfaAccept(DFA, [ ], CurrentState) :- 
    dfa_final(DFA, CurrentState),
    !.
