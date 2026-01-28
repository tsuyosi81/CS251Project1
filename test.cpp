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
void rot(vector<string>& strings, int amount) {
  for (string& s : strings) {      // Iterate through each string
    string decrypted;              // Declare decrypted string
    for (char c : s) {             // Iterate through each character
      if (isalpha(c)) {            // Check if the character is alphabet
        char upperC = toupper(c);  // Convert to uppercase
        char rotatedChar = rot(upperC, -amount);  // Rotate by negative amount
        decrypted += rotatedChar;  // Append rotated character to decrypted
      } else if (isspace(c)) {     // If not alphabet/space
        decrypted += c;            // Append original character to decrypted
      }
    }
    s = decrypted;  // Update the original string with decrypted version
  }
}

string clean(const string& s) {
  string cleaned = "";        // declare cleaned string
  for (char c : s) {          // iterate through each character
    if (isalpha(c)) {         // check if character is alphabet
      cleaned += toupper(c);  // convert to uppercase and append to cleaned
    }
  }
  return cleaned;
}

vector<string> splitBySpaces(
    const string&
        s) {  // this function splits a string into words based on spaces
  vector<string> words;

  // iterate through the string
  istringstream stream(s);
  string word;
  while (stream >> word) {
    words.push_back(word);  // add each word to the vector
  }
  return words;
}

int main() {
  vector<string> dictionary = loadDDictionary("dictionary.txt");

  string s = "cat123";
  char c = ALPHABET[2];  // character 'C'
  int amount = -4;       // Expected: 'C' -> 'Y'

  char character = rot(c, amount);
  string sentence = rot(s, amount);  // expected "cat" -> "YWP"

  cout << "Rotating amount: " << amount << endl;
  cout << "Character before rotating: " << c << endl;
  cout << "Character after rotating: " << character << endl;

  cout << "String before rotating: " << s << endl;
  cout << "String after rotating: " << sentence << endl;

  // decrypt example

  cout << "Decrypting example:" << endl;
  vector<string> testStrings = {"YWP"};
  rot(testStrings, -4);
  cout << "Decrypted: " << testStrings[0] << endl;

  // clean example
  string dirty = "Hello, World! 123";
  string cleaned = clean(dirty);
  cout << "Dirty string: " << dirty << endl;
  cout << "Cleaned string: " << cleaned << endl;

  // splitBySpaces example
  string phrase = "This is a test";
  vector<string> words = splitBySpaces(phrase);
  cout << "Original phrase: " << phrase << endl;
  cout << "Split words:" << endl;
  for (const string& word : words) {
    cout << word << endl;
  }

  caesarEncryptCommand();

  return 0;
}