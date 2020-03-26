// cabin-seater.h

#ifndef INCLUDE_CABIN_SEATER_H
#define INCLUDE_CABIN_SEATER_H

#include <math.h>
#include <map>
#include <vector>

enum class SimulatorState
{
	DECISION = 1,
	ACTION = 2
};

struct OccupiableSpace
{
	public:
		int id;
		bool occupied;
		void setOccupied();
		void setUnoccupied();
};

struct AisleSpace : OccupiableSpace
{
	public:
		int id;
		//bool occupySpace(Passenger newOccupant);
		//bool leaveSpace();
};

struct SeatSpace : OccupiableSpace
{
	public:
		//bool occupySpace(Passenger newOccupant);
		bool leaveSpace();
		SeatSpace(int id);
		SeatSpace();
};

class Passenger
{
	public:
		int id;
		int targetRow;
		int targetSeatInRow;
		bool occupySpace(OccupiableSpace &newSpace);
};

class SeatGrouplet
{
	public:
		int numSeats; //Number of seats in this grouplet
		std::map<int, SeatSpace> groupletSeats;
			//int: seat id
			//SeatSpace: space to occupy
		SeatGrouplet(int numSeats, int startingId);
		SeatGrouplet();
};

class CabinAisle
{
	public:
		std::map<int, std::pair<AisleSpace, std::pair<SeatGrouplet, SeatGrouplet>>> twoSidedSeating;
			//int: row number
			//pair<SeatGrouplet, SeatGrouplet>
				//SeatGrouplet (1): Seats on starboard side
				//SeatGrouplet (2): Seats on port side

};

#endif //INCLUDE_CABIN_SEATER_H
