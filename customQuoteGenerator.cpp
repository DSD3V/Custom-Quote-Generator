#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <ctime>
using namespace std;

void Welcome();
void Main(vector<string> topics, vector<vector<string>> subtopics, unordered_multimap<string, string> quotesMap, vector<string>& prevCustomQuotes, vector<string>& prevRandomQuotes);
void ParseFile(ifstream& infile, string line, vector<string>& topics, vector<vector<string>>& subtopics, unordered_multimap<string, string>& quotesMap);
void SetTopics(string line, string topic, string endfile, vector<string>& topics, vector<vector<string>>& subtopics, vector<string>& tempSubtopics);
void SetSubtopics(string line, string subtopic, vector<string>& tempSubtopics);
void SetQuotes(ifstream& infile, string line, string quote, vector<string>& tempSubtopics, unordered_multimap<string, string>& quotesMap);
bool IsInputValid(string line, int limit, int& inputNum);
void GenerateCustomQuote(string subtopic, unordered_multimap<string, string> quotesMap, vector<string>& prevCustomQuotes);
void GenerateRandomQuote(unordered_multimap<string, string> quotesMap, vector<string>& prevRandomQuotes);
bool CheckForRepeat(vector<string> prevValues, string value);
void PrintAndUpdateQuoteValues(vector<string> temp, vector<string>& quotes);
string FormatQuote(string quote);
void PrintQuote(string quote);

int main() {
	Welcome();

	//Get topics, subtopics, and quotes from file and close file
	vector<string> topics; vector<vector<string>> subtopics; unordered_multimap<string, string> quotesMap;
	ifstream infile("quotes.txt"); string line;
	ParseFile(infile, line, topics, subtopics, quotesMap);
	infile.close();

	srand(time(NULL));
	vector<string> prevCustomQuotes, prevRandomQuotes;
	Main(topics, subtopics, quotesMap, prevCustomQuotes, prevRandomQuotes);
}

void Main(vector<string> topics, vector<vector<string>> subtopics, unordered_multimap<string, string> quotesMap, vector<string>& prevCustomQuotes, vector<string>& prevRandomQuotes) {
	string input; int topicNum, subtopicNum, endNum; unordered_map<int, string> subtopicsMap;

	//Get topic from user
	cout << endl << "Select a topic of interest: (Type the numeric value below)" << endl;

	for (unsigned int j = 1, i = 0; i < topics.size(); i++, j++) {
		cout << endl << j << ": " << topics[i];
	}

	int random = topics.size() + 1, end = topics.size() + 2;

	cout << endl << random << ": " << "Generate a random quote"
		 << endl << end << ": " << "Exit program" << endl
		 << endl << "Enter the numeric value of your choice: ";

	do {
		getline(cin, input);
	} while (!IsInputValid(input, topics.size() + 2, topicNum));

	if (topicNum == random) {
		GenerateRandomQuote(quotesMap, prevRandomQuotes);
		Main(topics, subtopics, quotesMap, prevCustomQuotes, prevRandomQuotes);
	} else if (topicNum == end) {
		cout << endl << "Program terminated. Click \"Run\" to use the Custom Quote Generator again.";
		exit(0);
	}

	//Get subtopic from user
	cout << endl << "Select a subtopic of interest: (Type the numeric value below)" << endl;

	for (unsigned int j = 1, i = 0; i < subtopics[topicNum].size(); i++, j++) {
		subtopicsMap.insert(make_pair(j, subtopics[topicNum][i]));
		cout << endl << j << ": " << subtopics[topicNum][i];
	}

	int goBack = subtopics[topicNum].size() + 1; end = subtopics[topicNum].size() + 2;

	cout << endl << goBack << ": " << "Return to main screen"
		 << endl << end << ": " << "Exit program" << endl
		 << endl << "Enter the numeric value of your choice: ";

	do {
		getline(cin, input);
	} while (!IsInputValid(input, subtopics[topicNum].size() + 2, subtopicNum));

	if (subtopicNum == goBack) {
		Main(topics, subtopics, quotesMap, prevCustomQuotes, prevRandomQuotes);
	} else if (subtopicNum == end) {
		cout << endl << "Program terminated. Click \"Run\" to use the Custom Quote Generator again.";
		exit(0);
	}

	//Generate custom quote and ask user how they want to proceed
	string subtopicName;
	for (auto& subtopic : subtopicsMap) {
		if (subtopic.first == subtopicNum) {
			subtopicName = subtopic.second;
			prevCustomQuotes.clear();
			GenerateCustomQuote(subtopicName, quotesMap, prevCustomQuotes);
		}
	}

	do {
		cout << endl << "How would you like to proceed?" << endl << endl
			 << "1: Generate another " << subtopicName
			 << ((subtopicName == "Artificial Intelligence " || subtopicName == "Robotics ") ? "" : " ") << "quote" << endl //minor hack since subtopic name appears twice
			 << "2: Generate a random quote" << endl
			 << "3: Return to main screen" << endl
			 << "4: Exit program" << endl << endl
			 << "Enter the numeric value of your choice: ";
		do {
			getline(cin, input);
		} while (!IsInputValid(input, 4, endNum));

		switch(endNum) {
			case 1:
				GenerateCustomQuote(subtopicName, quotesMap, prevCustomQuotes);
				break;
			case 2:
				GenerateRandomQuote(quotesMap, prevRandomQuotes);
				break;
			case 3:
				Main(topics, subtopics, quotesMap, prevCustomQuotes, prevRandomQuotes);
				break;
			case 4:
				cout << endl << "Program terminated. Click \"Run\" to use the Custom Quote Generator again.";
				exit(0);
				break;
		}
	} while (true);
}

