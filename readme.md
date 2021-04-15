# cabin-seater
Airplane cabin seating simulator. Originally written for CS 6045 Advanced Algorithms. Check out my [paper](https://github.com/aczolan/cabin-seater/blob/master/Airplane_Cabin_Boarding_Methods.pdf) for a full breakdown of the simulation, as well as its goals and results. Inspired by CGP Grey's ["The Better Boarding Method Airlines Won't Use"](https://youtu.be/oAHbLRjF0vo) video.

## Build
```
mkdir build
cd build
cmake ..
make
```

## Run
```
./build/bin/cabin-seater [run name] [output csv name] [queueing algorithm id] [min stow time] [max stow time] [num. passengers] [num. rows]
```
Example:
```
./build/bin/cabin-seater run_1 run_1_output.csv 1 1 10 6 3
```
Check out the `scripts/` folder for some examples.