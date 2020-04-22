// cabin.cpp

#include <list>
#include <cstdio>

#include <cabin.h>

void OccupiableSpace::setOccupied()
{
	this->occupied = true;
	printf("Update to OcSpace %i: Occupied=%s\n", this->id, this->occupied ? "True" : "False");
}

void OccupiableSpace::setUnoccupied()
{
	this->occupied = false;
	printf("Update to OcSpace %i: Occupied=%s\n", this->id, this->occupied ? "True" : "False");
}

bool Passenger::occupySpace(OccupiableSpace &newSpace)
{
	if (newSpace.occupied)
	{
		return false;

	}
	else
	{
		printf("I am Passenger %i and I am occupying a space with id %i\n",
			this->id, newSpace.id);

		//If this passenger is in the aisle, set its old space to unoccupied
		this->currentSpace.setUnoccupied();

		//Move to the new one
		printf("Occupied: %s\n", newSpace.occupied ? "True" : "False");
		this->currentSpace = newSpace;
		newSpace.setOccupied();
		printf("Occupied: %s\n", newSpace.occupied ? "True" : "False");

		return true;
	}
}

bool Passenger::CurrentRowIsTarget()
{
	return (this->targetRow == this->currentSpace.id);
}

bool Passenger::IsEqual(Passenger p)
{
	bool conditions = 
		(this->id == p.id);
		// && (this->targetRow == p.targetRow) &&
		// (this->targetSeatInRow == p.targetSeatInRow) &&
		// (this->stowTime == p.stowTime) &&
		// (this->lifetime == p.lifetime);
	return conditions;
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
		newAisleSpace.occupied = false;

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
			auto seat = a_it->second.second.first.seatsMap.at(i);
			printf("\tSeat %i: Occupied: %s\n", seat.id, seat.occupied ? "True" : "False");
		}
		printf("Starboard Seats:\n");
		for (int i = stbdFirstId; i < stbdFirstId + numSeatsStbd; i++)
		{
			auto seat = a_it-> second.second.second.seatsMap.at(i);
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

void CabinAisle::SetSeatOccupied(int rowNumber, int seatNumber)
{
	auto portAndStbdGrouplets = this->twoSidedSeating.at(rowNumber).second;
	auto portGroupletMap = portAndStbdGrouplets.first.seatsMap;
	auto stbdGroupletMap = portAndStbdGrouplets.second.seatsMap;

	//Find seat in this row
	//Iterate over the port seats
	std::map<int, SeatSpace>::iterator m_it;
	for (m_it = portGroupletMap.begin(); m_it != portGroupletMap.end(); m_it++)
	{
		if ( (m_it->first == seatNumber) && (m_it->second.id == seatNumber) )
		{
			//This is the seat
			//Do the full call for now
			this->twoSidedSeating.at(rowNumber).second.first.seatsMap.at(seatNumber).occupied = true;
		}
	}

	//Iterate over the stbd seats
	for (m_it = stbdGroupletMap.begin(); m_it != stbdGroupletMap.end(); m_it++)
	{
		if ( (m_it->first == seatNumber) && (m_it->second.id == seatNumber) )
		{
			//This is the seat
			//Do the full call for now
			this->twoSidedSeating.at(rowNumber).second.second.seatsMap.at(seatNumber).occupied = true;
		}
	}

	//If we've gotten there and haven't set the seat to occupied then idk
}

void CabinAisle::SetSeatUnoccupied(int rowNumber, int seatNumber)
{
	auto portAndStbdGrouplets = this->twoSidedSeating.at(rowNumber).second;
	auto portGroupletMap = portAndStbdGrouplets.first.seatsMap;
	auto stbdGroupletMap = portAndStbdGrouplets.second.seatsMap;

	//Find seat in this row
	//Iterate over the port seats
	std::map<int, SeatSpace>::iterator m_it;
	for (m_it = portGroupletMap.begin(); m_it != portGroupletMap.end(); m_it++)
	{
		if ( (m_it->first == seatNumber) && (m_it->second.id == seatNumber) )
		{
			//This is the seat
			//Do the full call for now
			this->twoSidedSeating.at(rowNumber).second.first.seatsMap.at(seatNumber).occupied = false;
		}
	}

	//Iterate over the stbd seats
	for (m_it = stbdGroupletMap.begin(); m_it != stbdGroupletMap.end(); m_it++)
	{
		if ( (m_it->first == seatNumber) && (m_it->second.id == seatNumber) )
		{
			//This is the seat
			//Do the full call for now
			this->twoSidedSeating.at(rowNumber).second.second.seatsMap.at(seatNumber).occupied = false;
		}
	}

	//If we've gotten there and haven't set the seat to unoccupied then idk
}

//For testing only
void CabinAisle::FillAllSeats()
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
			seatsMap_it->second.occupied = true;
		}

		//Clear seats in the starboard grouplet
		for (seatsMap_it = stbdSeatsMap.begin(); seatsMap_it != stbdSeatsMap.end(); seatsMap_it++)
		{
			//Clear this seat
			seatsMap_it->second.occupied = true;
		}
	}
}
