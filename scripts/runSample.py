import subprocess

# ./build/bin/cabin-seater sampleRun_BackToFront results.csv 1 2 10 15 4
sampleCommand1 = ['../build/bin/cabin-seater', 'sampleRun_BackToFront', 'results.csv', '1', '2', '10', '15', '4']

# ./build/bin/cabin-seater sampleRun_FrontToBack results.csv 2 2 10 15 4
sampleCommand2 = ['../build/bin/cabin-seater', 'sampleRun_FrontToBack', 'results.csv', '2', '2', '10', '15', '4']

# ./build/bin/cabin-seater sampleRun_Random results.csv 3 2 10 10 4
sampleCommand3 = ['../build/bin/cabin-seater', 'sampleRun_Random', 'results.csv', '3', '2', '10', '10', '4']

# ./build/bin/cabin-seater sampleRun_WindowMiddleAisle results.csv 4 2 10 15 4
sampleCommand4 = ['../build/bin/cabin-seater', 'sampleRun_WindowMiddleAisle', 'results.csv', '4', '2', '10', '15', '4']

# ./build/bin/cabin-seater sampleRun_SteffenPerfect results.csv 5 2 10 15 4
sampleCommand5 = ['../build/bin/cabin-seater', 'sampleRun_SteffenPerfect', 'results.csv', '5', '2', '10', '15', '4']

# ./build/bin/cabin-seater sampleRun_SteffenModified results.csv 6 2 10 15 4
sampleCommand6 = ['../build/bin/cabin-seater', 'sampleRun_SteffenModified', 'results.csv', '6', '2', '10', '15', '4']


allCommands = [ sampleCommand1, sampleCommand2, sampleCommand3, sampleCommand4, sampleCommand5, sampleCommand6 ]

for command in allCommands:
	commandOut = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout, stderr = commandOut.communicate()
	print(stdout)


'''
# ./build/bin/cabin-seater sampleRun_LongPlaneSteffenPerfect results.csv 5 2 10 500 100
longCommand = ['../build/bin/cabin-seater', 'sampleRun_SteffenPerfect', 'results.csv', '5', '2', '10', '500', '100']

longCommandOut = subprocess.Popen(longCommand, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout, stderr = longCommandOut.communicate()
	print(stdout)
'''
