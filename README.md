# superttt

A simple competition suite for Super Tic Tac Toe.

Build the necessary files with a simple `make`.

The competition manager is `./competition.py`, for which help is available via `./competition.py -h`. `./viewcompetition` wants a competition log file as a parameter and views it. Functionality also available directly after a competition via the `-v` flag to `./competition.py`.


## Protocol

A player gets the line

    go

when it should output the first move in the game,

    nogo

if it will now receive the first move from the other player. The move format is

    x y

where `x` and `y` are integers in `[0-8]`. The board is as follows:

|     |     |     |     |     |     |     |     |     |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| 0 0 | 1 0 | 2 0 | 3 0 | 4 0 | 5 0 | 6 0 | 7 0 | 8 0 |
| 0 1 | 1 1 | 2 1 | 3 1 | 4 1 | 5 1 | 6 1 | 7 1 | 8 1 |
| 0 2 | 1 2 | 2 2 | 3 2 | 4 2 | 5 2 | 6 2 | 7 2 | 8 2 |
| 0 3 | 1 3 | 2 3 | 3 3 | 4 3 | 5 3 | 6 3 | 7 3 | 8 3 |
| 0 4 | 1 4 | 2 4 | 3 4 | 4 4 | 5 4 | 6 4 | 7 4 | 8 4 |
| 0 5 | 1 5 | 2 5 | 3 5 | 4 5 | 5 5 | 6 5 | 7 5 | 8 5 |
| 0 6 | 1 6 | 2 6 | 3 6 | 4 6 | 5 6 | 6 6 | 7 6 | 8 6 |
| 0 7 | 1 7 | 2 7 | 3 7 | 4 7 | 5 7 | 6 7 | 7 7 | 8 7 |
| 0 8 | 1 8 | 2 8 | 3 8 | 4 8 | 5 8 | 6 8 | 7 8 | 8 8 |

A player will also receive moves in the `x y` format. I think it's intuitive. The competition manager just first indicates who starts and who doesn't with `go` and `nogo` and then alternately 1. asks for an `x y` line and 2. gives an `x y` line to a player.

The provided C++ players work in that manner.
