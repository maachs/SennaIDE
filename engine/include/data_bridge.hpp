#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <string>

#include "ast.hpp"

struct AnalysisResult {
    std::unique_ptr<BlockNode> ast_root;
    std::vector<std::string> errors;
    bool success = false;
};

class DataBridge {
private:
    mutable std::mutex mtx;
    AnalysisResult current_result;

public:
    void update_result(AnalysisResult&& result);

    AnalysisResult fetch_result();
};
