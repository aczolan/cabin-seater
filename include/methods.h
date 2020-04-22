// methods.h

#ifndef INCLUDE_METHODS_H
#define INCLUDE_METHODS_H

#include <list>
#include <queue>
#include <vector>

#include <cabin.h>
#include <airplane.h>
#include <util.h>

#include <ctime>

int randInt(int lower, int upper)
{
	//srand(time(NULL));
	//return rand() % upper + lower;
	return ( rand() % upper ) + lower;
}

Passenger createPassenger(int id, int targetRow, int targetSeat, int stowTimeMin, int stowTimeMax)
{
	Passenger p;
	p.id = id;
	p.targetRow = targetRow;
	p.targetSeatInRow = targetSeat;
	p.stowTime = randInt(stowTimeMin, stowTimeMax);
	p.state = PassengerState::IN_QUEUE;

	return p;
}

void createPassengers_BackToFront_NonRandom(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{	
	//Add passengers to the queue in order of seat location from back of the cabin
	//Do not randomly assign seats within sections

	int currentRow = simAirplane.MainAisle.twoSidedSeating.end()->first; //Begin with id of last row
	int assignedRow = simAirplane.NumRows - 1;
	int maxSeatId = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int assignedSeat = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pEndingIndex = simAirplane.PassengerIdStartingIndex + simAirplane.NumPassengers;

	for (int i = pStartingIndex; i < pEndingIndex; i++)
	{
		//Create passenger
		Passenger p = createPassenger(i, p.targetRow, p.targetSeatInRow, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

		pAll.push_back(p);
		pQueue.push(p);

		if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

		//Increment next assigned seat for the next passenger
		assignedSeat++;
		if (assignedSeat > maxSeatId)
		{
			//Change next assigned row
			assignedRow--;
			if (assignedRow < 0)
			{
				//All seats in cabin have been assigned, cannot create any more passengers
				break;
			}
			else
			{
				assignedSeat = 0;
			}
		}
	}
}

//void createPassengers_BackToFront_Random(std::list<Passenger>, std::queue<Passenger>, int, int, int);

void createPassengers_FrontToBack_NonRandom(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Add passengers to the queue in order of seat location from the front of the cabin
	//Do not randomly assign seats within sections

	int currentRow = simAirplane.MainAisle.twoSidedSeating.begin()->first; //Begin with id of first row
	int assignedRow = 0;
	int maxSeatId = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int assignedSeat = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pEndingIndex = simAirplane.PassengerIdStartingIndex + simAirplane.NumPassengers;

	for (int i = pStartingIndex; i < pEndingIndex; i++)
	{
		//Craete passenger
		Passenger p = createPassenger(i, p.targetRow, p.targetSeatInRow, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

		pAll.push_back(p);
		pQueue.push(p);

		if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

		//Increment next assigned seat for the next passenger
		assignedSeat++;
		if (assignedSeat > maxSeatId)
		{
			//Change next assigned row
			assignedRow++;
			if (assignedRow	> simAirplane.NumRows - 1)
			{
				//All seats in cabin have been assigned, cannot create any more passengers
				break;				
			}
			else
			{
				assignedSeat = 0;
			}
		}
	}
}
//void createPassengers_FrontToBack_Random(std::list<Passenger>, std::queue<Passenger>, int, int, int);

void createPassengers_Random(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{

}

void createPassengers_WindowMiddleAisle(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	std::vector<int> indexesAssignmentOrder;

	//Get window seat indexes
	//Port side: we can assume that the window seat is index 0
	int portWindowIndex = 0;
	//Stbd side: numPortSeats + numStbdSeats - 1
	int stbdWindowIndex = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;

	indexesAssignmentOrder.push_back(portWindowIndex); //Port window
	indexesAssignmentOrder.push_back(stbdWindowIndex); //Stbd window

	int seatsPerRow = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd;
	for (int i = 0; i < seatsPerRow / 2; i++)
	{
		//i = number of seats away from the window seat
		int assignedSeatPort = portWindowIndex + i;
		int assignedSeatStbd = stbdWindowIndex - i;

		//Iterate over all rows
		for (int row = 0; row < simAirplane.NumRows; row++)
		{
			//Create a passenger at this row & seat
		}
	}
}

void createPassengers_SteffenPerfect(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{

}

void createPassengers_SteffenModified(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Start at the back of the cabin
	// std::vector<int> evenRowNumbers;
	// std::vector<int> oddRowNumbers;
	// for (int i = simAirplane.NumRows - 1; i > 0; i--)
	// {
	// 	if (isEven(i))
	// 	{
	// 		evenRowNumbers.push_back(i);
	// 	}
	// 	else
	// 	{
	// 		oddRowNumbers.push_back(i);
	// 	}
	// }

	//Assign seats to passengers
	int maxSeatId = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int numAssignedPassengers = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pEndingIndex = simAirplane.PassengerIdStartingIndex + simAirplane.NumPassengers;
	int pCurrentIndex = pStartingIndex;
	
	//Stbd even
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (simAirplane.verboseOutput) printf("Looking at starboard seats in row %i\n", i);
		if (!isEven(i))
		{
			continue;
		}

		for (int seatIndex = simAirplane.NumSeatsPort; seatIndex <= maxSeatId; seatIndex++)
		{
			if (numAssignedPassengers >= simAirplane.NumPassengers || pCurrentIndex > pEndingIndex)
			{
				//Can't create any more passengers
				continue;
			}
			else
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Port even
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (simAirplane.verboseOutput) printf("Looking at port seats in row %i\n", i);
		if (!isEven(i))
		{
			continue;
		}

		for (int seatIndex = 0; seatIndex < simAirplane.NumSeatsPort; seatIndex++)
		{
			if (numAssignedPassengers >= simAirplane.NumPassengers || pCurrentIndex > pEndingIndex)
			{
				//Can't create any more passengers
				continue;
			}
			else
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Stbd odd
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (simAirplane.verboseOutput) printf("Looking at starboard seats in row %i\n", i);
		if (isEven(i))
		{
			continue;
		}

		for (int seatIndex = simAirplane.NumSeatsPort; seatIndex <= maxSeatId; seatIndex++)
		{
			if (numAssignedPassengers >= simAirplane.NumPassengers || pCurrentIndex > pEndingIndex)
			{
				//Can't create any more passengers
				continue;
			}
			else
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Port odd
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (simAirplane.verboseOutput) printf("Looking at port seats in row %i\n", i);
		if (isEven(i))
		{
			continue;
		}

		for (int seatIndex = 0; seatIndex < simAirplane.NumSeatsPort; seatIndex++)
		{
			if (numAssignedPassengers >= simAirplane.NumPassengers || pCurrentIndex > pEndingIndex)
			{
				//Can't create any more passengers
				continue;
			}
			else
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers using Steffen Modified.\n", numAssignedPassengers);
}

#endif
