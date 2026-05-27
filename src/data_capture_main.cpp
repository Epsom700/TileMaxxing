#include "data_capture.h"
#include "network.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    int num_games = (argc > 1) ? std::atoi(argv[1]) : 150;
    int simulations = (argc > 2) ? std::atoi(argv[2]) : 2000;

    std::filesystem::create_directories("data");

    std::time_t ts = std::time(nullptr);
    std::string path = "data/run_" + std::to_string(ts) + ".jsonl";

    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "failed to open " << path << "\n";
        return 1;
    }

    std::cout << "writing to " << path
              << " (" << num_games << " games, " << simulations << " sims/move)\n";

    NetworkInference net("./model/tile_value_net.onnx");

    for (int game_id = 0; game_id < num_games; ++game_id) {
        int final_score = playAndLog(simulations, file, game_id, &net);
        file.flush();
        std::cout << "game " << game_id << " done, score " << final_score << "\n";
    }

    return 0;
}
