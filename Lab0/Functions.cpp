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
	CodeRLE("RandomText.txt", "RLE.txt");
	DecodeRLE("RLE.txt", "RLEDecoded.txt");
	GetAdditionalInformation("RandomText.txt", "HuffmanCode.txt", "HuffmanDecoded.txt", "RandomText.txt", "RLE.txt", "RLEDecoded.txt");
}

void Functions::CreateDictionary(std::string fileName)
{
	m_dictionary.clear();
	m_symbolCounter = 0;
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
		m_symbolCode.clear();

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

void Functions::GetAdditionalInformation(std::string origHuf, std::string compHuf, std::string decHuf,
	std::string origRLE, std::string compRLE, std::string decRLE)
{
	std::cout << "Алгоритм Хаффмана:";
	double codingCost = 0;
	for (int i = 0; i < m_probabilitiesReserved.size(); i++)
	{
		codingCost += m_probabilitiesReserved[i] * m_symbolCode[i].size();
	}

	std::cout << "\nЦена кодирования: " << codingCost;

	double originalSize = GetFileSize(origHuf);
	double comressedSize = GetFileSize(compHuf);
	std::cout << "\nКоэффицент сжатия: " << (comressedSize / originalSize);

	if (CheckDecoding(origHuf, decHuf)) std::cout << "\nДекодирование произошло верно";
	else std::cout << "\nДекодирование произошло неверно";

	std::cout << "\n\nАлгоритм RLE:";

	originalSize = GetFileSize(origRLE);
	comressedSize = GetFileSize(compRLE);
	std::cout << "\nКоэффицент сжатия: " << (comressedSize / originalSize);

	if (CheckDecoding(origRLE, decRLE)) std::cout << "\nДекодирование произошло верно";
	else std::cout << "\nДекодирование произошло неверно";

	double compValHR = HuffmanRLE();
	double compValRH = RLEHuffman();

	std::cout << "\n\nДвухступенчатое кодирование:";
	std::cout << "\nКоэффицент сжатия Хаффман + RLE: " << compValHR;
	std::cout << "\nКоэффицент сжатия RLE + Хаффман: " << compValRH;
	if (compValHR > compValRH) std::cout << "\nХаффман + RLE эффективней";
	if (compValHR < compValRH) std::cout << "\nRLE + Хаффман эффективней";
	if ((compValHR - compValRH) < DBL_EPSILON) std::cout << "\nОба варианта одинаково эффективны.";
	std::cout << '\n';
}

bool Functions::CheckDecoding(std::string firstName, std::string secondName)
{
	std::ifstream firstFin(firstName, std::ios_base::binary);
	std::ifstream secondFin(secondName, std::ios_base::binary);
	char firstSymbol = 0;
	char secondSymbol = 0;
	while (!firstFin.eof() && !secondFin.eof())
	{
		firstFin.get(firstSymbol);
		secondFin.get(secondSymbol);
		if (firstSymbol != secondSymbol) return false;
	}
	if (firstFin.eof() && secondFin.eof()) return true;
	else return false;
}

void Functions::CodeRLE(std::string inputName, std::string outputName)
{
	std::ifstream fin(inputName, std::ios_base::binary);
	std::ofstream fout(outputName, std::ios_base::binary);
	int counter = 1;
	char currentSymbol = 0;
	char previousSymbol = 0;

	if (!fin.eof()) fin.get(previousSymbol);

	while (!fin.eof())
	{
		fin.get(currentSymbol);
		if (currentSymbol == previousSymbol)
		{
			counter++;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				fout.put(counter >> (8 * i));
			}
			fout.put(previousSymbol);
			counter = 1;
		}
		previousSymbol = currentSymbol;
	}

	counter--;
	for (int i = 0; i < 4; i++)
	{
		fout.put(counter >> (8 * i));
	}
	fout.put(previousSymbol);

	fin.close();
	fout.close();
}

void Functions::DecodeRLE(std::string inputName, std::string outputName)
{
	std::ifstream fin(inputName, std::ios_base::binary);
	std::ofstream fout(outputName, std::ios_base::binary);
	char currentSymbol = 0;
	char numberChar = 0;
	int currentNumber = 0;
	int count = 0;
	while (!fin.eof())
	{
		if (count == 9998)
		{
			stop
		}
		for (int i = 0; i < 4; i++)
		{
			fin.get(numberChar);
			currentNumber |= numberChar << (8 * i);
		}
		fin.get(currentSymbol);
		if (fin.eof())
		{
			currentNumber--;
		}
		for (int i = 0; i < currentNumber; i++)
		{
			fout.put(currentSymbol);
			count++;
		}
		currentNumber = 0;
	}

	fin.close();
	fout.close();
}

double Functions::HuffmanRLE()
{
	CreateDictionary("RandomText.txt");
	CreateAlphabet();
	Huffman(m_myAlphabet.size());
	CodeHuffman("RandomText.txt", "HuffmanCode.txt");
	CodeRLE("HuffmanCode.txt", "HuffmanRLE.txt");
	return GetFileSize("HuffmanRLE.txt") / GetFileSize("RandomText.txt");
}

double Functions::RLEHuffman()
{
	CodeRLE("RandomText.txt", "RLE.txt");
	CreateDictionary("RLE.txt");
	CreateAlphabet();
	Huffman(m_myAlphabet.size());
	CodeHuffman("RLE.txt", "RLEHuffman.txt");
	return GetFileSize("RLEHuffman.txt") / GetFileSize("RandomText.txt");
}

double Functions::GetFileSize(std::string fileName)
{
	std::ifstream input(fileName, std::ios_base::binary);
	const auto begin = input.tellg();
	input.seekg(0, std::ios_base::end);
	const auto end = input.tellg();
	return end - begin;
}