﻿#include "GDAssetManager.h"

#include <filesystem>
#include "Hazel/Core/Log.h"


namespace GD
{
    void GDAssetManager::LoadAsset(std::filesystem::directory_entry entry)
    {
        const auto& path = entry.path();
        std::wstring extension = path.extension();
        if (extension == L".png")
        {
            std::wstring filenamew = path.filename().c_str();
            std::string filename(filenamew.begin(), filenamew.end());
            std::wstring filepathw = path.c_str();
            std::string filepath(filepathw.begin(), filepathw.end());
            HZ_TRACE("Loading Texture: {0}", filename);
            s_Textures[filename] = Hazel::Texture2D::Create(filepath);
        }
        else if (extension == L".mp3" || extension == L".ogg")
        {
            std::wstring filenamew = path.filename().c_str();
            std::string filename(filenamew.begin(), filenamew.end());
            std::wstring filepathw = path.c_str();
            std::string filepath(filepathw.begin(), filepathw.end());
            HZ_TRACE("Loading Audio: {0}", filename);
            s_AudioSources[filename] = Hazel::AudioSource::LoadFromFile(filepath);
        }
    }

    void GDAssetManager::Init()
    {
        std::string resPath = "assets/res";
        for (const auto& entry : std::filesystem::directory_iterator(resPath))
        {
            LoadAsset(entry);
        }
    }

    Hazel::Ref<Hazel::Texture2D> GDAssetManager::GetTexture(const std::string& name)
    {
        return s_Textures[name];
    }

    Hazel::AudioSource* GDAssetManager::GetAudioSource(const std::string& name)
    {
        return &s_AudioSources[name];
    }
}
