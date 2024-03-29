// airplane.h

#include <string>

#ifndef INCLUDE_AIRPLANE_H
#define INCLUDE_AIRPLANE_H

class Airplane
{
	public:
		bool verboseOutput;

		std::string RunID;
		std::string CSVname;
		int SelectedAlgorithmID;
		bool CovidSeating;

		int NumRows;
		int LastRowIndex;
		int NumSeatsPort;
		int NumSeatsStbd;

		int NumPassengers;
		int PassengerMinStowTime;
		int PassengerMaxStowTime;
		int PassengerIdStartingIndex;
		CabinAisle MainAisle;

		bool CheckSomeSeats(SeatGrouplet, int, SeatSpace&);
		bool CheckSeatsInRow(Passenger&, int, bool&, bool&);
		void PopulateMainAisle();

		//Constructor
		Airplane();
		Airplane(bool);
};

#endif //INCLUDE_AIRPLANE_H