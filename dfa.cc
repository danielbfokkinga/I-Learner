//$ reserved for end of input string
//# reserved for garbage state
//give input string "input" like this ./dfa "input $"
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include "language.cc"

using namespace std;

//returns true if x is a prefix of y, false if not
bool isPrefix(string x, string y){
	pair<string::iterator, string::iterator> res = mismatch(x.begin(), x.end(), y.begin());
	if(res.first == x.end())
		return true;
	else
		return false;
}

//gets the next symbol from input and removes it
string getSymbol(string &input){
	string symbol;
	string::size_type space;
	space = input.find(' ', 0);
	symbol = input.substr(0, space);
	input = input.substr(space+1);
	return symbol;
}

class Dfa{
	public:
		Dfa(string state, bool accepting);
		void addState(string state, bool accepting);
		void addTransition(string state1, string symbol, string state2);
		bool readSymbol(string symbol);
		bool accepting();
		set<string> checkLanguage(set<string> words);
		string checkLanguage2(set<string> words, set<string> alphabet);
		void printDFA(set<string> alphabet);
		set<string> states;
		map<pair<string, string>, string> transitions;
		
	private:
		set<string> acceptingStates;
		string initialState;
		string garbageState;
		string currentState;
};

//constructor, creates initial state
Dfa::Dfa(string state, bool accepting){
	garbageState = "#";
	initialState = state;
	addState(state, accepting);
	currentState = initialState;
}

void Dfa::addState(string state, bool accepting){
	states.insert(state);
	if(accepting)
		acceptingStates.insert(state);
}

void Dfa::addTransition(string state1, string symbol, string state2){
	transitions.insert(pair<pair<string, string>, string>(pair<string, string>(state1, symbol), state2));
}

//returns true if the current state is an accepting state
bool Dfa::accepting(){
	if(acceptingStates.count(currentState) > 0)
		return true;
}

//returns true if the transition corresponding to the current state and symbol exists
bool Dfa::readSymbol(string symbol){
	if(currentState == garbageState){
		return false;
	}	
	pair<string, string> transition = make_pair(currentState, symbol);
	if(transitions.count(transition)>0){
		currentState = transitions.find(transition)->second;
		return true;
	}else{
		currentState = garbageState;
		return false;
	}
}

//prints the DFA (the states, transitions, accepting and initial state)
void Dfa::printDFA(set<string> alphabet){
	cout << "states: " << endl;
	for(set<string>::iterator it=states.begin(); it!=states.end(); it++){
		cout << *it << endl;
	}
	cout << "initial state: " << initialState << endl;
	cout << "accepting states: " << endl;
	for(set<string>::iterator it=acceptingStates.begin(); it!=acceptingStates.end(); it++){
		cout << *it << endl;
	}
	cout << "transitions: " << endl;
	for(set<string>::iterator it=states.begin(); it!=states.end(); it++){
		for(set<string>::iterator it2=alphabet.begin(); it2!=alphabet.end(); it2++){
			pair<string, string> transition = make_pair(*it, *it2);
			cout << *it << " "<< *it2 << " " << transitions.find(transition)->second << endl;
		}
	}
	cout << endl;
}

//returns true if all the strings from words are accepted by the dfa
set<string> Dfa::checkLanguage(set<string> words){
	set<string> counterexamples;
	string nextSymbol, input;
	for(set<string>::iterator it=words.begin(); it!=words.end(); it++){
		 input = *it;
		 nextSymbol = getSymbol(input);
		 while(nextSymbol != "$"){
			readSymbol(nextSymbol);
			nextSymbol = getSymbol(input);
		}
		if(!accepting()){
			counterexamples.insert(*it);
		}
		currentState = initialState;
	}
	return counterexamples;
}

//returns a string not in words, but accepted by the dfa, or -1 if there is no such string
string Dfa::checkLanguage2(set<string> words, set<string> alphabet){
	set<string> strings;
	set<string> testedStates;
	map<string, string> stringToStates;
	set<string> newStrings;
	map<string, string> newStringToStates;
	strings.insert("");
	pair<string, string> empty = make_pair("", initialState);
	stringToStates.insert(empty);
	bool searching = true;
	while(searching){
		//check if we have found a string that is not a prefix of any of the words
		for(set<string>::iterator it=strings.begin(); it!=strings.end(); it++){
			bool noPrefix = false;
			for(set<string>::iterator it2=words.begin(); it2!=words.end(); it2++){
				if(isPrefix(*it, *it2)){
					noPrefix = true;
				}
			}
			if(!noPrefix){
				testedStates.insert(stringToStates.find(*it)->second);
				if(acceptingStates.count(stringToStates.find(*it)->second) > 0)
					return *it + " $";
			}
				
		}
		
		//search for new strings
		searching = false;
		newStrings.clear();
		newStringToStates.clear();
		for(set<string>::iterator it=strings.begin(); it!=strings.end(); it++){
			string state;
			for(set<string>::iterator it2=alphabet.begin(); it2!=alphabet.end(); it2++){
				state = stringToStates.find(*it)->second;
				pair<string, string> transition = make_pair(state, *it2);
				string newState = transitions.find(transition)->second;
				if(testedStates.count(newState) < 1){
					string temp = *it;
					if(temp.empty()){
						newStrings.insert(*it2);
						newStringToStates.insert(pair<string, string>(*it2, newState));
					}else{
						newStrings.insert(*it + " " + *it2);
						newStringToStates.insert(pair<string, string>(*it + " " + *it2, newState));
					}
					searching = true;
				}
			}
		}
		strings = newStrings;
		stringToStates = newStringToStates;
	}
	return "-1";
}