// methods.h

#ifndef INCLUDE_METHODS_H
#define INCLUDE_METHODS_H

#include <list>
#include <queue>

#include <cabin-seater.h>

int randInt(int lower, int upper)
{
	srand(time(NULL));
	return rand() % upper + lower;
}

int coolThing(int a, int b)
{
	return a + b;
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
		Passenger p;
		p.id = i;
		p.stowTime = randInt(simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

		//Assign seat
		p.targetRow = assignedRow;
		p.targetSeatInRow = assignedSeat;

		p.state = PassengerState::IN_QUEUE;
		pAll.push_back(p);
		pQueue.push(p);
		//if (g_Verbose) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);
		if (true) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

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
//void createPassengers_FrontToBack_NonRandom(std::list<Passenger>, std::queue<Passenger>, int, int, int);
//void createPassengers_FrontToBack_Random(std::list<Passenger>, std::queue<Passenger>, int, int, int);

#endif
