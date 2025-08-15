# Tord
## Description
A bot where I rewrote features of other bots when I got annoyed of them needing premium.
Just made for a small server I share with some friends, but I try to use good practices anyway.

Currently contains:
- Truth or Dare (or WYR, or NHIE, or Paranoia)
- Confessions (with a filter to hide some speech patterns (pirate speak))

Stuff planned for the future:
- Music/radio features
- Other games (RPS, some fighting stuff maybe, cards against humanity mayhaps?)
- Other little interactions

## Build Instructions
### Prereqs
Install required tools `sudo apt install cmake build-essential`

Install required libraries
- Install sqlite3 `sudo apt install sqlite3 libsqlite3-dev`
- Install dpp library from https://dpp.dev/

### Clone the repo
`git clone https://github.com/timerunner16/tordbot tordbot`

### Prepare the environment
`mkdir -p tordbot/build`

`cd tordbot/build`

Make a .env within the build directory containing the line `BOT_TOKEN="discord bot token here"`

### Build the Project:
`cmake ..`

`cmake --build .`

`./tordbot` to run
