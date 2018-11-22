#include "String.h"
#include <regex>
#include <math.h>
#include "encoder.h"

String String::fromVector(std::vector<std::string> vect, std::string appendElements)
{
	std::string result = "";
	for (std::string index : vect)
		result += index + appendElements;
	return String(result);
}

String String::fromVector(std::vector<String> vect, std::string appendElements)
{
	std::string resultstr = "";
	for (String index : vect)
		resultstr += index.toStdString() + appendElements;
	return String(resultstr);
}

String String::fromInt(int target)
{
	std::string result = "";
	if (target < 0)
		target = target * -1;
	char buffer[10];
	int digits = 1;
	while (target / pow(10, digits - 1) >= 10)
		digits++;
	int originalDigits = digits;
	for (int i = 1; i < 11; i++)
	{
		double d = pow(10, digits - 1) * i;
		if (target - d < 0)
		{
			buffer[originalDigits - digits] = i + 47;
			target -= pow(10, digits - 1) * (i-1);
			digits --;
			if (digits != 0)
				i = 0;
			else
				break;
		}
	}
	for (int i = 0; i < originalDigits; i ++)
		result += buffer[i];
	return String(result);
}

String::String(std::string base)
{
	this->base = base;
}

String String::operator+(const String& b)
{
	return String(base + b.base);
}

bool String::operator!=(const String& b)
{
	return base != b.base;
}

bool String::toBool()
{
	if (base == "true")
		return true;
	else if(base == "false")
		return false;
	else
		throw "Can't convert " + base + " to a boolean because it is not equal to either false, or true";
}

bool String::contains(std::string sfind, bool ignoreCase)
{
	return indexOf(sfind, ignoreCase) > -1;
}

bool String::contains(String sfind, bool ignoreCase)
{
	return indexOf(sfind, ignoreCase) > -1;
}

bool String::containsRegex(std::string sregex)
{
	return std::regex_match(base, std::regex(sregex));
}

bool String::startsWith(std::string prefix, bool ignoreCase)
{
	return indexOf(prefix, ignoreCase) == 0;
}

bool String::startsWith(String prefix, bool ignoreCase)
{
	return startsWith(prefix.toStdString());
}

bool String::endsWith(std::string suffix, bool ignoreCase)
{
	if (suffix.length() > base.length())
		return false;
	return indexOf(suffix, ignoreCase) == (int)(base.length() - suffix.length());
}

bool String::endsWith(String suffix, bool ignoreCase)
{
	return endsWith(suffix.toStdString());
}

int String::compare(std::string other)
{
	return base.compare(other);
}

int String::compare(String other)
{
	return compare(other.toStdString());
}

int String::indexOf(std::string sfind, bool ignoreCase, unsigned int fromPos, unsigned int occurences, bool fromBegin)
{
	if (!fromBegin)
	{
		if (invert().indexOf(sfind, ignoreCase, fromPos, occurences) == -1)
			return -1;
		return base.length() - 1 - invert().indexOf(sfind, ignoreCase, fromPos, occurences);
	}
	int result = -1;
	if (base.length() < sfind.length() || occurences < 1)
		return result;
	for (unsigned int i = fromPos; i < base.length(); i++)
	{
		for (unsigned int j = 0; j < sfind.length(); j ++)
		{
			if (ignoreCase && toupper(base[i + j]) != toupper(sfind[j]))
				break;
			else if (base[i + j] != sfind[j])
				break;
			if (j == sfind.length() - 1)
			{
				result = i;
				occurences --;
			}
		}
		if (occurences == 0)
			break;
	}
	if (occurences != 0)
		return -1;
	return result;
}

int String::indexOf(String sfind, bool ignoreCase, unsigned int fromPos, unsigned int occurences, bool fromBegin)
{
	return indexOf(sfind.toStdString(), ignoreCase, fromPos);
}

