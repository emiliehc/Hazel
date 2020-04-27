#include "GDAssetManager.h"

#include <filesystem>

#include "Hazel/Core/Log.h"

namespace GD
{
    void GDAssetManager::Init()
    {
        std::string resPath = "assets/res";
        // load all textures
        for (const auto& entry : std::filesystem::directory_iterator(resPath))
        {
            auto path = entry.path();
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
        }

        // TODO : audio and openal at the engine level, then load them
    }

    Hazel::Ref<Hazel::Texture2D> GDAssetManager::GetTexture(const std::string& name)
    {
        return s_Textures[name];
    }
}
