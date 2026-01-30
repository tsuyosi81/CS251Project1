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

// Load dictionary.txt into vector<string> dictionary
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

// Load quadgrams from file
pair<vector<string>, vector<int>> loadQuadgrams(const string& filename) {
  vector<string> quadgrams;
  vector<int> counts;
  ifstream file(filename);
  string line;
  while (getline(file, line)) {
    size_t comma = line.find(',');
    if (comma != string::npos) {
      string quad = line.substr(0, comma);
      int count = stoi(line.substr(comma + 1));
      quadgrams.push_back(quad);
      counts.push_back(count);
    }
  }
  return {quadgrams, counts};
}

// Function declarations for substitution cipher decryption
void decryptSubstCipherFromFileCommand(const QuadgramScorer& scorer);

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  // Load dictionary
  vector<string> dictionary = loadDictionary("dictionary.txt");

  // Load quadgrams score data
  auto [quadgrams, counts] = loadQuadgrams("english_quadgrams.txt");
  QuadgramScorer scorer(quadgrams, counts);

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
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    } else if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    } else if (command == "F" || command == "f") {
      decryptSubstCipherFromFileCommand(scorer);
    }

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
    meetsThreshold =
        false;  // If the attempt passed the quality check, it prints the
                // readable version and sets found to true
    if (cleaned.size() <=
        2) {  // if the cleaned size is less than or equal to 2
      // For short messages, require 100% match
      meetsThreshold =
          (validCount == (int)cleaned.size() &&
           validCount > 0);  // set meetsThreshold to true if validCount equals
                             // cleaned size and validCount is greater than 0
    } else {
      // For longer messages, 50% match is sufficient
      meetsThreshold =
          (validCount >=
           (int)cleaned.size() /
               2.0);  // set meetsThreshold to true if validCount is greater
                      // than or equal to half of cleaned size
    }

    // 6. Output Result
    if (meetsThreshold) {
      cout << joinWithSpaces(candidateWords)
           << endl;  // print the joined candidate words
      found = true;
    }
  }

  // 7. Final check if nothing was found
  if (!found) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;  // declare result string

  for (char c : s) {  // iterate through each character in the string

    if (isalpha(c)) {  // check if the character is alphabet
      char upperC =
          toupper(c);  // convert the character into uppercase (toupper)
      int charIdx =
          ALPHABET.find(upperC);  // find the index of the character (0-25)
      char substitutedChar =
          cipher[charIdx];        // get the random substituted char
      result += substitutedChar;  // append the substituted character
    } else {                      // if not alphabet,
      result += c;                // append the original character
    }
  }
  return result;
}

void applyRandSubstCipherCommand() {
  string line;
  cout << "Enter the text to encrypt: ";
  getline(cin, line);

  // Generate a random substitution cipher
  vector<char> cipher = genRandomSubstCipher();

  // Encrypt
  string encrypted = applySubstCipher(cipher, line);

  // Print result
  cout << encrypted << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  double totalScore = 0.0;
  // If string is too short for even one quadgram, return 0
  if (s.length() < 4) {
    return 0.0;
  }
  for (int i = 0; i <= s.size() - 4;
       i++) {  // ensure we don't go out of bounds by using s.size() - 4
    string quadgram = s.substr(i, 4);  // extract quadgram of length 4
    totalScore +=
        scorer.getScore(quadgram);  // assign calculated score to totalScore
  }
  return totalScore;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string line;
  cout << "Enter a string for englishness scoring: ";
  getline(cin, line);

  string cleaned = clean(line);  // Clean the input string

  double score = scoreString(scorer, cleaned);  // Compute the score

  cout << "Score: " << score << endl;
}

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> key = genRandomSubstCipher();  // start with random key

  string bestDecryption =
      applySubstCipher(key, ciphertext);  // initial decryption
  double bestScore =
      scoreString(scorer, clean(bestDecryption));  // initial score

  int trialsWithoutImprovement = 0;  // counter for trials without improvement

  while (trialsWithoutImprovement <
         1000) {                // stop after 1000 non-improving trials
    vector<char> newKey = key;  // copy current key

    // pick two random indices to swap
    int i = Random::randInt(25);  // i index from 0 to 25
    int j = Random::randInt(25);  // j index from 0 to 25
    while (i == j) {              // while i and j are the same,
      j = Random::randInt(25);    // re-pick j
    }
    swap(newKey[i], newKey[j]);  // swap characters at indices i and j

    string candidate =
        applySubstCipher(newKey, ciphertext);  // decrypt with new key
    double candidateScore =
        scoreString(scorer, clean(candidate));  // score the decryption

    if (candidateScore > bestScore) {  // if candidate is better than best
      key = newKey;                    // update key to new key
      bestDecryption = candidate;      // update best decryption
      bestScore = candidateScore;      // update best score
      trialsWithoutImprovement = 0;    // reset counter
    } else {
      trialsWithoutImprovement++;  // increment counter
    }
  }

  return key;  // return the best key found
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  vector<char> bestKey;     // to store the best key found
  double bestScore = -1e9;  // very low initial score
  int restarts = 30;        // number of hill climb restarts

  for (int i = 0; i < restarts; i++) {
    vector<char> candidateKey =
        hillClimb(scorer, ciphertext);  // get key from hill climb
    string candidateDecryption = applySubstCipher(
        candidateKey, ciphertext);  // decrypt with candidate key
    double candidateScore = scoreString(
        scorer, clean(candidateDecryption));  // score the decryption

    if (candidateScore > bestScore) {
      bestScore = candidateScore;  // update best score
      bestKey = candidateKey;      // update best key
    }
  }

  return bestKey;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string line;
  cout << "Enter the text to substitution decrypt:" << endl;  // Updated Prompt
  getline(cin, line);

  // Process the cipher
  vector<char> key = decryptSubstCipher(scorer, line);  // get decryption key
  string decrypted = applySubstCipher(key, line);  // decrypt the ciphertext

  cout << decrypted << endl;
}

// File-based substitution cipher decryption command
void decryptSubstCipherFromFileCommand(const QuadgramScorer& scorer) {
  string inputFilename, outputFilename;  // declare input and output filenames
  cout << "Enter input filename: ";
  getline(cin, inputFilename);

  cout << "Enter output filename: ";
  string outFilename;
  getline(cin, outFilename);

  // Read ciphertext from file
  ifstream inputFile(inputFilename);  // open input file
  if (!inputFile.is_open()) {
    cout << "Error: Could not open file " << inputFilename << endl;
    return;
  }
  stringstream buffer;               // create stringstream buffer
  buffer << inputFile.rdbuf();       // read file content into buffer
  string ciphertext = buffer.str();  // get string from buffer
  inputFile.close();                 // close input file
  // Uppercase letters, keep spaces/punctuation as-is
  for (char& c : ciphertext) {  // iterate through each character
    if (isalpha(c)) {           // check if character is alphabet
      c = toupper(c);           // convert to uppercase
    }
  }

  // Decrypt
  vector<char> key =
      decryptSubstCipher(scorer, ciphertext);  // get decryption key
  string decrypted =
      applySubstCipher(key, ciphertext);  // decrypt the ciphertext
  // Write decrypted text to output file
  ofstream outputFile(outFilename);  // open output file
  if (!outputFile.is_open()) {
    cout << "Error: Could not open file." << outFilename << endl;
    return;
  }
  outputFile << decrypted;
  outputFile.close();
  cout << "Decrypted complete. Output written to " << outFilename << endl;
}

#pragma endregion SubstDec
