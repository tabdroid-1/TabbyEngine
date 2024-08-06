// #pragma once
// #include <Tabby/Renderer/Shader.h>
// #include <Tabby/Renderer/Texture.h>
// #include <Tabby/Renderer/Material.h>
//
// namespace Tabby {
//
// class MaterialManager {
// public:
//     static Shared<MaterialManager> GetInstance();
//     void LoadAllShadersAndCreateMaterials();
//
//     Shared<Material> GetUnlitMaterial();
//     Shared<Material> GetLitMaterial();
//
//     const char* mMaterialNameList[2] = {
//         "Unlit",
//         "Lit"
//     };
//
//     const std::vector<Shared<Material>> GetAllMaterials() { return mAllMaterials; }
//
// private:
//     static Shared<MaterialManager> mInstance;
//
//     Shared<Shader> mUnlitShader;
//     Shared<Shader> mLitShader;
//     Shared<Shader> mHdrLitShader;
//     Shared<Shader> mBatchLitShader;
//
//     Shared<Material> mUnlitMat;
//     Shared<Material> mLitMat;
//     Shared<Material> mHdrLitMat;
//     Shared<Material> mBatchLitMat;
//
//     std::vector<Shared<Material>> mAllMaterials;
// };
// }
