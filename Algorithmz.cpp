#include "algorithmz.h"
#include "fstring.h"
#include <cmath>

/**
Represents an algorithm, based on the algorithm parameter.
This should be of format "x = 'int/x' 'operator' 'int/x' 'operator' etc"
It is possible to encase parts of the string in curved brackets to specify the order of operations.
Operators can be of following types: +, -, *, /, %, ^, R(root), !
For powers, roots and faculty operations write the operator behind the numer,
followed by the power (if applicable): 3^2, 27R3, 6! (27R3 = 3).
Don't go crazy on the numbers (think about MAX_INT)
Decimals, nested brackets, and negative numbers are not allowed,
and results of operations with / and R will be truncated.
*/
Algorithm::Algorithm(std::string algorithm) : algorithm(algorithm)
{
	for (unsigned int i = 0; i < algorithm.length(); i++)
		this->algorithm[i] = toupper(algorithm[i]);
}

bool charIsLegal(char target)
{
	if (isdigit(target))
		return true;
	switch(target)
	{
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '^':
		case 'R':
		case '(':
		case ')':
		case 'X':
			return true;
		default:
			return false;
	}
}

bool needsReferenceUpFront(char target)
{
	if (target == '+' || target == '-' || target == '*' || target == '/' || target == '%' || target == '^' || target == 'R' || target == ')')
		return true;
	return false;
}

bool takesPrevalenceOver(char operation, char target)
{
	if (target == 'R' || target == '^' || target == '!')
		return false;
	if (operation == '*')
		return true;
	if (operation == '/' && target != '*')
		return true;
	if (operation == '%' && target != '*')
		return true;
	return false;
}

std::string toFString(char c)
{
	std::string result = "";
	result += c;
	return result;
}

int readDigit(int target, std::string algoFString, int* pos, int len)
{
	char c = algoFString[*pos];
	++ *pos;
	if (c == 'X')
		return target;
	else
	{
		int result = atoi(toFString(c).c_str());
		while (*pos < len && isdigit(algoFString[*pos]))
		{
			c = algoFString[*pos];
			++ *pos;
			result = result * 10 + atoi(toFString(c).c_str());
		}
		return result;
	}
}

bool noFurtherOperation(std::string algoFString, int pos, int len, char operation)
{
	if (operation == 'R' || operation == '^')
		return true;
	if (takesPrevalenceOver(operation, algoFString[pos]))
		return true;
	return pos + 1 >= len;
}

struct Operation {
	Operation(char operation, double firstVal, double secondVal, bool negativ) :
		operation(operation), firstValue(firstVal), secondValue(secondVal), negative(negativ)
	{
		if (negative && operation != 'R' && operation != '^')
			firstValue = firstValue * -1;
	}
	char operation;
	double firstValue, secondValue;
	bool negative;
};

double doOperation(Operation operation)
{
	int negative = 1;
	if (operation.negative)
		negative = -1;
	switch(operation.operation)
	{
	case '+':
		return (operation.firstValue + operation.secondValue) * negative;
	case '-':
		return (operation.firstValue - operation.secondValue) * negative;
	case '*':
		return (operation.firstValue * operation.secondValue) * negative;
	case '/':
		return (operation.firstValue / operation.secondValue) * negative;
	case '%':
		return (fmod(operation.firstValue, operation.secondValue)) * negative;
	case '^':
		return pow(operation.firstValue, operation.secondValue);
	case 'R':
		return pow(operation.firstValue, 1/operation.secondValue);
	default:
		return 1 * negative;
	}
}

bool checkBracket(bool open, std::string algoFString, int* pos)
{
	char type = '(';
	if (!open)
		type = ')';
	if (algoFString[*pos] == type)
	{
		++ *pos;
		return true;
	}
	return false;
}

Operation getOperation(int target, std::string algoFString, int pos, int len, double* firstValue, bool isNegative)
{
	double secondValue = 0;
	bool bracket = false;
	bool closeBracket = false;
	while (pos != len)
	{
		bracket = checkBracket(true, algoFString, &pos);
		if (firstValue == nullptr)
			firstValue = new double(readDigit(target, algoFString, &pos, len));
		char operation = algoFString[pos];
		pos ++;
		bool negative = operation == '-';
		bracket = checkBracket(true, algoFString, &pos);
		secondValue = readDigit(target, algoFString, &pos, len);
		closeBracket = checkBracket(false, algoFString, &pos);
		if (closeBracket || (!bracket && noFurtherOperation(algoFString, pos, len, operation)))
		{
			if (pos + 1 < len)
				return getOperation(target, algoFString, pos, len, new double(doOperation(Operation(operation, *firstValue, secondValue, false))), isNegative);
			else
				return Operation(operation, *firstValue, secondValue, isNegative);
		}
		else
			return Operation(operation, *firstValue, doOperation(getOperation(target, algoFString, pos, len, &secondValue, negative)), isNegative);
	}
	throw "Could not create operation because algorithm is incomplete";
}

bool Algorithm::isValid()
{
	bool gotRef = false;
	bool digitOk = true;
	bool bracketOk = true;
	int brackets = 0;
	FString algoFString(algorithm);
	algoFString = algoFString.replace(" ", "");
	if (!FString(algorithm).startsWith("X="))
		return false;
	algoFString = algoFString.substring(2);
	int len = algoFString.toStdString().length();
	int pos = 0;
	while (pos != len)
	{
		char c = algoFString.toStdString()[pos];
		pos ++;
		if (!charIsLegal(c) || (needsReferenceUpFront(c) != gotRef && !isdigit(c)) || (isdigit(c) && !digitOk))
			return false;
		digitOk = !(c == 'X');
		if (c == '(')
		{
			if (!bracketOk)
				return false;
			brackets ++;
			bracketOk = false;
			continue;
		}
		if (c == ')')
		{
			if (!bracketOk)
				return false;
			brackets --;
			bracketOk = false;
			digitOk = false;
			gotRef = true;
			continue;
		}
		bracketOk = true;
		if (isdigit(c) || c == 'X')
			gotRef = true;
		else
			gotRef = false;
	}
	if (brackets != 0)
		return false;
	return true;
}

int Algorithm::execute(int target)
{
	if (isValid())
	{
		std::string algoString = FString(algorithm).replace(" ", "").substring(2).toStdString();
		Operation operation = getOperation(target, algoString, 0, algoString.length(), nullptr, false);
		return doOperation(operation);
	}
	else
	{
		std::string error = "Can't perform algorithm, because the Algorithm provided is unvalid: " + algorithm;
		throw error.c_str();
	}
}

int Algorithm::getMaxWithinRange(int lowest, int highest)
{
	if (lowest >= highest)
		throw "Can't check for highest value in algorithm because lowest value provided >= highest value provided in given range";
	int result = execute(lowest);
	for (int i = lowest + 1; i < highest + 1; i++)
	{
		int outcome = execute(i);
		if (outcome > result)
			result = outcome;
	}
	return result;
}
