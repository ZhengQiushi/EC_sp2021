#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_plot.hpp"
#include "imgui/imgui_custom.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "stb_image.h"
#include "serial/Serial.h"

#include "sp_scm.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void show_mywindow(GLFWwindow* window, bool* p_open, CSerial* serial);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 780;

float gyro[3], accel[3], temperature;
bool data_available;

ImFont* font1_L;

int main()
{
    CSerial serial;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SuperPower Client", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Set glfw window icon
    GLFWimage images[2];
    images[0].pixels = stbi_load("resources/logo_b.png", &images[0].width, &images[0].height, 0, 4);
    images[1].pixels = stbi_load("resources/logo_b.png", &images[1].width, &images[1].height, 0, 4);
    glfwSetWindowIcon(window, 2, images);
    stbi_image_free(images[0].pixels);
    stbi_image_free(images[1].pixels);

    //Hide the main window for docking windows
    //glfwHideWindow(window);

    //Set Window Transparent 
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    //glfwSetWindowOpacity(window, 0.8f);

    //glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
  

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    //io.ConfigDockingTransparentPayload = true;

    // Setup Dear ImGui style 这会改变FPS 为什么
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    //ImGui::custom_dark_theme0();
    //ImGui::custom_dark_theme1();
    //ImGui::custom_blue_theme0();
    ImGui::custom_blue_theme1();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("fonts/Hack-Regular.ttf", 18);
    io.Fonts->AddFontFromFileTTF("fonts/Hack-Italic.ttf", 18);
    io.Fonts->AddFontFromFileTTF("fonts/Hack-Bold.ttf", 18);
    io.Fonts->AddFontFromFileTTF("fonts/Hack-BoldItalic.ttf", 18);

    font1_L = io.Fonts->AddFontFromFileTTF("fonts/Hack-Regular.ttf", 28);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        SP_SCM_MainWindow();

        //static CSerial SP_Serial;
        //SP_Serial.Open(3, 115200);
        //char* lpBuffer = new char[400];
        //int nBytesRead = 0;
        //if(SP_Serial.ReadDataWaiting()>0)
        //    nBytesRead=SP_Serial.ReadData(lpBuffer, 400);
        //delete[] lpBuffer;
        //std::cout << nBytesRead << std::endl;

        // Rendering
        ImGui::Render();
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void show_mywindow(GLFWwindow* window,bool* p_open, CSerial* serial)
{
    static int port_number = 0;

    ImGui::Begin("Another Window", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    if (serial->IsOpened())
        ImGui::Text("Connection Established!");
    else
        ImGui::Text("Ready to connect....");

    if (ImGui::CollapsingHeader("Connection"))
    {
        static ImGuiComboFlags flags = 0;
        const char* items[] = { "COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16"};
        static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
        if (ImGui::BeginCombo("Port Number", item_current, flags)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (item_current == items[n]);
                if (ImGui::Selectable(items[n], is_selected))
                    item_current = items[n];
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }
            }
            ImGui::EndCombo();
        }

        ImGui::PushID(0);
        if (serial->IsOpened())
        {
            int i = 2;
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
        }
        else
        {
            int i = 0;
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
        }
        if (ImGui::Button("Connect"))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                if (item_current == items[n])
                    port_number = n+1;
            if (serial->IsOpened())
                serial->Close();
            else
                serial->Open(port_number, 115200);

        }
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Status Monitor", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static ImGui::PlotInterface plot,plotA,plotG,plotT;
        static std::vector<ImGui::PlotItem> items,Accel,Gyro,Temp;

        float t = glfwGetTime();
        double x, y;
        glfwGetCursorPos(window ,&x, &y);

        plotA.title = "Accel";
        plotA.x_axis.label = "time";
        plotA.x_axis.maximum = 10;
        plotA.x_axis.lock_min = true;  
        plotA.x_axis.lock_max = true;
        

        plotG.title = "Gyro";
        plotG.x_axis.label = "time";
        plotG.x_axis.maximum = 10;
        plotG.x_axis.lock_min = true;
        plotG.x_axis.lock_max = true;

        plotT.title = "Temperature";
        plotT.x_axis.label = "time";
        plotT.x_axis.maximum = 10;
        plotT.x_axis.lock_min = true;
        plotT.x_axis.lock_max = true;

        Accel.resize(3);
        Accel[0].label = 'x';
        Accel[0].type = ImGui::PlotItem::Line;
        Accel[0].color = ImVec4(1, 0, 0, 1);
        Accel[1].label = 'y';
        Accel[1].type = ImGui::PlotItem::Line;
        Accel[1].color = ImVec4(0, 1, 0, 1);
        Accel[2].label = 'z';
        Accel[2].type = ImGui::PlotItem::Line;
        Accel[2].color = ImVec4(0, 0, 1, 1);

        Gyro.resize(3);
        Gyro[0].label = "yaw";
        Gyro[0].type = ImGui::PlotItem::Line;
        Gyro[0].color = ImVec4(1, 0, 0, 1);
        Gyro[1].label = "pitch";
        Gyro[1].type = ImGui::PlotItem::Line;
        Gyro[1].color = ImVec4(0, 1, 0, 1);
        Gyro[2].label = "roll";
        Gyro[2].type = ImGui::PlotItem::Line;
        Gyro[2].color = ImVec4(0, 0, 1, 1);

        Temp.resize(1);
        Temp[0].label = "temperature";
        Temp[0].type = ImGui::PlotItem::Line;
        Temp[0].color = ImVec4(1, 0, 0, 1);

        if (data_available)
        {
            for(int i=0;i<3;i++)
                ImGui::PlotItemBufferPoint(Accel[i], t, accel[i], 1500);
            for (int i = 0; i < 3; i++)
                ImGui::PlotItemBufferPoint(Gyro[i], t, gyro[i], 1500);
            ImGui::PlotItemBufferPoint(Temp[0], t, temperature, 1500);

            data_available = false;
        }

        ImGui::PlotAxisScroll(plotA.x_axis, t, 10);
        ImGui::PlotAxisScroll(plotG.x_axis, t, 10);
        ImGui::PlotAxisScroll(plotT.x_axis, t, 10);
        //ImGui::Plot("My Buffering Plot", plot, items, ImVec2(-1, -1));
        float regionHeight = ImGui::GetContentRegionAvail().y;
        ImGui::Plot("Accel", plotA, Accel, ImVec2(-1, regionHeight/3));
        ImGui::Plot("Gyro", plotG, Gyro, ImVec2(-1, regionHeight/3));
        ImGui::Plot("Temp", plotT, Temp, ImVec2(-1, -1));

    }
   

    ImGui::End();
}