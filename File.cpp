#include "file.h"
#include "defines.h"
#include "string.h"
#include <sys/stat.h>
#include <algorithm>

#ifdef OS_Windows
#include "windows.h"
#include "minwindef.h"
#elif defined(OS_Linux)

#endif

std::string File::executableName;

std::string File::getPathToExe()
{
	if(!executableName.empty())
		return String(executableName).split("/", true, false, 1, false)[0].toStdString();
	std::string result = "UNDEFINED";
	#ifdef OS_Windows
		#ifdef UNICODE
			wchar_t buffer[MAX_PATH];
			GetModuleFileName(nullptr, buffer, MAX_PATH);
			std::wstring str(buffer);
			result = std::string(str.begin(), str.end());
		#else
			char buffer[MAX_PATH];
			GetModuleFileName(nullptr, buffer, MAX_PATH);
			result = buffer;
		#endif // UNICODE
	#elif defined(OS_Linux)
		char result[ PATH_MAX ];
		ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
		result = std::string( result, (count > 0) ? count : 0 );
	#endif
	executableName = String(result).replace("\\", "/").toStdString();
	return String(executableName).split("/", true, false, 1, false)[0].toStdString();
}

/**
Represents a file to read and write to. If platformSpecific = true it will convert
newline characters to \r\n for windows systems when performing write operations.
*/
File::File(std::string file, bool platformSpecific) : mopen(false), platformSpecific(platformSpecific)
{
	if (String(file).startsWith("./") || String(file).startsWith("../") || !String(file).contains("/"))
	{
		mabsoluteFileName = getPathToExe();
		while(String(file).startsWith("../"))
		{
			std::vector<String> split = String(mabsoluteFileName).split("/", true, false, 1, false);
			if (!split.empty())
				mabsoluteFileName = split[0].toStdString();
			file = String(file).substring(3, file.length()).toStdString();
		}
		if (String(file).startsWith("./"))
			file = String(file).replace("./", "/", true, false).toStdString();
		mabsoluteFileName += file;
	}
	else // not using relative file name
		mabsoluteFileName = file;
	isDir();
}

File::~File()
{
	if (mopen)
		fclose(f);
}

bool File::exists()
{
	struct stat buffer;
	return (stat(mabsoluteFileName.c_str(), &buffer) == 0);
}

bool File::isDir()
{
	if (dir == 'u')
	{
		struct stat buffer;
		if( stat(mabsoluteFileName.c_str(), &buffer) == 0 )
		{
			if( buffer.st_mode & S_IFDIR )
				dir = 'd';
			else if ( buffer.st_mode & S_IFREG )
				dir = 'f';
		}
	}
	return dir == 'd';
}

bool File::isOpen()
{
	return mopen;
}

bool File::atEOF()
{
	bool result = false;
	if (fgetc(f) == EOF)
		result = true;
	fseek(f, -1, SEEK_CUR);
	return result;
}

unsigned long File::getPos()
{
	if (mopen)
		return ftell(f);
	return 0;
}

