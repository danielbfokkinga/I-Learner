#include <set>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include "dfa.cc"

using namespace std;

const string EXECUTABLE = "simple" //replace with name of compiled executable

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
       	
//returns x*y
string concatenate(string x, string y){
	if(x == "/")
		return y;
	else if(y == "/")
		return x;
	else return x+y;
}

class Angluin{
	public:
		Angluin();
		void printTable();
		bool member(string queryString);
		bool closed();
		bool consistent();
		void run();
		
	private:
		bool sameRowInSA(string stringS);
		bool sameRowForAllA(string s_1, string s_2);
		Dfa proposeM();
		string row(string x);
		string findAE(string s_1, string s_2);
		string differentRow(string s1);
		Language language;
		set<string> members;
		set<string> membersNoSpace;
		set<string> a;
		set<string> s;
		set<string> e;
		map<string, int> t;
};

//initializes (s,e,t) with s=e={/}, a=alphabet.txt
Angluin::Angluin(){
	string removeFile = "rm tmp.txt";
	system(removeFile.c_str());
	language.searchStrings(20, 4, EXECUTABLE, "tmp.txt");
	language.fillSet("tmp.txt", members); 
	membersNoSpace = language.fillSetNoSpace(members);
	
	//fill alphabet with symbols from alphabet.txt
	string filenameAlphabet = "alphabet.txt";
	ifstream input(filenameAlphabet.c_str());
	for(string line; getline(input, line);){
		a.insert("Call(" + line + ")");
		a.insert("Ret(" + line + ")");
	}
	
	s.insert("/");
	e.insert("/");
	
	if(member("/"))
		t.insert(pair<string, int>("/", 1));
	else
		t.insert(pair<string, int>("/", 0));
	
	for(set<string>::iterator it=a.begin(); it!=a.end(); it++){
		if(member(*it))
			t.insert(pair<string, int>(*it, 1));
		else
			t.insert(pair<string, int>(*it, 0));
		
	}
}

//returns true if s_1 and s_2 in S with row(s_1)=row(s_1) then for all A row(s_1*a)=row(s_2*a)
bool Angluin::consistent(){
	string rowS_1, rowS_2;
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		for(set<string>::iterator it2=s.begin(); it2!=s.end(); it2++){
			rowS_1 = row(*it);
			rowS_2 = row(*it2);
			if(rowS_1 == rowS_2 && *it != *it2){
				if(sameRowForAllA(*it, *it2))
					continue;
				else
					return false;
			}
		}
	}
	return true;
}

//returns true if for s_1 and s_2, row(s_1*a)=row(s_2*a) for all a in A
bool Angluin::sameRowForAllA(string s_1, string s_2){
	string rowS_1, rowS_2;
	for(set<string>::iterator it=a.begin(); it!=a.end(); it++){;
		rowS_1 = row(concatenate(s_1, *it));
		rowS_2 = row(concatenate(s_2, *it));
		if(rowS_1 == rowS_2){
				continue;
			}else
				return false;
	}
	return true;
}

//returns true if for each t in s*a, there exists an s in s s.t. t(t)=t(s)
bool Angluin::closed(){
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		for(set<string>::iterator it2=a.begin(); it2!=a.end(); it2++){
			if(sameRowInSA(concatenate(*it, *it2))){
				continue;
			}else 
				return false;
		}
	}
	return true;
}

//returns true if there exists a row in S that is equal to row(stringS)
bool Angluin::sameRowInSA(string stringS){
	bool found;
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		found = true;
		for(set<string>::iterator it2=e.begin(); it2!=e.end(); it2++){
			if(t.find(concatenate(stringS, *it2))->second != t.find(concatenate(*it, *it2))->second){
				found = false;
				break;
			}
		}
		if(found)
			return true;
	}
	return false;
}

//returns the row of x in the observation table
string Angluin::row(string x){
	string rowX;
	for(set<string>::iterator it=e.begin(); it!=e.end(); it++){
		rowX = rowX+SSTR(t.find(concatenate(x, *it))->second);
	}
	return rowX;
}

//returns true if queryString is accepted by the unknown DFA, else false
bool Angluin::member(string queryString){
	for(set<string>::iterator it=membersNoSpace.begin(); it!=membersNoSpace.end(); it++){
		if(queryString == *it)
			return true;
	}
	//search for more strings and check again
	return false;
}

/*//returns true if queryString is accepted by the unknown DFA, else false
bool Angluin::member(string queryString){
	string input;
	cout << "Is " << queryString << " accepted by the DFA? (y/n)" << endl;
	cin >> input;
	if(input == "y"){
		cout << "You answered yes" << endl;
		return true;
	}else{
		cout << "You answered no" << endl;
		return false;
	}
}*/

//finds a new ae string for s_1, s_2 in S, a in A and e in E, s.t. T(s_1*a*e) != T(s_2*a*e)
string Angluin::findAE(string s_1, string s_2){
	for(set<string>::iterator it=a.begin(); it!=a.end(); it++){
		for(set<string>::iterator it2=e.begin(); it2!=e.end(); it2++){
			if(t.find(concatenate(concatenate(s_1, *it), *it2))->second != t.find(concatenate(concatenate(s_2, *it), *it2))->second){
				return concatenate(*it, *it2);
			}
		}
	}
	return "-1";
}

//finds an a in A s.t. row(s1*a) is different from all row(s) for s in S, returns -1 if such an a cannot be found
string Angluin::differentRow(string s1){
	bool found;
	for(set<string>::iterator it=a.begin(); it!=a.end(); it++){
		for(set<string>::iterator it2=s.begin(); it2!=s.end(); it2++){
			if(row(concatenate(s1, *it)) == row(*it2)){
				found = false;
				break;
			}else{
				found = true;
			}
		}
		if(found)
			return concatenate(s1, *it);
		else
			continue;
	}
	return "-1";
}

