# kmc-lotto

## About
kmc-lotto is a simple C++ library that performs event selection for arbitrary kinetic Monte Carlo (KMC) simulations.

This project is developed by the [Van der Ven group](https://labs.materials.ucsb.edu/vanderven/anton/) in the Materials Department at the University of California, Santa Barbara.
The repository is written and maintained by me, Jonas Kaufman.
[Dr. John Goiri](https://github.com/goirijo/) implemented the binary tree used for rejection-free event selection (and suggested the name "lotto") and [Dr. Sanjeev Kolli](https://github.com/skk74) provided guidance during development.

## Installation
kmc-lotto is a header-only template library, so it does not need to be compiled on its own.
You may find it convenient to access the headers by including kmc-lotto as a submodule in your program's repository.

You may also use autotools to install the headers in a specific location. To install under `$HOME/.local` (substitute this with your location of choice), run:
```bash
./boostrap.sh
./configure --prefix=$HOME/.local
make install
```

There are also tests included to make sure things are working properly. To compile and run the tests, run the configure step above and then:
```bash
mkdir build
cd build
make check
```

Note that a compiler with C++17 support is needed to compile code that uses kmc-lotto, including the tests.

## Usage
kmc-lotto makes very few assumptions about the nature of the KMC simulation it is driving.
As such, any functionality specific to your simulation you must implement yourself.
The library provides implementations of standard [KMC algorithms](https://en.wikipedia.org/wiki/Kinetic_Monte_Carlo#Algorithms) (rejection or rejection-free) via event selector classes.
Each event selector object takes care of its own pseudo-random number generation using the Mersenne Twister 19937 generator.

The event selector classes are templated on an event ID type and a rate calculator type, which have certain requirements:
* Every kinetic event that could possibly happen over the course of a simulation must be enumerated initially and assigned a unique ID.
* The ID type is up to you, as long as it supports copying and the `==` operator. For example, the IDs could be integers corresponding to indices into some data structure that stores the events, or pointers to the events themselves.
* You must define a rate calculator class with a method named `calculate_rate` that takes only an event ID and returns the event's rate. This class will likely have to interact with other parts of your simulation code.

To construct an event selector object:
* You must provide a list of all unique event IDs (as a `std::vector<EventIDType>`).
* For rejection event selection, you must provide an upper bound on the event rates. The tighter this upper bound is, the faster selection will be on average.
* For rejection-free event selection, you must provide an impact table (currently a `std::map` from `EventIDType` to `std::vector<EventIDType>`) that indicates which events' rates are impacted by carrying out a given event in your simulation.

Once constructed, calling an event selector's `select_event` method will select the next event, returning its ID and the time step for that selection (in units inverse to those of your event rates).
Note that the rejection event selector will repeatedly attempt to select until an event is accepted.

For an example of kmc-lotto in action, see [apb-kmc](https://github.com/jonaskaufman/apb-kmc).
