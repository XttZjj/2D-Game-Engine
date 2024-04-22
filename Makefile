main: ./src/*.cpp
	clang++ -std=c++17 -O3 -I./src -I./Third\ Party/Lua -I./Third\ Party/LuaBridge -I./Third\ Party ./src/*.cpp ./src/collision/*.cpp ./src/common/*.cpp ./src/dynamics/*.cpp ./src/rope/*.cpp -o game_engine_linux -lSDL2 -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf -llua5.4
