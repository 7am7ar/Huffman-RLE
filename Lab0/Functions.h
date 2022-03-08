#pragma once
#include <vector>
#include <deque>
#include <map>
#include <string>
class Functions
{
public:
	Functions();
	void CreateDictionary(std::string fileName);
	void CreateAlphabet();
	void CreateRandomText();
	void Huffman(int alphabetSize);
	int Up(int sizeOfProcessedPart, double sumToInsert);
	void Down(int sizeOfProcessedPart, int numberOfDividedLetter);
	void CodeHuffman(std::string inputName, std::string outputName);
	void DecodeHuffman(std::string inputName, std::string outputName);
	void WriteCodes();
	int FindCode(std::deque<bool>& buffer, int& startOfEmptyPart);
	void Start();
private:
	int m_symbolCounter;
	std::map<char, double> m_dictionary;
	std::vector<char> m_myAlphabet;
	std::vector<double> m_probabilities;
	std::vector<double> m_probabilitiesReserved;
	std::vector<std::vector<bool>> m_symbolCode;
};