// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// 获取编译时间戳的函数
extern "C" __declspec(dllexport) DWORD GetCompilationTimestamp()
{
    // 获取当前模块句柄
    HMODULE hModule = GetModuleHandle(nullptr);

    // 获取模块文件名
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(hModule, szFileName, MAX_PATH);

    // 打开模块文件
    HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return 0; // 打开文件失败
    }

    // 读取 DOS 头
    IMAGE_DOS_HEADER dosHeader;
    DWORD dwBytesRead;
    ReadFile(hFile, &dosHeader, sizeof(IMAGE_DOS_HEADER), &dwBytesRead, nullptr);

    // 定位到 PE 头
    SetFilePointer(hFile, dosHeader.e_lfanew, nullptr, FILE_BEGIN);

    // 读取 PE 头签名
    DWORD dwSignature;
    ReadFile(hFile, &dwSignature, sizeof(DWORD), &dwBytesRead, nullptr);

    if (dwSignature != IMAGE_NT_SIGNATURE)
    {
        CloseHandle(hFile);
        return 0; // 非有效的 PE 文件
    }

    // 读取文件头
    IMAGE_FILE_HEADER fileHeader;
    ReadFile(hFile, &fileHeader, sizeof(IMAGE_FILE_HEADER), &dwBytesRead, nullptr);

    // 关闭文件句柄
    CloseHandle(hFile);

    return fileHeader.TimeDateStamp;
}
