#include "Globals.h"
#include "Functions.h"
#include <random>
#include <ctime>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <deque>
#define stop __asm nop

Functions::Functions() : m_probabilities(), m_symbolCode(), m_probabilitiesReserved(), m_myAlphabet(), m_dictionary()
{
	m_symbolCounter = 0;
}

void Functions::Start()
{
	CreateRandomText();
	CreateDictionary("RandomText.txt");
	CreateAlphabet();
	Huffman(m_myAlphabet.size());
	CodeHuffman("RandomText.txt", "HuffmanCode.txt");
	DecodeHuffman("HuffmanCode.txt", "HuffmanDecoded.txt");
}

void Functions::CreateDictionary(std::string fileName)
{
	m_dictionary.clear();
	std::ifstream fin(fileName, std::ios_base::binary);
	char currentSymbol;
	while (!fin.eof())
	{
		fin.get(currentSymbol);
		m_dictionary[currentSymbol]++;
		m_symbolCounter++;
	}
	fin.close();
}

void Functions::CreateAlphabet()
{
	if (!m_dictionary.empty())
	{
		m_myAlphabet.clear();
		m_probabilities.clear();
		m_probabilitiesReserved.clear();

		for (auto i = m_dictionary.begin(); i != m_dictionary.end(); i++)
		{
			m_myAlphabet.push_back(i->first);
			m_probabilities.push_back((i->second) / m_symbolCounter);
		}

		for (int i = 0; i < m_myAlphabet.size() - 1; i++) {
			for (int j = 0; j < m_myAlphabet.size() - i - 1; j++) {
				if (m_probabilities[j] < m_probabilities[j + 1]) {
					double temp = m_probabilities[j];
					m_probabilities[j] = m_probabilities[j + 1];
					m_probabilities[j + 1] = temp;
					char tmp = m_myAlphabet[j];
					m_myAlphabet[j] = m_myAlphabet[j + 1];
					m_myAlphabet[j + 1] = tmp;
				}
			}
		}

		m_probabilitiesReserved = m_probabilities;
		//double sum = 0;
		//for (int i = 0; i < m_probabilities.size(); i++) sum += m_probabilities[i];
		//std::cout << sum;
	}
	else std::cout << "Fill the m_dictionary";
}

void Functions::CreateRandomText()
{
	std::ofstream fout("RandomText.txt", std::ios_base::binary);
	std::mt19937 mersenne(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < Length; i++)
	{
		int randomNumber = mersenne() % AlphabetSize;
		fout << Alphabet[randomNumber];
	}
	fout.close();
}

int Functions::Up(int sizeOfProcessedPart, double sumToInsert)
{
	int numberWhereInsert = 0;
	for (int i = sizeOfProcessedPart - 2; i > 0; i--)
	{
		if (m_probabilities[i - 1] < sumToInsert) m_probabilities[i] = m_probabilities[i - 1];
		else
		{
			numberWhereInsert = i;
			break;
		}
	}
	m_probabilities[numberWhereInsert] = sumToInsert;
	m_probabilities.pop_back();
	return numberWhereInsert;
}

void Functions::Down(int sizeOfProcessedPart, int numberOfDividingLetter)
{
	std::vector<bool> tempCode = m_symbolCode[numberOfDividingLetter];
	for (int i = numberOfDividingLetter; i < sizeOfProcessedPart - 2; i++)
	{
		m_symbolCode[i] = m_symbolCode[i + 1];
	}

	tempCode.push_back(0);
	m_symbolCode[sizeOfProcessedPart - 2] = tempCode;
	tempCode.pop_back();
	tempCode.push_back(1);
	m_symbolCode.push_back(tempCode);
}

void Functions::Huffman(int alphabetSize)
{
	if (alphabetSize == 2)
	{
		m_symbolCode.push_back(std::vector<bool>(1, 0));
		m_symbolCode.push_back(std::vector<bool>(1, 1));
	}
	else
	{
		double sumOfTwoLast = m_probabilities[alphabetSize - 2] + m_probabilities[alphabetSize - 1];
		int numberWhereInserted = Up(alphabetSize, sumOfTwoLast);
		Huffman(alphabetSize - 1);
		Down(alphabetSize, numberWhereInserted);
	}
}

void Functions::WriteCodes()
{
	for (int i = 0; i < m_symbolCode.size(); i++)
	{
		for (int j = 0; j < m_symbolCode[i].size(); j++) std::cout << m_symbolCode[i][j];
		std::cout << '\n';
	}
}

void Functions::CodeHuffman(std::string inputName, std::string outputName)
{
	std::ifstream fin(inputName, std::ios_base::binary);
	std::ofstream fout(outputName, std::ios_base::binary);
	if (fin.is_open())
	{
		char currentSymbol = 0;
		char finalSymbol = 0;
		int counter = 0;
		for (int i = 0; i < m_symbolCounter; i++)
		{
			fin.get(currentSymbol);
			for (int j = 0; j < m_myAlphabet.size(); j++)
			{
				if (m_myAlphabet[j] == currentSymbol)
				{
					for (int k = 0; k < m_symbolCode[j].size(); k++)
					{
						finalSymbol |= (1 << counter) * m_symbolCode[j][k];
						counter++;
						if (counter == 8)
						{
							fout.put(finalSymbol);
							finalSymbol = 0;
							counter = 0;
						}
					}
				}
			}
		}
		if (counter != 0)
		{
			fout.put(finalSymbol);
		}
	}
	fin.close();
	fout.close();
}

void Functions::DecodeHuffman(std::string inputName, std::string outputName)
{
	std::ifstream fin(inputName, std::ios_base::binary);
	std::ofstream fout(outputName, std::ios_base::binary);
	if (fin.is_open())
	{
		int counter = 0;
		char currentSymbol = 0;
		int startOfEmptyPart = 0;
		int numberOfFinalSymbol = 0;
		std::deque<bool> buffer(16, 0);
		while (!fin.eof())
		{
			fin.get(currentSymbol);
			for (int i = 0; i < 8; i++)
			{
				buffer[startOfEmptyPart] = currentSymbol & (1 << i);
				startOfEmptyPart++;
			}
			while (startOfEmptyPart >= 8)
			{
				numberOfFinalSymbol = FindCode(buffer, startOfEmptyPart);
				fout.put(m_myAlphabet[numberOfFinalSymbol]);
				counter++;
				if (counter == 10000)
				{
					fin.close();
					fout.close();
					return;
				}
			}
		}
	}
}

int Functions::FindCode(std::deque<bool>& buffer, int& startOfEmptyPart)
{
	for (int i = 0; i < m_myAlphabet.size(); i++)
	{
		bool isSame = true;
		for (int j = 0; j < m_symbolCode[i].size(); j++)
		{
			if (m_symbolCode[i][j] != buffer[j])
			{
				isSame = false;
				break;
			}
		}
		if (isSame == true)
		{
			for (int j = 0; j < (startOfEmptyPart - m_symbolCode[i].size()); j++)
			{
				buffer[j] = buffer[j + m_symbolCode[i].size()];
			}
			startOfEmptyPart -= m_symbolCode[i].size();
			return i;
		}
	}
}