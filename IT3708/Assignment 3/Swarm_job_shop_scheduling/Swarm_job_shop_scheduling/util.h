#pragma once
#include <vector>

namespace util {
	inline void normalize_vector(std::vector<double>& vec) {
		//Normalize vector
		double sum = 0;
		for (int i = 0; i < vec.size(); i++) {
			sum += vec[i];
		}

		for (int i = 0; i < vec.size(); i++) {
			vec[i] /= sum;
		}
	}

	inline int weighted_rand(std::vector<double> vec) {
		normalize_vector(vec);
		
		double p = (rand() % 100) / 100.0;

		double sum = 0;
		for (int i = 0; i < vec.size(); i++) {
			if (p < vec[i] + sum) {
				return i;
			}
			sum += vec[i];
		}

		return vec.size() - 1;
	}
};