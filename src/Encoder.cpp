#include "encoder.h"
#include <cstdlib>
#include <time.h>
#include <math.h>

/* Here is how it works, you give this thing an algorithm and it will encode your string, each character is put by its integral value
   into the algorithm (if the char is negative then it will be mapped to its hex value (somewhere between 128-255)). If the outcome > 255
   then there will be a marker to note that it is stored in more than one byte, meaning the next char read is still part of this one.
   and of course 255 will be added to the result. If the outcome < 255 then there will be a marker that the next byte is the last of this
   char. This marker does not cause 255 to be added to the final result. The markers work as following:
   > 255 maker if : ((outcome of 2 in encode algorithm) * (position %10)) % char > 10
   < 255 marker if: ((outcome of 2 in encode algorithm) * (position %10)) % char < 10
*/

int intValOfChar(char target)
{
	if (target < 0)
		return 256 + target;
	return target;
}

std::string toString(int target)
{
	std::string result = "";
	if (target < 0)
		target = target * -1;
	char buffer[10];
	int digits = 1;
	while (target / pow(10, digits - 1) > 10)
		digits++;
	int originalDigits = digits;
	for (int i = 1; i < 11; i++)
	{
		double d = pow(10, digits - 1) * i;
		if (target - d < 0)
		{
			cout << "ok: " << digits << " " << i << " " << d << endl;
			buffer[originalDigits - digits] = i + 47;
			target -= pow(10, digits - 1) * (i-1);
			digits --;
			if (digits != 0)
				i = 1;
			else
				break;
		}
	}
	for (int i = 0; i < originalDigits; i ++)
		result += buffer[i];
	return result;
}

int randomChar()
{
	return rand() % 256;
}

int needBiggerThanTen(int target)
{
	int ao = randomChar();
	int a = ao;
	while (target % a < 10)
	{
		if (ao < 128)
			a ++;
		else
			a --;
	}
	return a;
}

int needSmallerThanTen(int target)
{
	int ao = randomChar();
	int a = ao;
	while (target % a > 10)
	{
		if (ao < 128)
			a ++;
		else
			a --;
	}
	return a;
}

/**
Creating an object of Encoder will make a call to srand(),
might mess up your rand() calls in other threads if you are
using seeds.
*/
Encoder::Encoder(Algorithm encodeAlgorithm, algorithm decodeAlgorithm) : algorithm(encodeAlgorithm), decodeAlgorithm(decodeAlgorithm)
{
	srand(time(nullptr));
	twoInAlgorithm = toString(algorithm.execute(2));
}

std::string Encoder::encode(std::string target)
{
	std::string result = "";
	for (unsigned int i = 0; i < target.length(); i ++)
	{
		int charVal = intValOfChar(target[i]);
		int value = algorithm.execute(charVal);
		while (value > 255)
		{
			int next = Algorithm("x=" + twoInAlgorithm + "*(x%10)").execute(i);
			result += needBiggerThanTen(next);
			value -= 255;
		}
		int next = Algorithm("x=" + twoInAlgorithm + "*(x%10)").execute(i);
		result += needSmallerThanTen(next);
		retult += value;
	}
	return result;
}

std::string Encoder::decode(std::string target)
{
	std::string result = "";
	int pos = 0;
	int totalValue = 0;
	for (unsigned int i = 0; i < target.length(); i ++)
	{
		int charVal = intValOfChar(target[i]);
		int next = Algorithm("x=" + twoInAlgorithm +"*(x%10)")).execute(pos);
		if (next % charVal > 10)
			totalValue += 255;
		else
		{
			i++;
			if (i >= target.length())
				throw "Can't decode this string. It was not encoded with an encoder of this type.";
			totalValue += intValOfChar(target[i]);
			int decoded = decodeAlgorithm.execute(totalValue);
			if (decoded < 0 || decoded > 255)
				throw "Something went wrong decoding this string: invalid char value. Perhaps it was not well encoded.";
			result += (char)decoded;
			pos++;
			totalValue = 0;
		}
	}
	return result;
}
