#include "algorithm.h"
#include "String.h"

/**
Represents an algorithm, based on the algorithm parameter.
This should be of format "x = 'int/x' 'operator' 'int/x' 'operator' etc"
It is possible to encase parts of the string in curved brackets to specify the order of operations.
Operators can be of following types: +, -, *, /, %, ^, R(root), !
For powers, roots and faculty operations write the operator behind the numer,
followed by the power (if applicable): 3^2, 27R3, 6! (27R3 = 3).
Don't go crazy on the numbers (9999999999999999999999! will probably crash due to memory)
Decimals are not allowed, and it is wise not to use / and R when they could produce decimals, since
this might lead to unpredictable behaviour.
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
	if (target == '+' || target == '-' || target == '*' || target == '/' || target == '%' || target == '^')
		return true;
	return false;
}

bool Algorithm::isValid()
{
	bool gotRef = false;
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
		if (!charIsLegal(c) || (needsReferenceUpFront(c) != gotRef))
			return false;
		pos ++;
		if (c == '(')
		{
			brackets ++;
			continue;
		}
		if (c == ')')
		{
			brackets --;
			continue;
		}
		if (isdigit(c) || c == 'x' || c == '!' || c == 'R' || c == '^')
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
		return target;
	}
	else
		throw "Can't perform algorithm, because the Algorithm provided is unvalid: " + algorithm;
}
