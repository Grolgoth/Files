#include "plainsettings.h"
#include "../defines.h"
#include <algorithm>

PlainSettings::PlainSettings(File file, bool isEncoded, Encoder* encoder) : Settings(file, isEncoded, encoder)
{
	if (!valid())
	{
		std::cout << error << std::endl;
		throw std::string("Error creating PlainSettings file: " + error);
	}

	/* for if you want to know what's going on:
	std::cout << "RSL:" << std::endl;
	for (std::string str : reservedLines)
		std::cout << str << std::endl;
	std::cout << "\n\nRS:" << std::endl;
	for (std::string str : reservedSets)
		std::cout << str << std::endl;
	std::cout << "\n\nS:" << std::endl;
	for (std::string str : sets)
		std::cout << str << std::endl;
	*/

}

PlainSettings::~PlainSettings()
{
	//file is closed in parent destructor. Encoder is deleted there as well
}

namespace local
{
	std::vector<std::string> split(const std::string& delimiter, const std::string& base)
	{
		std::vector<std::string> result;
		size_t start = 0;
		size_t end = base.find(delimiter);
		if (end == std::string::npos)
			return result;

		while (end != std::string::npos)
		{
			result.push_back(base.substr(start, end - start));
			start = end + delimiter.length();
			end = base.find(delimiter, start);
		}
		result.push_back(base.substr(start));

		return result;
	}

	bool endsWith(const std::string& str, const std::string& suffix)
	{
		if (str.length() >= suffix.length())
			return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
		else
			return false;
	}

	bool startsWith(const std::string& str, const std::string& prefix)
	{
		if (str.length() >= prefix.length())
			return str.compare(0, prefix.length(), prefix) == 0;
		else
			return false;
	}
}

