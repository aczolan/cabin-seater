// methods.h

#ifndef INCLUDE_METHODS_H
#define INCLUDE_METHODS_H

#include <list>
#include <map>
#include <queue>
#include <vector>

#include <cabin.h>
#include <airplane.h>
#include <util.h>

#include <ctime>

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

void createPassengers_BackToFront(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{	
	//Add passengers to the queue in order of seat location from back of the cabin

	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pCurrentIndex = pStartingIndex;
	int numAssignedPassengers = 0;

	for (int row = simAirplane.NumRows - 1; row > 0; --row)
	{
		for (int seat = 0; seat < simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd; seat++)
		{
			if (numAssignedPassengers != simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, row, seat, 
											  simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				pAll.push_back(p);
				pQueue.push(p);
				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers with Back-to-Front.\n", numAssignedPassengers);
}

void createPassengers_FrontToBack(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Add passengers to the queue in order of seat location from the front of the cabin

	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pCurrentIndex = pStartingIndex;
	int numAssignedPassengers = 0;

	for (int row = 0; row < simAirplane.NumRows; row++)
	{
		for (int seat = 0; seat < simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd; seat++)
		{
			if (numAssignedPassengers != simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, row, seat, 
											  simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				pAll.push_back(p);
				pQueue.push(p);
				pCurrentIndex++;
				numAssignedPassengers++;
				if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers with Front-to-Back.\n", numAssignedPassengers);
}

void createPassengers_Random(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Create list of all selectable seats
	std::vector<std::pair<int, int>> SelectableSeats;
	//Iterate over all seats and fill the list
	for (int i = 0; i <= simAirplane.NumRows - 1; i++)
	{
		for (int j = 0; j <= simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1; j++)
		{
			auto seatPair = std::make_pair(i, j);
			SelectableSeats.push_back(seatPair);
		}
	}

	int numAssignedPassengers = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pCurrentIndex = pStartingIndex;

	while (!SelectableSeats.empty() && numAssignedPassengers <= simAirplane.NumPassengers)
	{
		//Randomly select an entry in the vector
		int vecMin = 0;
		int vecMax = SelectableSeats.size();
		int randIndex = randInt(vecMin, vecMax);
		std::pair<int, int> selectedSeat = SelectableSeats[randIndex];

		Passenger p = createPassenger(pCurrentIndex, selectedSeat.first, selectedSeat.second, 
									  simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);
		pAll.push_back(p);
		pQueue.push(p);

		if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

		numAssignedPassengers++;
		pCurrentIndex++;
		//Remove this entry from the selectable seats vector
		SelectableSeats.erase(SelectableSeats.begin() + randIndex);
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers with Random.\n", numAssignedPassengers);
}

void createPassengers_WindowMiddleAisle(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Create a lists for window, middle, and aisle seats
	//For now, we assume that:
		//Port Window: 0
		//Port Middle: Window + 1
		//Port Aisle: Window + 2
		//etc..
		//Stbd Window: Max Seat Index
		//Stbd Middle: Window - 1
		//Stbd Aisle: Window - 2
		//etc..
	//This algorithm also covers the case where there are more than three seats per grouplet

	std::queue< std::vector< std::pair<int, int> > > allBoardingGroups;

	int portWindowIndex = 0;
	int stbdWindowIndex = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int rowHalfwayIndex = stbdWindowIndex / 2;

	for (int indexesFromWindow = 0; indexesFromWindow < rowHalfwayIndex; indexesFromWindow++)
	{
		std::vector< std::pair<int, int> > thisBoardingGroup;

		for (int rowNumber = 0; rowNumber < simAirplane.NumRows; rowNumber++)
		{
			//Add the seat on the port side
			auto portSeat = std::make_pair(rowNumber, portWindowIndex + indexesFromWindow);
			thisBoardingGroup.push_back(portSeat);

			//Add the seat on the stbd side
			auto stbdSeat = std::make_pair(rowNumber, stbdWindowIndex - indexesFromWindow);
			thisBoardingGroup.push_back(stbdSeat);
		}

		allBoardingGroups.push(thisBoardingGroup);
	}

	//Assign seats within boarding groups randomly
	int numAssignedPassengers = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pCurrentIndex = pStartingIndex;

	while(!allBoardingGroups.empty() && numAssignedPassengers <= simAirplane.NumPassengers)
	{
		auto thisBoardingGroup = allBoardingGroups.front();
		allBoardingGroups.pop();
		//Randomly assign seats from this group
		while (!thisBoardingGroup.empty() && numAssignedPassengers <= simAirplane.NumPassengers)
		{
			//Randomly select an entry in the vector
			int vecMin = 0;
			int vecMax = thisBoardingGroup.size();
			int randIndex = randInt(vecMin, vecMax);
			std::pair<int, int> selectedSeat = thisBoardingGroup[randIndex];

			Passenger p = createPassenger(pCurrentIndex, selectedSeat.first, selectedSeat.second, 
										  simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);
			pAll.push_back(p);
			pQueue.push(p);

			if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

			numAssignedPassengers++;
			pCurrentIndex++;

			//Remove this entry from the selectable seats vector
			thisBoardingGroup.erase(thisBoardingGroup.begin() + randIndex);
		}
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers with Window-Middle-Aisle.\n", numAssignedPassengers);
}

void createPassengers_SteffenPerfect(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	std::queue< std::pair<int, int> > seatAssignmentQueue;

	int portWindowIndex = 0;
	int stbdWindowIndex = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int rowHalfwayIndex = stbdWindowIndex / 2;

	for (int indexesFromWindow = 0; indexesFromWindow < rowHalfwayIndex; indexesFromWindow++)
	{
		//Starting from the back of the cabin...

		//Select even seats on the port side
		for (int rowIndex = simAirplane.NumRows - 1; rowIndex >= 0; --rowIndex)
		{
			if (isEven(rowIndex))
			{
				auto thisSeat = std::make_pair(rowIndex, portWindowIndex + indexesFromWindow);
				seatAssignmentQueue.push(thisSeat);
			}
		}

		//Select even seats on the starboard side
		for (int rowIndex = simAirplane.NumRows - 1; rowIndex >= 0; --rowIndex)
		{
			if (isEven(rowIndex))
			{
				auto thisSeat = std::make_pair(rowIndex, stbdWindowIndex - indexesFromWindow);
				seatAssignmentQueue.push(thisSeat);
			}
		}

		//Select odd seats on the port side
		for (int rowIndex = simAirplane.NumRows - 1; rowIndex >= 0; --rowIndex)
		{
			if (!isEven(rowIndex))
			{
				auto thisSeat = std::make_pair(rowIndex, portWindowIndex + indexesFromWindow);
				seatAssignmentQueue.push(thisSeat);
			}
		}

		//Select odd seats on the starboard side
		for (int rowIndex = simAirplane.NumRows - 1; rowIndex >= 0; --rowIndex)
		{
			if (!isEven(rowIndex))
			{
				auto thisSeat = std::make_pair(rowIndex, stbdWindowIndex - indexesFromWindow);
				seatAssignmentQueue.push(thisSeat);
			}
		}
	}

	//Assign passengers one-to-one with entries in the seat queue
	int numAssignedPassengers = 0;
	int pCurrentIndex = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;

	while (!seatAssignmentQueue.empty() && numAssignedPassengers <= simAirplane.NumPassengers)
	{
		auto nextSeat = seatAssignmentQueue.front();
		seatAssignmentQueue.pop();

		Passenger p = createPassenger(pCurrentIndex, nextSeat.first, nextSeat.second,
									  simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

		pAll.push_back(p);
		pQueue.push(p);

		if (simAirplane.verboseOutput) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

		numAssignedPassengers++;
		pCurrentIndex++;
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers with Steffen Perfect.\n", numAssignedPassengers);
}

void createPassengers_SteffenModified(Airplane simAirplane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	//Assign seats to passengers
	int maxSeatId = simAirplane.NumSeatsPort + simAirplane.NumSeatsStbd - 1;
	int numAssignedPassengers = 0;
	int pStartingIndex = simAirplane.PassengerIdStartingIndex;
	int pCurrentIndex = pStartingIndex;

	// Passenger p = createPassenger(pCurrentIndex, 1, 1, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);
	// pAll.push_back(p);
	// pQueue.push(p);
	
	//Stbd even
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (true) printf("Looking at starboard seats in row %i\n", i);
		if (!isEven(i))
		{
			continue;
		}

		for (int seatIndex = simAirplane.NumSeatsPort; seatIndex <= maxSeatId; seatIndex++)
		{
			if (numAssignedPassengers < simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (true) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Port even
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (true) printf("Looking at port seats in row %i\n", i);
		if (!isEven(i))
		{
			continue;
		}

		for (int seatIndex = 0; seatIndex < simAirplane.NumSeatsPort; seatIndex++)
		{
			if (numAssignedPassengers < simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (true) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Stbd odd
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (true) printf("Looking at starboard seats in row %i\n", i);
		if (isEven(i))
		{
			continue;
		}

		for (int seatIndex = simAirplane.NumSeatsPort; seatIndex <= maxSeatId; seatIndex++)
		{
			if (numAssignedPassengers < simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (true) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	//Port odd
	for (int i = simAirplane.NumRows - 1; i >= 0; --i)
	{
		if (true) printf("Looking at port seats in row %i\n", i);
		if (isEven(i))
		{
			continue;
		}

		for (int seatIndex = 0; seatIndex < simAirplane.NumSeatsPort; seatIndex++)
		{
			if (numAssignedPassengers < simAirplane.NumPassengers)
			{
				Passenger p = createPassenger(pCurrentIndex, i, seatIndex, simAirplane.PassengerMinStowTime, simAirplane.PassengerMaxStowTime);

				p.state = PassengerState::IN_QUEUE;
				pAll.push_back(p);
				pQueue.push(p);

				pCurrentIndex++;
				numAssignedPassengers++;
				if (true) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);				
			}
		}
	}

	if (simAirplane.verboseOutput) printf("Created %i passengers using Steffen Modified.\n", numAssignedPassengers);
}

#endif
