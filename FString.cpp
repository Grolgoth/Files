#include "fstring.h"
#include <regex>
#include <math.h>
#include "encoder.h"

FString FString::fromVector(std::vector<std::string> vect, std::string appendElements)
{
	std::string result = "";
	for (std::string index : vect)
		result += index + appendElements;
	return FString(result);
}

FString FString::fromVector(std::vector<FString> vect, std::string appendElements)
{
	std::string resultstr = "";
	for (FString index : vect)
		resultstr += index.toStdString() + appendElements;
	return FString(resultstr);
}

FString FString::fromInt(int target)
{
	return FString(std::to_string(target));
	/* Just going to leave this in here 'cause it's so painful. Yes, this is good coding :D
	std::string result = "";
	if (target < 0)
	{
		result += "-";
		target = target * -1;
	}
	char buffer[10];
	int digits = 1;
	while (target / pow(10, digits - 1) >= 10)
		digits++;
	int originalDigits = digits;
	for (int i = 1; i < 11; i++)
	{
		int d = pow(10, digits - 1) * i;
		if (target - d < 0)
		{
			buffer[originalDigits - digits] = i + 47;
			target -= floor(pow(10, digits - 1) * (i-1));
			digits --;
			if (digits != 0)
				i = 0;
			else
				break;
		}
	}
	for (int i = 0; i < originalDigits; i ++)
		result += buffer[i];
	return FString(result); */
}

FString::FString(std::string base)
{
	this->base = base;
}

FString FString::operator+(const FString& b)
{
	return FString(base + b.base);
}

bool FString::operator!=(const FString& b)
{
	return base != b.base;
}

bool FString::toBool()
{
	if (base == "true")
		return true;
	else if(base == "false")
		return false;
	else
		throw "Can't convert " + base + " to a boolean because it is not equal to either false, or true";
}

bool FString::isEmpty()
{
	return base.length() == 0;
}

bool FString::isNumber()
{
	return std::all_of(base.begin(), base.end(), ::isdigit);
}

bool FString::contains(std::string sfind, bool ignoreCase)
{
	return indexOf(sfind, ignoreCase) > -1;
}

bool FString::contains(FString sfind, bool ignoreCase)
{
	return indexOf(sfind, ignoreCase) > -1;
}

bool FString::containsRegex(std::string sregex)
{
	return std::regex_match(base, std::regex(sregex));
}

bool FString::startsWith(const std::string prefix, bool ignoreCase)
{
	if (base.length() >= prefix.length())
		return base.compare(0, prefix.length(), prefix) == 0;
	else
		return false;
}

bool FString::startsWith(FString prefix, bool ignoreCase)
{
	return startsWith(prefix.toStdString());
}

bool FString::endsWith(const std::string& suffix, bool ignoreCase)
{
	if (base.length() >= suffix.length())
		return base.compare(base.length() - suffix.length(), suffix.length(), suffix) == 0;
	else
		return false;
}

bool FString::endsWith(FString suffix, bool ignoreCase)
{
	return endsWith(suffix.toStdString());
}

bool FString::allDigits()
{
	for (unsigned int i = 0; i < base.length(); i++)
	{
		if (!isdigit(base[i]))
			return false;
	}
	return true;
}

unsigned int FString::length()
{
	return base.length();
}

int FString::compare(std::string other)
{
	return base.compare(other);
}

int FString::compare(FString other)
{
	return compare(other.toStdString());
}

int FString::indexOf(std::string sfind, bool ignoreCase, unsigned int fromPos, unsigned int occurences, bool fromBegin)
{
	std::string baseCopy = base;
	if (!fromBegin)
	{
		std::reverse(sfind.begin(), sfind.end());
		std::reverse(baseCopy.begin(), baseCopy.end());
	}
	if (ignoreCase)
	{
		std::transform(baseCopy.begin(), baseCopy.end(), baseCopy.begin(), ::tolower);
        std::transform(sfind.begin(), sfind.end(), sfind.begin(), ::tolower);
	}
	if (baseCopy.length() < sfind.length() || occurences < 1)
		return -1;

	int result = -1;
	size_t pos = baseCopy.find(sfind, fromPos);

	while (pos != std::string::npos)
	{
        occurences--;
        if (occurences == 0)
        {
        	if (fromBegin)
				result = pos;
			else
				result = baseCopy.size() - pos - 1;
        }
        pos = baseCopy.find(sfind, pos + 1);
    }
	return result;
}

