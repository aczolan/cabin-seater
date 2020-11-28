# cabin-seater
Airplane cabin seating simulator.

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