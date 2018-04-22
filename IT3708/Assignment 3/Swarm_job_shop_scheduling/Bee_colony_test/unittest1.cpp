#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Swarm_job_shop_scheduling/bee_colony.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Bee_colony_test
{		
	TEST_CLASS(OttoRepTest)
	{
	public:
		
		/*TEST_METHOD(EqualityOperator) 
		{
			OttoRep p1("0000000"), p2("0000000");
			Assert::AreEqual(p1, p2);
		
		}*/

		TEST_METHOD(CrossoverTest)
		{
			OttoRep p1("6543210"), p2("0000000");
			OttoRep c1("6543210"), c2("0000000");
			p1.crossover(p2, c1, c2);

			Assert::IsTrue(c1 == c2);
			// TODO: Your test code here
		}

	};
}