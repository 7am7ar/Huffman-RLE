#include "Functions.h"
#include "Globals.h"
#include <iostream>
int main()
{
	setlocale(LC_ALL, "Russian");
	Functions test;
	test.CreateRandomText();
	//test.InsertProbabilities(7);
	test.Huffman(AlphabetSize);
	test.WriteCodes();
	test.CodeHuffman();
	//test.GetFile();
	test.DecodeHuffman();
	return 0;
}