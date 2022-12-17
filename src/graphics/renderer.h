#pragma once

#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include "util/file_util.h"
#include "graphics/asset_loader.h"
#include "graphics/shader_program.h"

namespace mkr {
    class renderer {
    private:
        std::shared_ptr<mesh> triangle_;
        std::shared_ptr<shader_program> sp_;

    public:
        renderer() {
            triangle_ = asset_loader::instance().make_triangle("triangle");
            auto vs_src = file_util::file_to_str("/mnt/PopWork/mkr_engine/src/shaders/vertex_shader.vs");
            auto fs_src = file_util::file_to_str("/mnt/PopWork/mkr_engine/src/shaders/fragment_shader.fs");
            spdlog::info(vs_src);
            spdlog::info(fs_src);
            std::vector<std::string> vs = {vs_src};
            std::vector<std::string> fs = {fs_src};
            sp_ = std::make_shared<shader_program>("sp", vs, fs);
        }

        ~renderer() {}

        void update() {
            // glEnable(GL_MULTISAMPLE);
            // glEnable(GL_CULL_FACE);
            // glEnable(GL_DEPTH_TEST);
            // glDepthFunc(GL_LESS);
            // glEnable(GL_BLEND);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // glEnable(GL_STENCIL_TEST);
            // glStencilMask(0xFF); // Each bit is written to the stencil buffer as-is.
            // glDisable(GL_STENCIL_TEST);
            // glStencilMask(0x00);

            glClearNamedFramebufferfv(0, GL_COLOR, 0, &colour::blue.r_);

            sp_->use();
            triangle_->bind();

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    };
}