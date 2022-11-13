﻿#include "pch.hpp"
#include "EditTool.h"

#include <glm/gtc/type_ptr.hpp>
#include <bitset>

#include "CanvasPanel.h"
#include "RenderingSystem.h"


EditTool::EditTool(CanvasPanel* canvas): Tool(canvas)
{
	
}

EditTool::~EditTool()
{
	DestroyRenderTarget();
}

void EditTool::ClickOrDragStart()
{
	SelectedStroke = Canvas->ActiveDrawing->Strokes.back().get();
	MousePrev = Canvas->MousePosOnDrawing;
}

void EditTool::Dragging()
{
	glm::vec2 delta = Canvas->MousePosOnDrawing - MousePrev;

	for (auto& p : SelectedStroke->Position)
	{
		p = { p.x() + delta.x, p.y() + delta.y };
	}
	SelectedStroke->OnChanged();
	MousePrev = Canvas->MousePosOnDrawing;
}

void EditTool::Activate()
{
	GenRenderTargetFromActiveDrawing();
	RenderTextureForSelection();
}

void EditTool::DragEnd()
{
	RenderTextureForSelection();
}

void EditTool::Deactivate()
{
	DestroyRenderTarget();
}

void EditTool::GenRenderTargetFromActiveDrawing()
{
	// Create textures used for selection
	glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
	glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Drawing* drawing = Canvas->ActiveDrawing;
	glTextureStorage2D(Texture, 1, GL_RGBA8, drawing->GetSizeInPixel().x, drawing->GetSizeInPixel().y);

	glCreateFramebuffers(1, &FrameBuffer);
	glNamedFramebufferTexture(FrameBuffer, GL_COLOR_ATTACHMENT0, Texture, 0);

	if (glCheckNamedFramebufferStatus(FrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Framebuffer incomplete");
	}
}

void EditTool::DestroyRenderTarget()
{
	glDeleteTextures(1, &Texture);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void EditTool::RenderTextureForSelection()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	Drawing* drawing = Canvas->ActiveDrawing;
	auto pixelSize = drawing->GetSizeInPixel();
	glViewport(0, 0, pixelSize.x, pixelSize.y);
	glUseProgram(RenderingSystem::ArticulatedLine->Program);
	glm::mat4 mvp = drawing->GetViewProjMatrix();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp)); // mvp

	for (auto& s : drawing->Strokes)
	{
		glUniform4f(1, 0.3, 0.3, 0.3, 1);// color
		RenderingSystem::ArticulatedLine->DrawStroke(s.get());
	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glm::vec4 EditTool::IndexToColor(uint32_t index)
// {
// 	std::bitset<32> bits{ index };
// 	float r = std::bitset<8>(bits.to_string(), 32-8, 8).to_ulong()/255.f;
// }
