#pragma once

#include "ManagedObject.h"
#include "../TTCore/Core.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace myCLI {


public ref class NCFileInfo : public ManagedObject<TTCore::NCFileInfo> {
public:
    NCFileInfo(String^ filePath);

    bool checkFileValid(String^% fileValidInfo);
    int getZLvDimLenName(String^% zLvDimName);

    bool checkIsWrfoutFile(String^% exceptionInfo);

    List<String^>^ getVarsName();
    void getVorDimsName(String^ vorVarName, List<String^>^ vorDimList);

};


}