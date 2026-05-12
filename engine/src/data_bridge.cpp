#include "data_bridge.hpp"
#include <cassert>

void DataBridge::update_result(AnalysisResult&& result) {
    std::lock_guard<std::mutex> lock(mtx);
    current_result = std::move(result);
}

AnalysisResult DataBridge::fetch_result() {
    std::lock_guard<std::mutex> lock(mtx);

    AnalysisResult res;
    res.success = current_result.success;
    res.errors = current_result.errors;

    if (current_result.ast_root) {
        res.ast_root = std::move(current_result.ast_root);
    }

    return res;
}

