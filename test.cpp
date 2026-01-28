#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";  // 26 characters

vector<string> loadDDictionary(const string& filename) {
  vector<string> dictionary;
  ifstream file(filename);
  string word;
  if (file.is_open()) {
    while (getline(file, word)) {
      dictionary.push_back(word);
    }
    file.close();
  } else {
    cerr << "Error: Could not open file " << filename << endl;
  }
  return dictionary;
}

char rot(char c, int amount) {
  // Find the index of the character (0-25: there are 26 characters in total)
  int charIdx = ALPHABET.find(c);

  // Add the rotation amount to the index: charIdx + amount
  // Use % (modulo operator) to to wrap the index number to be in the range of
  // 0-25.

  int newIdx = (charIdx + amount) % 26;

  // if the rotation amount/new index is negative, add 26

  if (newIdx < 0) {
    newIdx += 26;
  }

  // find the new character of the new index
  char newChar = ALPHABET[newIdx];

  return newChar;
}

string rot(const string& line, int amount) {
  // TODO: student

  string result;  // declare enc result

  // Iterate through string
  for (char c : line) {
    // Check if the character is alphabet (isalpha)
    if (isalpha(c)) {
      // Convert the character into uppercase (toupper)
      char upperC = toupper(c);
      // Rotate the character by calling rot(char, amount)
      char rotatedChar = rot(upperC, amount);
      // Append the rotated character to result
      result += rotatedChar;
    } else if (isspace(c)) {
      // If not alphabet/space, append the original character to result
      result += c;
    }
  }
  return result;
}

void caesarEncryptCommand() {
  string line;
  int amount;
  string result;

  cout << "Enter the text to encrypt:" << endl;
  getline(cin, line);

  cout << "Enter the number of characters to rotate by:" << endl;
  cin >> amount;

  result =
      rot(line, amount);  // Call rot(string, amount) to get encrypted result

  cout << "Encrypted result: " << result << endl;
}

// #MILESTONE 2

#pragma region CaesarDec

// Decrypt functions
// Decrypt functions
void rot(vector<string>& strings, int amount) {
  for (string& s : strings) {  // iterate through each string
    s = rot(s, amount);  // decrypt each string calling rot(string, amount)
  }
}

string clean(
    const string& s) {  // This function cleans a string by removing
                        // non-alphabetic characters and converting to uppercase
  string cleaned = "";  // declare cleaned string
  for (char c : s) {    // iterate through each character
    if (isalpha(c)) {   // check if character is alphabet
      cleaned += toupper(c);  // convert to uppercase and append to cleaned
    }
  }
  return cleaned;
}

vector<string> splitBySpaces(
    const string&
        s) {  // this function splits a string into words based on spaces
  vector<string> words;  // declare vector to hold words

  // iterate through the string
  istringstream stream(s);  // create input string stream
  string word;              // declare variable to hold each word
  while (stream >> word) {  // while there are words to read
    words.push_back(word);  // add each word to the vector
  }
  return words;
}

