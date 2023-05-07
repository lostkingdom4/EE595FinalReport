#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdio.h> 
using namespace std;

class CodeComparisonMethod {
public:
    virtual double compare(ifstream &file1, ifstream &file2) = 0;
};

class LineByLineComparison : public CodeComparisonMethod {
public:
    double compare(ifstream &file1, ifstream &file2){
        string line1, line2;
        int totalLines = 0;
        int matchingLines = 0;

        while ( getline(file1, line1) &&  getline(file2, line2)) {
            string normalizedLine1 = normalize(line1);
            string normalizedLine2 = normalize(line2);

            if (normalizedLine1.empty() && normalizedLine2.empty()) {
                continue; // Skip empty lines
            }

            totalLines++;

            if (normalizedLine1 == normalizedLine2) {
                matchingLines++;
            }
        }
        return static_cast<double>(matchingLines) / totalLines * 100;
    }

private:
    string normalize(const string &line) {
        // Remove comments from the line
        string normalizedLine =  regex_replace(line,  regex("//.*"), ""); 
        // Remove extra whitespaces and condense the line
        normalizedLine =  regex_replace(normalizedLine,  regex("\\s+"), " "); 

        // find variable name
        // define variable name pattern
        regex variableRegex("([a-zA-Z_]\\w*)");
        // Iterator to find all matching pattern
        sregex_iterator it(normalizedLine.begin(), normalizedLine.end(), variableRegex);
        sregex_iterator it_end;

        // store variable name
        map< string,  string> variableMap;
        int variableCounter = 0;

        // Iterate through the variable names and create standardized names for each unique variable
        for (; it != it_end; ++it) {
            
            string variableName = it->str();
            // If does not find the same variable, create a new one
            if (variableMap.find(variableName) == variableMap.end()) {
                string standardizedVariableName = "v" +  to_string(variableCounter++);
                variableMap[variableName] = standardizedVariableName;
            }
        }
        // replace variables
        for (const auto &var : variableMap) {
            normalizedLine =  regex_replace(normalizedLine,  regex("\\b" + var.first + "\\b"), var.second);
        }

        return normalizedLine;
    }
};

class GlobalComparison : public CodeComparisonMethod {
public:
    double compare(ifstream &file1, ifstream &file2){
        vector<string> normalizedFile1 = normalize(file1);
        vector<string> normalizedFile2 = normalize(file2);

        // put lines from file 1 to hashmap
        unordered_map<string, int> lineOccurrences;
        for (const string &line : normalizedFile1) {
            lineOccurrences[line]++;
        }

        // iterate file2, check if the line in the hashmap
        int matchingLines = 0;
        for (const string &line : normalizedFile2) {
            if (lineOccurrences[line] > 0) {
                matchingLines++;
                lineOccurrences[line]--;
            }
        }

        int totalLines = normalizedFile1.size() + normalizedFile2.size();
        return static_cast<double>(2 * matchingLines) / totalLines * 100;
    }

private:
    vector<string> normalize(ifstream &file) {
        vector<string> normalizedLines;
        string line;
        map<string, string> variableMap;
        int variableCounter = 0;

        while (getline(file, line)) {
            // Remove comments from the line
            string normalizedLine = regex_replace(line, regex("//.*"), ""); 
            // Remove extra whitespaces and condense the line
            normalizedLine = regex_replace(normalizedLine, regex("\\s+"), " "); 

            // Find variable name
            regex variableRegex("([a-zA-Z_]\\w*)");
            sregex_iterator it(normalizedLine.begin(), normalizedLine.end(), variableRegex);
            sregex_iterator it_end;

            for (; it != it_end; ++it) {
                string variableName = it->str();
                if (variableMap.find(variableName) == variableMap.end()) {
                    string standardizedVariableName = "v" + to_string(variableCounter++);
                    variableMap[variableName] = standardizedVariableName;
                }
            }

            // Replace variables
            for (const auto &var : variableMap) {
                normalizedLine = regex_replace(normalizedLine, regex("\\b" + var.first + "\\b"), var.second);
            }

            if (!normalizedLine.empty()) {
                normalizedLines.push_back(normalizedLine);
            }
        }

        return normalizedLines;
    }
};

class UpdatedLineByLine : public CodeComparisonMethod {
public:
    double compare(ifstream &file1, ifstream &file2){
        vector<string> normalizedFile1 = normalize(file1);
        vector<string> normalizedFile2 = normalize(file2);

        // put lines from file 1 to hashmap
        unordered_map<string, int> lineOccurrences;
        for (const string &line : normalizedFile1) {
            lineOccurrences[line]++;
        }

        // iterate file2, check if the line in the hashmap
        int matchingLines = 0;
        for (const string &line : normalizedFile2) {
            if (lineOccurrences[line] > 0) {
                matchingLines++;
                lineOccurrences[line]--;
            }
        }

        int totalLines = normalizedFile1.size() + normalizedFile2.size();
        return static_cast<double>(2 * matchingLines) / totalLines * 100;
    }

private:
    vector<string> normalize(ifstream &file) {
        vector<string> normalizedLines;
        string line;

        while (getline(file, line)) {
            // Remove comments from the line
            string normalizedLine = regex_replace(line, regex("//.*"), ""); 
            // Remove extra whitespaces and condense the line
            normalizedLine = regex_replace(normalizedLine, regex("\\s+"), " "); 

            // Find variable name
            regex variableRegex("([a-zA-Z_]\\w*)");
            sregex_iterator it(normalizedLine.begin(), normalizedLine.end(), variableRegex);
            sregex_iterator it_end;

            // Store variable name
            map<string, string> variableMap;
            int variableCounter = 0;

            for (; it != it_end; ++it) {
                string variableName = it->str();
                if (variableMap.find(variableName) == variableMap.end()) {
                    string standardizedVariableName = "v" + to_string(variableCounter++);
                    variableMap[variableName] = standardizedVariableName;
                }
            }

            // Replace variables
            for (const auto &var : variableMap) {
                normalizedLine = regex_replace(normalizedLine, regex("\\b" + var.first + "\\b"), var.second);
            }

            if (!normalizedLine.empty()) {
                normalizedLines.push_back(normalizedLine);
            }
        }

        return normalizedLines;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <method> <file1> <file2>\n";
        cerr << "Methods: 1 (Line by Line), 2 (Gloabl comparsion), 3 (Improved Line by Line)" << endl;
        return 1;
    }

    int method = stoi(argv[1]);

    ifstream file1(argv[2]);
    ifstream file2(argv[3]);

    if (!file1.is_open() || !file2.is_open()) {
        cerr << "Error: Unable to open one or both of the files." << endl;
        return 1;
    }

    double similarityScore = 0;

    switch (method) {
        case 1: {
            LineByLineComparison comparisonMethod;
            similarityScore = comparisonMethod.compare(file1, file2);
            cout << "Similarity score (Line by Line): " << similarityScore << "%" << endl;
            break;
        }
        case 2: {
            GlobalComparison comparisonMethod;
            similarityScore = comparisonMethod.compare(file1, file2);
            cout << "Similarity score (Gloabl comparsion): " << similarityScore << "%" << endl;
            break;
        }
        case 3: {
            UpdatedLineByLine comparisonMethod;
            similarityScore = comparisonMethod.compare(file1, file2);
            cout << "Similarity score (Improved Line by Line): " << similarityScore << "%" << endl;
            break;
        }
        default: {
            cerr << "Invalid method. Please select a valid method (1, 2, or 3)." << endl;
            return 1;
        }
    }

    file1.close();
    file2.close();

    return 0;
}
