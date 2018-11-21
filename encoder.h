#ifndef ENCODER_H
#define ENCODER_H

#include <iostream>
#include "algorithmz.h"

class Encoder {
	public:
		Encoder(Algorithm encodeAlgorithm, Algorithm decodeAlgorithm);

		std::string encode(std::string target);
		std::string decode(std::string target);
		inline Algorithm getEncodeAlgorithm(){ return algorithm; }
		inline Algorithm getDecodeAlgorithm(){ return decodeAlgorithm; }

	private:
		Algorithm algorithm;
		Algorithm decodeAlgorithm;
		std::string twoInAlgorithm;
};

#endif // ENCODER_H