string joinWithSpaces(
    const vector<string>&
        words) {  // joins a vector of words into a single string with spaces
  string result;
  for (int i = 0; i < words.size(); i++) {  // iterate through each word
    result += words[i];                     // add each word to the result
    if (i < words.size() - 1) {             // if not the last word
      result += " ";                        // add a space between words
    }
  }
  return result;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for (const string& w : words) {   // iterate through each word in words
    for (const string& d : dict) {  // iterate through each word in dictionary
      if (w == d) {                 // if word matches dictionary word
        count++;                    // increment count
        break;
      }
    }
  }
  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  int validCount = 0;
  bool found = false;
  bool meetsThreshold = false;
  string line;
  vector<string> originalWords, candidateWords, cleaned;

  // User Input
  cout << "Enter the text to Caesar decrypt:" << endl;
  getline(cin, line);

  // Process Input
  originalWords = splitBySpaces(line);

  // Brute Force Loop (Checking all 26 rotations)
  for (int i = 0; i < ALPHABET.length(); i++) {
    candidateWords = originalWords;  // make a copy of original words so that we
                                     // can rotate them separately.

    rot(candidateWords, i);  // Decrypt copied words with rotation i

    // Prepare a clean version of the words for dictionary checking
    cleaned.clear();  // call clear to empty the cleaned vector
    for (const string& w :
         candidateWords) {  // iterate through each word from candidateWords
                            // (copied & rotated)
      cleaned.push_back(clean(w));  // clean each word and add to cleaned vector
    }

    validCount =
        numWordsIn(cleaned, dict);  // Count how many words match the dictionary

    // Threshold Logic (Deciding if the decryption is in the dictionary)
    meetsThreshold = false;
    if (cleaned.size() <= 2) {
      // For short messages, require 100% match
      meetsThreshold = (validCount == (int)cleaned.size() && validCount > 0);
    } else {
      // For longer messages, 50% match is sufficient
      meetsThreshold = (validCount >= (int)cleaned.size() / 2.0);
    }

    // 6. Output Result
    if (meetsThreshold) {
      cout << joinWithSpaces(candidateWords) << endl;
      found = true;
    }
  }

  // 7. Final check if nothing was found
  if (!found) {
    cout << "No good decryptions found" << endl;
  }
}

int main() {
  vector<string> dictionary = loadDDictionary("dictionary.txt");

  string s = "cat123";
  char c = ALPHABET[2];               // character 'C'
  int amount = -4;                    // Expected: 'C' -> 'Y'
  char encChar = rot(c, amount);      // encrypted 'C' -> 'Y'
  string encString = rot(s, amount);  // expected "cat" -> "YWP"

  cout << "FUNCTION: rot() for char and string encrypt::" << endl << endl;
  cout << "Rotating amount: " << amount << endl;
  cout << "Character before rotating: " << c << endl;
  cout << "Character after rotating: " << encChar << endl;
  cout << "String before rotating: " << s << endl;
  cout << "String after rotating: " << encString << endl;
  cout << endl;

  cout << "FUNCTION: rot() for char and string decrypt::" << endl << endl;
  char decChar = rot(encChar, -amount);
  string decString = rot(encString, -amount);
  cout << "Rotating amount: " << -amount << endl;
  cout << "Character before rotating: " << encChar << endl;
  cout << "Character after rotating: " << decChar << endl;
  cout << "String before rotating: " << encString << endl;
  cout << "String after rotating: " << decString << endl;
  cout << endl;

  // clean example
  cout << "FUNCTION: clean() example::" << endl << endl;
  string dirty = "Hello, World! 123";
  string cleaned = clean(dirty);
  cout << "Dirty string: " << dirty << endl;
  cout << "Cleaned string: " << cleaned << endl;
  cout << endl;

  // splitBySpaces example
  cout << "FUNCTION: splitBySpaces() example::" << endl << endl;
  string phrase = "This is a test";
  vector<string> words = splitBySpaces(phrase);
  cout << "Original phrase: " << phrase << endl;
  cout << "Split words:" << endl;
  for (const string& word : words) {
    cout << word << endl;
  }
  cout << endl;

  // joinWithSpaces example
  cout << "FUNCTION: joinWithSpaces() example::" << endl << endl;
  cout << "Words to join:" << endl;
  for (const string& word : words) {
    cout << word << endl;
  }
  string joined = joinWithSpaces(words);
  cout << "Joined phrase: " << joined << endl;
  cout << endl;

  // numWordsIn example
  cout << "FUNCTION: numWordsIn() example::" << endl << endl;
  vector<string> sampleWords = {"HELLO", "WORLD", "FOO", "BAR"};
  vector<string> sampleDict = {"HELLO", "WORLD", "TEST"};
  cout << "Words to check:" << endl;
  for (const string& word : sampleWords) {
    cout << word << endl;
  }
  cout << "Dictionary words:" << endl;
  for (const string& word : sampleDict) {
    cout << word << endl;
  }
  int count = numWordsIn(sampleWords, sampleDict);
  cout << "Number of valid dictionary words: " << count << endl;
  cout << endl;

  caesarEncryptCommand();
  caesarDecryptCommand(dictionary);

  return 0;
}