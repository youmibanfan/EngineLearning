#pragma once

#include "Common.h"
#include "vulkan/vulkan.hpp"

namespace Core {
    class Renderer;

    class Attachment {
        no_copy_construction(Attachment)
    public:
        Attachment();
        Attachment(const vk::AttachmentDescription& description, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect);
        Attachment(Attachment &&rhs);
        void operator=(Attachment &&rhs);
        ~Attachment();
    INNER_VISIBLE:
        std::unique_ptr<Image> image;
        vk::ImageView image_view;
    };

    class FrameBuffer {
        no_copy_move_construction(FrameBuffer)
    public:
        FrameBuffer(const Renderer &renderer, const std::vector<vk::ImageView> &image_views);
        ~FrameBuffer();
    INNER_VISIBLE:
        vk::Framebuffer framebuffer;
    };

    class FrameBufferSet {
        no_copy_move_construction(FrameBufferSet)
    public:
        FrameBufferSet(const Renderer &renderer);
        ~FrameBufferSet();
    INNER_VISIBLE:
        std::vector<Attachment> attachments;
        std::vector<std::unique_ptr<FrameBuffer>> framebuffers;
    };
}
