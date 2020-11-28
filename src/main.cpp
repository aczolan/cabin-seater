// main.cpp

#include <list>
#include <queue>
#include <string>
#include <vector>

#include <cabin.h>
#include <airplane.h>
#include <methods.h>
#include <util.h>

const int setting_NUMROWS = 3;
const int setting_LASTROWINDEX = setting_NUMROWS - 1;
const int setting_NUMSEATS_PORT = 3;
const int setting_NUMSEATS_STBD = 3;

const int setting_NUMPASSENGERS = 17;
const int setting_PASSENGER_MINSTOWTIME = 2;
const int setting_PASSENGER_MAXSTOWTIME = 10;
const int setting_PASSENGERS_STARTING_INDEX = 100;

const int setting_GLOBAL_TIMER_TIMEOUT = 100;

bool setting_Verbose = true;
bool g_RunSimulation = true;
uint64_t g_globalTimer = 0;
//CabinAisle g_MainAisle;
SimulatorState g_SimState;

bool PopulatePassengerList(int queueingAlgorithm, Airplane plane, std::list<Passenger> &pAll, std::queue<Passenger> &pQueue)
{
	bool covidSeating = plane.CovidSeating;
	
	//1: Back-To-Front
	//2: Front-To-Back
	//3: Random
	//4: Window-Middle-Aisle
	//5: Steffen Perfect
	//6: Steffen Modified

	switch(queueingAlgorithm)
	{
		case 1:
			createPassengers_BackToFront(plane, pAll, pQueue, covidSeating);
			return true;
		case 2:
			createPassengers_FrontToBack(plane, pAll, pQueue, covidSeating);
			return true;
		case 3:
			createPassengers_Random(plane, pAll, pQueue, covidSeating);
			return true;
		case 4:
			createPassengers_WindowMiddleAisle(plane, pAll, pQueue, covidSeating);
			return true;
		case 5:
			createPassengers_SteffenPerfect(plane, pAll, pQueue, covidSeating);
			return true;
		case 6:
			createPassengers_SteffenModified(plane, pAll, pQueue, covidSeating);
			return true;
		default:
			return false;
	}
}

void PrintQueue(std::queue<Passenger> q)
{
	int index = 0;
	if (!q.empty())
	{
		printf("Passenger Queue:\n");
	}
	else
	{
		printf("The queue is empty.\n");
	}
	while(!q.empty())
	{
		Passenger p = q.front();
		printf("%i : ID %i TargetRow %i TargetSeat %i\n", index, p.id, p.targetRow, p.targetSeatInRow);

		q.pop();
		index++;
	}
}

void PrintPassengersList(std::list<Passenger> pList)
{
	printf("Passenger List: Size %lu\n", pList.size());
	std::list<Passenger>::iterator p_it;
	int index = 0;
	for (p_it = pList.begin(); p_it != pList.end(); p_it++)
	{
		printf("Passenger %i: ID %i, Target Seat (%i-%i), StowTime %i, Lifetime %i\n",
			   index, p_it->id, p_it->targetRow, p_it->targetSeatInRow, p_it->stowTime, p_it->lifetime );
	}
}

