
#include "core_vulkan/Renderer.h"

namespace Core {
    Renderer::Renderer(std::shared_ptr<RenderPassBuilder> builder) : render_pass_builder(builder), resized(false), current_in_flight(0) {
        render_pass = std::make_unique<RenderPass>(builder);
        framebuffer_set = std::make_unique<FrameBufferSet>(*this);

        image_available_semaphores.resize(setting.max_in_flight_frame);
        render_finished_semaphores.resize(setting.max_in_flight_frame);
        in_flight_fences.resize(setting.max_in_flight_frame);

        command_buffers = manager->command_pool->allocate_command_buffer(setting.max_in_flight_frame);
        current_in_flight = 0;
        runtime_setting->current_in_flight = 0;
        current_buffer = command_buffers[0];

        vk::SemaphoreCreateInfo semaphore_create_info {};
        vk::FenceCreateInfo fence_create_info {};
        fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
        for (uint32_t i = 0; i < setting.max_in_flight_frame; i ++) {
            image_available_semaphores[i] = manager->device->device.createSemaphore(semaphore_create_info);
            render_finished_semaphores[i] = manager->device->device.createSemaphore(semaphore_create_info);
            in_flight_fences[i] = manager->device->device.createFence(fence_create_info);
            in_flight_submit_infos.emplace_back();
        }
    }

    void Renderer::set_resize_flag() {
        resized = true;
    }

    void Renderer::wait_for_destroy() {
        vkDeviceWaitIdle(manager->device->device);
    }

    void Renderer::update_resources() {
        manager->recreate_swapchin();
        framebuffer_set.reset();
        framebuffer_set = std::make_unique<FrameBufferSet>(*this);
        for (auto &[id, callback] : callbacks) {
            callback();
        }
    }

    void Renderer::update_renderpass() {
        render_pass.reset();
        render_pass = std::make_unique<RenderPass>(render_pass_builder);
        update_resources();
    }

    Renderer::~Renderer() {
        current_graphics_shader_program.reset();
        current_ray_tracing_shader_program.reset();
        wait_for_destroy();
        callbacks.clear();
        for (uint32_t i = 0; i < setting.max_in_flight_frame; i ++) {
            manager->device->device.destroySemaphore(image_available_semaphores[i]);
            manager->device->device.destroySemaphore(render_finished_semaphores[i]);
            manager->device->device.destroyFence(in_flight_fences[i]);
        }
        framebuffer_set.reset();
        render_pass.reset();
        render_pass_builder.reset();
    }

    void Renderer::set_clear_value(const std::string &name, const vk::ClearValue &value) {
        uint32_t index = render_pass_builder->get_attachment_index(name, false);
        render_pass_builder->attachments[index].clear_value = value;
    }

    void Renderer::acquire_next_image() {
        vk_check(manager->device->device.waitForFences({ in_flight_fences[current_in_flight] }, VK_TRUE, UINT64_MAX));

        try {
            auto result = manager->device->device.acquireNextImageKHR(manager->swapchain->swapchain, UINT64_MAX, image_available_semaphores[current_in_flight], VK_NULL_HANDLE, &index);
            if (result == vk::Result::eErrorOutOfDateKHR) {
                update_resources();
                return;
            }
        } catch (vk::OutOfDateKHRError) {
            update_resources();
            return;
        }

        manager->device->device.resetFences({ in_flight_fences[current_in_flight] });

        current_subpass = 0;
        current_buffer.reset();

        vk::CommandBufferBeginInfo begin_info {};
        current_buffer.begin(begin_info);
    }

    void Renderer::begin_render_pass() {
        std::vector<vk::ClearValue> clear_values;
        clear_values.reserve(render_pass_builder->attachments.size());
        for (const auto &attachment_description : render_pass_builder->attachments) {
            clear_values.push_back(attachment_description.clear_value);
        }

        vk::RenderPassBeginInfo render_pass_begin_info {};
        render_pass_begin_info.setRenderPass(render_pass->render_pass)
            .setFramebuffer(framebuffer_set->framebuffers[index]->framebuffer)
            .setRenderArea({
                { 0, 0 },
                { runtime_setting->get_window_size().width, runtime_setting->get_window_size().height }
            })
            .setClearValues(clear_values);
        current_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    }

    void Renderer::end_render_pass() {
        current_buffer.endRenderPass();
    }

    void Renderer::report_submit_info(const vk::SubmitInfo &submit_info) {
        in_flight_submit_infos[current_in_flight].push_back(submit_info);
    }

