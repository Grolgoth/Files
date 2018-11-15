#include "algorithmz.h"
#include "String.h"
#include <cmath>

/**
Represents an algorithm, based on the algorithm parameter.
This should be of format "x = 'int/x' 'operator' 'int/x' 'operator' etc"
It is possible to encase parts of the string in curved brackets to specify the order of operations.
Operators can be of following types: +, -, *, /, %, ^, R(root), !
For powers, roots and faculty operations write the operator behind the numer,
followed by the power (if applicable): 3^2, 27R3, 6! (27R3 = 3).
Don't go crazy on the numbers (9999999999999999999999! will probably crash due to memory)
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

std::string toString(char c)
{
	std::string result = "";
	result += c;
	return result;
}

int readDigit(int target, std::string algoString, int* pos, int len)
{
	char c = algoString[*pos];
	++ *pos;
	if (c == 'X')
		return target;
	else
	{
		int result = atoi(toString(c).c_str());
		while (*pos < len && isdigit(algoString[*pos]))
		{
			c = algoString[*pos];
			++ *pos;
			result = result * 10 + atoi(toString(c).c_str());
		}
		return result;
	}
}

bool noFurtherOperation(std::string algoString, int pos, int len, char operation)
{
	if (operation == 'R' || operation == '^' || operation == '!')
		return true;
	if (takesPrevalenceOver(operation, algoString[pos]))
		return true;
	return pos + 1 >= len;
}

struct Operation {
	Operation(char operation, int firstVal, int secondVal, bool negativ) :
		operation(operation), firstValue(firstVal), secondValue(secondVal), negative(negativ)
	{
		if (negative)
			firstValue = firstValue * -1;
	}
	char operation;
	int firstValue, secondValue;
	bool negative;
};

int doOperation(Operation operation)
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
		return (operation.firstValue % operation.secondValue) * negative;
	case '^':
		return pow((double)operation.firstValue, (double)operation.secondValue) * negative;
	case 'R':
		return pow((double)operation.firstValue, 1/(double)operation.secondValue) * negative;
	default:
		return 1 * negative;
	}
}

bool checkBracket(bool open, std::string algoString, int* pos)
{
	char type = '(';
	if (!open)
		type = ')';
	if (algoString[*pos] == type)
	{
		++ *pos;
		return true;
	}
	return false;
}

Operation getOperation(int target, std::string algoString, int pos, int len, int* firstValue, bool isNegative)
{
	int secondValue = 0;
	bool bracket = false;
	bool closeBracket = false;
	while (pos != len)
	{
		bracket = checkBracket(true, algoString, &pos);
		if (firstValue == nullptr)
			firstValue = new int(readDigit(target, algoString, &pos, len));
		char operation = algoString[pos];
		pos ++;
		bool negative = operation == '-';
		bracket = checkBracket(true, algoString, &pos);
		secondValue = readDigit(target, algoString, &pos, len);
		closeBracket = checkBracket(false, algoString, &pos);
		if (closeBracket || (!bracket && noFurtherOperation(algoString, pos, len, operation)))
		{
			if (pos + 1 < len)
				return getOperation(target, algoString, pos, len, new int(doOperation(Operation(operation, *firstValue, secondValue, false))), isNegative);
			else
				return Operation(operation, *firstValue, secondValue, isNegative);
		}
		else
			return Operation(operation, *firstValue, doOperation(getOperation(target, algoString, pos, len, &secondValue, negative)), isNegative);
	}
	throw "Could not create operation because algorithm is incomplete";
}

bool Algorithm::isValid()
{
	bool gotRef = false;
	bool digitOk = true;
	bool bracketOk = true;
	int brackets = 0;
	String algoString(algorithm);
	algoString = algoString.replace(" ", "");
	if (!String(algorithm).startsWith("X="))
		return false;
	algoString = algoString.substring(2);
	int len = algoString.toStdString().length();
	int pos = 0;
	while (pos != len)
	{
		char c = algoString.toStdString()[pos];
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
		std::string algoString = String(algorithm).replace(" ", "").substring(2).toStdString();
		Operation operation = getOperation(target, algoString, 0, algoString.length(), nullptr, false);
		return doOperation(operation);
	}
	else
	{
		std::string error = "Can't perform algorithm, because the Algorithm provided is unvalid: " + algorithm;
		throw error.c_str();
	}
}