namespace validate
{

bool inVector(std::vector<std::string>* v, std::string x)
{
	return std::find(v->begin(), v->end(), x) != v->end();
}

bool isReservedLine(std::vector<std::string>* reservedLines, std::string linekey, std::string currentSet)
{
	for(const std::string& index : *reservedLines)
	{
		size_t pos = index.find("=");
		if (pos != std::string::npos && index.substr(pos + 1, index.length()) == currentSet)
		{
			std::string indexstr = index.substr(0, pos);
			if (indexstr == linekey)
				return true;
		}
	}
	return false;
}

int getSetIndex(std::string target)
{
	if (target[0] == '0')
		return 0;
	size_t pos = target.rfind(" ");
	if (pos != std::string::npos)
	{
		target = target.substr(pos + 1, target.length());
	}
	return atoi(target.c_str());
}

std::string nextInSets(const std::string& currentSet, std::string nextSet)
{
	int start = getSetIndex(nextSet);
	if (currentSet == "0")
		return std::to_string(start + 1);
	return currentSet + " " + std::to_string(start + 1);

}

std::string previousSet(std::string current)
{
	size_t pos = current.rfind(" ");
	if (pos != std::string::npos)
		return current.substr(0, pos);
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

bool resolveNormalLine(std::string linestr, std::string currentSet, std::vector<std::string>* reservedLines, int pos, std::string* error)
{
	std::vector<std::string> split = local::split("=", linestr);
	if (split.size() < 1 || split.size() > 2)
	{
		*error = "Found line with less or more than one equals (=) character. For this format we use a 'key=value' based format. Line: " + std::to_string(pos);
		return false;
	}
	if (split.size() == 1)
	{
		*error = "Found line with missing key or value. Make sure neither side of the equals sign (=) is empty. Line: " + std::to_string(pos);
		return false;
	}
	linestr = linestr + "=" + currentSet;
	if (isReservedLine(reservedLines, split[0], currentSet))
	{
		*error = "Found line with same key as other line, within the same set. Lines with the same key are allowed but not when they are both in the same set. Line: " + std::to_string(pos);
		return false;
	}
	reservedLines->push_back(linestr);
	return true;
}

bool closeSet(std::string linestr, std::string* currentSet, int pos, std::string* error)
{
	std::string currentSetstr = *currentSet;
	size_t index = currentSetstr.find(" ");
	if (index == std::string::npos && linestr == "~")
	{
		if (*currentSet == "0")
			*error = "Found ~ character, which is used for closing subsets, while no subsets or even global sets were opened. Line: " + std::to_string(pos);
		else
			*error = "Found ~ character, which is used for closing subsets, while no subsets were opened. Line: " + std::to_string(pos);
		return false;
	}
	if (index != std::string::npos && linestr == "")
	{
		*error = "Found empty line, while no global set was previously declared. An empty line can only be used to declare the end of a global set (a set that's not part of another set). Line: " + std::to_string(pos);
		return false;
	}
	*currentSet = previousSet(currentSetstr);
	return true;
}

bool openSet(std::string linestr, std::string* currentSet, std::vector<std::string>* sets, std::vector<std::string>* reservedSets, int pos, std::string* error)
{
	nextSet(sets, currentSet);
	if (linestr.back() != ']')
	{
		*error = "Found opening bracket ([) to declare subset without corresponding closing bracket (]) at the end of the line. Line: " + std::to_string(pos);
		return false;
	}
	std::string reservedstr = linestr.substr(1, linestr.length() - 2) + "=" + previousSet(*currentSet);
	if (reservedstr == previousSet(*currentSet) || inVector(reservedSets, reservedstr))
	{
		if (reservedstr != previousSet(*currentSet))
			*error = "Set was opened but there was already another set declared within the same set scope. Set-name: " + reservedstr + ". Line: " + std::to_string(pos);
		else
			*error = "Cannot declare a set with an empty name (\"\"). Line: " + std::to_string(pos);
		return false;
	}
	reservedSets->push_back(reservedstr);
	return true;
}

bool resolveLine(std::string line, std::string* currentSet, std::vector<std::string>* sets, std::vector<std::string>* reservedSets, std::vector<std::string>* reservedLines, int pos, std::string* error)
{
	#ifdef OS_Windows
		if (!line.empty() && line.back() == ('\r'))
			line = line.substr(0, line.length() - 1);
	#endif // OS_Windows
	if (line.find(".") != std::string::npos)
	{
		*error = "Found dot character (.). This character is reserved for this format and should not be used. Line: " + std::to_string(pos);
		return false;
	}
	if (!line.empty() && line[0] == '[')
		return openSet(line, currentSet, sets, reservedSets, pos, error);
	if (line == "~" || line.empty())
		return closeSet(line, currentSet, pos, error);
	return resolveNormalLine(line, *currentSet, reservedLines, pos, error);
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
		size_t posLast  = index.rfind("=");
		size_t posFirst = index.find("=");
		if (posLast != std::string::npos && index.substr(posLast + 1) == currentSet)
			if (index.substr(0, posFirst) == line)
				return index.substr(posFirst + 1, posLast - posFirst - 1);
	}
	return "";
}

bool inSets(std::string setName, std::string* currentSet, std::vector<std::string>* reservedSets, std::vector<std::string>* sets)
{
	for (unsigned int i = 0; i < reservedSets->size(); i++)
	{
		std::string indexstr = (*reservedSets)[i];
		if ( local::endsWith(indexstr, *currentSet) && local::startsWith(indexstr.substr(0, indexstr.length() - currentSet->length()), setName) )
		{
			*currentSet = (*sets)[i + 1];
			return true;
		}
	}
	return false;
}

std::string resolveKey(std::string keystr, std::string* currentSet, std::vector<std::string>* reservedSets, std::vector<std::string>* reservedLines, std::vector<std::string>* sets)
{
	size_t pos = keystr.find(".");
	if (pos == std::string::npos)
	{
		std::string result = findLine(keystr, *currentSet, reservedLines);
		return result;
	}
	std::string curset = keystr.substr(0, pos);
	if (inSets(curset, currentSet, reservedSets, sets))
		return resolveKey(keystr.substr(pos + 1), currentSet, reservedSets, reservedLines, sets);
	return "";
}

} //get

std::vector<std::string> PlainSettings::get(std::vector<std::string> keys)
{
	std::vector<std::string> results;
	for (std::string key : keys)
	{
		std::string currentSet = sets[0];
		std::string result = get::resolveKey(key, &currentSet, &reservedSets, &reservedLines, &sets);
		if (result != "")
			results.push_back(result);
	}
	return results;
}

std::vector<std::string> PlainSettings::getSet(std::string key)
{
	std::vector<std::string> result;
	std::vector<std::string> split = local::split(".", key);
	if (split.size() == 0)
		split.push_back(key);
	std::string currentSet = sets[0];
	for (unsigned int i = 0; i < split.size(); i++)
	{
		if (!get::inSets(split[i], &currentSet, &reservedSets, &sets))
			return {};
	}

	int subset = 0;
	for (int i = 0; i < reservedLines.size(); i++)
	{
		std::string index = reservedLines[i];
		size_t pos = index.find("=" + currentSet);
		size_t posFirst = index.find("=");
		if (pos == posFirst)
		{
			size_t posCorrected = index.substr(posFirst + 1).find("=" + currentSet);
			if (posCorrected != std::string::npos)
				pos = posFirst + 1 + posCorrected;
			else
				continue;
		}
		if (pos != std::string::npos)
		{
			if (index.substr(pos + 1).length() > currentSet.length()) //Opening subset
			{
				subset++;
				std::string thisSetName;
				int numInPreviousSet = std::stoi(index.substr(index.rfind(" ") + 1));
				for (std::string str : reservedSets)
				{
					if (local::endsWith(str, "=" + currentSet))
					{
						if (--numInPreviousSet == 0)
							thisSetName = str.substr(0, str.find("="));
					}
				}
				currentSet = index.substr(pos + 1);
				result.push_back(thisSetName + "{");
			}
			result.push_back(index.substr(0, index.length() - currentSet.length() - 1));
		}
		else if (subset > 0) //closing subset
		{
			result.push_back("}");
			currentSet = currentSet.substr(0, currentSet.rfind(" "));
			subset--;
			i--;
		}
	}
	return result;
}

