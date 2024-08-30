#include "File.h"
#include "defines.h"
#include "fstring.h"
#include "encoder.h"
#include <algorithm>
#include <filesystem>

#ifdef OS_Windows
#include <sys/stat.h>
#include "windows.h"
#include "windef.h"
#elif defined(OS_Linux)

#endif

std::string File::executableName;

std::string File::getPathToExe()
{
	if(!executableName.empty())
	{
		size_t pos = executableName.rfind('/');
		return executableName.substr(0, pos);
	}
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
	executableName = FString(result).replace("\\", "/").toStdString();
	return FString(executableName).split("/", true, false, 1, false)[0].toStdString();
}

std::vector<std::string> File::getFilesInDir()
{
	if (!exists() || !isDir())
		throw "Can't files from dir " + getAbsolutePath() + " because it either doesn't exists or isn't a directory";
	std::string path = getAbsolutePath();
	std::vector<std::string> result;
	#ifdef OS_Windows
	std::string pattern(path);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
        do
        {
            result.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
    #elif defined(OS_Linux)
	DIR* dirp = opendir(path.c_str());
	struct dirent * dp;
    while ((dp = readdir(dirp)) != nullptr)
	{
		result.push_back(dp->d_name);
	}
	closedir(dirp);
	#endif
	return result;
}

/**
Represents a file to read and write to. If platformSpecific = true it will convert
newline characters to \r\n for windows systems when performing write operations.
*/
File::File(std::string file, bool platformSpecific) : mopen(false), platformSpecific(platformSpecific)
{
	FString ffile(file);
	if (ffile.startsWith("./") || ffile.startsWith("../") || !ffile.contains("/"))
	{
		mabsoluteFileName = getPathToExe();
		while(ffile.startsWith("../"))
		{
			size_t pos = mabsoluteFileName.rfind('/');
			if (pos != std::string::npos)
				mabsoluteFileName = mabsoluteFileName.substr(0, pos);
			ffile = ffile.substring(3, ffile.length());
		}
		if (ffile.startsWith("./"))
			ffile = ffile.replace("./", "", true, false);
		mabsoluteFileName += "/" + ffile.toStdString();
	}
	else // not using relative file name
		mabsoluteFileName = file;
	getDirectoryTree();
	isDir();
}

void File::getDirectoryTree()
{
	std::vector<FString> splits = FString(mabsoluteFileName).split("/", true, false, 1, false)[0].split("/", true, true);
	std::string path = "";
	for(unsigned int i = 0; i < splits.size(); i++)
	{
		path += splits[i].toStdString();
		dirSplits.push_back(path);
		path += '/';
	}
}

File::~File()
{
	if (mopen)
		fclose(f);
}

File::File(const File& other) : mabsoluteFileName(other.mabsoluteFileName), dir(other.dir), mopen(other.mopen), platformSpecific(other.platformSpecific), f(other.f)
{
	if (other.mopen)
		throw "Error: should not call file copy constructor if the File object's stream is opened! (It will be closed in the destructor of the object that's copied)";
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
	if (FString(mabsoluteFileName).contains("/"))
		return FString(mabsoluteFileName).split("/", true, false, 1, false)[1].toStdString();
	return mabsoluteFileName;
}

std::string File::getPath()
{
	if (FString(mabsoluteFileName).contains("/"))
		return FString(mabsoluteFileName).split("/", true, false, 1, false)[0].toStdString();
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
			if(c != EOF && c != '\n' && c!= '\r')
				buffer[pos++] = (unsigned char)c;
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
	if (endpos == 0)
		endpos = getSize();
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

unsigned long File::findNext(long from, std::string search)
{
	std::string leftover = getFromFile(from);
	size_t index = leftover.find(search);
	if (index == std::string::npos)
		return -1;
	return index + from;
}

std::string File::toPlatform(std::string base)
{
	if (!platformSpecific)
		return base;
	#ifdef OS_Windows
		FString baseString(base);
		if (!baseString.contains("\n"))
			return base;
		base = "";
		std::vector<FString> pieces = baseString.split("\n");
		for (unsigned int i = 0; i < pieces.size(); i++)
		{
			if (i + 1 != pieces.size() || baseString.endsWith("\r") || baseString.endsWith("\n"))
			{
				if (!pieces[i].endsWith("\r"))
					base += pieces[i].toStdString() + "\r\n";
				else
					base += pieces[i].toStdString() + "\n";
			}
			else
				base += pieces[i].toStdString();
		}
	#endif
	return base;
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
	std::string allstr = FString::fromVector(lines, "\n").toStdString();
	if (fromPos != -1 && until != -1)
		allstr = FString(allstr).substring(0, until - fromPos).toStdString();
	if (!all)
	{
		while (occurences > 0)
		{
			indexes.push_back(FString(allstr).indexOf(find, ignoreCase, 0, occurences) + fromPos);
			occurences --;
		}
		std::reverse(indexes.begin(), indexes.end());
		return indexes;
	}
	for(int index : FString(allstr).findAll(find, ignoreCase))
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
		File fileDir(dirSplits.back());
		if (!fileDir.exists())
			fileDir.createDir();
		f = fopen(mabsoluteFileName.c_str(), "wb+");
		fclose(f);
	}
	else
		throw "Can't create file because it already exists";
}

void File::createDir()
{
	if (!exists())
	{
		for(unsigned int i = 1; i < dirSplits.size(); i++)
		{
			File checkFile(dirSplits[i]);
			if(checkFile.exists())
				continue;
			else
				std::filesystem::create_directory(dirSplits[i]);
		}
		std::filesystem::create_directory(mabsoluteFileName);
	}
	else
		throw "Can't create directory because it already exists";
}

void File::open()
{
	if (mopen)
		return;
	isDir();
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

void File::flush()
{
	if(exists() && mopen)
		fflush(f);
	else if (!exists())
		throw "Can't clear file because it doesn't exist";
	else
		throw "Can't clear file because it isn't open";
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
		text = toPlatform(text);
		fputs(text.c_str(), f);
		if (!rest.empty())
		{
			std::string all = FString::fromVector(rest, "\n").toStdString();
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
	target = toPlatform(target);
	std::string allstr = FString::fromVector(lines, "\n").toStdString();
	if (fromPos != -1)
	{
		if (until < 0)
			until = allstr.length();
		else
			until -= fromPos;
		if ((unsigned)until > allstr.length())
			throw "Can't execute replace operation because the end position specified is greater than the file length.";
		std::string first = FString(allstr).substring(0, until)
			.replace(find, target, ignoreCase, all, occurences, true)
			.toStdString();
		std::string second = FString(allstr).substring(until)
			.toStdString();
		std::string before = getFromFile(0, fromPos);
		allstr = before + first + second;
	}
	else
		allstr = FString(allstr).replace(find, target, ignoreCase, all, occurences, true).toStdString();
	clear();
	fputs(allstr.c_str(), f);
}

void File::write(FString text)
{
	write(text.toStdString());
}

void File::setPos(long pos, bool relativeToCurrentPos)
{
	if (relativeToCurrentPos)
		pos += ftell(f);
	if (pos < 0)
		throw "Shouldn't set position of file to a negative value";
	if ((unsigned)pos > getSize())
		throw "Can't set file position to something greater than its size.";
	fseek(f, pos, SEEK_SET);
}

/**
Encodes the file given an algorithm
*/
void File::encode(Algorithm algorithm)
{
	if (getSize() == 0)
		return;
	if(exists() && mopen)
	{
		FString file = getFromFile();
		FString fileEncoded = file.encode(algorithm);
		clear();
		fputs(fileEncoded.toStdString().c_str(), f);
	}
	else if (!exists())
		throw "Can't encode file because it doesn't exist";
	else
		throw "Can't encode file because it isn't open";
}

void File::decode(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm)
{
	if (getSize() == 0)
		return;
	if(exists() && mopen)
	{
		FString file = getFromFile();
		FString fileDecoded = file.decode(encodeAlgorithm, decodeAlgorithm);
		clear();
		fputs(fileDecoded.toStdString().c_str(), f);
	}
	else if (!exists())
		throw "Can't encode file because it doesn't exist";
	else
		throw "Can't encode file because it isn't open";
}
