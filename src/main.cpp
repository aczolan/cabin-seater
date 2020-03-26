// main.cpp

#include <utility>

#include <cabin-seater.h>

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

int main()
{
	SimulatorState g_SimState = SimulatorState::DECISION;
	CabinAisle g_MainAisle;

	//Populate the main aisle
	populateAisle(g_MainAisle, 5, 3, 7);

	//Print the aisle's contents
	printAisleContents(g_MainAisle);

	while (g_SimState != SimulatorState::DECISION)
	{
		
	}
}
