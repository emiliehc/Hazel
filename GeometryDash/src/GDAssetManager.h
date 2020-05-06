#pragma once
#include <map>
#include <filesystem>
#include <future>


#include "HazelAudio.h"
#include "Hazel/Core/Core.h"
#include "Hazel/Renderer/Texture.h"

namespace GD
{
    class GDAssetManager
    {
    public:
        GDAssetManager() = delete;

        static void Init();

        static Hazel::Ref<Hazel::Texture2D> GetTexture(const std::string& name);
        static Hazel::AudioSource* GetAudioSource(const std::string& name);
    private:
        inline static std::map<std::string, Hazel::Ref<Hazel::Texture2D>> s_Textures;
        inline static std::map<std::string, Hazel::AudioSource> s_AudioSources;

        static void LoadAsset(std::filesystem::directory_entry entry);
    };
}
