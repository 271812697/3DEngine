/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include<glad/glad.h>

#include "OvRendering/Buffers/Framebuffer.h"

OvRendering::Buffers::Framebuffer::Framebuffer(uint16_t p_width, uint16_t p_height)
	:width(p_width),height(p_height)
{

	/* Generate OpenGL objects */
	glCreateFramebuffers(1, &m_bufferID);
	//AddColorTexture(1, multisample);
    //AddDepStRenderBuffer(multisample);
}

OvRendering::Buffers::Framebuffer::~Framebuffer()
{
	/* Destroy OpenGL objects */
	glDeleteFramebuffers(1, &m_bufferID);
	for (auto id : color_attachments) {
		glDeleteTextures(1, &id);
	}
	if (m_depthStencilBuffer) {
		glDeleteRenderbuffers(1, &m_depthStencilBuffer);
		m_depthStencilBuffer = 0;
	}
	if (m_deptexture) {
		glDeleteTextures(1, &m_deptexture);
		m_deptexture = 0;
	}
}

void OvRendering::Buffers::Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
}

void OvRendering::Buffers::Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OvRendering::Buffers::Framebuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	this->width = p_width;
	this->height = p_height;
	int cnt = color_attachments.size();
	for (auto id : color_attachments) {
		glDeleteTextures(1, &id);
	}
	color_attachments.clear();
	AddColorTexture(cnt, is_multisample);
	if (m_depthStencilBuffer) {
		glDeleteRenderbuffers(1, &m_depthStencilBuffer);
		m_depthStencilBuffer = 0;
		AddDepStRenderBuffer(is_multisample);
	}

	if (m_deptexture) {
		glDeleteTextures(1, &m_deptexture);
		m_deptexture = 0;
		//AddDepStTexture();
	}
}
void OvRendering::Buffers::Framebuffer::AddColorTexture(uint16_t count, bool multisample) {
	is_multisample = multisample;
	size_t n_color_buffs = color_attachments.size();
	color_attachments.reserve(n_color_buffs + count);
	for (GLuint i = 0; i < count; i++) {
		GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		uint32_t m_renderTexture=0;
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderTexture);
		glTextureStorage2D(m_renderTexture, 1, GL_RGBA16F, width, height);
		static const float border[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// we cannot set any of the sampler states for multisampled textures
		if (!multisample) {
			glTextureParameteri(m_renderTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_renderTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTextureParameterfv(m_renderTexture, GL_TEXTURE_BORDER_COLOR, border);
		}
		glNamedFramebufferTexture(m_bufferID, GL_COLOR_ATTACHMENT0 + n_color_buffs + i, m_renderTexture, 0);
		color_attachments.emplace_back(m_renderTexture);;
	}
    SetDrawBuffers();
}
void OvRendering::Buffers::Framebuffer::AddDepStRenderBuffer(bool multisample) {
	if (m_depthStencilBuffer == 0) {
		is_multisample = multisample;
		glCreateRenderbuffers(1, &m_depthStencilBuffer);
		if (multisample)
		{
			glNamedRenderbufferStorageMultisample(m_depthStencilBuffer,4, GL_DEPTH24_STENCIL8, width, height);
		}
		else {
			glNamedRenderbufferStorage(m_depthStencilBuffer, GL_DEPTH24_STENCIL8, width, height);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
		glNamedFramebufferRenderbuffer(m_bufferID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}
void OvRendering::Buffers::Framebuffer::SetDrawBuffers() const
{
    if (size_t n = color_attachments.size(); n > 0) {
        GLenum* attachments = new GLenum[n];
        for (GLenum i = 0; i < n; i++) {
            *(attachments + i) = GL_COLOR_ATTACHMENT0 + i;
        }
        glNamedFramebufferDrawBuffers(m_bufferID, n, attachments);
        delete[] attachments;
    }
}
void OvRendering::Buffers::Framebuffer::CopyColor(const Framebuffer& fr, GLuint fr_idx, const Framebuffer& to, GLuint to_idx)
{

    // if the source and target rectangle areas differ in size, interpolation will be applied
    GLuint fw = fr.width, fh = fr.height;
    GLuint tw = to.width, th = to.height;

    glNamedFramebufferReadBuffer(fr.GetID(), GL_COLOR_ATTACHMENT0 + fr_idx);
    glNamedFramebufferDrawBuffer(to.GetID(), GL_COLOR_ATTACHMENT0 + to_idx);
    glBlitNamedFramebuffer(fr.GetID(), to.GetID(), 0, 0, fw, fh, 0, 0, tw, th, GL_COLOR_BUFFER_BIT, GL_NEAREST);

}
uint32_t OvRendering::Buffers::Framebuffer::GetID() const
{
    return m_bufferID;
}


uint32_t OvRendering::Buffers::Framebuffer::GetTextureID(uint16_t index)
{
	if(index<color_attachments.size())
	return color_attachments.at(index);
	return 0;
}
uint32_t OvRendering::Buffers::Framebuffer::GetRenderBufferID()
{
	return m_depthStencilBuffer;
}