int String::toInt()
{
    try
    {
		return atoi(base.c_str());
    }
    catch(...)
    {
        if(!isdigit(base[0]))
			throw "Can't convert " + base + " to an integer, because it doesn't start with a digit.";
		else
			throw "Can't convert " + base + " to an integer. Unknown error during conversion.";
    }
}

char* String::toCharArray()
{
	char* carray = new char[base.length()];
	for (unsigned int i = 0; i < base.length(); i ++)
		carray[i] = base[i];
	return carray;
}

std::string String::toStdString() const
{
	return base;
}

String String::substring(unsigned int ibegin, int iend)
{
	unsigned int uiend = base.length();
	if (iend >= 0)
		uiend = (unsigned)iend;
	std::string result = "";
	if (ibegin >= base.length() || uiend <= ibegin || uiend > base.length())
		return result;
	for (unsigned int i = ibegin; i < uiend; i++)
		result += base[i];
	String resultToCopy(result);
	return resultToCopy;
}

String String::invert(unsigned int ibegin, int iend)
{
	unsigned int uiend = base.length();
	if (iend >= 0)
		uiend = (unsigned)iend;
	std::string resultstr = "";
	if (ibegin >= base.length() || uiend <= ibegin || uiend > base.length())
		return String(resultstr);
	char* invSeg = substring(ibegin, uiend).toCharArray();
	for (int i = uiend - ibegin - 1; i > -1; i --)
		resultstr += invSeg[i];
	String result("");
	if (ibegin > 0)
		result = result + substring(0, ibegin);
	result = result + String(resultstr);
	result = result + substring(uiend);
	return result;
}

std::vector<int> String::findAll(std::string find, bool ignoreCase, int fromPos)
{
	std::vector<int> result;
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

String String::replace(std::string sfind, std::string target, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	std::string result = "";
	std::vector<String> split = this->split(sfind, ignoreCase, all, occurences, fromBegin);
	if (split.empty())
		return *this;
	for (String index : split)
	{
		result += index.toStdString();
		if (index != split.back())
			result += target;
	}
	return String(result);
}

String String::encode(Algorithm algorithm)
{
	if (!algorithm.isValid())
		throw "Can't perform transformation on String object because provided algorithm is invalid.";
	Encoder encoder(algorithm, Algorithm("x=1*x"));
	return String(encoder.encode(base));
}

String String::decode(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm)
{
	if (!encodeAlgorithm.isValid() || !decodeAlgorithm.isValid())
		throw "Can't perform transformation on String object because provided algorithm is invalid.";
	Encoder encoder(encodeAlgorithm, decodeAlgorithm);
	return String(encoder.decode(base));
}

std::vector<String> String::split(std::string sfind, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	std::vector<String> result;
	if (!contains(sfind, ignoreCase) || occurences < 1)
		return result;
	if (!fromBegin && !all)
	{
		String inverted = invert();
		result = inverted.split(sfind, ignoreCase, all, occurences);
		for (unsigned int i = 0; i < result.size(); i ++)
			result[i] = result[i].invert();
		std::reverse(result.begin(), result.end());
		return result;
	}
	else
	{
		result.push_back(substring(0, indexOf(sfind, ignoreCase)));
		int nextStringPos = indexOf(sfind, ignoreCase) + sfind.length();
		if ((unsigned)nextStringPos == base.length())
			return result;
		result.push_back(substring(nextStringPos, base.length()));
		if (all || occurences > 1)
		{
			while (result.back().contains(sfind, ignoreCase) && (all || occurences > 1 ))
			{
				std::vector<String> temp = result.back().split(sfind, ignoreCase);
				result.pop_back();
				for (String index : temp)
					result.push_back(index);
				occurences --;
			}
		}
	}
	return result;
}

std::vector<String> String::split(String sfind, bool ignoreCase, bool all, int occurences, bool fromBegin)
{
	return split(sfind.toStdString(), ignoreCase, all, occurences, fromBegin);
}
