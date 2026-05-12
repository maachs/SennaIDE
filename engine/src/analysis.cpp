#include "analysis.hpp"
#include "visualizer.hpp"
#include "semantic.hpp"
#include <cstdio>
#include <cassert>

extern FILE* yyin;
extern int yyparse();
extern std::unique_ptr<BlockNode> rootBlock;

AnalysisWorker::AnalysisWorker(DataBridge& bridge)
    : bridge(bridge), running(true), has_task(false) {
    worker_thread = std::thread(&AnalysisWorker::loop, this);
}

AnalysisWorker::~AnalysisWorker() {
    running = false;
    cv.notify_all();
    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

void AnalysisWorker::request_analysis(const std::string& filepath) {
    assert(!filepath.empty() && "Cannot analyze empty filepath");
    {
        std::lock_guard<std::mutex> lock(cv_m);
        pending_file = filepath;
        has_task = true;
    }
    cv.notify_one();
}

void AnalysisWorker::loop() {
    while (running) {
        std::string current_task;
        {
            std::unique_lock<std::mutex> lock(cv_m);
            cv.wait(lock, [this]{ return !running || has_task; });

            if (!running) break;

            current_task = pending_file;
            has_task = false;
        }

        perform_analysis(current_task);
    }
}

void AnalysisWorker::perform_analysis(const std::string& filepath) {
    AnalysisResult local_result;
    syntax_errors.clear();

    FILE* file = fopen(filepath.c_str(), "r");
    if (!file) {
        local_result.errors.push_back("Error: Cannot open source file.");
        bridge.update_result(std::move(local_result));
        return;
    }

    yyin = file;
    rootBlock = std::make_unique<BlockNode>();

    if (yyparse() == 0) {
        SemanticAnalyzer semantic_checker;
        rootBlock->accept(semantic_checker);

        if (!semantic_checker.has_errors()) {
            local_result.success = true;
            local_result.ast_root = std::move(rootBlock);

            Visualizer::generate_ast_image(
                local_result.ast_root.get(),
                "output/dot/ast_output.dot",
                "output/png/ast_output.png"
            );
        } else {
            local_result.success = false;
            local_result.errors = semantic_checker.get_errors();
        }
    } else {
        local_result.success = false;
        local_result.errors = syntax_errors;
    }

    fclose(file);

    bridge.update_result(std::move(local_result));
}