std::string GetOutputData(Airplane simAirplane, std::list<Passenger> pList)
{
	// run id, q alg, num passengers, num rows, num seats port, num seats stbd, min stow, max stow, avg passenger lifetime

	//Calculate avg passenger lifetime
	int sumLifetimes = 0;
	int sumStowTimes = 0;
	for (std::list<Passenger>::iterator p_it = pList.begin(); p_it != pList.end(); p_it++)
	{
		sumLifetimes += p_it->lifetime;
		sumStowTimes += p_it->initStowTime;
	}

	double pListDouble = pList.size() * 1.0;
	double avgLifetime = sumLifetimes / pListDouble;
	double avgStowtime = sumStowTimes / pListDouble;

	std::string ret = "" + 
					simAirplane.RunID + "," +
					std::to_string(simAirplane.SelectedAlgorithmID) + "," +
					std::to_string(simAirplane.NumPassengers) + "," +
					std::to_string(simAirplane.NumRows) + "," +
					std::to_string(simAirplane.NumSeatsPort) + "," +
					std::to_string(simAirplane.NumSeatsStbd) + "," +
					std::to_string(simAirplane.PassengerMinStowTime) + "," +
					std::to_string(simAirplane.PassengerMaxStowTime) + "," +
					std::to_string(avgLifetime) + "," +
					std::to_string(avgStowtime) +
					"\n";

	return ret;
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	g_SimState = SimulatorState::RUN;

	//Initialize an AirplaneSettings object and fill it with all our settings
	Airplane SimAirplane(setting_Verbose);
	SimAirplane.NumRows = setting_NUMROWS;
	SimAirplane.LastRowIndex = SimAirplane.NumRows - 1;
	SimAirplane.NumSeatsPort = setting_NUMSEATS_PORT;
	SimAirplane.NumSeatsStbd = setting_NUMSEATS_STBD;
	SimAirplane.NumPassengers = setting_NUMPASSENGERS;
	SimAirplane.PassengerMinStowTime = setting_PASSENGER_MINSTOWTIME;
	SimAirplane.PassengerMaxStowTime = setting_PASSENGER_MAXSTOWTIME;
	SimAirplane.PassengerIdStartingIndex = setting_PASSENGERS_STARTING_INDEX;
	SimAirplane.MainAisle;

	//Get command line arguments and set members of SimAirplane
	//argv[0] = program name
	//argv[1] = run id
	//argv[2] = csv to append to
	//argv[3] = queueing algorithm id
	//argv[4] = min stow time
	//argv[5] = max stow time
	//argv[6] = num passengers
	//argv[7] = num rows
	//argv[8] = enable covid seating

	int numExpectedArgs = 9;
	if (argc != numExpectedArgs)
	{
		//Wrong number of args
		printf("Incorrect number of args. Expected %i, got %i\n",
			   numExpectedArgs, argc);
		return 1;
		//exit
	}

	std::vector<char*> ArgsList;

	printf("GIVEN ARGS:\n");
	for (int i = 0; i < argc; ++i)
	{
		printf("%s\n", argv[i]);
		ArgsList.push_back(argv[i]);
	}

	std::string s_runID(ArgsList[1]);
	SimAirplane.RunID = s_runID;
	std::string s_CSVname(ArgsList[2]);
	SimAirplane.CSVname = s_CSVname;
	SimAirplane.SelectedAlgorithmID = std::atoi(ArgsList[3]);
	SimAirplane.PassengerMinStowTime = std::atoi(ArgsList[4]);
	SimAirplane.PassengerMaxStowTime = std::atoi(ArgsList[5]);
	SimAirplane.NumPassengers = std::atoi(ArgsList[6]);
	SimAirplane.NumRows = std::atoi(ArgsList[7]);
	SimAirplane.CovidSeating = std::atoi(ArgsList[8]);
	SimAirplane.LastRowIndex = SimAirplane.NumRows - 1;

	if (setting_Verbose) printf("ArgsList Contents:\n");
	for (int i = 0; i < argc; i++)
	{
		printf("%s\n", ArgsList[i]);
	}

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
	//**********
	//Select a queueing algorithm
	if (setting_Verbose) printf("Creating passengers with algorithm %i...\n", SimAirplane.SelectedAlgorithmID);
	bool createPassengersSuccess = PopulatePassengerList(SimAirplane.SelectedAlgorithmID, SimAirplane, allPassengers, passengersQueue);
	if (!createPassengersSuccess)
	{
		//Error creating passengers
		printf("Error creating passengers using algorithm %i.\n", SimAirplane.SelectedAlgorithmID);
		return 1;
		//exit
	}

	if (setting_Verbose) PrintPassengersList(allPassengers);
	if (setting_Verbose) PrintQueue(passengersQueue);
	
	//Iterate over all passengers and have them step forward
	int numPassengersFinished = 0;

	while (g_RunSimulation)
	{
		g_globalTimer++;
		bool passengerDequeuedThisTurn = false;
		if (setting_Verbose) printf("\n\n| TIME: %lu |\n\n", g_globalTimer);

		//Iterate over all passengers and modify their states/position as needed
		for (std::list<Passenger>::iterator p_it = allPassengers.begin(); p_it != allPassengers.end(); p_it++)
		{
			if (p_it->state == PassengerState::SATISFIED || p_it->state == PassengerState::FAILED)
			{
				//Nothing to do
				continue;
			}

			p_it->lifetime++;

			if (p_it->state == PassengerState::IN_QUEUE && !passengerDequeuedThisTurn)
			{
				if ( !(passengersQueue.front().IsEqual(*p_it)) )
				{
					//Do not move this passenger, since they are not at the front of the queue
					continue;
				}
				else
				{
					if (setting_Verbose) printf("Front of queue passenger: %i\n", passengersQueue.front().id);
				}

				//Move to the aisle's first space if it is unoccupied
				auto firstSpace = SimAirplane.MainAisle.twoSidedSeating.begin()->second.first;
				if (!firstSpace.occupied)
				{
					if (setting_Verbose) printf("Passenger %i: Identified first aisle space as id %i\n", p_it->id, firstSpace.id);
					if (p_it->occupySpace(firstSpace))
					{
						firstSpace.setOccupied();
						SimAirplane.MainAisle.SetAisleSpaceOccupied(firstSpace.id);

						passengersQueue.pop();
						p_it->state = PassengerState::IN_AISLE;

						if (setting_Verbose) printf("Passenger %i: Entered aisle at space %i\n", p_it->id, p_it->currentSpace.id);
						passengerDequeuedThisTurn = true;
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
					if (setting_Verbose) printf("Passenger %i: Staying in queue (First aisle space is occupied)\n", p_it->id);
				}
			}
			else if (p_it->state == PassengerState::IN_AISLE)
			{
				if (setting_Verbose) printf("Passenger %i: I'm at aisle space %i. Gonna try to find my seat (%i-%i) at this row.\n",
											p_it->id, p_it->currentSpace.id,
											p_it->targetRow, p_it->targetSeatInRow);

				//Check to see if the passenger has reached its target row
				if (p_it->CurrentRowIsTarget())
				{
					//Passenger has reached its target row!
					//Check if the passenger has any stow time remaining
					if (p_it->stowTime != 0)
					{
						//Passenger spends 1 time unit stowing
						p_it->stowTime -= 1;
						if (setting_Verbose) printf("Passenger %i: Stowing at row %i. Remaining stow time: %i\n",
													p_it->id, p_it->currentSpace.id, p_it->stowTime);
					}
					else
					{
						//TODO Refine this code section to manually set the target seat's status.

						//Check this row for the target seat
						//For now, immediately take the seat if it is unoccupied
						int thisRowId = p_it->currentSpace.id;
						bool foundSeat = false;
						bool tookSeat = false;
						if (SimAirplane.CheckSeatsInRow(*p_it, thisRowId, foundSeat, tookSeat))
						{
							if (setting_Verbose) printf("Passenger %i: CheckSeatsInRowResults: Found=%s, Took=%s\n",
														p_it->id, 
														foundSeat ? "True" : "False",
														tookSeat ? "True" : "False");

							//Seat was found
							if (tookSeat)
							{
								if (setting_Verbose) printf("Passenger %i found its seat!\n", p_it->id);
								SimAirplane.MainAisle.SetSeatOccupied(p_it->targetRow, p_it->targetSeatInRow);
								p_it->state = PassengerState::SATISFIED;
								numPassengersFinished++;
							}
							else
							{
								//Seat was found but not taken
								if (setting_Verbose) printf("Passenger %i: Error: Found target seat but could not take it.\n", p_it->id);

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
							//Seat was not found
							if (setting_Verbose) printf("Passenger %i: Error: Did not find target seat in target row.\n", p_it->id);

							//Failure state
							p_it->state = PassengerState::FAILED;
							//Set passenger's space to null
							OccupiableSpace *o = &(p_it->currentSpace);
							o = NULL;
							numPassengersFinished++;
						}

						//This passenger is no longer occupying the aisle space at this row.
						SimAirplane.MainAisle.SetAisleSpaceUnoccupied(thisRowId);
					}
				}
				else
				{
					//Passenger has not reached its target row yet
					//Move to the next aisle space if possible
					int nextAisleSpaceId = p_it->currentSpace.id + 1;
					if (nextAisleSpaceId > SimAirplane.LastRowIndex)
					{
						//Passenger hit the end of the aisle without finding a seat

						if (setting_Verbose) printf("Passenger %i: Error: Hit the end of the aisle without finding the target seat.\n", p_it->id);
						//Failure state
						p_it->state = PassengerState::FAILED;
						OccupiableSpace *o = &(p_it->currentSpace);
						o = NULL;
						numPassengersFinished++;
					}
					else
					{
						auto nextAisleSpace = SimAirplane.MainAisle.twoSidedSeating[nextAisleSpaceId].first;
						//if (p_it->occupySpace(nextAisleSpace))
						if (!nextAisleSpace.occupied)
						{
							//Next aisle space is not occupied
							SimAirplane.MainAisle.SetAisleSpaceUnoccupied(p_it->currentSpace.id);
							p_it->occupySpace(nextAisleSpace);
							nextAisleSpace.setOccupied();
							SimAirplane.MainAisle.SetAisleSpaceOccupied(nextAisleSpace.id);

							if (setting_Verbose) printf("Passenger %i: Moved forward to aisle space %i\n",
														p_it->id, p_it->currentSpace.id);
						}
						else
						{
							//Next aisle space is occupied
							if (setting_Verbose) printf("Passenger %i: Staying at aisle space %i\n",
														p_it->id, p_it->currentSpace.id);
						}
					}
					
				}

			}
		}

		//Check if we hit the end state
		if (numPassengersFinished == SimAirplane.NumPassengers)
		{
			//All passengers are done moving
			g_SimState = SimulatorState::COMPLETE;
			printf("All passengers found their seats!\n");
			g_RunSimulation = false;
		}

		//Check for infinite looping
		if (g_globalTimer > setting_GLOBAL_TIMER_TIMEOUT)
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
	PrintPassengersList(allPassengers);

	bool writeToOutput = appendLineToFile(SimAirplane.CSVname, GetOutputData(SimAirplane, allPassengers));
	if (writeToOutput) printf("Wrote to %s.\n", SimAirplane.CSVname.c_str());

	printf("Done!\n");
}
