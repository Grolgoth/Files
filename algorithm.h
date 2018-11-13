#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>


class Algorithm {
	public:

		Algorithm(std::string algorithm);

		bool isValid();
		int execute(int in);

	private:
		std::string algorithm;
};

#endif // ALGORITHM_H