//constructs a Dfa, and prints it, based on the current observation table
Dfa Angluin::proposeM(){
	Dfa dfa(row("/"), (t.find("/")->second != 0));
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		if(dfa.states.count(row(*it)) < 1){
			if(t.find(*it)-> second == 1)
				dfa.addState(row(*it), true);
			else
				dfa.addState(row(*it), false);
		}
	}
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		for(set<string>::iterator it2=a.begin(); it2!=a.end(); it2++){
			pair<string, string> transition = make_pair(row(*it), *it2);
			if(dfa.transitions.count(transition) < 1)
				dfa.addTransition(row(*it), *it2, row(concatenate(*it, *it2)));
		}
	}
	dfa.printDFA(a);
	return dfa;
}

//runs the L* algorithm from Dana Angluin
void Angluin::run(){
	again:
	while(!consistent() || !closed()){
		if(!consistent()){
			//printTable();
			cout << "not consistent" << endl;
			string rowS_1, rowS_2, ae;
			for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
				for(set<string>::iterator it2=s.begin(); it2!=s.end(); it2++){
					rowS_1 = row(*it);
					rowS_2 = row(*it2);
					if(rowS_1 == rowS_2 && *it != *it2){
						ae = findAE(*it, *it2);
						if(ae != "-1"){
							goto found;
						}else
							continue;
					}
				}
			}found:
			e.insert(ae);
			cout << "added " << ae << " to E" << endl;
			for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
				if(t.count(concatenate(*it, ae)) < 1){
					t.insert(pair<string, int>(concatenate(*it, ae), member(concatenate(*it, ae))));
				}
				for(set<string>::iterator it2=a.begin(); it2!=a.end(); it2++){
					if(t.count(concatenate(concatenate(*it, *it2), ae)) < 1){
						t.insert(pair<string, int>(concatenate(concatenate(*it, *it2), ae), member(concatenate(concatenate(*it, *it2), ae))));
					}
				}
			}
		}
		if(!closed()){
			//printTable();
			cout << "not closed" << endl;
			string s1a;
			for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
				s1a = differentRow(*it);
				if(s1a != "-1")
					break;
			}
			s.insert(s1a);
			cout << "added " << s1a << " to S" << endl;
			for(set<string>::iterator it=a.begin(); it!=a.end(); it++){
				t.insert(pair<string, int>(concatenate(s1a, *it), member(concatenate(s1a, *it))));
			}
		}
	} 
	
	//conjecture
	string counterexample;
	cout << endl << "Proposed DFA: " << endl;
	Dfa dfa = proposeM();
	set<string> counterexamples;
	counterexamples = dfa.checkLanguage(members);
	string counterexample2 = dfa.checkLanguage2(members, a);
	if(!counterexamples.empty()){
		string shortest = *counterexamples.begin();
		string temp;
		for(set<string>::iterator it=counterexamples.begin(); it!=counterexamples.end(); it++){
			temp = *it;
			if(temp.length() < shortest.length())
				shortest = temp;
		}
		counterexample = shortest;
	}else if(counterexample2 != "-1"){
		counterexample = counterexample2;
	}else{
		cout << "Done" << endl;
		counterexample = "-1";
	}
	if(counterexample != "-1"){
		string next, prefix;
		cout << counterexample << " given as counterexample" << endl;
		next = getSymbol(counterexample);
		prefix = next;
		while(next != "$"){
			cout << "Current prefix: " << prefix << endl;
			if(s.count(prefix) < 1){
				s.insert(prefix);
				cout << "added " << prefix << " to S" << endl;
				for(set<string>::iterator it=e.begin(); it!=e.end(); it++){
					if(t.count(concatenate(prefix, *it)) < 1){
						t.insert(pair<string, int>(concatenate(prefix, *it), member(concatenate(prefix, *it))));
					}
				}
			}
			for(set<string>::iterator it=a.begin(); it!=a.end(); it++){
				for(set<string>::iterator it2=e.begin(); it2!=e.end(); it2++){
					if(t.count(concatenate(concatenate(prefix, *it), *it2)) < 1){
						t.insert(pair<string, int>(concatenate(concatenate(prefix, *it), *it2), member(concatenate(concatenate(prefix, *it), *it2))));
					}
				}
			}
			next = getSymbol(counterexample);
			prefix = prefix+next;
		}
		goto again; //check if consistent and closed again...
	}
}

//prints the observation table in the form of a 2d array
void Angluin::printTable(){	
	cout << "T\t";
	for(set<string>::iterator it=e.begin(); it!=e.end(); it++){
		cout << *it << "\t";
	}
	cout << endl;
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		cout << *it << "\t";
		for(set<string>::iterator it2=e.begin(); it2!=e.end(); it2++){
			cout << t.find(concatenate(*it, *it2))->second << "\t";
		}
		cout << endl;
	}
	cout << endl;
	for(set<string>::iterator it=s.begin(); it!=s.end(); it++){
		for(set<string>::iterator it2=a.begin(); it2!=a.end(); it2++){
			string currentString = concatenate(*it, *it2);
			if(s.count(currentString) > 0)
				continue; //to prevent printing S followed by /
			cout << currentString << "\t";
			for(set<string>::iterator it3=e.begin(); it3!=e.end(); it3++){
				cout << t.find(concatenate(currentString, *it3))->second << "\t";
			}
			cout << endl;
		}
	}
	cout << endl;
}

int main(){
	Angluin angluin;
	angluin.run();
	return 0;
}