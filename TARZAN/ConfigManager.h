#pragma once
#include <string>
#include <windows.h>

#include "Singleton.h"

struct FEditorConfig
{
    bool bIsOrthogonal = false;
    float WorldGridScale = 1.f;
    float CameraSpeed = 1.f;
};

class ConfigManager : public TSingleton<ConfigManager>
{
public:

    // �⺻ ���ϸ� editor.ini
    ConfigManager(const std::wstring& file = L"Config/EditorConfig.ini") : _fileName(file) {}
    void LoadConfig();
    void SaveConfig() const;
    void Shutdown();

    FEditorConfig GetEditorConfig() const { return _editorConfig; }

private:
    std::wstring GetValue(const std::wstring& section, const std::wstring& key, const std::wstring& defaultValue = L"") const;
    bool SetValue(const std::wstring& section, const std::wstring& key, const std::wstring& value) const;

    // ���� �Լ�: ������ �����ϴ��� Ȯ��
    static bool DoesFileExist(const std::wstring& filename)
    {
        DWORD attrib = GetFileAttributesW(filename.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    static bool IsConfigDirectoryExist()
    {
        DWORD dwAttrib = GetFileAttributesW(L"Config");
        if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
        {
            CreateDirectoryW(L"Config", nullptr);
            return false;
        }
        return true;
    }

    // �̰� �ΰ� ���߿� �ϳ��� �������� �ҵ�
    // ���� ����ɶ����� �ѹ��� ���°ɷ�
    FEditorConfig _editorConfig;
    std::wstring _fileName;
};
