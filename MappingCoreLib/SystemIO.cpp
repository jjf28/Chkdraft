#include "SystemIO.h"
#include <SimpleIcu.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <experimental/filesystem>
#include <memory>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <Windows.h>
#endif

bool hasExtension(const std::string &systemFilePath, const std::string &extension)
{
    const std::string systemExtensionSeparator = ".";
    if ( extension.find_last_of(systemExtensionSeparator) == extension.length()-extension.size() )
        return systemFilePath.find_last_of(extension) == systemFilePath.length() - extension.length();
    else
        return systemFilePath.find_last_of(systemExtensionSeparator + extension) == systemFilePath.length() - extension.length() - 1;
}

std::string GetSystemFileSeparator()
{
    std::experimental::filesystem::path::value_type separatorChar = std::experimental::filesystem::path::preferred_separator;
    icux::codepoint separatorArray[2] = {(icux::codepoint)separatorChar, icux::nullChar};
    return icux::toUtf8(icux::filestring(separatorArray));
}

std::string GetSystemFileName(const std::string &systemFilePath)
{
    const std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemFilePath.find_last_of(systemFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length()-systemFileSeparator.length() )
        return systemFilePath.substr(lastSeparator+systemFileSeparator.length(), systemFilePath.length());
    else
        return systemFilePath;
}

std::string GetSystemFileExtension(const std::string &systemFilePath)
{
    const std::string systemExtensionSeparator = ".";
    size_t lastSeparator = systemFilePath.find_last_of(systemExtensionSeparator);
    if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length()-systemExtensionSeparator.length() )
        return systemFilePath.substr(lastSeparator+systemExtensionSeparator.length(), systemFilePath.length());
    else
        return "";
}

std::string GetSystemFileDirectory(const std::string &systemFilePath, bool includeTrailingSeparator)
{
    const std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemFilePath.find_last_of(systemFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < systemFilePath.length() )
    {
        if ( includeTrailingSeparator )
            return systemFilePath.substr(0, lastSeparator+systemFileSeparator.length());
        else
            return systemFilePath.substr(0, lastSeparator);
    }
    return systemFilePath;
}

std::string MakeSystemFilePath(const std::string &systemDirectory, const std::string &fileName)
{
    const std::string systemFileSeparator = GetSystemFileSeparator();
    size_t lastSeparator = systemDirectory.find_last_of(systemFileSeparator);
    if ( lastSeparator == systemDirectory.length() - systemFileSeparator.length() )
        return systemDirectory + fileName;
    else
        return systemDirectory + systemFileSeparator + fileName;
}

std::string MakeExtSystemFilePath(const std::string &systemFilePath, const std::string &extension)
{
    const std::string systemExtensionSeparator = ".";
    const bool extensionIncludesSeparator = extension.find_first_of(systemExtensionSeparator) == 0;
    if ( extensionIncludesSeparator )
        return systemFilePath + extension;
    else
        return systemFilePath + systemExtensionSeparator + extension;
}

std::string MakeExtSystemFilePath(const std::string &systemDirectory, const std::string &fileName, const std::string &extension)
{
    const std::string systemFileSeparator = GetSystemFileSeparator();
    const std::string systemExtensionSeparator = ".";
    size_t lastSeparator = systemDirectory.find_last_of(systemFileSeparator);
    const bool directoryIncludesSeparator = lastSeparator == systemDirectory.length() - systemFileSeparator.length();
    const bool extensionIncludesSeparator = extension.find_first_of(systemExtensionSeparator) == 0;
    if ( directoryIncludesSeparator )
    {
        if ( extensionIncludesSeparator )
            return systemDirectory + fileName + extension;
        else
            return systemDirectory + fileName + systemExtensionSeparator + extension;
    }
    else
    {
        if ( extensionIncludesSeparator )
            return systemDirectory + systemFileSeparator + fileName + extension;
        else
            return systemDirectory + systemFileSeparator + fileName + systemExtensionSeparator + extension;
    }
}

std::string GetMpqFileSeparator()
{
    return "\\";
}

