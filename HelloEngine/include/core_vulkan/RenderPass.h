#pragma once

#include "Common.h"
#include "core_vulkan/RenderPassBuilder.h"

namespace Core {
    class RenderPass {
        no_copy_move_construction(RenderPass)
    public:
        RenderPass(std::shared_ptr<RenderPassBuilder> builder);
        ~RenderPass();
    INNER_VISIBLE:
        VkRenderPass render_pass;
    };
}
