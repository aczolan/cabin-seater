// main.cpp

#include <list>
#include <queue>

#include <cabin-seater.h>
#include <methods.h>

const int setting_NUMROWS = 3;
const int setting_LASTROWINDEX = setting_NUMROWS - 1;
const int setting_NUMSEATS_PORT = 3;
const int setting_NUMSEATS_STBD = 3;

const int setting_NUMPASSENGERS = 4;
const int setting_PASSENGER_MINSTOWTIME = 2;
const int setting_PASSENGER_MAXSTOWTIME = 10;
const int setting_PASSENGERS_STARTING_INDEX = 100;

bool setting_Verbose = true;
bool g_RunSimulation = true;
uint64_t g_globalTimer = 0;
//CabinAisle g_MainAisle;
SimulatorState g_SimState;

int main()
{
	g_SimState = SimulatorState::RUN;

	//Initialize an AirplaneSettings object and fill it with all our settings
	Airplane SimAirplane(setting_Verbose);
	SimAirplane.NumRows = setting_NUMROWS;
	SimAirplane.LastRowIndex = setting_LASTROWINDEX;
	SimAirplane.NumSeatsPort = setting_NUMSEATS_PORT;
	SimAirplane.NumSeatsStbd = setting_NUMSEATS_STBD;
	SimAirplane.NumPassengers = setting_NUMPASSENGERS;
	SimAirplane.PassengerMinStowTime = setting_PASSENGER_MINSTOWTIME;
	SimAirplane.PassengerMaxStowTime = setting_PASSENGER_MAXSTOWTIME;
	SimAirplane.PassengerIdStartingIndex = setting_PASSENGERS_STARTING_INDEX;
	//SimAirplane.MainAisle;


	//Populate the main aisle
	SimAirplane.PopulateMainAisle();
	SimAirplane.MainAisle.ClearAllSeats();

	//Print the aisle's contents
	SimAirplane.MainAisle.PrintAisle();

	bool allPassengersSatisfied = false;

	//Create passenger list and queue
	std::list<Passenger> allPassengers;
	std::queue<Passenger> passengersQueue;

	//Populate passenger list
	//Select a queueing algorithm
	createPassengers_BackToFront_NonRandom(SimAirplane, allPassengers, passengersQueue);
	
	//Iterate over all passengers and have them step forward
	int numPassengersFinished = 0;
	while (g_RunSimulation)
	{
		g_globalTimer++;
		if (setting_Verbose) printf("| TIME: %i |\n", g_globalTimer);

		//Iterate over all passengers and modify their states/position as needed
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
				auto firstSpace = SimAirplane.MainAisle.twoSidedSeating.begin()->second.first;
				if (!firstSpace.occupied)
				{
					if (setting_Verbose) printf("Passenger %i: Identified first aisle space as id %i\n", p_it->id, firstSpace.id);
					if (p_it->occupySpace(firstSpace))
					{
						firstSpace.setOccupied();
						p_it->state = PassengerState::IN_AISLE;
						if (setting_Verbose) printf("Passenger %i: Entered aisle at space %i\n", p_it->id, p_it->currentSpace.id);
					}
					else
					{
						//Was not able to occupy the space for some reason
						if (setting_Verbose) printf("Passenger %i: Couldn't enter aisle space %i. Its state is: %s\n", p_it->id, firstSpace.id, firstSpace.occupied ? "Occupied" : "NOT Occupied");
						if (setting_Verbose) printf("Passenger %i: Gonna stay in queue for now\n", p_it->id);
					}
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
				//if (analyzeRow(*p_it, SimAirplane.MainAisle.twoSidedSeating[p_it->currentSpace.id], foundSeat, tookSeat))
				if (SimAirplane.CheckSeatsInRow(*p_it, p_it->currentSpace.id, foundSeat, tookSeat))
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
					if (nextAisleSpaceId > SimAirplane.LastRowIndex)
					{
						//Passenger hit the end of the aisle without finding a seat
						//Failure state
						p_it->state = PassengerState::FAILED;
						OccupiableSpace *o = &(p_it->currentSpace);
						o = NULL;
						numPassengersFinished++;
					}
					auto nextAisleSpace = SimAirplane.MainAisle.twoSidedSeating[nextAisleSpaceId].first;
					p_it->occupySpace(nextAisleSpace);
					nextAisleSpace.setOccupied();

					if (setting_Verbose) printf("Passenger %i: Moved forward to aisle space %i\n", p_it->id, p_it->currentSpace.id);
				}
			}
		}

		//Check if we hit the end state
		if (numPassengersFinished == SimAirplane.NumPassengers)
		{
			//All passengers are done moving
			g_SimState = SimulatorState::COMPLETE;
			g_RunSimulation = false;
		}

		//Check for infinite looping
		if (g_globalTimer > 30)
		{
			printf("Breaking\n");
			break;
		}
	}
	
	//All done!
	//Print the aisle's contents
	printf("FINAL AISLE CONTENTS:\n");
	//printAisleContents(SimAirplane.MainAisle);
	SimAirplane.MainAisle.PrintAisle();
	printf("Done.\n");
}