int FString::indexOf(FString sfind, bool ignoreCase, unsigned int fromPos, unsigned int occurences, bool fromBegin)
{
	return indexOf(sfind.toStdString(), ignoreCase, fromPos, occurences, fromBegin);
}

int FString::toInt()
{
    try
    {
    	if (base.length() > 0 && base[0] == '-')
			return std::stoi(base) * -1;
		else
			return std::stoi(base);
    }
    catch(...)
    {
        if(!isdigit(base[0]))
			std::cout <<  "Can't convert " + base + " to an integer, because it doesn't start with a digit." << std::endl;
		else
			std::cout << "Can't convert " + base + " to an integer. Unknown error during conversion." << std::endl;
		return 0;
    }
}

char FString::charAt(unsigned int index)
{
	if (index >= base.length())
		throw "Can't return char at position " + std::to_string(index) + " for string " + base + " because the index specified is bigger than its length.";
	return base[index];
}

std::string FString::toStdString() const
{
	return base;
}

FString FString::substring(unsigned int ibegin, int iend)
{
	unsigned int uiend = base.length();
	if (iend >= 0)
		uiend = (unsigned)iend;
	if (ibegin >= base.length() || uiend <= ibegin || uiend > base.length())
		return FString("");
	FString resultToCopy(base.substr(ibegin, uiend - ibegin));
	return resultToCopy;
}

FString FString::toUpper()
{
	std::string result = base;
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return FString(result);
}

FString FString::toLower()
{
	std::string result = base;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return FString(result);
}

FString FString::invert(unsigned int ibegin, int iend)
{
	unsigned int uiend = base.length();
	if (iend >= 0)
		uiend = (unsigned)iend;
	if (ibegin >= base.length() || uiend <= ibegin || uiend > base.length())
		return FString("");

	std::string inverted = base;
	std::reverse(inverted.begin() + ibegin, inverted.begin() + uiend);
	return FString(inverted);
	/*
	unsigned int uiend = base.length();
	if (iend >= 0)
		uiend = (unsigned)iend;
	std::string resultstr = "";
	if (ibegin >= base.length() || uiend <= ibegin || uiend > base.length())
		return FString(resultstr);
	char* invSeg = substring(ibegin, uiend).toCharArray();
	for (int i = uiend - ibegin - 1; i > -1; i --)
		resultstr += invSeg[i];
	FString result("");
	if (ibegin > 0)
		result = result + substring(0, ibegin);
	result = result + FString(resultstr);
	result = result + substring(uiend);
	return result; */
}

std::vector<unsigned int> FString::findAll(std::string find, bool ignoreCase, int fromPos)
{
	std::vector<unsigned int> result;
	int occurences = 1;
	while (true)
	{
		int f = indexOf(find, ignoreCase, fromPos, occurences);
		if (f == -1)
			break;
		result.push_back(f);
		occurences ++;
	}
	return result;
}

FString FString::replace(std::string sfind, std::string target, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	std::string result = "";
	std::vector<FString> split = this->split(sfind, ignoreCase, all, occurences, fromBegin);
	if (split.empty())
		return *this;
	for (FString index : split)
	{
		result += index.toStdString();
		if (index != split.back() || split.size() == 1)
			result += target;
	}
	return FString(result);
}

FString FString::encode(Algorithm algorithm)
{
	if (!algorithm.isValid())
		throw "Can't perform transformation on String object because provided algorithm is invalid.";
	Encoder encoder(algorithm, Algorithm("x=1*x"));
	return FString(encoder.encode(base));
}

FString FString::decode(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm)
{
	if (!encodeAlgorithm.isValid() || !decodeAlgorithm.isValid())
		throw "Can't perform transformation on String object because provided algorithm is invalid.";
	Encoder encoder(encodeAlgorithm, decodeAlgorithm);
	return FString(encoder.decode(base));
}

