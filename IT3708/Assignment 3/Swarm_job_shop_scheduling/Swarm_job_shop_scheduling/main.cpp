#include "genetics.h"
#include "config.h"
#include <iostream>
using namespace std;

int main() {
	OttoRep p1("00132200");// , p2("9876543210123456789");
	//OttoRep c1, c2;
	
	unsigned int **job_order = new unsigned int*[1];
	job_order[0] = new unsigned int[N_JOBS];
	p1.convert_to_phenotype(job_order);

	cout << job_order[0][0];
	for (int i = 1; i < N_JOBS; i++) {
		cout << ", " << job_order[0][i];
	}
	cout << endl;

	system("PAUSE");
	return 0;
}