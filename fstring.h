#ifndef FSTRING_H_INCLUDED
#define FSTRING_H_INCLUDED

#include <iostream>
#include <vector>
#include "algorithmz.h"

class FString {
	public:
		static FString fromVector(std::vector<FString> vect, std::string appendElements = "");
		static FString fromVector(std::vector<std::string> vect, std::string appendElements = "");
		static FString fromInt(int target);

		FString(std::string base);

		FString operator+(const FString& b);
		bool operator!=(const FString& b);
		bool toBool();
		bool isEmpty();
		bool isNumber();
		bool contains(std::string sfind, bool ignoreCase = true);
		bool contains(FString sfind, bool ignoreCase = true);
		bool containsRegex(std::string sregex);
		bool startsWith(const std::string prefix, bool ignoreCase = true);
		bool startsWith(FString prefix, bool ignoreCase = true);
		bool endsWith(const std::string& suffix, bool ignoreCase = true);
		bool endsWith(FString suffix, bool ignoreCase = true);
		bool allDigits();
		unsigned int length();
		int compare(std::string other);
		int compare(FString other);
		int indexOf(std::string sfind, bool ignoreCase = true, unsigned int fromPos = 0, unsigned int occurences = 1, bool fromBegin = true);
		int indexOf(FString sfind, bool ignoreCase = true, unsigned int fromPos = 0, unsigned int occurences = 1, bool fromBegin = true);
		int toInt();
		char charAt(unsigned int index);
		std::string toStdString() const;
		FString substring(unsigned int ibegin, int iend = -1);
		FString toUpper();
		FString toLower();
		FString invert(unsigned int ibegin = 0, int iend = -1);
		FString replace(std::string sfind, std::string target, bool ignoreCase = true, bool all = true, int occurences = 1, bool fromBegin = true);
		FString encode(Algorithm algorithm);
		FString decode(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm);
		std::vector<unsigned int> findAll(std::string find, bool ignoreCase = true, int fromPos = 0);
		std::vector<FString> split(std::string sfind, bool ignoreCase = true, bool all = false, int occurences = 1, bool fromBegin = true);
		std::vector<FString> split(FString sfind, bool ignoreCase = true, bool all = false, int occurences = 1, bool fromBegin = true);
		std::vector<std::string> getSplits(std::string find, bool ignoreCase = false, int after = 0, bool all = false, int occurences = 1, bool fromBegin = true);
		std::vector<std::string> getRanges(std::string first, std::string second, bool all = true, int occurences = 1, bool fromBegin = true, bool extraDelimiterAtEnd = false);
		std::vector<std::string> getArray(std::string firstIndex, std::string secondIndex, std::string delimiter, bool force = false, int occurences = 1, bool fromBegin = true, bool ignorecase = false);

	private:
		std::string base;
};

#endif // FSTRING_H_INCLUDED