namespace write
{

void writeIt(std::string key, std::string value, File* file, int nested)
{
	size_t pos = key.find(".");
	for(int i = 0; i < nested && pos != std::string::npos; i ++)
	{
		key = key.substr(pos + 1);
		pos = key.find(".");
	}
	int closes = 0;
	while (pos != std::string::npos)
	{
		file->write("[" + key.substr(0, pos) + "]\n");
		key = key.substr(pos + 1);
		pos = key.find(".");
		closes ++;
	}
	file->write(key + "=" + value + "\n");
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
		std::string line;
		for (unsigned int i = 0; i < sets->size(); i++)
		{
			if ((*sets)[i] == currentSet)
				break;
			do {
				line = file->readline();
			} while (line[0] != '[');
		}
	}
	if (file->atEOF())
	{
		if (file->getSize() < 2)
			return;
		int newlineCount = 2;
		std::string str = file->getFromFile(file->getSize() - 2, 0);
		file->setPos(file->getSize());
		if (str[1] == '\n')
		{
			newlineCount --;
			if (str[0] == '\n')
				newlineCount --;
		}
		for (int i = newlineCount; i > 0; i --)
			file->write("\n");
	}
}

void getToRightPosInSet(std::string keystr, File* file, long pos)
{
	std::string key = "";
	size_t index = keystr.find(".");
	if(index != std::string::npos)
		key = keystr.substr(index + 1);
	else
		key = keystr;
	long subspos = file->findNext(pos, "[");
	long findpos = file->findNext(pos, "\n" + key);
	while(subspos != -1 && subspos < findpos)
	{
		pos = file->findNext(subspos, "~");
		subspos = file->findNext(pos, "[");
		findpos = file->findNext(pos, key);
	}
	file->setPos(findpos + 1);
}

void deleteNextLine(File* file, long pos)
{
	unsigned long nextEnd = file->findNext(pos, "\n") + 1;
	std::string line = file->getFromFile(pos, nextEnd);
	file->replace(line, "", false, false, 1, pos);
}

bool checkBasis(std::string key, std::string value)
{
	if (key == "" || value == "")
		return false;
	if (key[0] == '.' || key.back() == '.' || key.find("=") != std::string::npos || value.find(".") != std::string::npos || value.find("=") != std::string::npos)
		return false;
	return true;
}

void resetSets(std::vector<std::string>* sets, std::vector<std::string>* reservedSets, std::vector<std::string>* reservedLines)
{
	sets->clear();
	reservedSets->clear();
	reservedLines->clear();
}

} //write

bool PlainSettings::write(std::string key, std::string value, bool overwriteIfExists)
{
	if (!write::checkBasis(key, value))
		return false;
	std::string currentSet = sets[0];
	std::string previousValue = get::resolveKey(key, &currentSet, &reservedSets, &reservedLines, &sets);
	if(previousValue != "" && !overwriteIfExists)
		return false;
	int nested = local::split(" ", currentSet).size() - 1;
	if (nested < 0)
		nested = 0;
	if (currentSet != sets[0])
		nested ++;
	if (previousValue != "")
		nested = 0;
	file.open();
	if (encoded)
	{
		file.decode(encoder->getEncodeAlgorithm(), encoder->getDecodeAlgorithm());
		file.setPos(0, false);
	}
	write::getToRightPosInFile(currentSet, &sets, &file);
	if (previousValue != "")
	{
		std::vector<std::string> split = local::split(".", key);
		if (split.size() > 0)
			key = split.back();
		long pos = file.getPos();
		if (pos > 0)
			pos--; //put the pos back 1 char because we are looking for \n + key, not just key. (This is because otherwise we might find the key in a value part
				//of another key instead of at the beginning of a newline where it should be)
		file.close();
		file.open();
		write::getToRightPosInSet(key, &file, pos);
	}
	write::writeIt(key, value, &file, nested);
	if (previousValue != "")
	{
		long pos = file.getPos();
		file.close();
		file.open();
		write::deleteNextLine(&file, pos); //To delete oldValue if it existed
	}
	file.close();
	file.open();
	write::resetSets(&sets, &reservedSets, &reservedLines);
	return valid();
}

bool PlainSettings::exists(std::string key)
{
	std::string currentSet = sets[0];
	bool result = get::resolveKey(key, &currentSet, &reservedSets, &reservedLines, &sets) != "";
	currentSet = sets[0];
	return result;
}
