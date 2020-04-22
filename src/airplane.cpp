
#include <cabin.h>
#include <airplane.h>

//Constructor
Airplane::Airplane()
{
	//nothing here
}

Airplane::Airplane(bool verbose)
{
	this->verboseOutput = verbose;
}

//If this were C#, this would be an extension method
bool Airplane::CheckSomeSeats(SeatGrouplet groupletToCheck, int targetSeatId, SeatSpace &foundSpace)
{
	auto seatsToCheck = groupletToCheck.seatsMap;
	std::map<int, SeatSpace>::iterator seat_it;
	for (seat_it = seatsToCheck.begin(); seat_it != seatsToCheck.end(); seat_it++)
	{
		if (verboseOutput) printf(".. Checking seat with id %i\n", seat_it->second.id);
		if (seat_it->second.id == targetSeatId)
		{
			if (verboseOutput) printf(".. Match! seat_it->second.id=%i, targetSeatId=%i\n",
									  seat_it->second.id, targetSeatId);
			//Found the target
			foundSpace = seat_it->second;
			return true;
		}
		else
		{
			if (verboseOutput) printf(".. Not a match. seat_it->second.id=%i, targetSeatId=%i\n",
									  seat_it->second.id, targetSeatId);
		}
	}
	//Could not find target
	return false;
}

bool Airplane::CheckSeatsInRow(Passenger &p, int rowNumber, bool &foundSeat, bool &tookSeat)
{
	//get all seats at this row index
	std::pair<SeatGrouplet, SeatGrouplet> groupletsToCheck = this->MainAisle.twoSidedSeating.at(rowNumber).second;
	auto portSeatsGrouplet = groupletsToCheck.first;
	auto stbdSeatsGrouplet = groupletsToCheck.second;
	SeatSpace target;

	bool foundSeatCondition = (this->CheckSomeSeats(portSeatsGrouplet, p.targetSeatInRow, target) || 
					  		   this->CheckSomeSeats(stbdSeatsGrouplet, p.targetSeatInRow, target));
	if (foundSeatCondition)
	{
		//Found the target seat
		foundSeat = true;
		if (verboseOutput) printf("Passenger %i found seat %i in row %i\n", p.id, p.targetSeatInRow, p.targetRow);

		if (p.occupySpace(target))
		{
			//Passenger successfully occupies the target space
			if (verboseOutput) printf("Passenger %i takes seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = true;
			return true;
		}
		else
		{
			//Passenger did not occupy the target space
			if (verboseOutput) printf("Passenger %i does not take seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = false;
			return true;
		}
	}
}

void Airplane::PopulateMainAisle()
{
	this->MainAisle.Populate(this->NumRows, this->NumSeatsPort, this->NumSeatsStbd);
}
