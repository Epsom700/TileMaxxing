#pragma once
#include <fstream>

class NetworkInference;

int playAndLog(int simulations, std::ofstream& file, int game_id,
               NetworkInference* net = nullptr);