    void Renderer::present(const std::vector<vk::PipelineStageFlags> &wait_stages, const std::vector<vk::Semaphore> &wait_samaphores) {
        current_buffer.end();

        auto &submit_info = in_flight_submit_infos[current_in_flight].emplace_back();
        auto wait_stages_ = wait_stages;
        wait_stages_.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        auto wait_samaphores_ = wait_samaphores;
        wait_samaphores_.push_back(image_available_semaphores[current_in_flight]);

        submit_info.setWaitSemaphores(wait_samaphores_)
            .setWaitDstStageMask(wait_stages_)
            .setCommandBuffers(current_buffer)
            .setSignalSemaphores(render_finished_semaphores[current_in_flight]);
        manager->device->graphics_queue.submit(in_flight_submit_infos[current_in_flight], in_flight_fences[current_in_flight]);

        vk::PresentInfoKHR present_info {};
        present_info.setWaitSemaphores(render_finished_semaphores[current_in_flight])
            .setSwapchains(manager->swapchain->swapchain)
            .setImageIndices(index)
            .setPResults(nullptr);
        try {
            auto result = manager->device->present_queue.presentKHR(present_info);
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || resized) {
                update_resources();
                resized = false;
            }
        } catch (vk::OutOfDateKHRError) {
            update_resources();
        }
        current_in_flight = (current_in_flight + 1) % setting.max_in_flight_frame;
        runtime_setting->current_in_flight = current_in_flight;
        current_buffer = command_buffers[current_in_flight];
        in_flight_submit_infos[current_in_flight].clear();
    }

    void Renderer::begin_render() {
        acquire_next_image();
        begin_render_pass();
    }

    void Renderer::end_render() {
        end_render_pass();
        present();
    }

    void Renderer::begin_layer_render(const std::string &name) {
        layers[layers_map.at(name)]->begin_render();
    }

    void Renderer::end_layer_render(const std::string &name) {
        layers[layers_map.at(name)]->end_render();
    }

    void Renderer::inner_bind_shader_program(vk::PipelineBindPoint bind_point, std::shared_ptr<ShaderProgram> shader_program) {
        current_buffer.bindPipeline(bind_point, shader_program->pipeline);
        if (!shader_program->descriptor_sets.empty()) {
            std::vector<vk::DescriptorSet> sets;
            for (auto &descriptor_set : shader_program->descriptor_sets) {
                sets.push_back(descriptor_set.value()->descriptor_sets[current_in_flight]);
            }
            current_buffer.bindDescriptorSets(bind_point, shader_program->layout, 0, sets, {});
        }
        if (shader_program->push_constants.has_value()) {
            auto push_constants = shader_program->push_constants.value();
            current_buffer.pushConstants(shader_program->layout, push_constants->range.stageFlags, 0, push_constants->constants_size, push_constants->constants.data());
        }
    }

    void Renderer::bind_vertex_buffer(const std::shared_ptr<VertexBuffer> &vertex_buffer, uint32_t binding) {
        current_buffer.bindVertexBuffers(binding, { vertex_buffer->buffer->buffer }, { 0 });
    }

    void Renderer::bind_vertex_buffers(const std::vector<std::shared_ptr<VertexBuffer>> &vertex_buffers, uint32_t first_binding) {
        std::vector<vk::Buffer> buffers(vertex_buffers.size());
        std::vector<vk::DeviceSize> sizes(vertex_buffers.size());
        for (uint32_t i = 0; i < vertex_buffers.size(); i ++) {
            buffers[i] = vertex_buffers[i]->buffer->buffer;
            sizes[i] = 0;
        }
        current_buffer.bindVertexBuffers(first_binding, buffers, sizes);
    }

    void Renderer::bind_index_buffer(std::shared_ptr<IndexBuffer> index_buffer) {
        current_buffer.bindIndexBuffer(index_buffer->buffer->buffer, 0, index_buffer->type);
    }

    void Renderer::set_viewport(float x, float y, float width, float height) {
        set_viewport(x, y, width, height, 0.0f, 1.0f);
    }

    void Renderer::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) {
        vk::Viewport viewport {
            x,
            y,
            width,
            height,
            min_depth,
            max_depth,
        };
        current_buffer.setViewport(0, { viewport });
    }

    void Renderer::set_scissor(int x, int y, uint32_t width, uint32_t height) {
        vk::Rect2D scissor {
            { x, y },
            { width, height }
        };
        current_buffer.setScissor(0, { scissor });
    }

    void Renderer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) {
        current_buffer.drawIndexed(index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void Renderer::draw_mesh(std::shared_ptr<const Mesh> mesh, uint32_t instance_count, uint32_t first_instance) {
        current_buffer.drawIndexed(mesh->indices.size(), instance_count, mesh->position.index_buffer_offset, mesh->position.vertex_buffer_offset, first_instance);
    }

    void Renderer::draw_model_mesh(std::shared_ptr<const Model> model, const std::string &name, uint32_t instance_count, uint32_t first_instance) {
        draw_mesh(model->get_mesh_by_name(name), instance_count, first_instance);
    }

    void Renderer::draw_model(std::shared_ptr<const Model> model, uint32_t instance_count, uint32_t first_instance) {
        current_buffer.drawIndexed(model->index_count, instance_count, model->position.index_buffer_offset, model->position.vertex_buffer_offset, first_instance);
    }

    void Renderer::trace_rays(uint32_t width, uint32_t height, uint32_t depth) {
        if (width == uint32_t(-1)) {
            width = runtime_setting->window_size.width;
        }
        if (height == uint32_t(-1)) {
            height = runtime_setting->window_size.height;
        }
        current_buffer.traceRaysKHR(current_ray_tracing_shader_program->raygen_region, current_ray_tracing_shader_program->miss_region, current_ray_tracing_shader_program->hit_region, {}, width, height, depth, manager->dispatcher);
    }

    void Renderer::dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) {
        current_buffer.dispatch(group_count_x, group_count_y, group_count_z);
    }

    void Renderer::next_subpass() {
        current_buffer.nextSubpass(vk::SubpassContents::eInline);
        current_subpass += 1;
    }

    void Renderer::continue_subpass_to(const std::string &subpass_name) {
        auto subpass_idx = render_pass_builder->get_subpass_index(subpass_name);
        while (current_subpass != subpass_idx) {
            next_subpass();
        }
    }

    void Renderer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
        current_buffer.draw(vertex_count, instance_count, first_vertex, first_instance);
    }

    vk::CommandBuffer Renderer::get_command_buffer() {
        return current_buffer;
    }

    uint32_t Renderer::register_resource_recreate_callback(const ResourceRecreateCallback &callback) {
        uint32_t id = current_callback_id;
        current_callback_id ++;
        callbacks.insert(std::make_pair(id, std::move(callback)));
        return id;
    }

    void Renderer::remove_resource_recreate_callback(uint32_t id) {
        callbacks.erase(id);
    }
}