unsigned long File::getSize()
{
    struct stat stat_buf;
    int rc = stat(mabsoluteFileName.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string File::getFileName()
{
	if (String(mabsoluteFileName).contains("/"))
		return String(mabsoluteFileName).split("/", true, false, 1, false)[1].toStdString();
	return mabsoluteFileName;
}

std::string File::getPath()
{
	if (String(mabsoluteFileName).contains("/"))
		return String(mabsoluteFileName).split("/", true, false, 1, false)[0].toStdString();
	return mabsoluteFileName;
}

std::string File::getAbsolutePath()
{
	return mabsoluteFileName;
}

std::string File::readline()
{
	if (mopen)
	{
		int size = 1024;
		int c(0), pos(0);
		char *buffer = (char *)malloc(size);
		do
		{
			c = fgetc(f);
			if(c != EOF && c != '\n')
				buffer[pos++] = (char)c;
			if(pos >= size - 1)
			{
				size *=2;
				buffer = (char*)realloc(buffer, size);
			}
        } while(c != EOF && c != '\n');
        std::string result = "";
        for (int i = 0; i < pos; i ++)
			result += buffer[i];
		free(buffer);
		return result;
	}
	else
		throw "Can't read file because it isn't open";
}

std::string File::getFromFile(unsigned long beginpos, unsigned long endpos)
{
	std::string result = "";
	if (beginpos >= endpos)
		throw "Can't get snippet from file because the specified begin position >= the end position.";
	fseek(f, beginpos, SEEK_SET);
	long remainder = endpos - beginpos;
	int c = fgetc(f);
	while(c != EOF && remainder > 0)
	{
		result += (char)c;
		c = fgetc(f);
		remainder --;
	}
	return result;
}

/**
Get all indexes of find param in specified part of file, cursor position will be lost
*/
std::vector<long> File::findAll(std::string find, bool ignoreCase, bool all, int occurences, long fromPos, long until)
{
	if (fromPos != -1 && (fromPos < 0 || (until <= fromPos && until != -1)))
		throw "Can't search in file because the positions specified in the file are invalid.";
	if (fromPos != -1 && fromPos >= 0 && (unsigned)fromPos >= getSize())
		throw "Can't search in file because the start position in the file is bigger than the file lenght.";
	std::vector<std::string> lines;
	std::vector<long> indexes;
	if (fromPos != -1)
	{
		fseek(f, fromPos, SEEK_SET);
		lines = getLines(true);
	}
	else
	{
		lines = getLines(false);
		fromPos = 0;
	}
	std::string allstr = String::fromVector(lines, "\n").toStdString();
	if (fromPos != -1 && until != -1)
		allstr = String(allstr).substring(0, until - fromPos).toStdString();
	if (!all)
	{
		while (occurences > 0)
		{
			indexes.push_back(String(allstr).indexOf(find, ignoreCase, 0, occurences) + fromPos);
			occurences --;
		}
		std::reverse(indexes.begin(), indexes.end());
		return indexes;
	}
	for(int index : String(allstr).findAll(find, ignoreCase))
		indexes.push_back(index + fromPos);
	return indexes;
}

/**
Get all lines of the file, cursor position in file will be set to EOF
*/
std::vector<std::string> File::getLines(bool fromCurrentPos)
{
	if (mopen)
	{
		std::vector<std::string> result;
		if (!fromCurrentPos)
			if (ftell(f) != 0)
				fseek(f, 0, SEEK_SET);
		std::string line = "";
		while (true)
		{
			line = readline();
			if (line == "" && atEOF())
				break;
			result.push_back(line);
		}
		return result;
	}
	else
		throw "Can't read file because it isn't open";
}

void File::create()
{
	if (!exists())
	{
		f = fopen(mabsoluteFileName.c_str(), "wb+");
		fclose(f);
	}
	else
		throw "Can't create file because it already exists";
}

void File::open()
{
	if (dir == 'f')
	{
		f = fopen(mabsoluteFileName.c_str(), "rb+");
		mopen = true;
	}
	else if (dir == 'd')
		throw "Can't open file because it's a directory.";
	else
		throw "Can't open file because it doesn't exist.";
}

void File::close()
{
	if(mopen)
		fclose(f);
	mopen = false;
}

void File::clear()
{
	if(exists() && mopen)
	{
		fclose(f);
		f = fopen(mabsoluteFileName.c_str(), "wb+");
	}
	else if (!exists())
		throw "Can't clear file because it doesn't exist";
	else
		throw "Can't clear file because it isn't open";
}

void File::write(std::string text)
{
	if (mopen)
	{
		int mempos = ftell(f);
		std::vector<std::string> rest = getLines(true);
		fseek(f, mempos, SEEK_SET);
		fputs(text.c_str(), f);
		if (!rest.empty())
		{
			std::string all = String::fromVector(rest, "\n").toStdString();
			fputs(all.c_str(), f);
		}
		fseek(f, mempos + text.length(), SEEK_SET);
	}
	else
		throw "Can't write to file because it isn't open.";
}

/**
Get all lines of the file, cursor position in file will be lost
*/
void File::replace(std::string find, std::string target, bool ignoreCase, bool all, int occurences, long fromPos, long until)
{
	if (fromPos != -1 && (fromPos < 0 || (until <= fromPos && until != -1)))
		throw "Can't execute replace operation because the positions specified in the file are invalid.";
	if (fromPos != -1 && fromPos >= 0 && (unsigned)fromPos >= getSize())
		throw "Can't execute replace operation because the start position in the file is bigger than the file lenght.";
	std::vector<std::string> lines;
	if (fromPos != -1)
	{
		fseek(f, fromPos, SEEK_SET);
		lines = getLines(true);
	}
	else
		lines = getLines(false);
	std::string allstr = String::fromVector(lines, "\n").toStdString();
	if (fromPos != -1)
	{
		if (until < 0)
			until = allstr.length();
		if ((unsigned)until > allstr.length())
			throw "Can't execute replace operation because the end position specified is greater than the file length.";
		std::string first = String(allstr).substring(0, until - fromPos)
			.replace(find, target, ignoreCase, all, occurences, true)
			.toStdString();
		std::string second = String(allstr).substring(until - fromPos)
			.toStdString();
		std::string before = getFromFile(0, fromPos);
		allstr = before + first + second;
	}
	else
		allstr = String(allstr).replace(find, target, ignoreCase, all, occurences, true).toStdString();
	clear();
	fputs(allstr.c_str(), f);
}

void File::write(String text)
{
	write(text.toStdString());
}

void File::setPos(long pos, bool relativeToCurrentPos)
{
	if (relativeToCurrentPos)
		pos += ftell(f);
	if (pos < 0)
		throw "Shouldn't set position of file to a negative value";
	if ((unsigned)pos >= getSize())
		throw "Can't set file position to something greater than its size.";
	fseek(f, pos, SEEK_SET);
}
