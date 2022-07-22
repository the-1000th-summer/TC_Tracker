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
    NCFileInfo(String^ filePath, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor);
    NCFileInfo(String^ filePath, bool isWrfoutFile, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor, int zLevelIndex, double toGridRes, String^ tempFileDir);

    bool checkFileValid(String^% fileValidInfo);
    int getZLvDimLenName(String^% zLvDimName);

    bool checkIsWrfoutFile(String^% exceptionInfo);

    List<String^>^ getVarsName();
    List<String^>^ getVorDimsName(String^ vorVarName);

};


}