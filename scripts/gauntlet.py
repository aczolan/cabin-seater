# gauntlet.py
# Goal: Run multiple iterations of cabin-seater, evaluating effectiveness of each boarding method
# For each algorithm, run 100 iterations
# Print the results of the algorithm's 100 iterations to its own seperate CSV

# Num Passengers: 96
# Num Rows: 16
# Num Seats Port: 3
# Num Seats Stbd: 3
# Passenger Min Stow Time: 1
# Passenger Max Stow Time: 10

import subprocess

NumPassengers = 96
NumRows = 16
MinStowTime = 1
MaxStowTime = 10

cmd = '../build/bin/cabin-seater'

alg1prefix = 'BackToFront_'
alg1csv = 'BackToFrontGauntlet.csv'

alg2prefix = 'FrontToBack_'
alg2csv = 'FrontToBackGauntlet.csv'

alg3prefix = 'RandomQueueing_'
alg3csv = 'RandomQueueingGauntlet.csv'

alg4prefix = 'WindowMiddleAisle_'
alg4csv = 'WindowMiddleAisleGauntlet.csv'

alg5prefix = 'SteffenPerfect_'
alg5csv = 'SteffenPerfectGauntlet.csv'

alg6prefix = 'SteffenModified_'
alg6csv = 'SteffenModifiedGauntlet.csv'

for i in range(100):

	BackToFrontCommand = [cmd, alg1prefix + str(i), alg1csv, '1', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]
	FrontToBackCommand = [cmd, alg2prefix + str(i), alg2csv, '2', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]
	RandomQueueCommand = [cmd, alg3prefix + str(i), alg3csv, '3', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]
	WMACommand = [cmd, alg4prefix + str(i), alg4csv, '4', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]
	StefPerfectCommand = [cmd, alg5prefix + str(i), alg5csv, '5', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]
	StefModCommand = [cmd, alg6prefix + str(i), alg6csv, '6', str(MinStowTime), str(MaxStowTime), str(NumPassengers), str(NumRows)]

	allCommands = [ BackToFrontCommand, FrontToBackCommand, RandomQueueCommand, WMACommand, StefPerfectCommand, StefModCommand ] 

	for command in allCommands:
		commandOut = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdout, stderr = commandOut.communicate()
		#print(stdout)

	if i % 10 == 0:
		print ('Finished iteration ' + str(i))

print('Done!')