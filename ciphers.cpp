#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!
vector<string> loadDictionary(const string& filename) {
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

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  vector<string> dictionary = loadDictionary("dictionary.txt");

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      caesarEncryptCommand();
    } else if (command == "D" || command == "d") {
      caesarDecryptCommand(dictionary);
    }  // else if (command == "A" || command == "a") {
    //   applyRandSubstCipherCommand();
    // } else if (command == "E" || command == "e") {
    //   computeEnglishnessCommand(scorer);
    // } else if (command == "S" || command == "s") {
    //   decryptSubstCipherCommand(scorer);
    // } else if (command == "F" || command == "f") {
    //   decryptSubstCipherFileCommand(scorer);
    // }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

// load dictionary.txt into vector<string> dictionary

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  int charIdx = ALPHABET.find(c);  // Find the index of the character (0-25)
  int newIdx = (charIdx + amount) %
               26;  // Add the rotation amount to the index: charIdx + amount

  if (newIdx < 0) {  // if the rotation amount/new index is negative, add 26
    newIdx += 26;
  }

  char newChar = ALPHABET[newIdx];  // assign new character of the new index

  return newChar;
}

string rot(const string& line, int amount) {
  string result;  // declare encrypted result

  for (char c : line) {  // Iterate through string

    if (isalpha(c)) {  // Check if the character is alphabet (isalpha)
      char upperC =
          toupper(c);  // Convert the character into uppercase (toupper)
      char rotatedChar = rot(
          upperC, amount);  // Rotate the character by calling rot(char, amount)
      result += rotatedChar;  // Append the rotated character to result
    } else if (isspace(c)) {  // If not alphabet/space,
      result += c;            // append the original character to result
    }
  }
  return result;
}

void caesarEncryptCommand() {
  string line;
  string result;
  int amount;

  cout << "Enter the text to encrypt:" << endl;
  getline(cin, line);

  cout << "Enter the number of characters to rotate by:" << endl;
  cin >> amount;

  result =
      rot(line, amount);  // Call rot(string, amount) to get encrypted result

  cout << "Encrypted result: " << result << endl;
}

#pragma endregion CaesarEnc

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
  string line;
  int amount;

  cout << "Enter the text to decrypt:" << endl;
  getline(cin, line);

  cout << "Enter the number of characters to rotate by:" << endl;
  cin >> amount;

  vector<string> words = splitBySpaces(line);
  rot(words, amount);  // decrypt the words

  string decryptedLine = joinWithSpaces(words);
  cout << "Decrypted result: " << decryptedLine << endl;

  // Optional: Count valid words in decrypted text
  vector<string> cleanedWords;
  for (const string& w : words) {
    cleanedWords.push_back(clean(w));
  }
  int validWordCount = numWordsIn(cleanedWords, dict);
  cout << "Number of valid dictionary words: " << validWordCount << endl;
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  // TODO: student
  return "";
}

void applyRandSubstCipherCommand() {
  // TODO: student
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student
  return 0.0;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student
}

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  // TODO: student
  return vector<char>{};
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // TODO: student
  return vector<char>{};
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student
}

#pragma endregion SubstDec
