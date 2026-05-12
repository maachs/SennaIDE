#pragma once
#include <string>
#include "ast.hpp"

class Visualizer {
public:
    static bool generate_ast_image(BlockNode* root,
                                   const std::string& out_dot,
                                   const std::string& out_png);
};