std::string GetMpqFileName(const std::string &mpqFilePath)
{
    std::string mpqFileSeparator = GetMpqFileSeparator();
    size_t lastSeparator = mpqFilePath.find_last_of(mpqFileSeparator);
    if ( lastSeparator >= 0 && lastSeparator < mpqFilePath.length()-mpqFileSeparator.length() )
        return mpqFilePath.substr(lastSeparator+mpqFileSeparator.length(), mpqFilePath.length());
    else
        return mpqFilePath;
}

std::string MakeMpqFilePath(const std::string &mpqDirectory, const std::string &fileName)
{
    std::string mpqFileSeparator = GetMpqFileSeparator();
    size_t lastSeparator = mpqDirectory.find_last_of(mpqFileSeparator);
    if ( lastSeparator == mpqDirectory.length() - mpqFileSeparator.length() )
        return mpqDirectory + fileName;
    else
        return mpqDirectory + mpqFileSeparator + fileName;
}

bool FindFile(const std::string &filePath)
{
    icux::filestring fFilePath = icux::toFilestring(filePath);
    if ( !filePath.empty() )
    {
#ifdef WINDOWS_UTF16
        FILE* file = _wfopen(fFilePath.c_str(), L"r");
#else
        FILE* file = fopen(fFilePath.c_str(), "r");
#endif
        if ( file != nullptr )
        {
            std::fclose(file);
            return true;
        }
    }
    return false;
}

bool PatientFindFile(const std::string &filePath, int numWaitTimes, int* waitTimes)
{
    if ( FindFile(filePath) )
        return true;

    for ( int i = 0; i < numWaitTimes; i++ )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTimes[i]));
        if ( FindFile(filePath) )
            return true;
    }

    return false;
}

bool FileToBuffer(const std::string &fileName, buffer &buf)
{
    bool success = false;
    if ( fileName.length() > 0 )
    {
        FILE* pFile = std::fopen(fileName.c_str(), "rb");
        if ( pFile != nullptr )
        {
            buf.flush();
            std::fseek(pFile, 0, SEEK_END);
            u32 fileSize = (u32)std::ftell(pFile);
            if ( buf.setSize(fileSize) )
            {
                buf.sizeUsed = fileSize;
                std::rewind(pFile);
                size_t lengthRead = std::fread(buf.data, 1, buf.sizeUsed, pFile);
                success = (lengthRead == buf.sizeUsed);
            }
            std::fclose(pFile);
        }
    }
    return success;
}

bool FileToString(const std::string &fileName, std::string &str)
{
    try {
        str.clear();
        std::ifstream file(fileName, std::ifstream::in | std::ifstream::ate); // Open at ending characters position
        if ( file.is_open() )
        {
            auto size = file.tellg(); // Grab size via current position
            str.reserve((size_t)size); // Set string size to file size
            file.seekg(0); // Move reader to beggining of file
            while ( !file.eof() )
                str += file.get();

            if ( str.length() > 0 && str[str.length() - 1] == (char)-1 )
                str[str.length() - 1] = '\0';

            return true;
        }
    }
    catch ( std::exception ) { }
    return false;
}

bool MakeFileCopy(const std::string &inFilePath, const std::string &outFilePath)
{
    bool success = false;
    std::ifstream inFile;
    std::ofstream outFile;
    try
    {
        inFile.open(inFilePath, std::fstream::binary);
        if ( inFile.is_open() )
        {
            outFile.open(outFilePath, std::fstream::out|std::fstream::binary);
            if ( outFile.is_open() )
            {
                outFile << inFile.rdbuf();
                success = true;
                outFile.close();
            }
            inFile.close();
        }
    }
    catch ( std::exception ) { }

    if ( inFile.is_open() )
        inFile.close();

    if ( outFile.is_open() )
        outFile.close();

    return success;
}

bool MakeDirectory(const std::string &directory)
{
    icux::filestring directoryPath = icux::toFilestring(directory);
#ifdef WINDOWS_UTF16
    return _wmkdir(directoryPath.c_str()) == 0;
#else
    return _mkdir(directoryPath.c_str()) == 0;
#endif
}

bool RemoveFile(const std::string &filePath)
{
    icux::filestring sysFilePath = icux::toFilestring(filePath);
    // Return whether the file with the given filePath is not on the system
#ifdef WINDOWS_UTF16
    return filePath.empty() || _wremove(sysFilePath.c_str()) == 0 || !FindFile(filePath);
#else
    return sysFilePath.empty() || remove(sysFilePath.c_str()) == 0 || !FindFile(filePath);
#endif
}

bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName)
{
    bool success = RemoveFile(firstFileName) && RemoveFile(secondFileName);
    return success;
}

bool RemoveFiles(const std::string &firstFileName, const std::string &secondFileName, const std::string &thirdFileName)
{
    bool success = RemoveFile(firstFileName) && RemoveFile(secondFileName) && RemoveFile(thirdFileName);
    return success;
}

bool GetModuleDirectory(output_param std::string &moduleDirectory)
{
#ifdef _WIN32
    icux::codepoint cModulePath[MAX_PATH] = {};
    if ( GetModuleFileName(NULL, cModulePath, MAX_PATH) != MAX_PATH )
    {
        icux::filestring modulePath(cModulePath);
        auto lastBackslashPos = modulePath.find_last_of('\\');
        if ( lastBackslashPos != std::string::npos && lastBackslashPos < modulePath.size() )
        {
            moduleDirectory = icux::toUtf8(modulePath.substr(0, lastBackslashPos));
            return true;
        }
    }
#endif
    return false;
}

// Windows registry functions
#ifdef _WIN32
DWORD GetSubKeyString(HKEY hParentKey, const std::string &subKey, const std::string &valueName, output_param std::string &data)
{
    icux::filestring sysSubKey = icux::toFilestring(subKey);
    icux::filestring sysValueName = icux::toFilestring(valueName);

    HKEY hKey = NULL;

    DWORD errorCode = RegOpenKeyEx(hParentKey, sysSubKey.c_str(), 0, KEY_QUERY_VALUE, &hKey); // Open key
    if ( errorCode == ERROR_SUCCESS ) // Open key success
    {
        DWORD bufferSize = size_1kb;
        do
        {
            std::unique_ptr<char> buffer(new char[bufferSize]);
            bufferSize *= 2;

            errorCode = RegQueryValueEx(hKey, sysValueName.c_str(), NULL, NULL, (LPBYTE)buffer.get(), &bufferSize); // Read Key
            if ( errorCode == ERROR_SUCCESS )
                data = std::string(buffer.get());
        }
        while ( errorCode == ERROR_MORE_DATA && bufferSize <= size_1gb );
        RegCloseKey(hKey); // Close key
    }
    return errorCode; // Return success/error message
}

bool GetRegString(const std::string &subKey, const std::string &valueName, output_param std::string &data)
{
    DWORD errorCode = GetSubKeyString(HKEY_CURRENT_USER, subKey, valueName, data); // Try HKCU
    if ( errorCode != ERROR_SUCCESS ) // Not found in HKCU
        errorCode = GetSubKeyString(HKEY_LOCAL_MACHINE, subKey, valueName, data); // Try HKLM

    return errorCode == ERROR_SUCCESS;
}
#endif

bool GetDefaultScPath(output_param std::string &data)
{
#ifdef _WIN32
    return GetRegString("SOFTWARE\\Blizzard Entertainment\\Starcraft", "InstallPath", data);
#else
    return false;
#endif
}

std::string GetDefaultScPath()
{
    std::string scPath;
    if ( GetDefaultScPath(scPath) )
        return scPath;
    else
        return "";
}

PromptResult GetYesNo(const std::string &text, const std::string &caption)
{
#ifdef _WIN32
    int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNO);
    switch ( result ) {
        case IDYES: return PromptResult::Yes;
        case IDNO: return PromptResult::No;
        default: return PromptResult::Unknown;
    }
#else
    return PromptResult::Unknown;
#endif
}

PromptResult GetYesNoCancel(const std::string &text, const std::string &caption)
{
#ifdef _WIN32
    int result = MessageBox(NULL, icux::toUistring(text).c_str(), icux::toUistring(caption).c_str(), MB_YESNOCANCEL);
    switch ( result ) {
        case IDYES: return PromptResult::Yes;
        case IDNO: return PromptResult::No;
        case IDCANCEL: return PromptResult::Cancel;
        default: return PromptResult::Unknown;
    }
#else
    return PromptResult::Unknown;
#endif
}

