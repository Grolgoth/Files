#ifndef ALGORITHMZ_H
#define ALGORITHMZ_H

#include <iostream>


class Algorithm {
	public:

		Algorithm(std::string algorithm);

		bool isValid();
		int execute(int in);

	private:
		std::string algorithm;
};

#endif // ALGORITHMZ_H
