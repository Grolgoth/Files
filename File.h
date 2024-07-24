#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <iostream>
#include <vector>

class FString;
class Algorithm;

class File {
	public:

		static std::string getPathToExe();

		/** The path to the file can be relative(./ or ../../ etc). If you don't include any forward slash (/) the path will be searched from the
		executable location. Never write the path with backslashes (\) always forward slashes even if using windows.*/
		File(std::string file, bool platformSpecific = false);
		~File();
		File(const File& other);

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
		std::string getFromFile(unsigned long beginpos = 0, unsigned long endpos = 0);
		unsigned long findNext(long from, std::string search);
		std::vector<long> findAll(std::string find, bool ignoreCase = true, bool all = true, int occurences = 1, long fromPos = -1, long until = -1);
		std::vector<std::string> getLines(bool fromCurrentPos);
		std::vector<std::string> getFilesInDir();
		void create();
		void createDir();
		void close();
		void open();
		void flush();
		void clear();
		void write(std::string text);
		void write(FString text);
		void replace(std::string find, std::string target, bool ignoreCase = true, bool all = true, int occurences = 1, long fromPos = -1, long until = -1);
		void setPos(long pos, bool relativeToCurrentPos = false);
		void encode(Algorithm algorithm);
		void decode(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm);

	protected:
		static std::string executableName;
		std::string mabsoluteFileName;
		std::vector<std::string> dirSplits;
		char dir = 'u';
		bool mopen;
		bool platformSpecific;
		FILE* f = nullptr;

		std::string toPlatform(std::string base);

    private:
        void getDirectoryTree();
};

#endif // FILE_H_INCLUDED
