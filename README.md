# TC_Tracker

👉 [English Version](README_en.md)

TC_Tracker 是一个热带气旋路径识别与追踪工具。仓库中包含命令行程序 `TC_Tracker_CLI`，以及 Windows/macOS 图形界面项目。该工具可读取 NetCDF 气象数据，识别热带气旋路径，并可输出为 `.json`、`.pb` 或 `.nc` 文件。

## 获取预编译版本

在 GitHub Releases 下载与你系统对应的压缩包：

- Windows CLI：`TC_Tracker_CLI-<版本>-windows-x64.zip`
- Linux CLI：`TC_Tracker_CLI-<版本>-linux-x64.tar.gz`
- macOS CLI：`TC_Tracker_CLI-<版本>-macos-arm64.tar.gz`
- Windows GUI：`TC_Tracker-<版本>-windows-x64.zip`
- macOS GUI：`TC_Tracker-<版本>-macos-arm64.zip`

Windows 和 Linux 的 CLI 包已经包含运行所需文件，解压后即可运行。解压后请保留同目录下的 `data/myMap.json`，CLI 会使用该数据文件。

## 安装 CLI

### Windows

解压 `TC_Tracker_CLI-<版本>-windows-x64.zip`，进入解压目录后运行：

```powershell
.\TC_Tracker_CLI.exe --help
```

### Linux

解压并运行：

```bash
tar -xzf TC_Tracker_CLI-<版本>-linux-x64.tar.gz
cd TC_Tracker_CLI-<版本>-linux-x64
./TC_Tracker_CLI --help
```

### macOS arm64

macOS CLI 包不内置 Homebrew 动态库，运行前需要先安装依赖库：

```bash
brew install boost protobuf netcdf netcdf-cxx libomp
```

然后解压并运行：

```bash
tar -xzf TC_Tracker_CLI-<版本>-macos-arm64.tar.gz
./TC_Tracker_CLI --help
```

如果系统提示文件来自互联网而无法打开，需在系统设置中允许打开该应用。

## 基本用法

```bash
TC_Tracker_CLI [选项] input.nc output.[json|pb|nc]
```

常用选项：

- `-z, --z-lv-index`：指定垂直层索引，从 0 开始。
- `-n, --var-names`：指定变量名，顺序为 `time,lat,lon,vorticity,uwnd,vwnd`，用英文逗号分隔。
- `-r, --to-grid-res`：设置重网格分辨率；小于等于 0 时不重网格。
- `--thread`：设置追踪线程数，`0` 表示使用最大线程数。
- `-t, --no-temp-files`：不输出中间文件。
- `-p, --temp-files-dir`：设置中间文件目录。
- `-c, --compact-nc-file`：输出更紧凑的 NetCDF 文件。

示例：

```bash
TC_Tracker_CLI -z 0 --thread 0 input.nc result.json
```

对于 WRF 输出文件，程序会自动使用 `XTIME`、`XLAT`、`XLONG`、`U`、`V` 等变量名；其他 NetCDF 文件通常需要通过 `-n` 指定变量名。

## 从源码构建

CLI 源码位于 `code/projects/TC_Tracker_CLI`。可参考 release workflow 从源码构建。
