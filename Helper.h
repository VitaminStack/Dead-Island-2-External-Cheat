#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <d3dx9.h>
#include <directxmath.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <psapi.h>
#include <iomanip>
#include <TlHelp32.h>
#include <tchar.h>  
#include <iostream>

struct Vector2
{
	float x, y;
};
class Vector3
{
public:

	Vector3(void)
	{
		x = y = z = 0.0f;
	}

	Vector3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	Vector3(float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vector3(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vector3(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	Vector3(const Vector2& v)
	{
		x = v.x; y = v.y; z = 0.0f;
	}

	Vector3& operator=(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}



	Vector3& operator=(const Vector2& v)
	{
		x = v.x; y = v.y; z = 0.0f; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vector3& operator+=(const Vector3& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	Vector3& operator*=(const Vector3& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	Vector3& operator/=(const Vector3& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	Vector3& operator==(const Vector3& v)
	{

	}

	Vector3& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	Vector3& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	Vector3& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	Vector3& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}



	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 operator/(const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}


	Vector3 operator-(float v) const
	{
		return Vector3(x - v, y - v, z - v);
	}


	Vector3 operator/(float v) const
	{
		return Vector3(x / v, y / v, z / v);
	}

	void Set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	float Length(void) const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	float Length2d(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float Length2dSqr(void) const
	{
		return (x * x + y * y);
	}

	float DistTo(const Vector3& v) const
	{
		return (*this - v).Length();
	}

	float DistToSqr(const Vector3& v) const
	{
		return (*this - v).LengthSqr();
	}

	// Skalierung (Vector3 * float)
	Vector3 operator*(float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	// Addition (Vector3 + Vector3)
	Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	// Dot-Produkt (Skalarprodukt)
	float Dot(const Vector3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	// Cross-Produkt (Vektorprodukt)
	Vector3 Cross(const Vector3& v) const
	{
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}

	bool IsZero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
			y > -0.01f && y < 0.01f &&
			z > -0.01f && z < 0.01f);
	}



public:
	float x, y, z;
};
struct Vector4 {
	float x, y, z, w; // x, y, z = Vektoranteil, w = Skalaranteil (Quaternion)

	Vector4 Conjugate() const {
		return { -x, -y, -z, w };
	}

	Vector4 operator*(const Vector4& other) const {
		return {
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z
		};
	}
};

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
uintptr_t GetModuleBaseAddress(const wchar_t* lpszModuleName, DWORD pID, DWORD& pSize);
uintptr_t ScanAOB(std::vector<int> signature, const wchar_t* ModBaseName, HANDLE hProcess, DWORD ProcID);
std::string getNameFromID(int ID, uintptr_t GNames);
void ReadMemory(const void* address, void* buffer, size_t size);
D3DXMATRIX ToMatrix(Vector3 Rotation);

extern uintptr_t OEntList;
extern uintptr_t CEntList;
uintptr_t CamPosadr;
uintptr_t Rotationadr; //Pitch

uintptr_t GEngine;
uintptr_t OEntListadr;
uintptr_t CEntListadr;


extern DWORD Procid;
extern HANDLE hProcess;
extern HWND targetHwnd;
extern uintptr_t ModuleBase;
LPCWSTR TargetGame;
DWORD pSize;

//Func
extern uintptr_t RecoilAdr;
extern uintptr_t SpreadAdr;

extern uintptr_t AmmoAdr;
#pragma once
#include <Windows.h>
#include <cmath>

class Camera
{
public:
	Vector3 CamPos;  // Weltposition der Kamera
	Vector3 Rotation;  // Pitch, Yaw, Roll (ViewAngles)
	float Fov;         // Sichtfeld (Field of View)
	

	Camera() : CamPos(0, 0, 0), Rotation(0, 0, 0), Fov(90.0f) {}

	// Setzt die Kamera-Daten aus dem Spiel-Speicher
	void Update(HANDLE hProcess)
	{
		ReadProcessMemory(hProcess, (uintptr_t*)(CamPosadr), &CamPos, sizeof(CamPos), nullptr);
		ReadProcessMemory(hProcess, (uintptr_t*)(Rotationadr), &Rotation, sizeof(Rotation), nullptr);
		ReadProcessMemory(hProcess, (uintptr_t*)(CamPosadr + 0x18), &Fov, sizeof(Fov), nullptr);
	}

	bool UEWorldToScreen(const Vector3& worldLoc, Vector2& screenPos, Vector3 Rotation, Vector3 CamPos, float FOV, int ScreenHöhe, int ScreenBreite)
	{
		D3DMATRIX tempMatrix = ToMatrix(Rotation); // Matrix

		float PI = 3.1415926535897932f;


		Vector3 vDelta;
		vDelta.x = worldLoc.x - CamPos.x;
		vDelta.y = worldLoc.y - CamPos.y;
		vDelta.z = worldLoc.z - CamPos.z;

		Vector3 vTransformed = {
			vDelta.Dot({ tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2] }),
			vDelta.Dot({ tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2] }),
			vDelta.Dot({ tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2] })
		};

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;
		float screenCenterX = ScreenBreite / 2.0f;
		float screenCenterY = ScreenHöhe / 2.0f;

		screenPos.x = (screenCenterX)+vTransformed.x * (screenCenterX / tanf(FOV * PI / 360.f)) / vTransformed.z;
		screenPos.y = (screenCenterY - vTransformed.y * (screenCenterX / tanf(FOV * PI / 360.f)) / vTransformed.z);

		if (screenPos.x > ScreenBreite || screenPos.x < 0.f || screenPos.y > ScreenHöhe || screenPos.y < 0.f)
			return false;

		return true;
	}
};

struct BoneOffsets
{
	static constexpr DWORD spine_2 = 0x40;       // Mittlerer Rücken

	static constexpr DWORD neck = 0xd60;          // Hals
	static constexpr DWORD head = 0x2140;          // Kopf

	static constexpr DWORD left_shoulder = 0x5e0; // Linke Schulter
	static constexpr DWORD left_elbow = 0x640;    // Linker Ellbogen
	static constexpr DWORD left_hand = 0x9a0;     // Linke Hand

	static constexpr DWORD right_shoulder = 0x160; // Rechte Schulter
	static constexpr DWORD right_elbow = 0x1c0;    // Rechter Ellbogen
	static constexpr DWORD right_hand = 0x460;     // Rechte Hand

	static constexpr DWORD left_hip = 0x25c0;      // Linke Hüfte
	static constexpr DWORD left_knee = 0x28C0;     // Linkes Knie
	static constexpr DWORD left_foot = 0x2620;     // Linker Fuß

	static constexpr DWORD right_hip = 0x26e0;     // Rechte Hüfte
	static constexpr DWORD right_knee = 0x2500;    // Rechtes Knie
	static constexpr DWORD right_foot = 0x2560;    // Rechter Fuß
};



Vector3 GetBoneWorldPosition(HANDLE hProcess, uintptr_t meshAddress, DWORD boneOffset, Vector3 BasePos)
{
	if (!meshAddress) return { 0, 0, 0 };

	uintptr_t boneAddr = meshAddress + boneOffset;

	// Rotation liegt 0x10 vor der Position
	Vector4 boneRotation;
	ReadProcessMemory(hProcess, (LPCVOID)(boneAddr - 0x10), &boneRotation, sizeof(Vector4), NULL);

	Vector3 bonePos;
	ReadProcessMemory(hProcess, (LPCVOID)boneAddr, &bonePos, sizeof(Vector3), NULL);

	// 2. Addiere die Root-Position (BasePos)
	return bonePos + BasePos;
}




void DrawSkeleton(HANDLE hProcess, uintptr_t meshAddress, Vector3 BasePos, ImDrawList* Drawlist, Camera cam, RECT rect)
{
	// Bone-Paare mit richtigen Typen
	std::vector<std::pair<DWORD, DWORD>> bonePairs = {
		//{BoneOffsets::head, BoneOffsets::neck},
		{BoneOffsets::neck, BoneOffsets::spine_2},

		{BoneOffsets::neck, BoneOffsets::left_shoulder},
		{BoneOffsets::left_shoulder, BoneOffsets::left_elbow},
		{BoneOffsets::left_elbow, BoneOffsets::left_hand},

		{BoneOffsets::neck, BoneOffsets::right_shoulder},
		{BoneOffsets::right_shoulder, BoneOffsets::right_elbow},
		{BoneOffsets::right_elbow, BoneOffsets::right_hand},

		{BoneOffsets::spine_2, BoneOffsets::left_hip},
		{BoneOffsets::left_hip, BoneOffsets::left_knee},
		{BoneOffsets::left_knee, BoneOffsets::left_foot},

		{BoneOffsets::spine_2, BoneOffsets::right_hip},
		{BoneOffsets::right_hip, BoneOffsets::right_knee},
		{BoneOffsets::right_knee, BoneOffsets::right_foot}
	};


	for (const std::pair<DWORD, DWORD>& bonePair : bonePairs)
	{
		DWORD bone1 = bonePair.first;
		DWORD bone2 = bonePair.second;

		// 1. Lese die Bone-Weltpositionen
		Vector3 bone1Pos = GetBoneWorldPosition(hProcess, meshAddress, bone1, BasePos);
		Vector3 bone2Pos = GetBoneWorldPosition(hProcess, meshAddress, bone2, BasePos);

		// 2. Wandle Bone-Positionen in Bildschirm-Koordinaten um
		Vector2 screenBone1, screenBone2;
		cam.UEWorldToScreen(bone1Pos, screenBone1, cam.Rotation, cam.CamPos, cam.Fov, rect.bottom, rect.right);
		cam.UEWorldToScreen(bone2Pos, screenBone2, cam.Rotation, cam.CamPos, cam.Fov, rect.bottom, rect.right);
		
		// 3. Zeichne die Verbindungslinie zwischen den Bones
		Drawlist->AddLine(ImVec2(screenBone1.x, screenBone1.y), ImVec2(screenBone2.x, screenBone2.y), ImColor(255, 0, 0, 255), 1.0f);
		
	}
}


void initHax()
{		
	targetHwnd = FindWindow(nullptr, TargetGame); //TargetName
	GetWindowThreadProcessId(targetHwnd, &Procid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Procid);
	ReadProcessMemory(hProcess, (uintptr_t*)(0x146EA8A88), &GEngine, sizeof(GEngine), nullptr); //DeadIsland2
	OEntListadr = FindDMAAddy(hProcess, (GEngine + 0x7D8), { 0x78, 0x30, 0x98 });
	CEntListadr = FindDMAAddy(hProcess, (GEngine + 0x7D8), { 0x78, 0x150, 0x2D0 });
	CamPosadr = FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x2F0, 0x1360 });
	Rotationadr = FindDMAAddy(hProcess, (GEngine + 0xEA8), { 0x38, 0x0, 0x30, 0x2F0, 0x136C });

	ModuleBase = GetModuleBaseAddress(L"DeadIsland-Win64-Shipping.exe", Procid, pSize);

	//RecoilAdr = ScanAOB({ 0xF3, 0x0F, 0x58, 0x87, 0xAC, 0x01, 0x00, 0x00, 0xFF, 0x90, 0xB0, 0x02, 0x00, 0x00 }, L"DeadIsland-Win64-Shipping.exe", hProcess, Procid);
	
}


void UpdateHax()
{		
	ReadProcessMemory(hProcess, (uintptr_t*)(OEntListadr), &OEntList, sizeof(OEntList), nullptr);
	ReadProcessMemory(hProcess, (uintptr_t*)(CEntListadr), &CEntList, sizeof(CEntList), nullptr);
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}
std::string getNameFromID(int ID, uintptr_t GNames)
{
	char charName[35];
	uintptr_t fNamePtr = *(uintptr_t*)(GNames + int(ID / 0x4000) * 0x8);
	uintptr_t fName = *(uintptr_t*)(fNamePtr + 0x8 * int(ID % 0x4000));
	const void* bName = (uintptr_t*)(fName + 0x10);
	ReadMemory(bName, charName, sizeof(charName));
	std::stringstream ss;
	std::string Name;
	ss << charName;
	ss >> Name;

	return Name;
}
char* getNameFromIDEx(int ID, uintptr_t GNamesAdr, HANDLE hProcess)
{
	uintptr_t GNames;
	uintptr_t fNamePtr;
	uintptr_t fName;
	char charName[20];
	ReadProcessMemory(hProcess, (uintptr_t*)(GNamesAdr + int(ID / 0x4000) * 0x8), &fNamePtr, sizeof(fNamePtr), nullptr);
	ReadProcessMemory(hProcess, (uintptr_t*)(0x147720E50 + 0x8 * int(ID % 0x4000)), &fName, sizeof(fName), nullptr);
	ReadProcessMemory(hProcess, (uintptr_t*)(fName + 0x10), &charName, sizeof(charName), nullptr);
	

	return charName;
}
char GetNameFromFName(int ID, uintptr_t GNamesAdr, HANDLE hProcess)
{
	char Name;
	DWORD_PTR fNamePtr;
	DWORD_PTR fName;
	// Read the fNamePtr
	ReadProcessMemory(hProcess, (LPCVOID)(GNamesAdr + int(ID / 0x4000) * 0x8), &fNamePtr, sizeof(DWORD_PTR), NULL);

	// Read the fName
	ReadProcessMemory(hProcess, (LPCVOID)(GNamesAdr + 0x8 * int(ID % 0x4000)), &fName, sizeof(DWORD_PTR), NULL);
	
	// Read the text struct
	ReadProcessMemory(hProcess, (LPCVOID)(fName + 0x10), &Name, sizeof(Name), NULL);
	

	return Name;

}
void ReadMemory(const void* address, void* buffer, size_t size)
{
	DWORD back = NULL;

	if (VirtualProtect((LPVOID)address, size, PAGE_READWRITE, &back))
	{
		memcpy(buffer, address, size);

		VirtualProtect((LPVOID)address, size, back, &back);
	}
}

D3DXMATRIX ToMatrix(Vector3 Rotation)
{
	Vector3 origin;
	origin.x = 0.f;
	origin.y = 0.f;
	origin.z = 0.f;
	float radPitch = (Rotation.x * float(3.1415926535897932f) / 180.f);
	float radYaw = (Rotation.y * float(3.1415926535897932f) / 180.f);
	float radRoll = (Rotation.z * float(3.1415926535897932f) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;

}
std::string replaceAll(std::string& str, const std::string& from, const std::string& to) {

	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return str;
}
float CalcMiddlePos(float vScreenX, const char* Text)
{
	float itextX = vScreenX - ((strlen(Text) / 2) * 5);
	return itextX;
}
void SetOverlayToTarget(HWND WindowHandle, HWND OverlayHandle)
{
	RECT rect;
	GetWindowRect(WindowHandle, &rect);
	int Breite = (rect.right - rect.left);
	int Höhe = (rect.bottom - rect.top);

	//MoveWindow(OverlayHandle, rect.left + 8, rect.top, Breite - 16, Höhe - 8, true);
	MoveWindow(OverlayHandle, rect.left, rect.top, Breite, Höhe, true);
}

// Eine Funktion, die einen 3D-Punkt aus der Weltkoordinate in Bildschirmkoordinaten umwandelt
void WorldToScreen(Vector3 worldPoint, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, int screenWidth, int screenHeight, float& VscreenPosX, float& VscreenPosY)
{
	// Berechne die View-Projection-Matrix
	DirectX::XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix;

	// Transformiere die 3D-Position in Homogene Koordinaten
	DirectX::XMVECTOR worldPosVec = DirectX::XMVectorSet(worldPoint.x, worldPoint.y, worldPoint.z, 1.0f);

	// Transformiere die 3D-Position in Bildschirmkoordinaten
	DirectX::XMVECTOR screenPosVec = XMVector3Project(worldPosVec, 0, 0, screenWidth, screenHeight, 0.0f, 1.0f, viewProjectionMatrix, viewMatrix, DirectX::XMMatrixIdentity());

	// Extrahiere die X-, Y- und Z-Komponenten der Bildschirmkoordinaten
	float screenPosX = DirectX::XMVectorGetX(screenPosVec);
	float screenPosY = DirectX::XMVectorGetY(screenPosVec);
	float screenPosZ = DirectX::XMVectorGetZ(screenPosVec);

	// Wandle die Bildschirmkoordinaten in Pixel um
	VscreenPosX = (screenPosX * 0.5f + 0.5f) * screenWidth;
	VscreenPosY = (screenPosY * 0.5f + 0.5f) * screenHeight;
}

uintptr_t GetModuleBaseAddress(const wchar_t* lpszModuleName, DWORD pID, DWORD &pSize) {
	uintptr_t dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32))
	{
		do {
			if (wcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
			{
				dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
				pSize = ModuleEntry32.modBaseSize;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32));


	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

uintptr_t GetDynamicAddress(HANDLE hProcess, uintptr_t baseAddress, std::vector<DWORD> offsets) {
	uintptr_t dynamicAddress = baseAddress;
	for (int i = 0; i < offsets.size() - 1; i++)
	{
		ReadProcessMemory(hProcess, (LPCVOID)(dynamicAddress + offsets[i]), &dynamicAddress, sizeof(offsets.at(i)), NULL);
		//std::cout << "Current Adress: " << std::hex << healthAddress << std::endl;
	}
	dynamicAddress += offsets[offsets.size() - 1];
	return dynamicAddress;
}

uintptr_t GetAddressFromSignature(std::vector<int> signature, HANDLE hProcess, uintptr_t pBaseAddress, DWORD pSize) {
	if (pBaseAddress == NULL || hProcess == NULL) {
		return NULL;
	}
	std::vector<byte> memBuffer(pSize);
	if (!ReadProcessMemory(hProcess, (LPCVOID)(pBaseAddress), memBuffer.data(), pSize, NULL)) {
		return NULL;
	}
	for (int i = 0; i < pSize; i++) {
		for (uintptr_t j = 0; j < signature.size(); j++) {
			if (signature.at(j) != -1 && signature[j] != memBuffer[i + j])
				//std::cout << std::hex << signature.at(j) << std::hex << memBuffer[i + j] << std::endl;
				break;
			if (signature[j] == memBuffer[i + j] && j > 0)
				
			if (j + 1 == signature.size())
				return pBaseAddress + i;
		}
	}
	return NULL;
}

uintptr_t ScanAOB(std::vector<int> signature, const wchar_t* ModBaseName, HANDLE hProcess, DWORD ProcID)
{
	DWORD pSize;
	uintptr_t Modulebase = GetModuleBaseAddress(ModBaseName, Procid, pSize);
	uintptr_t signature_pointer = GetAddressFromSignature(signature, hProcess, Modulebase, pSize);

	return signature_pointer;
}

void DetourEx(HANDLE hProcess, LPVOID targetFunctionAddress, LPVOID codecaveAddress)
{
	
	//Jmp zur CodeCave auf die originale Func
	BYTE jmpBytes[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	if (!WriteProcessMemory(hProcess, targetFunctionAddress, jmpBytes, sizeof(jmpBytes), NULL))
	{
		// Fehlerbehandlung
	}
	if (!WriteProcessMemory(hProcess, (LPVOID)((ULONGLONG)targetFunctionAddress + 6), &codecaveAddress, sizeof(codecaveAddress), NULL))
	{
		// Fehlerbehandlung
	}


	//Jump Back zurück nach den überschriebenen Bytes
	BYTE backjmpBytes[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uintptr_t backrelativeBackOffset = (uintptr_t)((uintptr_t)targetFunctionAddress + ((uintptr_t)0xE)); //0xE = 14 bytes
	if (!WriteProcessMemory(hProcess, (LPVOID)((ULONGLONG)codecaveAddress), backjmpBytes, sizeof(backjmpBytes), NULL))
	{
		// Fehlerbehandlung
	}
	if (!WriteProcessMemory(hProcess, (LPVOID)((ULONGLONG)codecaveAddress + 6), &backrelativeBackOffset, sizeof(backrelativeBackOffset), NULL))
	{
		// Fehlerbehandlung
	}		
}

void NopFunc(HANDLE hProcess, LPVOID targetFunctionAddress, int bytes)
{
	byte* Nopbytes = new byte[bytes];
	for (int i = 0; i < bytes; i++)
	{
		Nopbytes[i] = 0x90;
	}
	WriteProcessMemory(hProcess, (LPVOID)((ULONGLONG)targetFunctionAddress), Nopbytes, sizeof(Nopbytes), NULL);
}