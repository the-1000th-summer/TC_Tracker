#include <vector>
#include "NCFileInfo.h"

using namespace System::Collections::Generic;

namespace myCLI {
    NCFileInfo::NCFileInfo(String ^filePath) : ManagedObject(new TTCore::NCFileInfo(string_to_char_array(filePath))) {
        
    }

    void NCFileInfo::checkFileValid() {
        m_Instance->checkFileValid();
    }

    //void NCFileInfo::openFile() {
    //    m_Instance->openFile();
    //}

    List<String^>^ NCFileInfo::getVarsName() {
        std::vector<std::string> varsName;
        m_Instance->getVarsName(varsName);

        //Debug::WriteLine("varsName::"+varsName);
        //List<String> ^csVarsName = gcnew List<String>;
        //int a = ;
        List<String^> ^sd = gcnew List<String^>(varsName.size());
        //varsNam
        for (auto &f : varsName) {
            sd->Add(gcnew String(f.c_str()));
        }
        return sd;
    }
}
