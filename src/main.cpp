// main.cpp

#include <utility>
#include <list>
#include <queue>

#include <cabin-seater.h>

bool g_Verbose = true;
bool g_RunSimulation = true;
uint64_t g_globalTimer = 0;
CabinAisle g_MainAisle;
SimulatorState g_SimState;

const int g_NUMROWS = 5;
const int g_NUMSEATS_PORT = 3;
const int g_NUMSEATS_STBD = 3;

void populateAisle(CabinAisle &aisle, int numAisles, int numSeatsPort, int numSeatsStarboard)
{
	for (int i = 0; i < numAisles; i++)
	{
		AisleSpace newAisleSpace;
		newAisleSpace.id = i;

		SeatGrouplet port(numSeatsPort, 0);
		SeatGrouplet starboard(numSeatsStarboard, numSeatsPort);

		//Create pairs
		auto seatsPair = std::make_pair(port, starboard);
		auto aisleToSeats = std::make_pair(newAisleSpace, seatsPair);
		auto newEntry = std::make_pair(i, aisleToSeats);
		aisle.twoSidedSeating.insert(newEntry);
	}
}

void printAisleContents(CabinAisle aisle)
{
	std::map<int, std::pair<AisleSpace, std::pair<SeatGrouplet, SeatGrouplet>>>::iterator a_it;
	for (a_it = aisle.twoSidedSeating.begin(); a_it != aisle.twoSidedSeating.end(); a_it++)
	{
		//Print general SeatGrouplet info
		int entryId = a_it->first;
		int aisleId = a_it->second.first.id;
		int numSeatsPort = a_it->second.second.first.numSeats;
		int numSeatsStbd = a_it->second.second.second.numSeats;
		printf("Entry: %i, Aisle: %i, NumSeats Port: %i, Starboard: %i\n", entryId, aisleId, numSeatsPort, numSeatsStbd);

		int portFirstId = a_it->second.second.first.groupletSeats.begin()->first;
		int stbdFirstId = a_it->second.second.second.groupletSeats.begin()->first;

		//Print info on seats in this grouplet
		printf("Port Seats:\n");
		for (int i = portFirstId; i < portFirstId + numSeatsPort; i++)
		{
			auto seat = a_it->second.second.first.groupletSeats[i];
			printf("\tSeat %i: Occupied: %s\n", seat.id, seat.occupied ? "True" : "False");
		}
		printf("Starboard Seats:\n");
		for (int i = stbdFirstId; i < stbdFirstId + numSeatsStbd; i++)
		{
			auto seat = a_it-> second.second.second.groupletSeats[i];
			printf("\tSeat %i: Occupied: %s\n", seat.id, seat.occupied ? "True" : "False");
		}
	}
}

