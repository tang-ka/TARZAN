#include "stdafx.h"
#include "ConfigManager.h"
#include "Framework/Core/Engine.h"
#include "Framework/Core/UWorldGridComponent.h"

// ini파일 한 줄 읽기
std::wstring ConfigManager::GetValue(const std::wstring& section, const std::wstring& key, const std::wstring& defaultValue) const
{
    wchar_t buffer[256] = { 0 };
    GetPrivateProfileStringW(section.c_str(), key.c_str(), defaultValue.c_str(), buffer, 256, _fileName.c_str());
    return std::wstring(buffer);
}

// ini파일 한 줄 쓰기
bool ConfigManager::SetValue(const std::wstring& section, const std::wstring& key, const std::wstring& value) const
{
    BOOL result = WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), _fileName.c_str());
    return (result != 0);
}

void ConfigManager::LoadConfig()
{
    // Config 폴더가 있는지 확인
    IsConfigDirectoryExist();

    // 파일이 존재하지 않으면 기본값으로 파일 생성
    if (!DoesFileExist(_fileName))
    {
        // 기본값은 _editorConfig에 이미 들어있음 (false, 1, 1)
        SaveConfig();
        // 옵션에 따라 로드 후 바로 반환하거나 계속 진행할 수 있음.
        return;
    }

    // 파일이 존재하는 경우, INI 파일에서 값을 읽어옴
    std::wstring orthStr = GetValue(L"EditorConfig", L"bIsOrthgonal", L"false");
    ConfigManager::GetInstance()._editorConfig.bIsOrthogonal = (orthStr == L"true");

    std::wstring scaleStr = GetValue(L"EditorConfig", L"WorldGridScale", L"1");
    ConfigManager::GetInstance()._editorConfig.WorldGridScale = static_cast<float>(_wtof(scaleStr.c_str()));

    std::wstring sensStr = GetValue(L"EditorConfig", L"CameraSpeed", L"1");
    ConfigManager::GetInstance()._editorConfig.CameraSpeed = static_cast<float>(_wtof(sensStr.c_str()));
}

void ConfigManager::SaveConfig() const
{
    // Config 폴더가 있는지 확인
    const_cast<ConfigManager*>(this)->IsConfigDirectoryExist();

    SetValue(L"EditorConfig", L"bIsOrthgonal", ConfigManager::GetInstance()._editorConfig.bIsOrthogonal ? L"true" : L"false");
    SetValue(L"EditorConfig", L"WorldGridScale", std::to_wstring(ConfigManager::GetInstance()._editorConfig.WorldGridScale));
    SetValue(L"EditorConfig", L"CameraSpeed", std::to_wstring(ConfigManager::GetInstance()._editorConfig.CameraSpeed));
}

void ConfigManager::Shutdown()
{
    // 예를 들어, UEngine이 WorldGridComponent를 private 멤버로 가지고 있고, public getter가 있다면:
    UWorldGridComponent* grid = UEngine::GetInstance().GetWorldGridComponent();
    UCameraComponent* camera= CRenderer::Instance()->GetMainCamera();
    if (grid && camera)
    {
        // ConfigManager의 설정값을 현재 grid의 값으로 업데이트
        ConfigManager::GetInstance()._editorConfig.WorldGridScale = grid->GetGridScale();
        ConfigManager::GetInstance()._editorConfig.CameraSpeed = camera->CameraSpeed;
    }

    // 그 외의 shutdown 작업
    SaveConfig();
}
