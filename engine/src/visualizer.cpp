#include "visualizer.hpp"
#include "dot_print.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>

bool Visualizer::generate_ast_image(BlockNode* root, const std::string& out_dot, const std::string& out_png) {
    assert(root != nullptr && "Cannot visualize a null AST root");
    assert(!out_dot.empty() && !out_png.empty() && "Output paths must not be empty");

    std::ofstream out_file(out_dot);
    if (!out_file.is_open()) {
        std::cerr << "Engine Error: Failed to open " << out_dot << " for writing.\n";
        return false;
    }

    GraphDump dumper(out_file);
    dumper.header_write();
    root->accept(dumper);
    dumper.footer_write();
    out_file.close();

    std::string command = "./scripts/dot2png.sh " + out_dot + " " + out_png;

    int ret_code = system(command.c_str());
    if (ret_code != 0) {
        std::cerr << "Engine Error: dot2png.sh script failed with code " << ret_code << "\n";
        return false;
    }

    return true;
}
