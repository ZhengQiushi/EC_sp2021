#pragma once

#include "imgui.h"

extern ImFont* font1_L;

namespace ImGui {

	void ShowAppDockSpace(bool* p_open);
	//Useful widgets
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);

	bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

	void ToggleButton(const char* str_id, bool* v);

	bool EnergyPanel(const char* label, float radius, float percent, float thickness, const ImU32& color);

	bool AnnulusPanel(const char* label, float radius, float thickness, float fraction0, float fraction1, const ImU32& colAnnulus0, const ImU32& colAnnulus1,
		float indicator0 = 0.0f, float indicator1 = 0.0f);
	//Themes
	void custom_dark_theme0();

	void custom_blue_theme0();

	void custom_blue_theme1();

}

