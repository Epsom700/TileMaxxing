#pragma once
#include <string>
#include <onnxruntime_cxx_api.h>

class NetworkInference {
public:
    explicit NetworkInference(const std::string& model_path);
    float evaluate(const int board[4][4]) const;

private:
    Ort::Env env_;
    Ort::Session session_;
};