std::vector<FString> FString::split(std::string sfind, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	std::vector<FString> result;
	if (!contains(sfind, ignoreCase) || occurences < 1)
		return result;
	if (!fromBegin && !all)
	{
		std::reverse(sfind.begin(), sfind.end());
		FString inverted = invert();
		result = inverted.split(sfind, ignoreCase, all, occurences);
		for (unsigned int i = 0; i < result.size(); i++)
			result[i] = result[i].invert();
		std::reverse(result.begin(), result.end());
		return result;
	}
	else
	{
		result.push_back(substring(0, indexOf(sfind, ignoreCase)));
		int nextStringPos = indexOf(sfind, ignoreCase) + sfind.length();
		if ((unsigned)nextStringPos == base.length())
		{
			result.push_back(FString(""));
			return result;
		}
		result.push_back(substring(nextStringPos, base.length()));
		if (all || occurences > 1)
		{
			while (result.back().contains(sfind, ignoreCase) && (all || occurences > 1 ))
			{
				std::vector<FString> temp = result.back().split(sfind, ignoreCase);
				result.pop_back();
				for (FString index : temp)
					result.push_back(index);
				occurences --;
			}
		}
	}
	return result;
}

std::vector<FString> FString::split(FString sfind, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	return split(sfind.toStdString(), ignoreCase, all, occurences, fromBegin);
}

std::vector<std::string> FString::getSplits(std::string find, bool ignoreCase, int after, bool all, int occurences, bool fromBegin)
{
	std::vector<std::string> result;
	std::vector<FString> splits = split(find, ignoreCase, all, occurences, fromBegin);
	for (FString index : splits)
	{
		if (after > 0)
		{
			after --;
			continue;
		}
		result.push_back(index.toStdString());
	}
	return result;
}

std::vector<std::string> FString::getRanges(std::string first, std::string second, bool all, int occurences, bool fromBegin)
{
	std::vector<std::string> result;
	std::string target = base;
	if (!fromBegin)
		std::reverse(target.begin(), target.end());
	FString ftarget = FString(target);
	std::vector<unsigned int> startIndices = ftarget.findAll(first, 0);
	std::vector<unsigned int> endIndices = ftarget.findAll(second, 0);
	if (endIndices.size() == startIndices.size() - 1)
		endIndices.push_back(base.length() - 1);
	if (startIndices.size() < endIndices.size() || endIndices.size() < startIndices.size() - 1)
	{
		std::cout << "Error in FString::getRanges. Discrepancy between number of first delimiter and second delimiter matches: " << startIndices.size() << " vs " << endIndices.size() << " delimiters: " << first << " " << second << "in string " << base << std::endl;
		return {};
	}
	if (first == second)
	{
		for (unsigned int i = 0; i < startIndices.size();)
		{
			if ((startIndices.size() + i) % 2 == 1)
				startIndices.erase(startIndices.begin() + i);
			else
			{
				endIndices.erase(endIndices.begin() + i);
				++i;
			}
		}
	}
	if (!fromBegin)
	{
		for (unsigned int& index : startIndices)
			index = target.length() - 1 - index;
		for (unsigned int& index : endIndices)
			index = target.length() - 1 - index;
	}
	for (int i = 0; i < startIndices.size() && i < endIndices.size() && (all || i < occurences); i++)
	{
		if (startIndices[i] == base.length() - 1 || startIndices[i] >= endIndices[i] - 1)
			break;
		std::string str = base.substr(startIndices[i] + 1, endIndices[i] - startIndices[i] - 1);
		result.push_back(str);
	}
	return result;
}

std::vector<std::string> FString::getArray(std::string firstIndex, std::string secondIndex, std::string delimiter, bool force, int occurences, bool fromBegin, bool ignorecase)
{
	std::string target = base;
	int first = indexOf(firstIndex, ignorecase, 0, occurences, fromBegin);
	int second = indexOf(secondIndex, ignorecase, first + 1, 1, true);
	if (first == -1 || (!force && second == -1))
	{
		std::cout << "Error in FString::getArray. firstIndex " << firstIndex << " or secondIndex " << secondIndex << " could not be found in string " << base << std::endl;
		return {};
	}
	if (second == -1)
		second = base.length();
	if (second <= first)
	{
		std::cout << "Error in FString::getArray. secondIndex " << secondIndex << "found at pos: " << second << " is equal to or smaller than firstIndex " << firstIndex << " found at pos " << first << " for string " << base << std::endl;
		return {};
	}
	target = FString(target).substring(first + 1, second).toStdString();
	if (!FString(target).contains(delimiter, ignorecase))
		return {target};
	return FString(target).getSplits(delimiter, ignorecase, 0, true);
}
