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
		this->groupletSeats.insert(newEntry);
	}
}

SeatGrouplet::SeatGrouplet()
{
	//constructor
	//do something here
}
