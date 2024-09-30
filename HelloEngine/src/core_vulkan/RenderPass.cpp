
#include "core_vulkan/RenderPass.h"
#include "core_vulkan/Manager.h"
namespace Core {
    RenderPass::RenderPass(std::shared_ptr<RenderPassBuilder> builder) {
        auto create_info = builder->build();
        render_pass = g_manager->m_device->m_device.createRenderPass(create_info);
    }

    RenderPass::~RenderPass() {
        g_manager->m_device->m_device.destroyRenderPass(render_pass);
    }
}