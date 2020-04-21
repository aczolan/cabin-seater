// cabin.cpp

#include <list>
#include <cstdio>

#include <cabin-seater.h>

void OccupiableSpace::setOccupied()
{
	this->occupied = true;
}

void OccupiableSpace::setUnoccupied()
{
	this->occupied = false;
}

bool Passenger::occupySpace(OccupiableSpace &newSpace)
//bool SeatSpace::occupySpace(Passenger newOccupant)
{
	if (newSpace.occupied)
	{
		return false;
	}
	else
	{
		//printf("I am SeatSpace %i and I am being occupied by Passenger %i\n", this->id, newOccupant.id);
		printf("I am Passenger %i and I am occupying SeatSpace %i\n",this->id, newSpace.id);
		//this->occupant = &newOccupant;
		//this->occupied = true;
		printf("Occupied: %s\n", newSpace.occupied ? "True" : "False");
		newSpace.setOccupied();
		printf("Occupied: %s\n", newSpace.occupied ? "True" : "False");
		//newSpace.occupant = this;

		return true;
	}
}

bool SeatSpace::leaveSpace()
{
	this->occupied = false;
	return true;
}

SeatSpace::SeatSpace(int id)
{
	//constructor
	
	this->id = id;
	this->occupied = false;
}

SeatSpace::SeatSpace()
{
	//constructor
	
	this->occupied = false;
}

SeatGrouplet::SeatGrouplet(int numSeats, int startingId)
{
	//constructor
	
	this->numSeats = numSeats;
	
	//Populate seats
	for (int i = startingId; i < startingId + numSeats; i++)
	{
		SeatSpace s(i);
		auto newEntry = std::make_pair(i, s);
		this->seatsMap.insert(newEntry);
	}
}

SeatGrouplet::SeatGrouplet()
{
	//constructor
	//do something here
}

void CabinAisle::Populate(int numRows, int numSeatsPort, int numSeatsStbd)
{
	for (int i = 0; i < numRows; i++)
	{
		AisleSpace newAisleSpace;
		newAisleSpace.id = i;

		SeatGrouplet portGrouplet(numSeatsPort, 0);
		SeatGrouplet stbdGrouplet(numSeatsStbd, numSeatsPort);

		//Create pairs
		auto seatsPair = std::make_pair(portGrouplet, stbdGrouplet);
		auto aisleToSeats = std::make_pair(newAisleSpace, seatsPair);
		auto newEntry = std::make_pair(i, aisleToSeats);
		this->twoSidedSeating.insert(newEntry);
	}
}

void CabinAisle::PrintAisle()
{
	std::map<int, std::pair<AisleSpace, std::pair<SeatGrouplet, SeatGrouplet>>>::iterator a_it;
	for (a_it = this->twoSidedSeating.begin(); a_it != this->twoSidedSeating.end(); a_it++)
	{
		//Print general SeatGrouplet info
		int entryId = a_it->first;
		int aisleId = a_it->second.first.id;
		int numSeatsPort = a_it->second.second.first.numSeats;
		int numSeatsStbd = a_it->second.second.second.numSeats;
		printf("Entry: %i, Aisle: %i, NumSeats Port: %i, Starboard: %i\n", entryId, aisleId, numSeatsPort, numSeatsStbd);

		int portFirstId = a_it->second.second.first.seatsMap.begin()->first;
		int stbdFirstId = a_it->second.second.second.seatsMap.begin()->first;

		//Print info on seats in this grouplet
		printf("Port Seats:\n");
		for (int i = portFirstId; i < portFirstId + numSeatsPort; i++)
		{
			auto seat = a_it->second.second.first.seatsMap[i];
			printf("\tSeat %i: Occupied: %s\n", seat.id, seat.occupied ? "True" : "False");
		}
		printf("Starboard Seats:\n");
		for (int i = stbdFirstId; i < stbdFirstId + numSeatsStbd; i++)
		{
			auto seat = a_it-> second.second.second.seatsMap[i];
			printf("\tSeat %i: Occupied: %s\n", seat.id, seat.occupied ? "True" : "False");
		}
	}
}

void CabinAisle::ClearAllSeats()
{
	std::map<int, std::pair<AisleSpace, std::pair<SeatGrouplet, SeatGrouplet>>>::iterator a_it;
	for (a_it = this->twoSidedSeating.begin(); a_it != this->twoSidedSeating.end(); a_it++)
	{
		//Get port and starboard seats in this row
		SeatGrouplet portSeatsGrouplet = a_it->second.second.first;
		auto portSeatsMap = portSeatsGrouplet.seatsMap;
		SeatGrouplet stbdSeatsGrouplet = a_it->second.second.second;
		auto stbdSeatsMap = stbdSeatsGrouplet.seatsMap;

		//Clear seats in the port grouplet
		std::map<int, SeatSpace>::iterator seatsMap_it;
		for (seatsMap_it = portSeatsMap.begin(); seatsMap_it != portSeatsMap.end(); seatsMap_it++)
		{
			//Clear this seat
			seatsMap_it->second.occupied = false;
		}

		//Clear seats in the starboard grouplet
		for (seatsMap_it = stbdSeatsMap.begin(); seatsMap_it != stbdSeatsMap.end(); seatsMap_it++)
		{
			//Clear this seat
			seatsMap_it->second.occupied = false;
		}
	}
}

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
		if (seat_it->second.id == targetSeatId)
		{
			//Found the target
			foundSpace = seat_it->second;
			return true;
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
