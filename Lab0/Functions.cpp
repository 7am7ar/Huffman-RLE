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

Functions::Functions() : m_probabilities(AlphabetSize, 0), m_codeLength(), m_symbolCode(),
	m_probabilitiesReserved(), MyAlphabet(Alphabet, Alphabet + AlphabetSize)
{
	Count = 0;
}

void Functions::CreateRandomText()
{
	std::ofstream fout("RandomText.txt", std::ios_base::binary);
	std::mt19937 mersenne(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < Length; i++)
	{
		int randomNumber = mersenne() % AlphabetSize;
		m_probabilities[randomNumber]++;
		fout << MyAlphabet[randomNumber];
	}
	fout.close();
	for (int i = 0; i < AlphabetSize; i++) m_probabilities[i] /= Length;

	for (int i = 0; i < AlphabetSize - 1; i++) {
		for (int j = 0; j < AlphabetSize - i - 1; j++) {
			if (m_probabilities[j] > m_probabilities[j + 1]) {
				double temp = m_probabilities[j];
				m_probabilities[j] = m_probabilities[j + 1];
				m_probabilities[j + 1] = temp;
				char tmp = MyAlphabet[j];
				MyAlphabet[j] = MyAlphabet[j + 1];
				MyAlphabet[j + 1] = tmp;
			}
		}
	}
	
	m_probabilitiesReserved = m_probabilities;


	//for (int i = 0; i < AlphabetSize; i++) std::cout << m_probabilities[i] << ' ';
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
		m_codeLength[i] = m_codeLength[i + 1];
	}

	tempCode.push_back(0);
	m_symbolCode[sizeOfProcessedPart - 2] = tempCode;
	tempCode.pop_back();
	tempCode.push_back(1);
	m_symbolCode.push_back(tempCode);
	m_codeLength[sizeOfProcessedPart - 2] = tempCode.size();
	m_codeLength.push_back(tempCode.size());
}

void Functions::Huffman(int alphabetSize)
{
	if (alphabetSize == 2)
	{
		m_symbolCode.push_back(std::vector<bool>(1, 0));
		m_symbolCode.push_back(std::vector<bool>(1, 1));
		m_codeLength.push_back(1);
		m_codeLength.push_back(1);
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
		for (int j = 0; j < m_codeLength[i]; j++) std::cout << m_symbolCode[i][j];
		std::cout << '\n';
	}
}

void Functions::InsertProbabilities(int alphabetSize)
{
	double temp = 0;
	for (int i = 0; i < alphabetSize; i++)
	{
		std::cin >> m_probabilities[i];
	}
	for (int i = 0; i < AlphabetSize - alphabetSize; i++) m_probabilities.pop_back();
}

void Functions::CodeHuffman()
{
	std::ifstream fin("RandomText.txt", std::ios_base::binary);
	std::ofstream fout("Huffman.txt", std::ios_base::binary);
	if (fin.is_open())
	{
		char currentSymbol = 0;
		char finalSymbol = 0;
		int counter = 0;
		for (int i = 0; i < Length; i++)
		{
			fin.get(currentSymbol);
			for (int j = 0; j < AlphabetSize; j++)
			{
				if (MyAlphabet[j] == currentSymbol)
				{
					for (int k = 0; k < m_codeLength[j]; k++)
					{
						finalSymbol |= (1 << counter) * m_symbolCode[j][k];
						counter++;
						if (counter == 8)
						{
							fout.put(finalSymbol);
							Count++;
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
			Count++;
		}
	}
	fin.close();
	fout.close();
}

void Functions::DecodeHuffman()
{
	std::ifstream fin("Huffman.txt", std::ios_base::binary);
	std::ofstream fout("DecodedHuffman.txt", std::ios_base::binary);
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
				std::cout << MyAlphabet[numberOfFinalSymbol];
				fout << MyAlphabet[numberOfFinalSymbol];
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

void Functions::GetFile()
{
	std::ifstream fin("Huffman.txt", std::ios_base::binary);
	std::ofstream fout("Stunt.txt", std::ios_base::binary);
	std::deque<char> buffer;
	char currentSymbol;
	int counter = 0;
	while (fin.get(currentSymbol))
	{
		fout.put(currentSymbol);
	}
	fin.close();
	fout.close();
	stop
}

int Functions::FindCode(std::deque<bool>& buffer, int& startOfEmptyPart)
{
	for (int i = 0; i < AlphabetSize; i++)
	{
		bool isSame = true;
		for (int j = 0; j < m_codeLength[i]; j++)
		{
			if (m_symbolCode[i][j] != buffer[j])
			{
				isSame = false;
				break;
			}
		}
		if (isSame == true)
		{
			for (int j = 0; j < (startOfEmptyPart - m_codeLength[i]); j++)
			{
				buffer[j] = buffer[j + m_codeLength[i]];
			}
			startOfEmptyPart -= m_codeLength[i];
			return i;
		}
	}
	
}