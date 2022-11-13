﻿#pragma once

class CanvasPanel;

class Tool
{
protected:
	chrono::time_point<chrono::steady_clock> StartDraggingTimePoint{};
	chrono::duration<float, std::milli> DraggingDuration;
	bool IsDragging = false;
public:
	explicit Tool(CanvasPanel* canvas);
	CanvasPanel* Canvas = nullptr;

	virtual void Activate()
	{
	}

	virtual void Deactivate()
	{
	}

	virtual void ClickOrDragStart()
	{
	}

	virtual void Dragging()
	{
	}

	virtual void DragEnd()
	{
	}

	// Run under the invisible button of canvas, default behavior
	virtual void Run()
	{
		if (ImGui::IsMouseClicked(0) && ImGui::IsItemActivated())
		{
			if (IsDragging) IsDragging = false;
			StartDraggingTimePoint = std::chrono::high_resolution_clock::now();
			ClickOrDragStart();
			return;
		}

		if (ImGui::IsMouseDragging(0) && !ImGui::IsItemActivated() && ImGui::IsItemActive())
		{
			IsDragging = true;
			DraggingDuration = chrono::high_resolution_clock::now() - StartDraggingTimePoint;
			Dragging();
			return;
		}

		if(IsDragging && !ImGui::IsMouseDragging(0))
		{
			IsDragging = false;
			DragEnd();
			DraggingDuration = chrono::duration<float, std::milli>::zero();
			return;
		}
	}
};
