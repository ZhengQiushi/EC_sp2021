/*
Some useful function and user theme
*/

#include "imgui_custom.h"
#include "imgui_internal.h"


#include <cmath>



namespace ImGui {


    void ShowAppDockSpace(bool* p_open)
    {
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", p_open, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        else
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
            ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
            ImGui::SameLine(0.0f, 0.0f);
            if (ImGui::SmallButton("click here"))
                io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Docking"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                ImGui::Separator();
                if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
                    *p_open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
    //https://github.com/ocornut/imgui/issues/1901

    bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;

        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

        const float t = g.Time;
        const float r = size.y / 2;
        const float speed = 1.5f;

        const float a = speed * 0;
        const float b = speed * 0.333f;
        const float c = speed * 0.666f;

        const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
        const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
        const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
    }

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
                centre.y + ImSin(a + g.Time * 8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
    }

    bool EnergyPanel(const char* label, float radius,float fraction,float thickness,const ImU32& color)
    {
        //获取当前窗口
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        //获取内容与主题编辑指针 ？id干啥用的？
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        //计算位置信息 ImRect似乎是用于确定渲染区域的结构体
        ImVec2 label_size = CalcTextSize(label, NULL);
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius+ thickness) * 2, (radius + thickness) * 2);
        ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImRect BB(pos, ImVec2(pos.x + size.x, pos.y + size.y +label_size.y));
        ItemSize(BB, style.FramePadding.y);
        if (!ItemAdd(BB, id))
            return false;

        const ImVec2 centre = ImVec2(pos.x + radius+thickness, pos.y + radius + thickness);
 
        // Render
        
        RenderFrame(BB.Min, BB.Max, GetColorU32(ImGuiCol_FrameBg), true, 10);

        window->DrawList->PathClear();
        window->DrawList->PathArcTo(centre, radius, IM_PI*0.2, IM_PI*2.1, 28);
        window->DrawList->PathStroke(color, false, thickness);
        window->DrawList->PathArcTo(centre, radius*1.1, IM_PI * 1.4, IM_PI * 1.5, 28);

        const ImU32 red = ImGui::GetColorU32(ImVec4(1, 0, 0, 1));
        window->DrawList->PathStroke(red, false, thickness/4);

        // Default displaying the fraction as percentage string, but user can override it
        char overlay_buf[32];
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%\n%d", fraction * 100 + 0.01f,100);

        ImGui::PushFont(font1_L);
        ImVec2 overlay_size = CalcTextSize(overlay_buf, NULL);
        if (overlay_size.x > 0.0f)
            RenderTextClipped(bb.Min, bb.Max, overlay_buf, NULL, &overlay_size, ImVec2(0.5f, 0.5f), &bb);
        ImGui::PopFont();

        RenderTextClipped(BB.Min, ImVec2(BB.Max.x,BB.Max.y- style.FramePadding.y), label, NULL, &label_size, ImVec2(0.5f, 1.0f), &BB);

    }

    //label 底部的标签
    //fraction0 底层圆弧
    //fraction1 上层圆弧
    //color     圆环背景颜色
    bool AnnulusPanel(const char* label, float radius, float thickness, float fraction0, float fraction1, const ImU32& colAnnulus0, const ImU32& colAnnulus1,
        float indicator0, float indicator1)
    {
        //获取当前窗口
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        //获取内容与主题编辑指针 ？id干啥用的？
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        //计算位置信息 ImRect似乎是用于确定渲染区域的结构体
        ImVec2 label_size = CalcTextSize(label, NULL);
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius + thickness) * 2, (radius + thickness) * 2);
        ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y)); //bb 为圆环区域
        ImRect BB(pos, ImVec2(pos.x + size.x, pos.y + size.y + label_size.y));//BB 为圆环区域+底部标签区域
        ItemSize(BB, style.FramePadding.y);
        if (!ItemAdd(BB, id))
            return false;

        const ImVec2 centre = ImVec2(pos.x + radius + thickness, pos.y + radius + thickness);

        // Render

        RenderFrame(BB.Min, BB.Max, GetColorU32(ImGuiCol_FrameBg), true, 10);

        const ImU32 colAnnulusBg = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
        //const ImU32 colAnnulus0    = ImGui::GetColorU32(ImGuiCol_TabActive);
        //const ImU32 colAnnulus1    = ImGui::GetColorU32(ImGuiPlotType_Lines);
        //const ImU32 colAnnulus1 = ImColor(ImVec4(0.44, 1.0, 0.3, 1.0));

        window->DrawList->PathClear();
        //背景圆环
        window->DrawList->PathArcTo(centre, radius, 0, IM_PI * 2, 62);
        window->DrawList->PathStroke(colAnnulusBg, false, thickness * 1.2);
        //底层圈弧 fraction0
        if (fraction0 > 0.005f)
        {
            window->DrawList->PathArcTo(centre, radius, IM_PI * 1.5, ImLerp(IM_PI * 1.5, IM_PI * 3.5, fraction0), 40);
            window->DrawList->PathStroke(colAnnulus0, false, thickness);
        }
        //上层圈弧 fraction1
        if (fraction1 > 0.005f)
        {
            window->DrawList->PathArcTo(centre, radius, IM_PI * 1.5, ImLerp(IM_PI * 1.5, IM_PI * 3.5, fraction1), 40);
            window->DrawList->PathStroke(colAnnulus1, false, thickness *3.0/ 4.0);
        }

        //指示符
        const ImU32 colIndicator = ImColor(ImVec4(1.0, 1.0, 1.0, 1.0));
        if (indicator0 > 0.005f)
        {
            window->DrawList->PathArcTo(centre, radius + thickness/2+2, IM_PI * 1.5 + IM_PI * 2 * (indicator0 - 0.005), IM_PI * 1.5 + IM_PI * 2 * (indicator0 + 0.005), 18);
            window->DrawList->PathStroke(colIndicator, false, 4);
        }
        if (indicator1 > 0.005f)
        {
            window->DrawList->PathArcTo(centre, radius + thickness/2+2, IM_PI * 1.5 + IM_PI * 2 * (indicator1 - 0.005), IM_PI * 1.5 + IM_PI * 2 * (indicator1 + 0.005), 18);
            window->DrawList->PathStroke(colIndicator, false, 4);
        }

        
        // Default displaying the fraction as percentage string, but user can override it
        char overlay_buf[32];
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.1f%%", fraction1 * 100);
        //使用大字体 该字体在main.c导入，在此头文件中extern
        ImGui::PushFont(font1_L);
        ImVec2 overlay_size = CalcTextSize(overlay_buf, NULL);
        if (overlay_size.x > 0.0f)
            RenderTextClipped(bb.Min, bb.Max, overlay_buf, NULL, &overlay_size, ImVec2(0.5f, 0.5f), &bb);
        ImGui::PopFont();

        RenderTextClipped(BB.Min, ImVec2(BB.Max.x, BB.Max.y - style.FramePadding.y), label, NULL, &label_size, ImVec2(0.5f, 1.0f), &BB);
        return false;
    }
    //https://github.com/ocornut/imgui/issues/1537

    void ToggleButton(const char* str_id, bool* v)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float height = ImGui::GetFrameHeight();
        float width = height * 1.55f;
        float radius = height * 0.50f;

        ImGui::InvisibleButton(str_id, ImVec2(width, height));
        if (ImGui::IsItemClicked())
            *v = !*v;

        float t = *v ? 1.0f : 0.0f;

        ImGuiContext& g = *GImGui;
        float ANIM_SPEED = 0.08f;
        if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
        {
            float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
            t = *v ? (t_anim) : (1.0f - t_anim);
        }

        ImU32 col_bg;
        if (ImGui::IsItemHovered())
            col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
        else
            col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
        draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    }

    //Some collected or my theme
    void custom_dark_theme0()
    {
        ImGui::GetStyle().FrameRounding = 4.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

//    void custom_dark_theme1()
//    {
//        ImGuiStyle& style = ImGui::GetStyle();
//        ImVec4* colors = style.Colors;
//
//        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//        colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
//        colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
//        colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
//        colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
//        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
//        colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
//        colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
//        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
//        colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
//        colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
//        colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
//        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
//        colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
//        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
//        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
//        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
//        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
//        colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
//        colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
//        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
//        colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
//        colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
//        colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
//        colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
//        colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
//        colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
//        colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
//        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
//        colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
//        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
//        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
//        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
//        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
//        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
//        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
//        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
//        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
//        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
//        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
//        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
//        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
//
//        style.PopupRounding = 3;
//
//        style.WindowPadding = ImVec2(4, 4);
//        style.FramePadding = ImVec2(6, 4);
//        style.ItemSpacing = ImVec2(6, 2);
//
//        style.ScrollbarSize = 18;
//
//        style.WindowBorderSize = 1;
//        style.ChildBorderSize = 1;
//        style.PopupBorderSize = 1;
//        style.FrameBorderSize = is3D;
//
//        style.WindowRounding = 3;
//        style.ChildRounding = 3;
//        style.FrameRounding = 3;
//        style.ScrollbarRounding = 2;
//        style.GrabRounding = 3;
//
////#ifdef IMGUI_HAS_DOCK 
//        style.TabBorderSize = is3D;
//        style.TabRounding = 3;
//
//        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
//        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
//        colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
//        colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
//        colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
//        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
//        colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
//
//        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        {
//            style.WindowRounding = 0.0f;
//            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//        }
////#endif
//    }

    void custom_blue_theme0()
    {
        // FIXME: those should become parameters to the function
        static int hue = 140;
        static float col_main_sat = 180.f / 255.f;
        static float col_main_val = 161.f / 255.f;
        static float col_area_sat = 124.f / 255.f;
        static float col_area_val = 100.f / 255.f;
        static float col_back_sat = 59.f / 255.f;
        static float col_back_val = 40.f / 255.f;

        ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
        ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
        ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
        ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

        ImGui::GetStyle().FrameRounding = 4.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;

        ImVec4* Colors = ImGui::GetStyle().Colors;

        Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
        Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
        Colors[ImGuiCol_ChildBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
        Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
        Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
        Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
        Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
        Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
        Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
        Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        //Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
        Colors[ImGuiCol_CheckMark] = ImVec4(45.f / 255.f, 180.f / 255.f, 236.f / 255.f, 0.80f);
        Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
        Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
        Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
        Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
        Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
        Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        //Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
        //Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
        //Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
        Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
        Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        //Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
        //Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
        //Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
        Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
        Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
        Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
        //Colors[ImGuiCol_TooltipBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
        Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    }

    void custom_blue_theme1()
    {

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.93f, 0.94f, 0.95f, 0.58f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.24f, 0.31f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.58f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.18f, 0.24f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.17f, 0.24f, 0.31f, 0.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.17f, 0.24f, 0.31f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.35f, 0.44f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.22f, 0.31f, 0.41f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.47f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.16f, 0.50f, 0.73f, 0.21f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.86f, 1.00f, 0.80f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.62f, 0.85f, 0.50f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.47f, 0.66f, 0.91f, 0.50f);
        colors[ImGuiCol_Button] = ImVec4(0.16f, 0.50f, 0.73f, 0.50f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.16f, 0.50f, 0.73f, 0.76f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.50f, 0.73f, 0.15f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.60f, 0.75f, 0.79f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.77f, 0.96f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.77f, 0.89f, 0.84f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.46f, 0.55f, 0.82f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.30f, 0.55f, 0.71f, 0.84f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.36f, 0.66f, 0.75f, 0.31f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.16f, 0.50f, 0.73f, 0.43f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.71f, 0.90f, 0.80f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.73f);

    }

}

