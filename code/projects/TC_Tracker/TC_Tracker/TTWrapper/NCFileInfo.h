#pragma once

#include "ManagedObject.h"
#include "../TTCore/Core.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace myCLI {

public delegate void CliCallBack(int pp);
public delegate void CliCallBack2(double pp);

public ref class Typhoon {
public:
    // Typhoon();
    int serialNo;
    List<Tuple<float, float>^>^ maxVorCells = gcnew List<Tuple<float, float>^>();
    List<Tuple<float, float>^>^ geoCenters = gcnew List<Tuple<float, float>^>();
    int startTimeIndex;
    int endTimeIndex;
};

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

    void startTracking(List<Typhoon^>^ realTCs, CancellationToken cancelToken, CliCallBack^ pFun, CliCallBack2^ pFun2);

};


}