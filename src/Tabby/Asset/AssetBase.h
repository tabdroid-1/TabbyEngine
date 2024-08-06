#pragma once

#include <Tabby/Asset/AssetType.h>
#include <Tabby/Foundation/Types.h>

namespace Tabby {

class AssetBase {
public:
    AssetHandle Handle;
    AssetType Type;

    virtual void Destroy() = 0;
    bool Valid() { return Handle.Valid(); }
};

}
