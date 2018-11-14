#include "algorithmz.h"
#include "String.h"

/**
Represents an algorithm, based on the algorithm parameter.
This should be of format "x = 'int/x' 'operator' 'int/x' 'operator' etc"
It is possible to encase parts of the string in curved brackets to specify the order of operations.
Operators can be of following types: +, -, *, /, %, ^, R(root), !
For powers, roots and faculty operations write the operator behind the numer,
followed by the power (if applicable): 3^2, 27R3, 6! (27R3 = 3).
Don't go crazy on the numbers (9999999999999999999999! will probably crash due to memory)
Decimals and negative numbers are not allowed, and it is wise not to use / and R when they could
produce decimals, since this might lead to unpredictable behaviour.
*/
Algorithm::Algorithm(std::string algorithm) : algorithm(algorithm)
{

}

bool charIsLegal(char target)
{
	if (isdigit(target))
		return true;
	switch(toupper(target))
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
		case 'x':
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

std::string toString(char c)
{
	std::string result = "";
	result += c;
	return result;
}

int readDigit(int target, std::string algorithm, int* pos, int len)
{
	char c = algorithm[*pos];
	pos ++;
	if (c == 'x')
		return target;
	else
	{
		int result = atoi(toString(c).c_str());
		while (*pos + 1 < len && isdigit(algorithm[++ *pos]))
		{
			c = algorithm[*pos];
			result = result * 10 + atoi(toString(c).c_str());
		}
		return result;
	}
}

bool noFurtherOperation(std::string algoString, int pos, int len, char operation)
{
	return true;
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
	switch(operation.operation)
	{

	}
	//if (operation.negative)
		//return result * -1;
	return 1;
}

Operation getOperation(int target, std::string algoString, int pos, int len, int* firstValue, bool isNegative)
{
	int secondValue = 0;
	bool bracket = false;
	while (pos != len)
	{
		if (firstValue == nullptr)
			firstValue = new int(readDigit(target, algoString, &pos, len));
		char operation = algoString[pos];
		pos ++;
		bool negative = operation == '-';
		if (algoString[pos] == '(')
		{
			pos ++;
			bracket = true;
		}
		secondValue = readDigit(target, algoString, &pos, len);
		if (!bracket || noFurtherOperation(algoString, pos, len, operation))
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
	bool bracketOk = false;
	int brackets = 0;
	String algoString(algorithm);
	algoString = algoString.replace(" ", "");
	if (String(algorithm).startsWith("x="))
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
		digitOk = !(c == 'x');
		bracketOk = !(c == '(' || c == ')');
		if (c == '(')
		{
			if (!bracketOk)
				return false;
			brackets ++;
			continue;
		}
		if (c == ')')
		{
			if (!bracketOk)
				return false;
			brackets --;
			digitOk = false;
			gotRef = true;
			continue;
		}
		if (isdigit(c) || c == 'x')
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
		std::string algoString = String(algorithm).substring(2).toStdString();
		Operation operation = getOperation(target, algoString, 0, algoString.length(), nullptr, false);
		return doOperation(operation);
	}
	else
		throw "Can't perform algorithm, because the Algorithm provided is unvalid: " + algorithm;
}
