#pragma once
#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>
#include <mutex>
#include "data_bridge.hpp"

class AnalysisWorker {
private:
    DataBridge& bridge;

    std::thread worker_thread;
    std::condition_variable cv;
    std::mutex cv_m;

    std::atomic<bool> running;
    std::string pending_file;
    bool has_task;

    void loop();

    void perform_analysis(const std::string& filepath);

public:
    explicit AnalysisWorker(DataBridge& bridge);

    ~AnalysisWorker();

    void request_analysis(const std::string& filepath);
};
