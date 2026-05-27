#include "network.h"
#include <array>
#include <cmath>
#include <stdexcept>

NetworkInference::NetworkInference(const std::string& model_path)
    : env_(ORT_LOGGING_LEVEL_WARNING, "TileMaxxing"),
      session_(env_, model_path.c_str(), Ort::SessionOptions{}) {}

float NetworkInference::evaluate(const int board[4][4]) {
    // Build one-hot tensor: [1, 17, 4, 4] in NCHW layout
    // Channel k represents tile value 2^k; channel 0 = empty cell
    std::array<float, 17 * 4 * 4> input_data{};
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            int v = board[r][c];
            int ch = (v > 0) ? __builtin_ctz(v) : 0;
            input_data[ch * 16 + r * 4 + c] = 1.0f;
        }
    }

    std::array<int64_t, 4> input_shape{1, 17, 4, 4};
    auto mem = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        mem, input_data.data(), input_data.size(),
        input_shape.data(), input_shape.size());

    const char* input_names[]  = {"board"};
    const char* output_names[] = {"value"};

    auto outputs = session_.Run(
        Ort::RunOptions{nullptr},
        input_names,  &input_tensor, 1,
        output_names, 1);

    // Model was trained on log1p(eval); invert to restore original score scale
    float log_val = outputs[0].GetTensorMutableData<float>()[0];
    return std::expm1f(log_val);
}
