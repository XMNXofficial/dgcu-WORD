#include"main.hpp"
#include<nfd.h>
#include<core.hpp>
#include<extern.hpp>
#include<sstream>
#include<fstream>
#include<iostream>
#include<Windows.h>
#include<imgui_internal.h>
#include<imgui_default_settings.h>
core* APPcore = nullptr;

int main()
{
	APP app;
	APPcore = new core("xmnx.zip");
	app.Run();
	delete APPcore;
	return 0;
}

void APP::mainUI()
{
	ImGui::StyleColorsLight();

	static int width = 500;


	static std::string text_name = "作业姓名:";
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text_name.c_str()).x - width) / 2);
	ImGui::Text(text_name.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width);
	ImGui::InputText("##input_姓名", word_name, sizeof(word_name));

	static std::string text_class = "作业班级:";
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text_class.c_str()).x - width) / 2);
	ImGui::Text(text_class.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width);
	ImGui::InputText("##input_班级", word_class, sizeof(word_class));

	static std::string text_schoolID = "作业学号:";
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text_schoolID.c_str()).x - width) / 2);
	ImGui::Text(text_schoolID.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width);
	ImGui::InputText("##input_学号", word_schoolID, sizeof(word_schoolID));

	static std::string text_title = "作业标题:";
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text_title.c_str()).x - width) / 2);
	ImGui::Text(text_title.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width);
	ImGui::InputText("##input_作业标题", word_title, sizeof(word_title));

	ImGui::Text("正文:");
	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::RadioButton("中文", &word_type, 0);
	ImGui::SameLine();
	ImGui::RadioButton("英文", &word_type, 1);
	ImGui::InputTextMultiline("##input_正文", word_buffer, sizeof(word_buffer), ImVec2(ImGui::GetWindowSize().x, ImGui::GetContentRegionAvail().y - 100));



	if (ImGui::Button("生成!", ImVec2(ImGui::GetWindowSize().x, ImGui::GetContentRegionAvail().y)))
	{
		APPcore->release_resource();
		APPcore->unZipFile(APPcore->OutputZipName);
		auto result = APPcore->SplitString(word_buffer, word_type == 0 ? 35 : 80);
		std::string MainText = APPcore->GenerateXML_MainText(result);
		APPcore->GenerateDocument(word_name, word_class, word_schoolID, word_title, MainText);

		NFD_Init();
		nfdchar_t* savePath;
		// prepare filters for the dialog
		nfdfilteritem_t filterItem[1] = { {"word文档", "docx"} };
		// show the dialog
		nfdresult_t nfd_result = NFD_SaveDialog(&savePath, filterItem, 1, NULL, std::string(std::string(word_name) + std::string(word_schoolID)).c_str());
		if (nfd_result == NFD_OKAY) {
			puts("Success!");
			puts(savePath);
			// remember to free the memory (since NFD_OKAY is returned)
		}
		else if (nfd_result == NFD_CANCEL) {
			puts("User pressed cancel.");
			return;
		}
		else {
			printf("Error: %s\n", NFD_GetError());
			return;
		}
		// Quit NFD
		APPcore->GenerateWord(std::string(savePath));//要在释放前使用
		NFD_FreePath(savePath);
		NFD_Quit();

	}
}
void APP::Run()
{
	HelloImGui::Run(p);
}
APP::APP()
{
	p.appWindowParams.windowTitle = "DGCU-WORD";
	p.appWindowParams.windowGeometry.size = { 800,600 };
	p.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenWindow;
	p.imGuiWindowParams.showStatusBar = true;
	p.imGuiWindowParams.showStatus_Fps = false;
	p.imGuiWindowParams.rememberStatusBarSettings = false;
	p.callbacks.LoadAdditionalFonts = [this]() {
		ImFontGlyphRangesBuilder a;
		static ImVector<ImWchar>b;
		a.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
		a.BuildRanges(&b);
		gFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyhbd.ttc", 35.0f, nullptr, b.Data); };
	p.callbacks.ShowGui = mainUI;
	p.callbacks.ShowStatus = []() {ImGui::Text("免费开源软件\t作者:XMNXofficial\tGithub项目地址:https://github.com/XMNXofficial/dgcu-WORD"); };

}
APP::~APP()
{
}