bool BrowseForFile(inout_param std::string &filePath, inout_param u32 &filterIndex, const std::map<std::string, std::string> &filterToLabel,
    const std::string &initialDirectory, const std::string &title, bool pathMustExist, bool provideOverwritePrompt)
{
#ifdef _WIN32
    OPENFILENAME ofn = {};
    icux::codepoint fileNameBuffer[FILENAME_MAX] = {};
    if ( !initialDirectory.empty() )
    {
        icux::filestring initFileNameBuf = icux::toFilestring(filePath);
        if ( initFileNameBuf.length() < FILENAME_MAX )
        {
            #ifdef UTF16_FILESYSTEM
            wcscpy(fileNameBuffer, initFileNameBuf.c_str());
            #else
            strcpy(fileNameBuffer, initFileNameBuf.c_str());
            #endif
        }
    }

    std::string filterString = "";
    for ( auto filterLabel : filterToLabel )
    {
        const std::string &filter = filterLabel.first;
        const std::string &label = filterLabel.second;
        filterString += label + '\0' + filter + '\0';
    }

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = icux::toUistring(filterString).c_str();
    ofn.lpstrFile = fileNameBuffer;

    if ( initialDirectory.empty() )
        ofn.lpstrInitialDir = NULL;
    else
        ofn.lpstrInitialDir = icux::toFilestring(initialDirectory).c_str();

    if ( title.empty() )
        ofn.lpstrTitle = NULL;
    else
        ofn.lpstrTitle = icux::toUistring(title).c_str();

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = FILENAME_MAX;
    DWORD flags = OFN_HIDEREADONLY;
    if ( pathMustExist )
        flags |= OFN_PATHMUSTEXIST;
    if ( provideOverwritePrompt )
        flags |= OFN_OVERWRITEPROMPT;

    ofn.Flags = flags;
    ofn.nFilterIndex = filterIndex;

    bool success = GetOpenFileName(&ofn) == TRUE;
    if ( success )
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex;
    }

    return success;
#else
    return false;
#endif
}

bool BrowseForSave(inout_param std::string &filePath, inout_param u32 &filterIndex, const std::map<std::string, std::string> &filterToLabel,
    const std::string &initialDirectory, const std::string &title, bool pathMustExist, bool provideOverwritePrompt)
{
#ifdef _WIN32
    OPENFILENAME ofn = {};
    icux::codepoint fileNameBuffer[FILENAME_MAX] = {};
    if ( !initialDirectory.empty() )
    {
        icux::filestring initFileNameBuf = icux::toFilestring(filePath);
        if ( initFileNameBuf.length() < FILENAME_MAX )
        {
            #ifdef UTF16_FILESYSTEM
            wcscpy(fileNameBuffer, initFileNameBuf.c_str());
            #else
            strcpy(fileNameBuffer, initFileNameBuf.c_str());
            #endif
        }
    }

    std::string filterString = "";
    for ( auto filterLabel : filterToLabel )
    {
        const std::string &filter = filterLabel.first;
        const std::string &label = filterLabel.second;
        filterString += label + '\0' + filter + '\0';
    }

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = icux::toUistring(filterString).c_str();
    ofn.lpstrFile = fileNameBuffer;

    if ( initialDirectory.empty() )
        ofn.lpstrInitialDir = NULL;
    else
        ofn.lpstrInitialDir = icux::toFilestring(initialDirectory).c_str();

    if ( title.empty() )
        ofn.lpstrTitle = NULL;
    else
        ofn.lpstrTitle = icux::toUistring(title).c_str();

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = FILENAME_MAX;
    DWORD flags = OFN_HIDEREADONLY;
    if ( pathMustExist )
        flags |= OFN_PATHMUSTEXIST;
    if ( provideOverwritePrompt )
        flags |= OFN_OVERWRITEPROMPT;

    ofn.Flags = flags;
    ofn.nFilterIndex = filterIndex;

    bool success = GetOpenFileName(&ofn) == TRUE;
    if ( success )
    {
        filePath = icux::toUtf8(fileNameBuffer);
        filterIndex = ofn.nFilterIndex;
    }

    return success;
#else
    return false;
#endif
}