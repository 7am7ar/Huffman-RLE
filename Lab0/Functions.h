#pragma once
#include <vector>
#include <deque>

class Functions
{
public:
	Functions();
	void CreateRandomText();
	void Huffman(int alphabetSize);
	int Up(int sizeOfProcessedPart, double sumToInsert);
	void Down(int sizeOfProcessedPart, int numberOfDividedLetter);
	void CodeHuffman();
	void DecodeHuffman();
	void GetFile();
	void WriteCodes();
	int FindCode(std::deque<bool>& buffer, int& startOfEmptyPart);
	void InsertProbabilities(int alphabetSize);
private:
	int Count;
	std::vector<char> MyAlphabet;
	std::vector<double> m_probabilities;
	std::vector<double> m_probabilitiesReserved;
	std::vector<std::vector<bool>> m_symbolCode;
	std::vector<int> m_codeLength;
};