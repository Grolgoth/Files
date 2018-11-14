#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <iostream>
#include <vector>
#include "algorithmz.h"

class String {
	public:
		static String fromVector(std::vector<String> vect, std::string appendElements = "");
		static String fromVector(std::vector<std::string> vect, std::string appendElements = "");

		String(std::string base);

		String operator+(const String& b);
		bool operator!=(const String& b);
		bool toBool();
		bool contains(std::string sfind, bool ignoreCase = true);
		bool contains(String sfind, bool ignoreCase = true);
		bool containsRegex(std::string sregex);
		bool startsWith(std::string prefix, bool ignoreCase = true);
		bool startsWith(String prefix, bool ignoreCase = true);
		bool endsWith(std::string suffix, bool ignoreCase = true);
		bool endsWith(String suffix, bool ignoreCase = true);
		int compare(std::string other);
		int compare(String other);
		int indexOf(std::string sfind, bool ignoreCase = true, unsigned int fromPos = 0, unsigned int occurences = 1, bool fromBegin = true);
		int indexOf(String sfind, bool ignoreCase = true, unsigned int fromPos = 0, unsigned int occurences = 1, bool fromBegin = true);
		int toInt();
		char* toCharArray();
		std::string toStdString() const;
		String substring(unsigned int ibegin, int iend = -1);
		String invert(unsigned int ibegin = 0, int iend = -1);
		String replace(std::string sfind, std::string target, bool ignoreCase = true, bool all = true, int occurences = 1, bool fromBegin = true);
		String transform(Algorithm algorithm);
		std::vector<int> findAll(std::string find, bool ignoreCase = true, int fromPos = 0);
		std::vector<String> split(std::string sfind, bool ignoreCase = true, bool all = false, int occurences = 1, bool fromBegin = true);
		std::vector<String> split(String sfind, bool ignoreCase = true, bool all = false, int occurences = 1, bool fromBegin = true);

	private:
		std::string base;
};

#endif // STRING_H_INCLUDED