bool checkSeatsInRow(std::map<int, SeatSpace> seatsToCheck, int targetSeatId, SeatSpace &foundSpace)
{
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

bool analyzeRow(Passenger &p, std::pair<AisleSpace, std::pair<SeatGrouplet, SeatGrouplet>> row, bool &foundSeat, bool &tookSeat)
{
	auto portSeats = row.second.first.groupletSeats;
	auto stbdSeats = row.second.second.groupletSeats;
	SeatSpace target;
	
	//TODO Collapse these ifs
	
	//Check port seats
	if (checkSeatsInRow(portSeats, p.targetSeatInRow, target))
	{
		//Found seat
		foundSeat = true;
		if (g_Verbose) printf("Passenger %i found seat %i in row %i\n", p.id, p.targetSeatInRow, p.targetRow);

		if (p.occupySpace(target))
		{
			//Passenger successfully occupies the target space
			if (g_Verbose) printf("Passenger %i takes seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = true;
			return true;
		}
		else
		{
			//Passenger did not occupy the target space
			if (g_Verbose) printf("Passenger %i does not take seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = false;
			return true;
		}
	}
	//Check starboard seats
	else if (checkSeatsInRow(stbdSeats, p.targetSeatInRow, target))
	{
		//Found seat
		foundSeat = true;
		printf("Passenger %i found seat %i in row %i\n", p.id, p.targetSeatInRow, p.targetRow);
		if (p.occupySpace(target))
		{
			//Passenger successfully occupies the target space
			if (g_Verbose) printf("Passenger %i takes seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = true;
			return true;
		}
		else
		{
			//Passenger did not occupy the target space
			if (g_Verbose) printf("Passenger %i does not take seat %i in row %i\n", p.id, target.id, p.targetRow);
			tookSeat = false;
			return true;
		}
	}
}

void createPassengers_BackToFront(std::list<Passenger> &pAll, std::queue<Passenger> &pQueue, int numPassengers, int startingId)
{
	//Add passengers to the queue in order of seat location from back of the cabin
	int currentRow = g_MainAisle.twoSidedSeating.end()->first; //Get id of last row
	int assignedRow = g_NUMROWS - 1;
	int maxSeatId = g_NUMSEATS_PORT + g_NUMSEATS_STBD - 1;
	int assignedSeat = 0;

	for (int i = startingId; i < startingId + numPassengers; i++)
	{
		Passenger p;
		p.id = i;

		//Assign seat
		p.targetRow = assignedRow;
		p.targetSeatInRow = assignedSeat;

		p.state = PassengerState::IN_QUEUE;
		pAll.push_back(p);
		pQueue.push(p);
		if (g_Verbose) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);

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

int main()
{
	g_SimState = SimulatorState::DECISION;

	//Populate the main aisle
	populateAisle(g_MainAisle, 7, 3, 3);

	//Print the aisle's contents
	printAisleContents(g_MainAisle);

	bool allPassengersSatisfied = false;

	//Create passenger list
	std::list<Passenger> allPassengers;
	std::queue<Passenger> passengersQueue;
	//Populate passenger list
	int numPassengers = 3;
	int startingId = 100;
	createPassengers_BackToFront(allPassengers, passengersQueue, numPassengers, startingId);

	// for (int i = startingId; i < startingId + numPassengers; i++)
	// {
	// 	Passenger p;
	// 	p.id = i;
	// 	p.targetRow = 4;
	// 	p.targetSeatInRow = 9;
	// 	p.state = PassengerState::IN_QUEUE;
	// 	allPassengers.push_back(p);
	// 	passengersQueue.push(p);
	// 	if (g_Verbose) printf("Created Passenger %i: Target Row: %i, Target Seat: %i\n", p.id, p.targetRow, p.targetSeatInRow);
	// }
	
	//Iterate over all passengers and have them step forward
	int numPassengersFinished = 0;
	while (g_RunSimulation)
	{
		g_globalTimer++;
		//if (g_Verbose) printf("%i | ", g_globalTimer);
		for (std::list<Passenger>::iterator p_it = allPassengers.begin(); p_it != allPassengers.end(); p_it++)
		{
			if (p_it->state == PassengerState::SATISFIED || p_it->state == PassengerState::FAILED)
			{
				//Nothing to do
				continue;
			}

			p_it->lifetime++;

			if (p_it->state == PassengerState::IN_QUEUE)
			{
				//Move to the aisle's first space if it is unoccupied
				auto firstSpace = g_MainAisle.twoSidedSeating.begin()->second.first;
				if (!firstSpace.occupied)
				{
					if (g_Verbose) printf("Identified first aisle space as id %i\n", firstSpace.id);
					p_it->occupySpace(firstSpace);
					p_it->state = PassengerState::IN_AISLE;
					if (g_Verbose) printf("Passenger %i: Entered aisle at space %i\n", p_it->id, p_it->currentSpace.id);
				}
				else
				{
					//Stay in queue
				}
			}
			else if (p_it->state == PassengerState::IN_AISLE)
			{
				//Check this row for the target seat
				//For now, immediately take the seat if it is unoccupied
				bool foundSeat = false;
				bool tookSeat = false;
				if (analyzeRow(*p_it, g_MainAisle.twoSidedSeating[p_it->currentSpace.id], foundSeat, tookSeat))
				{
					//Seat was found
					if (tookSeat)
					{
						p_it->state = PassengerState::SATISFIED;
						numPassengersFinished++;
					}
					else
					{
						//Seat was found but not taken
						//Failure state
						p_it->state = PassengerState::FAILED;
						//Set passenger's space to null
						OccupiableSpace *o = &(p_it->currentSpace);
						o = NULL;
						numPassengersFinished++;
					}
				}
				else
				{
					//Target seat was not in this row
					//Move to the next aisle space if possible
					auto currentAisleSpaceId = p_it->currentSpace.id;
					int nextAisleSpaceId = currentAisleSpaceId + 1;
					auto nextAisleSpace = g_MainAisle.twoSidedSeating[nextAisleSpaceId].first;
					p_it->occupySpace(nextAisleSpace);
				}
			}
		}

		//Check if we hit the end state
		if (numPassengersFinished == numPassengers)
		{
			//All passengers are done moving
			g_RunSimulation = false;
		}
	}
	

	// int passengerLocation = 0;
	// bool pSatisfied = false;
	// Passenger p;
	// //Check if this space is on the correct row
	// auto currentRow = g_MainAisle.twoSidedSeating[passengerLocation];
	// p.currentSpace = currentRow.first;
	// if (g_Verbose) printf("Passenger %i: Current row space: %i\n", p.id, passengerLocation);
	// if (passengerLocation == p.targetRow)
	// {
	// 	if (g_Verbose) printf("Passenger %i: Found target row: %i\n", p.id, passengerLocation);
	// 	bool foundSeat = false;
	// 	bool tookSeat = false;
	// 		//Will find the target seat, and occupy it if it is found and not empty
	// 	bool analyzeResult = analyzeRow(p, currentRow, foundSeat, tookSeat);
	// 	if (foundSeat && tookSeat)
	// 	{
	// 		pSatisfied = true;
	// 	}
	// }
	// else
	// {
	// 	//Do nothing and wait to move forward again
	// }
	// 	//Check if all passengers have their seats satisfied
	// //This should be a list query
	// if (pSatisfied)
	// {
	// 	g_SimState = SimulatorState::COMPLETE;
	// }
	//} //while g_SimState == DECISION
	
	//All done!
	//Print the aisle's contents
	printf("FINAL AISLE CONTENTS:\n");
	printAisleContents(g_MainAisle);
	printf("Done.\n");
}
