# ktane

keep talkin and nobody goes poof :D

this is a physical recreation of the... _briefcase_, from KEEP TALKING AND NOBODY \<REDACTED\>. the intention of this project is to be as faithful as possible to the original, so some modules might not be included or otherwise altered so that they can exist within the fabric of reality :]

# gameplay loop

as reality cannot be procedurally generated, and is hence often disappointing, the briefcase must be set up at a designated time before the game is considered "active", where every physical element can be assembled manually.

as such, the briefcase will have the following states:

```
enum Phase {
  SETUP, ACTIVE, SOLVED
};
```

1. setup (if applicable):  
   puzzle is inactive, time is frozen at starting time (e.g. 5:00)  
   modules can be inserted or disconnected  
   modules cannot cause strikes or be completed

2. active:
   puzzle is active, time will start ticking down, and modules can be completed/cause mistakes

3. solved (win/loss):
   puzzle is inactive, time is at 0:00  
   modules should not be connected or disconnected at this time  
   modules cannot cause strikes or be completed

# modules

as the original game is grouped into modules, it'd be an absolute shame not to engineer the recreation as modular, as each physical "module" can roughly correspond to one virtual module.

## generic module implementation

each of these modules will be designed as a standalone ATMega328p (IC from an Arduino R3), as this is a lot more cost-effective than dedicating an entire dev board to a module.

as the modules must stay in sync and send/receive information, they must be connected through communication. we're using a simple 2-wire I<sup>2</sup>C communication bus (for details, see "Communication")

all modules will have four pinouts to connect to a bus, aside from the exposed interfaces needed for the module:

- 5v (red)
- gnd (black)
- scl (white)
- sda (yellow)

further circuit schematics can be found in the `kicad/` folder.

## controller "module"

this isn't an actual module, but it has the same footprint and effectively acts as a normal module, so we're counting it here. this is the designated main hub for all of the briefcase's unique properties, including:

- battery count
- indicators
- serial number
- strikes
- timer

this controller will also include a handful of interactive elements, such as a speaker for sound effects and buttons to configure and advance state.

## wires module (finally, the good stuff)

during setup phase, this module will accept any number of pin connections between the 6 left and 6 right terminals. these connections must be made with custom wires with a resistance representing their colour, as follows:

- yellow: 100 &#937;
- red: 220 &#937;
- blue: 470 &#937;
- black: 680 &#937;
- white: 1k &#937;

once setup phase is complete, the module will commit the final configuration of the wires as the "starting state", and any disconnections will cause either a strike or a completion.

any CONNECTIONS... i mean, idk man don't do that lol

# communication

communication across the puzzle is handled through an I<sup>2</sup>C multi-master (different addresses can choose to initiate communication) bus. currently, there is no plan to implement arbitration, so a simple protocol must be created to prevent crosstalk. the general idea is that for any given "state change" event (e.g. user interacting with a module, module/controller connecting, needy module reactivating), only one address should be initiating communication, though this can include any number or combination of send/receives.

## controller

always on address 8

### possible initiations

- request state from module
- send reset signal to module
- send activate signal to module

## module

allocated 56 addresses, could be between 9-64  
there are 14 total modules, but addresses must be unique, so each module type can assign as one of 4 addresses:

- 9-12: wires
- 13-16: button
- 17-20: keypad
- 21-24: simon says
- 25-28: who's on first
- 29-32: memory
- 33-36: morse code
- 37-40: complicated wires
- 41-44: wire sequences(??)
- 45-48: maze
- 49-52: password
- 53-56: gas
- 57-60: capacitor
- 61-64: knob

### possible initiations

- request data from controller
- send status to controller (-1/255 for "caused strike")
