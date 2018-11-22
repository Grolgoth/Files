#include "plainsettings.h"
#include "../defines.h"
#include <algorithm>

PlainSettings::PlainSettings(File file, bool isEncoded, Encoder* encoder) : Settings(file, isEncoded, encoder)
{
	if (!valid())
		throw std::string("Error creating PlainSettings file: " + error);
}

PlainSettings::~PlainSettings()
{

}

namespace validate
{

bool inVector(std::vector<std::string>* v, std::string x)
{
	return std::find(v->begin(), v->end(), x) != v->end();
}

bool isReservedLine(std::vector<std::string>* reservedLines, std::string linekey, std::string currentSet)
{
	for(std::string index : *reservedLines)
	{
		String indexstr(index);
		if (indexstr.endsWith(String(currentSet), false))
			if (indexstr.split("=")[0].toStdString() == linekey)
				return true;
	}
	return false;
}

int getSetIndex(std::string target)
{
	if (target[0] == '0')
		return 0;
	String setstr(target);
	if (setstr.contains(" "))
	{
		setstr = setstr.split(" ", true, false, 1, false)[1];
	}
	return setstr.toInt();
}

std::string nextInSets(std::string currentSet, std::string nextSet)
{
	int start = getSetIndex(nextSet);
	if (currentSet == "0")
		return String::fromInt(start + 1).toStdString();
	return currentSet + " " + String::fromInt(start + 1).toStdString();

}

std::string previousSet(std::string current)
{
	String currentSetstr(current);
	if (currentSetstr.contains(" "))
		return currentSetstr.split(" ", true, false, 1, false)[0].toStdString();
	return "0";
}

void nextSet(std::vector<std::string>* sets, std::string* currentSet)
{
	std::string nextSet = nextInSets(*currentSet, "0");
	while (inVector(sets, nextSet))
		nextSet = nextInSets(*currentSet, nextSet);
	*currentSet = nextSet;
	sets->push_back(*currentSet);
}

bool resolveNormalLine(String linestr, std::string currentSet, std::vector<std::string>* reservedLines, int pos, std::string* error)
{
	if (linestr.split("=").size() < 1 || linestr.split("=").size() > 2)
	{
		*error = "Found line with less or more than one equals (=) character. For this format we use a 'key=value' based format. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	if (linestr.split("=").size() == 1)
	{
		*error = "Found line with missing key or value. Make sure neither side of the equals sign (=) is empty. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	linestr = linestr + String(currentSet);
	if (isReservedLine(reservedLines, linestr.split("=")[0].toStdString(), currentSet))
	{
		*error = "Found line with same key as other line, within the same set. Lines with the same key are allowed but not when they are both in the same set. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	reservedLines->push_back(linestr.toStdString());
	return true;
}

bool closeSet(String linestr, std::string* currentSet, int pos, std::string* error)
{
	String currentSetstr(*currentSet);
	if (!currentSetstr.contains(" ") && linestr.toStdString() == "~")
	{
		if (*currentSet == "0")
			*error = "Found ~ character, which is used for closing subsets, while no subsets or even global sets were opened. Line: " + String::fromInt(pos).toStdString();
		else
			*error = "Found ~ character, which is used for closing subsets, while no subsets were opened. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	if (currentSetstr.contains(" ") && linestr.toStdString() == "")
	{
		*error = "Found empty line, while no global set was previously declared. An empty line can only be used to declare the end of a global set (a set that's not part of another set). Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	*currentSet = previousSet(currentSetstr.toStdString());
	return true;
}

bool openSet(String linestr, std::string* currentSet, std::vector<std::string>* sets, std::vector<std::string>* reservedSets, int pos, std::string* error)
{
	nextSet(sets, currentSet);
	if (!linestr.endsWith("]"))
	{
		*error = "Found opening bracket ([) to declare subset without corresponding closing bracket (]) at the end of the line. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	std::string reservedstr = linestr.substring(1, linestr.toStdString().length() - 1).toStdString() + previousSet(*currentSet);
	if (reservedstr == previousSet(*currentSet) || inVector(reservedSets, reservedstr))
	{
		if (reservedstr != previousSet(*currentSet))
			*error = "Set was opened but there was already another set declared within the same set scope. Set-name: " + reservedstr + ". Line: " + String::fromInt(pos).toStdString();
		else
			*error = "Cannot declare a set with an empty name (\"\"). Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	reservedSets->push_back(reservedstr);
	return true;
}

bool resolveLine(std::string line, std::string* currentSet, std::vector<std::string>* sets, std::vector<std::string>* reservedSets, std::vector<std::string>* reservedLines, int pos, std::string* error)
{
	String linestr(line);
	#ifdef OS_Windows
		if (linestr.endsWith("\r"))
			linestr = linestr.substring(0, linestr.toStdString().length() - 1);
	#endif // OS_Windows
	if (linestr.contains("."))
	{
		*error = "Found dot character (.). This character is reserved for this format and should not be used. Line: " + String::fromInt(pos).toStdString();
		return false;
	}
	if (linestr.startsWith("["))
		return openSet(linestr, currentSet, sets, reservedSets, pos, error);
	if (linestr.toStdString() == "~" || linestr.toStdString() == "")
		return closeSet(linestr, currentSet, pos, error);
	return resolveNormalLine(linestr, *currentSet, reservedLines, pos, error);
}

} // validate

bool PlainSettings::valid()
{
	std::vector<std::string> lines = file.getLines(false);
	if (encoded)
		file.encode(encoder->getEncodeAlgorithm());
	file.close();
	sets.push_back("0");
	std::string currentSet = sets[0];
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		if (!validate::resolveLine(lines[i], &currentSet, &sets, &reservedSets, &reservedLines, i + 1, &error))
			return false;
	}
	return true;
}

namespace get
{

std::string findLine(std::string line, std::string currentSet, std::vector<std::string>* reservedLines)
{
	for (std::string index : *reservedLines)
	{
		String indexstr(index);
		if (indexstr.endsWith(String(currentSet), false))
			if (indexstr.split("=")[0].toStdString() == line)
				return indexstr.split("=")[1].toStdString();
	}
	return "";
}

bool inSets(String setName, std::string* currentSet, std::vector<std::string>* reservedSets, std::vector<std::string>* sets)
{
	for (unsigned int i = 0; i < reservedSets->size(); i++)
	{
		String indexstr((*reservedSets)[i]);
		if (indexstr.endsWith(*currentSet, false) && indexstr.startsWith(setName, false))
		{
			*currentSet = (*sets)[i + 1];
			return true;
		}
	}
	return false;
}

std::string resolveKey(String keystr, std::string* currentSet, std::vector<std::string>* reservedSets, std::vector<std::string>* reservedLines, std::vector<std::string>* sets)
{
	if (!keystr.contains("."))
	{
		String result(findLine(keystr.toStdString(), *currentSet, reservedLines));
		if (result.toStdString() == "")
			return result.toStdString();
		return result.split(*currentSet, false)[0].toStdString();
	}
	String curset = keystr.split(".")[0];
	if (inSets(curset, currentSet, reservedSets, sets))
		return resolveKey(keystr.split(".")[1], currentSet, reservedSets, reservedLines, sets);
	return "";
}

}

std::vector<std::string> PlainSettings::get(std::vector<std::string> keys)
{
	std::vector<std::string> result;
	for (std::string key : keys)
	{
		String keystr(key);
		std::string currentSet = sets[0];
		result.push_back(get::resolveKey(keystr, &currentSet, &reservedSets, &reservedLines, &sets));
	}
	return result;
}

namespace write
{

void writeIt(String key, std::string value, File* file, int nested)
{
	for(int i = 0; i < nested; i ++)
		key = key.split(".")[1];
	int closes = 0;
	while (key.contains("."))
	{
		file->write("[" + key.split(".")[0].toStdString() + "]\n");
		key = key.split(".")[1];
		closes ++;
	}
	file->write(key.toStdString() + "=" + value + "\n");
	for (int i = 0; i < closes; i ++)
	{
		if (i == closes -1 && nested == 0)
			file->write("\n");
		else
			file->write("~\n");
	}
}

void getToRightPosInFile(std::string currentSet, std::vector<std::string>* sets, File* file)
{
	if (currentSet != (*sets)[0])
	{
		unsigned int setsInt = 0;
		for (unsigned int i = 1; i < sets->size(); i++)
		{
			if ((*sets)[i] == currentSet)
			{
				std::string line;
				do
				{
					if (setsInt == i)
						break;
					line = file->readline();
					if (line[0] == '[')
						setsInt ++;
				} while (line != "");
				break;
			}
		}
	}
	else
		file->setPos(file->getSize());
}

bool checkBasis(std::string key, std::string value)
{
	if (key == "" || value == "")
		return false;
	String keystr(key);
	String valuestr(value);
	if (keystr.startsWith(".") || keystr.endsWith(".") || keystr.contains("=") || valuestr.contains(".") || valuestr.contains("="))
		return false;
	return true;
}

}

bool PlainSettings::write(std::string key, std::string value, bool overwriteIfExists)
{
	if (!write::checkBasis(key, value))
		return false;
	String keystr(key);
	std::string currentSet = sets[0];
	if(get::resolveKey(String(key), &currentSet, &reservedSets, &reservedLines, &sets) != "" && !overwriteIfExists)
		return false;
	int nested = String(currentSet).findAll(" ", false, 0).size();
	if (currentSet != sets[0])
		nested ++;
	file.open();
	if (encoded)
	{
		file.decode(encoder->getEncodeAlgorithm(), encoder->getDecodeAlgorithm());
		file.setPos(0, false);
	}
	write::getToRightPosInFile(currentSet, &sets, &file);
	write::writeIt(keystr, value, &file, nested);
	return valid();
}

bool PlainSettings::exists(std::string key)
{
	std::string currentSet = sets[0];
	return get::resolveKey(String(key), &currentSet, &reservedSets, &reservedLines, &sets) != "";
}
