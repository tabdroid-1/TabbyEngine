// #include <Tabby/Renderer/MaterialManager.h>
//
// namespace Tabby {
//
// Shared<MaterialManager> MaterialManager::mInstance = nullptr;
//
// Shared<MaterialManager> MaterialManager::GetInstance()
// {
//     if (mInstance == nullptr)
//         mInstance = CreateShared<MaterialManager>();
//
//     return mInstance;
// }
//
// void MaterialManager::LoadAllShadersAndCreateMaterials()
// {
//     // TODO: Modify the code to load all shaders automatically
//     // TODO: Modify code to show material properties in material gui depending on shader uniform keywords
//
//     // Shaders
//     std::string shaderDir = std::filesystem::current_path().string() + "shaders/";
//     mUnlitShader = Shader::Create("UnlitShader", shaderDir + "Unlit.vert", shaderDir + "Unlit.frag");
//     mLitShader = Shader::Create("LitShader", shaderDir + "Lit.vert", shaderDir + "Lit.frag");
//     // mHdrLitShader = Shader::Create("HDRLighting", shaderDir + "HDRLighting.vert", shaderDir + "HDRLighting.frag");
//     // mBatchLitShader = Shader::Create("BatchLit", shaderDir + "BatchLit.vert", shaderDir + "BatchLit.frag");
//
//     // Materials
//     mUnlitMat = CreateShared<Material>("UnlitMaterial", mUnlitShader); // Create default material
//     mLitMat = CreateShared<Material>("LitMaterial", mLitShader);
//     // mHdrLitMat = CreateShared<Material>("UnlitMaterial", mUnlitShader);
//     // mBatchLitMat = CreateShared<Material>("BatchMaterial", mBatchLitShader);
//
//     mAllMaterials.push_back(mUnlitMat);
//     mAllMaterials.push_back(mLitMat);
// }
//
// Shared<Material> MaterialManager::GetUnlitMaterial()
// {
//     return mUnlitMat;
// }
//
// Shared<Material> MaterialManager::GetLitMaterial()
// {
//     return mLitMat;
// }
// }
