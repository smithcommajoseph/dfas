-- Running ghci from the directory in which this file is located, 
-- this module was tested by doing the following (with inputs ouputs listed)
-- 
--   :l dfa.hs
-- 
--   dfaAccept stateFactory "0.01"
--      True
--   dfaAccept stateFactory ".01"
--      True
--   dfaAccept stateFactory "01.01"
--      True
--   dfaAccept stateFactory "01.011."
--      False
--   dfaAccept stateFactory "1000.12"
--      *** Exception: 2 is not in the DFA's alphabet
--      CallStack (from HasCallStack):
--      error, called at joseph_smith__assignment_7.hs:92:32 in main:Dfa
--   dfaAccept stateFactory "01.011.ab"
--      *** Exception: a is not in the DFA's alphabet
--      CallStack (from HasCallStack):
--      error, called at joseph_smith__assignment_7.hs:92:32 in main:Dfa
--   dfaAccept stateFactory "1"
--      False
--   dfaAccept stateFactory "1."
--      True


-- Create the Dfa module and set up the exports
module Dfa (
    stateFactory
,   dfaAccept
) where  


-- 
-- Custom types
-- 

type State = [Char]
type Letter = [Char]
type Transition = (State, Letter, State)
type DFA = ([State], [Letter], State, [State], [Transition])


-- 
-- Private functions
-- 

-- Returns the first item, a list of possible states, from the DFA tuple
getFirstStateItem :: DFA -> [State]
getFirstStateItem (x, _, _, _, _) = x

-- Returns the second item, a list of 'letters' or the alphabet, from the DFA tuple
getSecondStateItem :: DFA -> [Letter]
getSecondStateItem (_, x, _, _, _)= x

-- Returns the third item, the start state, from the DFA tuple
getThirdStateItem :: DFA -> State
getThirdStateItem (_, _, x, _, _) = x   

-- Returns the fourth item, the list of final states, from the DFA tuple
getFourthStateItem :: DFA -> [State]
getFourthStateItem (_, _, _, x, _) = x

-- Returns the fifth item, the list of transitions, from the DFA tuple
getFifthStateItem :: DFA -> [Transition]
getFifthStateItem (_, _, _, _, x) = x

-- Returns the list of all possible states from the given DFA
allStates :: (() -> DFA) -> [State]
allStates (stateFactory) = getFirstStateItem (stateFactory())

-- Returns the alphabet from the given DFA
alphabet :: (() -> DFA) -> [Letter]
alphabet (stateFactory) = getSecondStateItem (stateFactory())

-- Returns the first state from the given DFA
firstState :: (() -> DFA) -> State
firstState (stateFactory) = getThirdStateItem (stateFactory())

-- Returns the accept/final states from the given DFA
acceptStates :: (() -> DFA) -> [State]
acceptStates (stateFactory) = getFourthStateItem (stateFactory())

-- Returns the list of transitions from the given DFA
allTransitions :: (() -> DFA) -> [Transition]
allTransitions (stateFactory) = getFifthStateItem (stateFactory())

-- Returns the initial state (first item) from given transition
transFromState :: Transition -> State
transFromState (x, _, _) = x

-- Returns the letter/label/edge (second item) from given transition
transLabel :: Transition -> Letter
transLabel (_, x, _) = x

-- Returns the target state (third item) from given transition
transToState :: Transition -> State
transToState (_, _, x) = x

-- Returns an array with a single transition matching the given starting state and label
findTransition :: State -> Letter -> [Transition] -> [Transition]
findTransition curState label transitions = [transition | transition <- transitions
    , transFromState transition == curState
    , transLabel transition == label]

-- Returns the next state, ultimately returning the final state based on the 
-- given DFA and input strings
findNextState :: (() -> DFA) -> [Char] -> State
findNextState stateFactory "" = firstState stateFactory 
findNextState stateFactory input 
    | length transition == 0 = error  (last input : " is not in the DFA's alphabet")
    | otherwise = transToState (head transition)
    where
    nextState = findNextState stateFactory (init input)
    transition = findTransition nextState [(last input)] (allTransitions(stateFactory))


-- 
-- Public functions
-- 

-- Returns a fully formed Dfa from hardcoded literal values
stateFactory :: () -> DFA
stateFactory () = (states, epsilon, startState, endStates, transitions)
    where 
    states = ["q0", "q1", "q2", "q3", "q4"]
    epsilon = ["0", "1", "."]
    startState = "q0"
    endStates = ["q3"]
    transitions = [ ("q0", "0", "q1")
        ,   ("q0", "1", "q1")
        ,   ("q0", ".", "q2")
        ,   ("q1", "0", "q1")
        ,   ("q1", "1", "q1")
        ,   ("q1", ".", "q3")
        ,   ("q2", "0", "q3")
        ,   ("q2", "1", "q3")
        ,   ("q2", ".", "q4")
        ,   ("q3", "0", "q3")
        ,   ("q3", "1", "q3")
        ,   ("q3", ".", "q4")
        ,   ("q4", "0", "q4")
        ,   ("q4", "1", "q4")
        ,   ("q4", ".", "q4") ]

-- Evaluates a user provided string against the Dfa function provided, returning
-- a boolean to indicate success or failure
dfaAccept :: (() -> DFA) -> [Char] -> Bool
dfaAccept stateFactory input = finalState `elem` finalStates
    where 
    finalState = findNextState stateFactory input
    finalStates = acceptStates stateFactory
