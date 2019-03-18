#include "encoder.h"
#include <cstdlib>
#include <time.h>
#include <math.h>
#include "fstring.h"

/* Here is how it works, you give this thing an algorithm and it will encode your string, each character is put by its integral value
   into the algorithm (if the char is negative then it will be mapped to its hex value (somewhere between 128-255)). If the outcome > 255
   then there will be a marker to note that it is stored in more than one byte, meaning the next char read is still part of this one.
   and of course 255 will be added to the result. If the outcome < 255 then there will be a marker that the next byte is the last of this
   char. This marker does not cause 255 to be added to the final result. The markers work as following these chars will never be 0 ('\0'):
   > 255 maker if : ((outcome of 2 in encode algorithm) * (position %10)) >= char
   < 255 marker if: ((outcome of 2 in encode algorithm) * (position %10)) < char
*/

int intValOfChar(char target)
{
	if (target < 0)
		return 256 + target;
	return target;
}

int needBiggerOrEqualTo(int target)
{
	int border = 256 - target;
	int result = rand() % border + target;
	if (result == 0)
		return 1;
	return result;
}

int needSmallerThan(int target)
{
	int result = rand() % target;
	if (result == 0)
		return 1;
	return result;
}

int getCompareBasis(int pos, std::string twoInAlgorithm)
{
	int result = Algorithm("x=" + twoInAlgorithm + "*(x%10) + 10").execute(pos);
	while (result > 255)
		result -= 255;
	while (result < 0)
	result += 255;
	if (result < 2)
		result = 2;
	return result;
}

/**
Creating an object of Encoder will make a call to srand(),
might mess up your rand() calls in other threads if you are
using seeds.
*/
Encoder::Encoder(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm) : algorithm(encodeAlgorithm), decodeAlgorithm(decodeAlgorithm)
{
	srand(time(nullptr));
	twoInAlgorithm = FString::fromInt(algorithm.execute(2)).toStdString();
}

std::string Encoder::encode(std::string target)
{
	std::string result = "";
	for (unsigned int i = 0; i < target.length(); i ++)
	{
		int compareBasis = getCompareBasis(i, twoInAlgorithm);
		int charVal = intValOfChar(target[i]);
		int value = algorithm.execute(charVal);
		if (value < 0)
		{
			result += needBiggerOrEqualTo(compareBasis);
			value = value * -1;
		}
		else
			result += needSmallerThan(compareBasis);
		while (value > 255)
		{
			result += needBiggerOrEqualTo(compareBasis);
			value -= 255;
		}
		result += needSmallerThan(compareBasis);
		result += value;
	}
	return result;
}

std::string Encoder::decode(std::string target)
{
	std::string result = "";
	int pos = 0;
	int totalValue = 0;
	bool checkNegative = true;
	int negative = 1;
	for (unsigned int i = 0; i < target.length(); i ++)
	{
		int compareBasis = getCompareBasis(pos, twoInAlgorithm);
		int charVal = intValOfChar(target[i]);
		if (checkNegative)
		{
			checkNegative = false;
			if (compareBasis <= charVal)
				negative = -1;
			else
				negative = 1;
			continue;
		}
		if (compareBasis <= charVal)
			totalValue += 255 * negative;
		else
		{
			i++;
			if (i >= target.length())
				throw "Can't decode this string. It was not encoded with an encoder of this type.";
			totalValue += intValOfChar(target[i]) * negative;
			int decoded = decodeAlgorithm.execute(totalValue);
			if (decoded < 0 || decoded > 255)
				throw "Something went wrong decoding this string: invalid char value. Perhaps it was not well encoded.";
			result += (char)decoded;
			pos++;
			totalValue = 0;
			checkNegative = true;
		}
	}
	return result;
}
