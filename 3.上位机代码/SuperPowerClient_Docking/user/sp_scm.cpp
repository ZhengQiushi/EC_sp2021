#include "sp_scm.h"
#include "deque"
#include "crc32.h"

#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
std::deque<char> UartRecvBuffer;

const int SerialFrameLenRx[SP_DATA_NUM] = {
    SP00_LenRx,
    SP01_LenRx,
    SP02_LenRx,
    SP03_LenRx
};

const int SerialFrameLenTx[SP_DATA_NUM] = {
    SP00_LenTx,
    SP01_LenTx,
    SP02_LenTx,
    SP03_LenTx
};

void SP_SCM_MainWindow()
{
    static SCM_StatusType SCM_Status,SCM_StatusSet;
    static CSerial SP_Serial;
    
    static bool show_app_dockspace = true;
    static bool show_demo_window = false;
    static bool show_metrics_window = false;
    static bool show_sp_main_window = true;
    static bool show_sp_serial_connection = true;
    static bool show_sp_powerplot = true;
    static bool show_sp_imuplot = true;
    static bool show_sp_emptywindow = false;

    static double PreviousTransmitTime = 0;


    if (show_app_dockspace)           ImGui::ShowAppDockSpace(&show_app_dockspace);
    if (show_demo_window)             ImGui::ShowDemoWindow(&show_demo_window);
    if (show_metrics_window)          ImGui::ShowMetricsWindow(&show_metrics_window);
    if (show_sp_serial_connection)    SP_SerialConnection(&show_sp_serial_connection, &SP_Serial,&SCM_Status);
    if (show_sp_powerplot)            SP_SCM_PlotPower(&show_sp_powerplot, &SCM_Status);
    if (show_sp_imuplot)              SP_SCM_PlotIMU(&show_sp_imuplot, &SCM_Status);
    if (show_sp_emptywindow)          SP_EmptyWindow(&show_sp_emptywindow);

    SP_SerialDataReceive(&SP_Serial, &SCM_Status, &SCM_StatusSet);
    SP_SCM_Display(&SCM_Status);
    SP_SCM_Settings(&SCM_Status,&SCM_StatusSet);

    //Uart Frame Transmit
    if (ImGui::GetTime() - PreviousTransmitTime > SCM_DATASENNDING_PERIOD)
    {
        PreviousTransmitTime = ImGui::GetTime();
        SP_SerialDataTransmit(&SP_Serial, &SCM_StatusSet,&SCM_Status);
    }
    

    if (show_sp_main_window)
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("MainWindow");                          // Create a window called "Hello, world!" and append into it.
        ImGui::PushFont(font1_L);
        ImGui::Text("Main Control");
        ImGui::PopFont();             
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Metrics Window", &show_metrics_window);
        ImGui::Checkbox("Serial Connection",&show_sp_serial_connection);
        ImGui::Checkbox("Power Plot", &show_sp_powerplot);
        ImGui::Checkbox("IMU Plot", &show_sp_imuplot);
        ImGui::Checkbox("Empty Window", &show_sp_emptywindow);
      
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        //ImGui::PopFont();
        ImGui::End();
    }

    
}
void SP_EmptyWindow(bool* p_open)
{
    ImGui::Begin("Empty Display",p_open);
    ImGui::Text("This is an empty window!");
    ImGui::End();
}
void SP_SCM_Display(SCM_StatusType* SCM_Status)
{
    ImGui::Begin("SCM Status");
    // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
    // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
    if (SCM_Status->Online)
        ImGui::Text("SCM Online");
    else
        ImGui::Text("SCM Offline");


    //Capacitor Information
    //Capacitor :5F 27V
    static float EnergySum = 26.5 * 26.5 * 5.0 / 2.0;
    static float EnergyLow = 18.0 * 18.0 * 5.0 / 2.0;
    float Energy0 = 0.0f, Energy1 = 0.0f;
    float EnergyNow = pow(SCM_Status->CapVoltage, 2) * 5.0 / 2;
    if (SCM_Status->Online && SCM_Status->AdvancedSettingEnable)
    {
        EnergySum = pow(SCM_Status->Cap_MaxVol_H, 2) * 5.0 / 2.0;
        EnergyLow = pow(SCM_Status->Cap_MinVol_L, 2) * 5.0 / 2.0;
        Energy0 = pow(SCM_Status->Cap_MinVol_H, 2) * 5.0 / 2.0- EnergyLow;
        Energy1 = pow(SCM_Status->Cap_MaxVol_L, 2) * 5.0 / 2.0- EnergyLow;
    }
    float EnergyAvailable = EnergySum - EnergyLow;
    float EnergyReserved= EnergyNow - EnergyLow;

    ImU32 colAnnulus0 = ImGui::GetColorU32(ImGuiCol_TabActive);
    ImU32 colAnnulus1 = ImColor(ImVec4(0.2, 1.0, 0.75, 0.9));
    char buf[50];
    ImFormatString(buf, IM_ARRAYSIZE(buf), "Cap:%.1fJ", EnergyNow);
    if (EnergyReserved > 0.0f)
        ImGui::AnnulusPanel(buf, 65, 20, 1.0f, EnergyReserved/ EnergyAvailable, colAnnulus0, colAnnulus1, Energy0/ EnergyAvailable, Energy1 / EnergyAvailable);
    else
        ImGui::AnnulusPanel(buf, 65, 20, EnergyNow/ EnergyLow,0.0f, colAnnulus0, colAnnulus1);

    ImGui::SameLine();
    colAnnulus1 = ImColor(ImVec4(0.1, 1.0, 0.9, 0.7));
    ImGui::AnnulusPanel("Charge Power", 65, 20, SCM_Status->SetPow/120.0f, SCM_Status->CapPower / 120.0f, colAnnulus0, colAnnulus1);

    //float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;

    sprintf(buf, "%.1f/%.1f", SCM_Status->CapPower, 125.0);
    ImGui::ProgressBar((SCM_Status->CapPower) / 130.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("CapPower");

    sprintf(buf, "%.1f/%.1f", SCM_Status->CapVoltage, 30.0);
    ImGui::ProgressBar((SCM_Status->CapVoltage) / 30.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("CapVoltage");

    sprintf(buf, "%.1f/%.1f", SCM_Status->CapCurrent, 5.0);
    ImGui::ProgressBar((SCM_Status->CapCurrent) / 5.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("CapCurrent");

    ImGui::Separator();

    sprintf(buf, "%.1f/%.1f", SCM_Status->BatteryPower, 125.0);
    ImGui::ProgressBar((SCM_Status->BatteryPower) / 125.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("BatteryPower");

    sprintf(buf, "%.1f/%.1f", SCM_Status->BatteryVoltage, 30.0);
    ImGui::ProgressBar((SCM_Status->BatteryVoltage) / 30.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("BatteryVoltage");

    sprintf(buf, "%.1f/%.1f", SCM_Status->BatteryCurrent, 5.0);
    ImGui::ProgressBar((SCM_Status->BatteryCurrent) / 5.0, ImVec2(0.f, 0.f), buf);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("BatteryCurrent");

    ImGui::End();
}
void SP_SCM_Settings(SCM_StatusType* SCM_Status, SCM_StatusType* SCM_StatusSet)
{
    //如果太快地更新状态将导致无法正常调节，因为刚设置的值还未应用到MCU便被其反馈值更新了
    //事实上这个延迟大一些不会影响实际的控制，因为只要SCM_StatusSet更改了，在下一次循环中会立即更新，不过此处的反馈会慢一些
    //但是时间若是控制的不好，还是影响操控体验 
    //这个周期总的上要大于控制指令发送周期
    static double PreviousRenewTime = -1.0;
    if (ImGui::GetTime() - PreviousRenewTime > 1.0 && SCM_Status->Online)
    {
        PreviousRenewTime = ImGui::GetTime();
        SCM_StatusSet->Buck = SCM_Status->Buck;
        SCM_StatusSet->Mode = SCM_Status->Mode;
        SCM_StatusSet->ForceUseCap = SCM_Status->ForceUseCap;
        SCM_StatusSet->SetPow = SCM_Status->SetPow;

        SCM_StatusSet->Cap_MaxVol_H = SCM_Status->Cap_MaxVol_H;
        SCM_StatusSet->Cap_MaxVol_L = SCM_Status->Cap_MaxVol_L;
        SCM_StatusSet->Cap_MinVol_H = SCM_Status->Cap_MinVol_H;
        SCM_StatusSet->Cap_MinVol_L = SCM_Status->Cap_MinVol_L;
    }

    bool ValueChangedFlag = false;
    ImGui::Begin("SCM Settings");
    //注意以下设置可能由于部分状态不符合规则，被MCU自行修改
    //Eg:如当前电压过低时不允许使用电容放电等
    //Charge Mode Setting
    if (SCM_StatusSet->Buck)
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
    if (ImGui::Button("Charge"))
    {
        SCM_StatusSet->Buck = !SCM_StatusSet->Buck;
        ValueChangedFlag = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (SCM_StatusSet->Buck)
    {
        if (SCM_StatusSet->SetPow > 1.1f)
            ImGui::Text("Charging Capacitor!");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Warning:Power Setting Low");
    }
    else
    {
        if(SCM_StatusSet->SetPow>1.1f)
            ImGui::Text("Ready for Charging...");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.7f, 1.0f), "Warning:Power Setting Low");
    }

    //Power Supply Mode Setting
    if (SCM_StatusSet->Mode)
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
    if (ImGui::Button(" Mode "))
    {
        SCM_StatusSet->Mode = !SCM_StatusSet->Mode;
        ValueChangedFlag = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (SCM_StatusSet->Mode)
    {
       if(SCM_Status->CapVoltage>16.0f)
            ImGui::Text("Capacitor Output!");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Warning:Capacitor Voltage Low!");
    }
    else
    {
        if (SCM_Status->CapVoltage > 16.0f)
            ImGui::Text("Battery Output!");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.7f, 1.0f), "Warning:Capacitor Voltage Low!");
    }

    //Capacitor Output Flag
    if (SCM_StatusSet->ForceUseCap)
    {
        int i = 6;
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
    }
    else
    {
        int i = 2;
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
    }
    if (ImGui::Button(" Auto "))
    {
        SCM_StatusSet->ForceUseCap = !SCM_StatusSet->ForceUseCap;
        ValueChangedFlag = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (SCM_StatusSet->ForceUseCap)
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Waring: Forced use Cap!Auto configuration disable!");
    else
        ImGui::Text("Auto configuration enable!");
    //SetPower
    if (!SCM_Status->Online)
        SCM_StatusSet->SetPow = 0;
    ImGui::SliderFloat("Set Power",&SCM_StatusSet->SetPow, 0.0f, 120.0f, "%.1f", 1.0f);
    //if (SCM_StatusSet->SetPow > 120.0f)
    //    SCM_StatusSet->SetPow = SCM_StatusSet->SetPow;
    //AdvancedSetting: Capacitor voltage settings, revelant to serverial status auto configuration
    ImGui::Separator();
    if (SCM_Status->Online && SCM_Status->AdvancedSettingEnable)
    {
        ValueChangedFlag |= ImGui::DragFloatRange2("Cap Voltage High", &(SCM_StatusSet->Cap_MaxVol_L), &(SCM_StatusSet->Cap_MaxVol_H), 0.1f, 24.0f, 26.5f, "Min: %.1fV", "Max: %.1fV");
        ValueChangedFlag |= ImGui::DragFloatRange2("Cap Voltage Low", &(SCM_StatusSet->Cap_MinVol_L), &(SCM_StatusSet->Cap_MinVol_H), 0.1f, 16.0f, 23.0f, "Min: %.1fV", "Max: %.1fV");
    }
    else
        ImGui::TextDisabled("Advanced Settings Disable!");
    


    ImGui::End();

    //避免在设置未生效时被重置
    if (ValueChangedFlag)
        PreviousRenewTime = ImGui::GetTime();
}

void SP_SCM_PlotPower(bool* p_open, SCM_StatusType* SCM_Status)
{
    ImGui::Begin("SCM PowerPlot", p_open);

    static ImGui::PlotInterface plotP;
    static std::vector<ImGui::PlotItem> powerData;
    static double PoltStartTime = ImGui::GetTime();

    if(!SCM_Status->Online)
        PoltStartTime = ImGui::GetTime();
        

    plotP.title = "Power";
    plotP.x_axis.label = "Time Since Connection Established";
    plotP.x_axis.maximum = 10;
    plotP.x_axis.lock_min = true;
    plotP.x_axis.lock_max = true;

    powerData.resize(3);
    powerData[0].label = "Real Power";
    powerData[0].type = ImGui::PlotItem::Line;
    powerData[0].size = 2;
    powerData[0].color = ImVec4(1.0, 1.0, 1.0, 0.9);
    powerData[1].label = "Set Power";
    powerData[1].type = ImGui::PlotItem::Line;
    powerData[1].size = 2;
    powerData[1].color = ImVec4(0.3, 0.3, 1.0, 0.9);
    powerData[2].label = "Battery Power";
    powerData[2].type = ImGui::PlotItem::Line;
    powerData[2].size = 2;
    powerData[2].color = ImVec4(0.3, 1.0, 0.3, 0.9);

    if (SCM_Status->Online)
    {
        double PlotTime = ImGui::GetTime() - PoltStartTime;
        ImGui::PlotItemBufferPoint(powerData[0], PlotTime, SCM_Status->CapPower, 1500);
        ImGui::PlotItemBufferPoint(powerData[1], PlotTime, SCM_Status->SetPow, 1500);
        ImGui::PlotItemBufferPoint(powerData[2], PlotTime, SCM_Status->BatteryPower, 1500);
        ImGui::PlotAxisScroll(plotP.x_axis, PlotTime, 10);
    }

    ImGui::Plot("PowerPlot", plotP, powerData, ImVec2(-1, -1));

    ImGui::End();
}
void SP_SCM_PlotIMU(bool* p_open, SCM_StatusType* SCM_Status)
{
    ImGui::Begin("SCM IMUPlot", p_open);

    static ImGui::PlotInterface plotIMU;
    static std::vector<ImGui::PlotItem> IMU_Data;
    static double PoltStartTime = ImGui::GetTime();

    if (!SCM_Status->Online)
        PoltStartTime = ImGui::GetTime();


    plotIMU.title = "IMU Data";
    plotIMU.x_axis.label = "Time Since Connection Established";
    plotIMU.x_axis.maximum = 10;
    plotIMU.x_axis.lock_min = true;
    plotIMU.x_axis.lock_max = true;

    IMU_Data.resize(2);
    IMU_Data[0].label = "Accel";
    IMU_Data[0].type = ImGui::PlotItem::Line;
    IMU_Data[0].size = 2;
    IMU_Data[0].color = ImVec4(1.0, 1.0, 1.0, 0.9);
    IMU_Data[1].label = "Speed";
    IMU_Data[1].type = ImGui::PlotItem::Line;
    IMU_Data[1].size = 2;
    IMU_Data[1].color = ImVec4(0.3, 0.3, 1.0, 0.9);


    if (SCM_Status->Online)
    {
        double PlotTime = ImGui::GetTime() - PoltStartTime;
        ImGui::PlotItemBufferPoint(IMU_Data[0], PlotTime, sqrt(SCM_Status->Accel[0] * SCM_Status->Accel[0] + SCM_Status->Accel[1] * SCM_Status->Accel[1]), 1500);
        ImGui::PlotItemBufferPoint(IMU_Data[1], PlotTime, sqrt(SCM_Status->Speed[0] * SCM_Status->Speed[0] + SCM_Status->Speed[1] * SCM_Status->Speed[1]), 1500);

        ImGui::PlotAxisScroll(plotIMU.x_axis, PlotTime, 10);
    }

    ImGui::Plot("IMU Plot", plotIMU, IMU_Data, ImVec2(-1, -1));

    ImGui::End();
}

void SP_SerialConnection(bool* p_open, CSerial* Serial, SCM_StatusType* SCM_Status)
{
    static int port_number = 0;

    ImGui::Begin("Connection", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    if (Serial->IsOpened())
        ImGui::Text("Connection Established!");
    else
        ImGui::Text("Ready to connect....");

    //if (ImGui::CollapsingHeader("Connection", ImGuiTreeNodeFlags_DefaultOpen))
    //{
        static ImGuiComboFlags flags = 0;
        const char* items[] = { "COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16" };
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
        if (Serial->IsOpened())
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
                    port_number = n + 1;
            if (Serial->IsOpened())
                Serial->Close();
            else
                Serial->Open(port_number, 115200);

        }
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    //}
    char buf[50];

    //Display Error Frame Rate
    if (SCM_Status->FrameCount > 0)
    {
        ImGui::Text("ErrorFrameRate:%.1f%%", (float)SCM_Status->ErrorFrameCount / SCM_Status->FrameCount * 100);
        sprintf(buf, "Error:%d Sum:%d", SCM_Status->ErrorFrameCount, SCM_Status->FrameCount);
        ImGui::ProgressBar((float)SCM_Status->ErrorFrameCount/ SCM_Status->FrameCount, ImVec2(0.f, 0.f), buf);

    }

    ImGui::End();
}

void SP_SerialDataReceive(CSerial* Serial, SCM_StatusType* SCM_Status, SCM_StatusType* SCM_StatusSet)
{
    //Online Status judge
    if (ImGui::GetTime() - SCM_Status->OnlineTime > 1.0f )
    {
        SCM_Status->Online = false;
        SCM_Status->AdvancedSettingEnable = false;
        
    }
    //Error frame count
    static int FrameNum = 0, ErrorFrameNum = 0;
    if (ImGui::GetTime() - SCM_Status->ErrorCalTime > 60.0f)
    {
        SCM_Status->ErrorCalTime = ImGui::GetTime();
        SCM_Status->FrameCount = FrameNum;
        SCM_Status->ErrorFrameCount = ErrorFrameNum;
        FrameNum = 0;
        ErrorFrameNum = 0;
    }
    //Serial Receive
    if (Serial->IsOpened())
    { 
        SerialDataType Uart_Recv[200];
        char* lpBuffer = new char[400];

        int nBytesRead = Serial->ReadData(lpBuffer, 400);
        int index = 0;
        //在deque容器中存入接收到的数据
        while (index < nBytesRead)
            UartRecvBuffer.push_back(lpBuffer[index++]);

        delete[]lpBuffer;

        //待处理有效数据帧的长度
        int Uart_RecvSize = 0;
        //判断是否为头帧，在数据量较小时，暂不处理
        while (UartRecvBuffer.size() >= 100)
        {
            if (UartRecvBuffer[0] == 'S' && UartRecvBuffer[1] == 'P' && UartRecvBuffer[2] == '0')
            {
                //继续判断是否为头帧
                if (UartRecvBuffer[3] < '0' || UartRecvBuffer[3]>'9')
                {
                    UartRecvBuffer.pop_front();
                    continue;
                }

                int DataFrameSize = -1;

                if (UartRecvBuffer[3] == '0') DataFrameSize = SerialFrameLenRx[0] * 4;
                if (UartRecvBuffer[3] == '1') DataFrameSize = SerialFrameLenRx[1] * 4;
                if (UartRecvBuffer[3] == '2') DataFrameSize = SerialFrameLenRx[2] * 4;
                if (UartRecvBuffer[3] == '3') DataFrameSize = SerialFrameLenRx[3] * 4;

                if (DataFrameSize > 0)
                {
                    int tempIndex = Uart_RecvSize;
                    for (int i = tempIndex; i < tempIndex+DataFrameSize; i++)
                    {
                        Uart_Recv[i / 4].data[i % 4] = UartRecvBuffer.front();
                        UartRecvBuffer.pop_front();
                        Uart_RecvSize++;
                    }
                }
                
               
            }
            else
                UartRecvBuffer.pop_front();//非头帧，删去当前头部元素

        }
        //经过以上处理后数据帧是完整的
        index = 0;
        while (index  < Uart_RecvSize)
        {
            if (Uart_Recv[index].data[0] == 'S' &&
                Uart_Recv[index].data[1] == 'P' &&
                Uart_Recv[index].data[2] == '0'
                )
            {
                //更新数据接收时间，用于判断模块是否连接
                SCM_Status->Online = true;
                SCM_Status->OnlineTime = ImGui::GetTime();
                FrameNum++;
                //SP00
                if (Uart_Recv[index].data[3] == '0')
                {
                    int crcIndex = index;
                    //如果是约定的数据包，则将index更新至下一个包头位置
                    index += SerialFrameLenRx[0];
                    //检测CRC值是否正确，若错误则直接丢弃
                    if (!crc_check(&Uart_Recv[crcIndex], SerialFrameLenRx[0]))
                    {
                        ErrorFrameNum++;
                        continue;
                    }
                    continue;
                }
                //SP01
                if (Uart_Recv[index].data[3] == '1')
                {
                    int crcIndex = index;
                    int index_temp = index + 1;
                    index += SerialFrameLenRx[1];
                    if (!crc_check(&Uart_Recv[crcIndex], SerialFrameLenRx[1]))
                    {
                        ErrorFrameNum++;
                        continue;
                    }
                    
                    SCM_Status->Accel[0] = Uart_Recv[index_temp++].value;
                    SCM_Status->Accel[1] = Uart_Recv[index_temp++].value;
                    SCM_Status->Accel[2] = Uart_Recv[index_temp++].value;
                    SCM_Status->Speed[0] = Uart_Recv[index_temp++].value;
                    SCM_Status->Speed[1] = Uart_Recv[index_temp++].value;
                    SCM_Status->Speed[2] = Uart_Recv[index_temp++].value;
                    SCM_Status->Temp = Uart_Recv[index_temp++].value;

                    continue;
                }
                //SP02
                if (Uart_Recv[index].data[3] == '2')
                {
                    int crcIndex = index;
                    int index_temp = index+1;
                    index += SerialFrameLenRx[2];
                    if (!crc_check(&Uart_Recv[crcIndex], SerialFrameLenRx[2]))
                    {
                        ErrorFrameNum++;
                        continue;
                    }

                    SCM_Status->Buck            = (Uart_Recv[index_temp].data[0])&1;
                    SCM_Status->Mode            = ((Uart_Recv[index_temp].data[0]) & (1 << 1)) >> 1;
                    SCM_Status->ForceUseCap     = ((Uart_Recv[index_temp].data[0]) & (1 << 2)) >> 2;
                    index_temp++;
                    SCM_Status->SetPow          = Uart_Recv[index_temp++].value;
                    SCM_Status->CapPower        = Uart_Recv[index_temp++].value;
                    SCM_Status->CapCurrent      = Uart_Recv[index_temp++].value;
                    SCM_Status->CapVoltage      = Uart_Recv[index_temp++].value;
                    SCM_Status->BatteryPower    = Uart_Recv[index_temp++].value;
                    SCM_Status->BatteryCurrent  = Uart_Recv[index_temp++].value;
                    SCM_Status->BatteryVoltage  = Uart_Recv[index_temp++].value;

                    
                    continue;
                }
                //SP03
                if (Uart_Recv[index].data[3] == '3')
                {
                    int crcIndex = index;
                    int index_temp = index + 1;
                    index += SerialFrameLenRx[3];
                    if (!crc_check(&Uart_Recv[crcIndex], SerialFrameLenRx[3]))
                    {
                        ErrorFrameNum++;
                        continue;
                    }
                    
                    SCM_Status->Cap_MaxVol_H = Uart_Recv[index_temp++].value;
                    SCM_Status->Cap_MaxVol_L = Uart_Recv[index_temp++].value;
                    SCM_Status->Cap_MinVol_H = Uart_Recv[index_temp++].value;
                    SCM_Status->Cap_MinVol_L = Uart_Recv[index_temp++].value;

                    if (!SCM_Status->AdvancedSettingEnable)
                    {
                        SCM_StatusSet->Cap_MaxVol_H = SCM_Status->Cap_MaxVol_H;
                        SCM_StatusSet->Cap_MaxVol_L = SCM_Status->Cap_MaxVol_L;
                        SCM_StatusSet->Cap_MinVol_H = SCM_Status->Cap_MinVol_H;
                        SCM_StatusSet->Cap_MinVol_L = SCM_Status->Cap_MinVol_L;
                    }
                    SCM_Status->AdvancedSettingEnable = true;

                    continue;
                }
            }
            else
                break;
        }

    }

}

void SP_SerialDataTransmit(CSerial* Serial, SCM_StatusType* SCM_StatusSet, SCM_StatusType* SCM_Status)
{
    if (Serial->IsOpened())
    {
        SerialDataType TxData[100];

        uint16_t index = 0, data_size = 0;
        //SP02 SCM_StatusData Frame
        TxData[index].data[0] = 'S';
        TxData[index].data[1] = 'P';
        TxData[index].data[2] = '0';
        TxData[index].data[3] = '2';
        index++;

        TxData[index].data[0] = 0x00;
        TxData[index].data[0] |= SCM_StatusSet->Buck;
        TxData[index].data[0] |= (SCM_StatusSet->Mode << 1);
        TxData[index].data[0] |= (SCM_StatusSet->ForceUseCap << 2);
        index++;
        TxData[index++].value = SCM_StatusSet->SetPow;
        TxData[index].value_i = crc32(&TxData[0].data[0], index * 4);
        index++;

        data_size += index;
        index = 0;

        if (SCM_Status->AdvancedSettingEnable)
        {
            //SP03 SCM Advanced Settings Frame
            TxData[data_size + index].data[0] = 'S';
            TxData[data_size + index].data[1] = 'P';
            TxData[data_size + index].data[2] = '0';
            TxData[data_size + index].data[3] = '3';
            index++;
            TxData[data_size + index++].value = SCM_StatusSet->Cap_MaxVol_H;
            TxData[data_size + index++].value = SCM_StatusSet->Cap_MaxVol_L;
            TxData[data_size + index++].value = SCM_StatusSet->Cap_MinVol_H;
            TxData[data_size + index++].value = SCM_StatusSet->Cap_MinVol_L;

            TxData[data_size + index].value_i = crc32(&TxData[data_size].data[0], index * 4);
            index++;
            data_size += index;
            index = 0;
        }
       

        char* lpBuffer = new char[400];
        for (int i = 0; i < data_size * 4; i++)
            lpBuffer[i] = TxData[i / 4].data[i % 4];

        Serial->SendData(lpBuffer, data_size * 4);

        delete[] lpBuffer;

    }
}


bool crc_check(SerialDataType* Data, uint16_t Len)
{
    const uint32_t ExpectValue = (Data + Len - 1)->value_i;
    uint32_t CalValue = crc32(&Data->data[0], (Len - 1) * 4);
    if (CalValue == ExpectValue)
        return true;
    else
        return false;
}