#include "artificial_bee.h"
#include "config.h"

FoodSource::FoodSource() : time_to_live(TIME_TO_LIVE) { 
	*solution = OttoRep(); 

}

FoodSource::FoodSource(char* representation) {
	if (representation == "OttoRep") {
		*solution = OttoRep();
	}

}