void Welcome() {
	string welcomeMessage = "Welcome to the Custom Quote Generator!";
	string space = string(10, ' ');
	string stars = string(welcomeMessage.length(), '*');
	string border = string(20 + welcomeMessage.length(), '=');
	cout << border << endl
		 << space << stars << endl
		 << space << welcomeMessage << endl
		 << space << stars << endl
		 << border << endl;
}

void ParseFile(ifstream& infile, string line, vector<string>& topics, vector<vector<string>>& subtopics, unordered_multimap<string, string>& quotesMap) {
	string topic = "MAINTOPIC: ", subtopic = "SUBTOPIC: ", quote = "QUOTE: ", endfile = "ENDFILE"; vector<string> tempSubtopics;
	while (getline(infile, line)) {
    		SetTopics(line, topic, endfile, topics, subtopics, tempSubtopics);
    		SetSubtopics(line, subtopic, tempSubtopics);
    		SetQuotes(infile, line, quote, tempSubtopics, quotesMap);
    	}
}

void SetTopics(string line, string topic, string endfile, vector<string>& topics, vector<vector<string>>& subtopics, vector<string>& tempSubtopics) {
	if (line.find(topic) != string::npos || line.find(endfile) != string::npos) {
		if (line.find(topic) != string::npos) {
			topics.push_back(line.substr(topic.length()));
		}
		subtopics.push_back(tempSubtopics);
		tempSubtopics.clear();
	}
}

void SetSubtopics(string line, string subtopic, vector<string>& tempSubtopics) {
	if (line.find(subtopic) != string::npos) {
		tempSubtopics.push_back(line.substr(subtopic.length()));
	}
}

void SetQuotes(ifstream& infile, string line, string quote, vector<string>& tempSubtopics, unordered_multimap<string, string>& quotesMap) {
	if (line.find(quote) != string::npos) {
		string tempQuote = line.substr(quote.length());
		if (tempQuote.back() != '"') {
			while (getline(infile, line) && line.size()) {
				tempQuote = tempQuote + "\n" + line;
			}
		} else {
			while (getline(infile, line) && line.size()) {
				tempQuote = tempQuote + "\n" + line;
			}
		}
		quotesMap.insert(make_pair(tempSubtopics[tempSubtopics.size() - 1], tempQuote));
	}
}

bool IsInputValid(string input, int limit, int& inputNum) {
	try {
		if (!input.length()) throw -1;
		for (unsigned int i = 0; i < input.length(); i++) {
			if ((int) input[i] < 48 || (int) input[i] > 57) throw -1;
		}
		if (stoi(input) > 0 && stoi(input) <= limit) {
			inputNum = stoi(input);
			return true;
		} else throw -1;
	} catch(int e) {
		cout << endl << "Invalid input. Please enter the numeric value " << "(1-" << limit << ")" << " corresponding to an option: ";
		return false;
	}
}

void GenerateCustomQuote(string subtopic, unordered_multimap<string, string> quotesMap, vector<string>& prevCustomQuotes) {
	vector<string> temp;
	auto quotes = quotesMap.equal_range(subtopic);
	for (auto itr = quotes.first; itr != quotes.second; itr++) {
		if (!CheckForRepeat(prevCustomQuotes, itr->second)) {
			temp.push_back(itr->second);
		}
	}
	PrintAndUpdateQuoteValues(temp, prevCustomQuotes);
}

void GenerateRandomQuote(unordered_multimap<string, string> quotesMap, vector<string>& prevRandomQuotes) {
	vector<string> temp;
	for (auto itr = quotesMap.begin(); itr != quotesMap.end(); itr++) {
		if (!CheckForRepeat(prevRandomQuotes, itr->second)) {
			temp.push_back(itr->second);
		}
	}
	PrintAndUpdateQuoteValues(temp, prevRandomQuotes);
}

bool CheckForRepeat(vector<string> prevValues, string value) {
	if (prevValues.size()) {
		for (auto& prevValue : prevValues) {
			if (prevValue == value) {
				return true;
			}
		}
	}
	return false;
}

void PrintAndUpdateQuoteValues(vector<string> temp, vector<string>& quotes) {
	int randomIndex = rand() % temp.size();
	quotes.push_back(temp[randomIndex]);
	PrintQuote(FormatQuote(temp[randomIndex]));
	if (temp.size() == 1) quotes.clear();
}

string FormatQuote(string quote) {
	bool source = false;
	for (unsigned int charIndex = 0, afterNewline = 0; charIndex < quote.length(); charIndex++, afterNewline++) {
		if (quote[charIndex] == '\n') {
			afterNewline = 0;
		}
    		if (quote[charIndex] == '"' && charIndex != 0) {
			quote.insert(charIndex + 1, "\n");
	    		afterNewline = 0;
	    		source = true;
    		}
		if (afterNewline % 50 == 0 && afterNewline != 0) {
			int tempCharIndex = charIndex;
			while (quote[tempCharIndex] != ' ') {
				if (quote[tempCharIndex] == '\n' || quote[tempCharIndex] == '\0') break;
				tempCharIndex++;
			}
			if (quote[tempCharIndex] != '\n' && quote[tempCharIndex] != '\0') {
				if (source) quote.insert(tempCharIndex + 1, " ");
				quote.insert(tempCharIndex, "\n");
			}
	 	}
	}
	return quote;
}

void PrintQuote(string quote) {
	string border = string(65, '-');
	cout << endl << endl << endl << endl << endl
		<< border << endl
		<< quote << endl
		<< border << endl;
}
