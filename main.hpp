#pragma once
#include<core.hpp>
#include<hello_imgui.h>
class APP
{
private:
	HelloImGui::RunnerParams p;
	ImFont* gFont;
	static void mainUI();
public:
	APP();
	~APP();
	void Run();
};