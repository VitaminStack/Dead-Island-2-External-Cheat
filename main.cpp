#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "Helper.h"
#pragma comment(lib, "dwmapi.lib")
#include "Dwmapi.h"
#include "Debugging.h"
// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
static ID3D11BlendState*        m_pBlendState = nullptr;

int Screen_w = 2560;
int Screen_h = 1440;
const char* OverlayName = "Overlay";

extern LPCWSTR TargetGame = L"Dead Island 2  ";
//extern LPCWSTR TargetGame = L"Hogwarts Legacy  ";
extern uintptr_t GNames = 0x147720E40;


uintptr_t OEntList;
uintptr_t CEntList;
uintptr_t ModuleBase;
HANDLE hProcess;
DWORD Procid;
HWND targetHwnd;
LPVOID codecaveAddress;

bool Hook = false;
bool FlyHackfertig = false;
bool GodModefertig = false;
bool NoRecoil = false;
bool NoSpread = false;
bool UnlAmmo = false;
bool OESPBool = false;
bool ZESPBool = true;
bool ZombieIgnore = true;
bool GodMode = true;
bool FlyHack = false;
int Ents = 0;
//Func
uintptr_t RecoilAdr;
uintptr_t SpreadAdr;
uintptr_t AmmoAdr;

//Menu
bool ESP;
bool Misc;
float DistSliderItems = 25.0f;
float DistSliderCharakter = 200.f;
float ItemColor[4] = { 0, 0, 255, 1 };
float ZombieColor[4] = { 255, 0, 0, 1 };
bool openMenu = true;
bool Clickability = false;
bool demoWindow = false;
bool checkAdress = false;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void ChangeClickability(bool canclick, HWND ownd)
{
    long style = GetWindowLong(ownd, GWL_EXSTYLE);
    if (canclick) {
        style &= ~WS_EX_LAYERED;
        SetWindowLong(ownd, GWL_EXSTYLE, style);
        SetForegroundWindow(ownd);
        //windowstate = 1;
    }
    else {
        style |= WS_EX_LAYERED;
        SetWindowLong(ownd, GWL_EXSTYLE, style);
        //windowstate = 0;
    }
}


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("Overlay"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT, wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 0, 0, Screen_w, Screen_h, nullptr, nullptr, nullptr, nullptr);
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);   
    ::UpdateWindow(hwnd);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_None;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    bool done = false;



    Camera gameCamera;
    OpenConsole();
    initHax();    
    codecaveAddress = VirtualAllocEx(hProcess, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!codecaveAddress) {
    }
        
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        

        
        
        UpdateHax();
        gameCamera.Update(hProcess);

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            Clickability = !Clickability;
        }
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            openMenu = !openMenu;
        }
        
        
        ChangeClickability(Clickability, hwnd);
        SetOverlayToTarget(targetHwnd, hwnd);


        ImGui_ImplDX11_NewFrame();        
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImDrawList* Drawlist = ImGui::GetForegroundDrawList();
        RECT rect;
        GetWindowRect(hwnd, &rect);
        
        
        if (openMenu)
        {            
            ImGui::Begin("Made by SozialHilfe");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Checkbox("Misc", &Misc);
            ImGui::Checkbox("ESP", &ESP);    
            ImGui::NewLine();            
            ImGui::Checkbox("ShowDemo", &demoWindow);
            ImGui::Checkbox("Debugging", &checkAdress); 
            ImGui::Text("INSERT Key -> Cursor");
            ImGui::Text("DELETE Key -> Hide Menu");
            ImGui::End();
        }
        if (checkAdress)
        {
            ImGui::Begin("Checking");
            ImGui::Text("GEngine: %p", GEngine);
            ImGui::Text("ObjektEntityList: %p", OEntList);
            ImGui::Text("CharakterEntityList: %p", CEntList);
            ImGui::Text("CamPos: %.2f | %.2f | %.2f", gameCamera.CamPos.x, gameCamera.CamPos.y, gameCamera.CamPos.z);
            ImGui::Text("Rotation: %.2f | %.2f | %.2f", gameCamera.Rotation.x, gameCamera.Rotation.y, gameCamera.Rotation.z);
            ImGui::Text("FOV: %.2f", gameCamera.Fov);
            ImGui::Text("Ents: %d", Ents);
            ImGui::End();
        }
        if (Misc && openMenu)
        {
            ImGui::Begin("MISC");
            ImGui::Checkbox("Godmode", &GodMode);
            if (ImGui::Checkbox("Zombie Ignore", &ZombieIgnore))
            {
                byte ZIgnore;
                if (ZombieIgnore)
                {
                    ZIgnore = 0;
                    WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x9E8, 0xA2 })), &ZIgnore, sizeof(ZIgnore), nullptr); //Zombie Ignore
                }
                if (!ZombieIgnore)
                {
                    ZIgnore = 131;
                    WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x9E8, 0xA2 })), &ZIgnore, sizeof(ZIgnore), nullptr); //Zombie Ignore
                }
            }
            ImGui::Checkbox("FlyHack", &FlyHack);
            if (ImGui::Checkbox("No Recoil", &NoRecoil))
            {
                if (NoRecoil)
                {
                    NopFunc(hProcess, (LPVOID)(ModuleBase + 0x15F1754), 8);
                    NopFunc(hProcess, (LPVOID)(ModuleBase + 0x15F1729), 8);
                }
                if (!NoRecoil)
                {
                    BYTE originalBytes1[] = { 0xF3, 0x0F, 0x11, 0x87, 0xAC, 0x01, 0x00, 0x00 };
                    WriteProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x15F1754), originalBytes1, sizeof(originalBytes1), NULL);
                    BYTE originalBytes2[] = { 0xF3, 0x0F, 0x11, 0x87, 0xA8, 0x01, 0x00, 0x00 };
                    WriteProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x15F1729), originalBytes2, sizeof(originalBytes2), NULL);
                }
            }
            if (ImGui::Checkbox("No Spread", &NoSpread))
            {
                if (NoSpread)
                {
                    NopFunc(hProcess, (LPVOID)(ModuleBase + 0x15F1800), 8);
                }
                if (!NoSpread)
                {
                    BYTE originalBytes2[] = { 0xF3, 0x0F, 0x11, 0x8F, 0x28, 0x01, 0x00, 0x48 };
                    WriteProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x15F1800), originalBytes2, sizeof(originalBytes2), NULL);
                }
            }
            if (ImGui::Checkbox("Unl Ammo", &UnlAmmo))
            {
                if (UnlAmmo)
                {
                    BYTE originalBytes2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
                    WriteProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x15DCBB6), originalBytes2, sizeof(originalBytes2), NULL);
                }
                if (!UnlAmmo)
                {
                    BYTE originalBytes2[] = { 0x89, 0x8B, 0x04, 0x01, 0x00, 0x00 };
                    WriteProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x15DCBB6), originalBytes2, sizeof(originalBytes2), NULL);
                }
            }
            ImGui::End();
        }
        if (ESP && openMenu)
        {
            ImGui::Begin("ESP");
            ImGui::Checkbox("ObjektESP", &OESPBool);
            ImGui::SliderFloat("I_Range", &DistSliderItems, 0.f, 300.f);
            ImGui::ColorEdit4("ItemColor", ItemColor);
            ImGui::Checkbox("CharakterESP", &ZESPBool);
            ImGui::SliderFloat("Z_Range", &DistSliderCharakter, 0.f, 300.f);
            ImGui::ColorEdit4("ZombieColor", ZombieColor);
            ImGui::End();
        }
        
        byte bcache;
        byte fcache;
        float up;
        float HitRange;
        byte ZIgnore;
        if (!GodMode && !GodModefertig)
        {
            bcache = 116;
            HitRange = 60;
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x73 })), &bcache, sizeof(bcache), nullptr); // GodMode
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x9E0, 0xEC })), &HitRange, sizeof(HitRange), nullptr); //Kein Kameraschütteln
            GodModefertig = true;
        }
        if (GodMode)
        {
            bcache = 0;
            HitRange = 0;
            float Stamina = 100.f;
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x73 })), &bcache, sizeof(bcache), nullptr);
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x9E0, 0xEC })), &HitRange, sizeof(HitRange), nullptr); //Kein Kameraschütteln 
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x9C0, 0x114 })), &Stamina, sizeof(Stamina), nullptr); //maxStamina
            GodModefertig = false;
        }
        if (!FlyHack && !FlyHackfertig)
        {

            fcache = 1;
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x1280, 0x1A4 })), &fcache, sizeof(fcache), nullptr);
            FlyHackfertig = true;
        }
        if (FlyHack)
        {
            fcache = 5;
            WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x1280, 0x1A4 })), &fcache, sizeof(fcache), nullptr);
            if (GetAsyncKeyState(VK_SPACE))
            {
                up = 500;
                WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x1280, 0x104 })), &up, sizeof(up), nullptr);
            }
            if (GetAsyncKeyState(VK_CONTROL))
            {
                up = -500;
                WriteProcessMemory(hProcess, (BYTE*)(FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x288, 0x1280, 0x104 })), &up, sizeof(up), nullptr);
            }
            FlyHackfertig = false;
        }

        Ents = 0;        
        if (OESPBool)
        {
            DWORD Loop = 0x0;
            for (int i = 0; i < 800; i++)
            {                
                Vector3 Pos = { 0.f,0.f,0.f };
                Vector2 vScreen;
                ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (OEntList + Loop), { 0x160, 0x184 })), &Pos, sizeof(Pos), nullptr); //all objects
                float Dist = glm::distance(glm::vec3(Pos.x, Pos.y, Pos.z), glm::vec3(gameCamera.CamPos.x, gameCamera.CamPos.y, gameCamera.CamPos.z)) / 100;

                if (Pos.x != 0.f && Dist < DistSliderItems && Dist > 0.1f)
                {
                    Ents++;
                    RECT rect;
                    GetWindowRect(hwnd, &rect);                    
                    Pos.z = Pos.z - (Dist * 4);
                    DWORD NameID;
                    ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (OEntList + Loop), { 0x10, 0xC })), &NameID, sizeof(NameID), nullptr); //name


                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << Dist << "m";
                    std::string formattedFloatValue = ss.str();
                    gameCamera.UEWorldToScreen(Pos, vScreen, gameCamera.Rotation, gameCamera.CamPos, gameCamera.Fov, rect.bottom, rect.right);
                    Drawlist->AddText(ImVec2(CalcMiddlePos(vScreen.x, formattedFloatValue.c_str()), vScreen.y), ImColor(ItemColor[0], ItemColor[1], ItemColor[2], ItemColor[3]), formattedFloatValue.c_str());
                    
                }
                Loop += 0x8;
            }                   
        }
        if (ZESPBool)
        {            
            DWORD Loop = 0x0;
            for (int i = 0; i < 100; i++)
            {
                Vector3 ZPos = { 0.f,0.f,0.f };
                float Health;
                Vector2 ZvScreen;
                ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x2c8, 0x168 })), &ZPos, sizeof(ZPos), nullptr); //Pos
                ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x9B8, 0x178 })), &Health, sizeof(Health), nullptr); //Health
                float Dist = glm::distance(glm::vec3(ZPos.x, ZPos.y, ZPos.z), glm::vec3(gameCamera.CamPos.x, gameCamera.CamPos.y, gameCamera.CamPos.z)) / 100;
                
                if (ZPos.x != 0.f && Health > 0.f && Dist < DistSliderCharakter && Dist > 1.5f)
                {
                    Ents++;
                    ZPos.z -= 90.f;
                                        
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << Dist << "m";
                    std::string formattedFloatValue = ss.str();
                    gameCamera.UEWorldToScreen(ZPos, ZvScreen, gameCamera.Rotation, gameCamera.CamPos, gameCamera.Fov, rect.bottom, rect.right);
                    //Drawlist->AddCircle(ImVec2(ZvScreen.x, ZvScreen.y), 1, ImColor(255, 0, 0, 255), 10, 5);
                    Drawlist->AddText(ImVec2(CalcMiddlePos(ZvScreen.x, formattedFloatValue.c_str()), ZvScreen.y), ImColor(ZombieColor[0], ZombieColor[1], ZombieColor[2], ZombieColor[3]), formattedFloatValue.c_str());
                    







                    Vector3 TestPos;
                    uintptr_t EntBaseAdresse;
                    Vector2 TestVScreen;
                    uintptr_t BoneBase;
                    ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x2c8, 0x168 })), &TestPos, sizeof(TestPos), nullptr);
                    ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x2c8, 0x550 })), &BoneBase, sizeof(BoneBase), nullptr);
                    ReadProcessMemory(hProcess, (uintptr_t*)(CEntList + Loop), &EntBaseAdresse, sizeof(EntBaseAdresse), nullptr);

                    TestPos.z -= 100.f;

                    uintptr_t BoneBaseAdresse;
                    ReadProcessMemory(hProcess, (uintptr_t*)FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x2c8, 0x550 }), &BoneBaseAdresse, sizeof(BoneBaseAdresse), 0);
                    DrawSkeleton(hProcess, BoneBaseAdresse,TestPos, Drawlist, gameCamera, rect);

                    //DWORD BLoop = 0x40;
                    //for (int i = 0; i < 400; i++)
                    //{
                    //    

                    //    Vector3 BonePos;
                    //    Vector2 vBoneScreen;
                    //    ReadProcessMemory(hProcess, (uintptr_t*)(FindDMAAddy(hProcess, (CEntList + Loop), { 0x180, 0x2c8, 0x550, BLoop })), &BonePos, sizeof(BonePos), nullptr);
                    //    BonePos.x += TestPos.x;
                    //    BonePos.y += TestPos.y;
                    //    BonePos.z += TestPos.z;
                    //    gameCamera.UEWorldToScreen(BonePos, vBoneScreen, gameCamera.Rotation, gameCamera.CamPos, gameCamera.Fov, rect.bottom, rect.right);
                    //    
                    //    std::stringstream sss;
                    //    sss << std::fixed << std::hex << BLoop;
                    //    std::string sssformattedFloatValue = sss.str();
                    //    
                    //    //Drawlist->AddCircleFilled(ImVec2(vBoneScreen.x, vBoneScreen.y), 3, ImColor(255, 123, 0, 255), 5);
                    //    Drawlist->AddText(ImVec2(CalcMiddlePos(vBoneScreen.x, sssformattedFloatValue.c_str()), vBoneScreen.y), ImColor(ZombieColor[0], ZombieColor[1], ZombieColor[2], ZombieColor[3]), sssformattedFloatValue.c_str());
                    //    BLoop += 0x20;
                    //}


                }

                


                

                Loop += 0x8;
            }
        }
        if (demoWindow)
        {
            
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
       


        float TransparentColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, TransparentColor);
        ImGui::Render();
        float blend[4] = { 0 };
        

        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        //g_pSwapChain->Present(1, 0); // Present with vsync
        g_pSwapChain->Present(0, 0);
    }

    VirtualFreeEx(hProcess, codecaveAddress, 0, MEM_RELEASE);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CloseHandle(hProcess);

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}
void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}
void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}
void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
