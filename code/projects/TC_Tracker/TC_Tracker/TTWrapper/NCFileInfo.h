#pragma once

#include "ManagedObject.h"
#include "../TTCore/Core.h"

namespace myCLI {


public ref class NCFileInfo : public ManagedObject<TTCore::NCFileInfo> {
public:
    NCFileInfo(String^ filePath);

    void echoFilePath();
}

};