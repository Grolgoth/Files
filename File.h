#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <iostream>
#include <vector>
#include "string.h"

class File {
	public:

		static std::string getPathToExe();

		File(std::string file);
		~File();

		bool exists();
		bool isDir();
		bool isOpen();
		bool atEOF();
		unsigned long getPos();
		unsigned long getSize();
		std::string getAbsolutePath();
		std::string getPath();
		std::string getFileName();
		std::string readline();
		std::vector<std::string> getLines(bool fromCurrentPos);
		void close();
		void open();
		void clear();
		void write(std::string text);
		void write(String text);
		void replace(std::string find, std::string target, bool ignorecase = true, bool all = true, int occurences = 1, long fromPos = -1, long until = -1);
		void setPos(long pos, bool relativeToCurrentPos = false);

	protected:
		static std::string executableName;
		std::string mabsoluteFileName;
		char dir = 'u';
		bool mopen;
		FILE* f = nullptr;
};

#endif // FILE_H_INCLUDED
