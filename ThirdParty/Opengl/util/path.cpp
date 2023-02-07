#include "../pch.h"
#include "path.h"

namespace utils::paths {

    std::filesystem::path solution;

    std::string root, source, resource;
    std::string font, model, screenshot, shader, texture;


    void SearchPaths() {
        solution = std::filesystem::current_path();

        while (!std::filesystem::exists(solution / "sketchpad.sln")) {
            solution = solution.parent_path();
        }

        if (!std::filesystem::is_directory(solution)) {
            std::cout << "Solution directory does not exist!" << std::endl;
            std::cin.get();  // pause the console before exiting
            exit(EXIT_FAILURE);
        }

        if (std::filesystem::is_empty(solution)) {
            std::cout << "Solution directory is empty!" << std::endl;
            std::cin.get();  // pause the console before exiting
            exit(EXIT_FAILURE);
        }

        auto src_path = solution / "src";
        auto res_path = solution / "res";

        root     = solution.string() + "\\";
        source   = src_path.string() + "\\";
        resource = res_path.string() + "\\";

        font       = (res_path / "font"      ).string() + "\\";
        model      = (res_path / "model"     ).string() + "\\";
        screenshot = (res_path / "screenshot").string() + "\\";
        shader     = (res_path / "shader"    ).string() + "\\";
        texture    = (res_path / "texture"   ).string() + "\\";
    }

}