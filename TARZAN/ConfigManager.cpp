#include "stdafx.h"
#include "ConfigManager.h"
#include "Framework/Core/Engine.h"
#include "Framework/Core/UWorldGridComponent.h"

// ini���� �� �� �б�
std::wstring ConfigManager::GetValue(const std::wstring& section, const std::wstring& key, const std::wstring& defaultValue) const
{
    wchar_t buffer[256] = { 0 };
    GetPrivateProfileStringW(section.c_str(), key.c_str(), defaultValue.c_str(), buffer, 256, _fileName.c_str());
    return std::wstring(buffer);
}

// ini���� �� �� ����
bool ConfigManager::SetValue(const std::wstring& section, const std::wstring& key, const std::wstring& value) const
{
    BOOL result = WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), _fileName.c_str());
    return (result != 0);
}

void ConfigManager::LoadConfig()
{
    // Config ������ �ִ��� Ȯ��
    IsConfigDirectoryExist();

    // ������ �������� ������ �⺻������ ���� ����
    if (!DoesFileExist(_fileName))
    {
        // �⺻���� _editorConfig�� �̹� ������� (false, 1, 1)
        SaveConfig();
        // �ɼǿ� ���� �ε� �� �ٷ� ��ȯ�ϰų� ��� ������ �� ����.
        return;
    }

    // ������ �����ϴ� ���, INI ���Ͽ��� ���� �о��
    std::wstring orthStr = GetValue(L"EditorConfig", L"bIsOrthgonal", L"false");
    ConfigManager::GetInstance()._editorConfig.bIsOrthogonal = (orthStr == L"true");

    std::wstring scaleStr = GetValue(L"EditorConfig", L"WorldGridScale", L"1");
    ConfigManager::GetInstance()._editorConfig.WorldGridScale = static_cast<float>(_wtof(scaleStr.c_str()));

    std::wstring sensStr = GetValue(L"EditorConfig", L"CameraSpeed", L"1");
    ConfigManager::GetInstance()._editorConfig.CameraSpeed = static_cast<float>(_wtof(sensStr.c_str()));
}

void ConfigManager::SaveConfig() const
{
    // Config ������ �ִ��� Ȯ��
    const_cast<ConfigManager*>(this)->IsConfigDirectoryExist();

    SetValue(L"EditorConfig", L"bIsOrthgonal", ConfigManager::GetInstance()._editorConfig.bIsOrthogonal ? L"true" : L"false");
    SetValue(L"EditorConfig", L"WorldGridScale", std::to_wstring(ConfigManager::GetInstance()._editorConfig.WorldGridScale));
    SetValue(L"EditorConfig", L"CameraSpeed", std::to_wstring(ConfigManager::GetInstance()._editorConfig.CameraSpeed));
}

void ConfigManager::Shutdown()
{
    // ���� ���, UEngine�� WorldGridComponent�� private ����� ������ �ְ�, public getter�� �ִٸ�:
    UWorldGridComponent* grid = UEngine::GetInstance().GetWorldGridComponent();
    UCameraComponent* camera= CRenderer::Instance()->GetMainCamera();
    if (grid && camera)
    {
        // ConfigManager�� �������� ���� grid�� ������ ������Ʈ
        ConfigManager::GetInstance()._editorConfig.WorldGridScale = grid->GetGridScale();
        ConfigManager::GetInstance()._editorConfig.CameraSpeed = camera->CameraSpeed;
    }

    // �� ���� shutdown �۾�
    SaveConfig();
}
