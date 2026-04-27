
目前由于在C++/CLI项目中使用vcpkg安装abseil库后编译会出现LNK2005: already defined...的问题，暂且将Windows GUI项目的输出protobuf文件的功能置为无效。

## 备忘
TCsP.proto加入Windows GUI项目时，右键proto文件->Properties，Item type设置为Custom Build Tool，随后界面会出现Custom Build Tool，在Command Line中填入
``` 
$(VcpkgManifestRoot)\vcpkg_installed\$(VcpkgTriplet)\$(VcpkgTriplet)\tools\protobuf\protoc.exe --proto_path=$(SolutionDir)..\core --cpp_out=$(ProjectDir) TCsP.proto
```

Outputs中填入
``` 
$(ProjectDir)%(Filename).pb.h
$(ProjectDir)%(Filename).pb.cc 
```

这使vcpkg安装的protobuf能自动编译此proto文件。

上述步骤完成后，不管能否编译成功，先编译一次，生成的`.cc`文件需要通过`Add->Existing Item`放入Solution Explorer中，这样才能让Visual Studio找到编译出的`.cc`文件。
