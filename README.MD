# Tord
## Description
Truth or Dare bot made for a personal discord server

Basically just my own interface for api.truthordarebot.xyz with additional features

## Build Instructions
### Prereqs
Install required tools `sudo apt install cmake build-essential`
Install dpp library from https://dpp.dev/
### Clone the repo
- `git clone https://github.com/timerunner16/tordbot tordbot`
- `mkdir -p tordbot/build`
- `cd tordbot/build`
### Prepare the environment
Make a .env within the build directory containing the line `BOT_TOKEN="discord bot token here"`
### Build the Project:
`cmake ..` `cmake --build .`
- `./tordbot` to run
