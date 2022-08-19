#pragma once

#include "ManagedObject.h"
#include "../TTCore/Core.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace myCLI {

public delegate void StepPgCallback(int pp);
public delegate void ProgressCallback(double pp);

public ref class Typhoon {
public:
    // Typhoon();
    int serialNo;
    List<Tuple<float, float>^>^ maxVorCells = gcnew List<Tuple<float, float>^>();
    List<Tuple<float, float>^>^ geoCenters = gcnew List<Tuple<float, float>^>();
    int startTimeIndex;
    int endTimeIndex;
};

public ref class TCInfo {
public:
    TCInfo(String^ timeUnits, bool time_noleap, double timeInterval, double firstTValue);

    String^ timeUnits;
    bool time_noleap;
    double timeInterval;
    /// 文件中time variable第一个数据的值
    double firstTValue;
};

public ref class TCs {
public:
    TCs(List<Typhoon^>^ tcs, TCInfo^ tcInfo);
    List<Typhoon^>^ tcs;
    TCInfo^ tcInfo;
};

public ref class NCFileInfo : public ManagedObject<TTCore::NCFileInfo> {
public:
    NCFileInfo();
    NCFileInfo(String^ filePath);
    NCFileInfo(String^ filePath, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor);
    NCFileInfo(String^ filePath, bool isWrfoutFile, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor, int zLevelIndex, int threadNum, double toGridRes, String^ tempFileDir);
    ~NCFileInfo() { if (shouldCancel != nullptr) delete shouldCancel; shouldCancel = nullptr; }
    !NCFileInfo() { if (shouldCancel != nullptr) delete shouldCancel; shouldCancel = nullptr; }

    bool checkFileValid(String^% fileValidInfo);
    int getZLvDimLenName(String^% zLvDimName);

    bool checkIsWrfoutFile(String^% exceptionInfo);

    List<String^>^ getVarsName();
    List<String^>^ getVorDimsName(String^ vorVarName);

    TCs^ startTracking(StepPgCallback^ stepPgCallback, ProgressCallback^ progressCallback, CancellationToken cancelToken);
    TCs^ copyToManaged(TTCore::TCs& inTC);

    void exportFile_json(TCs^ tcs, String^ oNcFilePath);
    void exportFile_proto3(TCs^ tcs, String^ oNcFilePath);
    void exportFile_nc(TCs^ tcs, String^ oNcFilePath, String^ fullCommand);
    void exportFile_nc_compact(TCs^ tcs, String^ oNcFilePath, String^ fullCommand);

private:
    bool *shouldCancel;
    void Canceled() { if (shouldCancel != nullptr) *shouldCancel = true; }
    TTCore::TCs toCppTCs(TCs^ tcs);
};


}