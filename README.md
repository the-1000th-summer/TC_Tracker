# TC_Tracker

👉 [中文版](README_zh.md)

TC_Tracker is a tropical cyclone track identification and tracking tool. This repository includes the command-line program `TC_Tracker_CLI` as well as Windows/macOS graphical interface projects. The tool can read NetCDF meteorological data, identify tropical cyclone tracks, and export the results as `.json`, `.pb`, or `.nc` files.

## Get Prebuilt Releases

Download the archive for your system from GitHub Releases.

The Windows and Linux CLI packages already include the files required at runtime and can be run after extraction. After extracting the archive, keep `data/myMap.json` in the same directory, as the CLI uses this data file.

The macOS CLI package does not bundle dynamic libraries. Use Homebrew to install the required dependencies before running it:

```bash
brew install boost protobuf netcdf netcdf-cxx libomp
```

Then extract and run:

```bash
tar -xzf TC_Tracker_CLI-<version>-macos-arm64.tar.gz
./TC_Tracker_CLI --help
```

If macOS reports that the file was downloaded from the internet and cannot be opened, allow the app to open in System Settings.

## Basic Usage

```bash
TC_Tracker_CLI [options] input.nc output.[json|pb|nc]
```

Common options:

- `-z, --z-lv-index`: Specify the vertical level index, starting from 0.
- `-n, --var-names`: Specify variable names in the order `time,lat,lon,vorticity,uwnd,vwnd`, separated by English commas.
- `-r, --to-grid-res`: Set the regridding resolution; values less than or equal to 0 disable regridding.
- `--thread`: Set the number of tracking threads; `0` uses the maximum number of threads.
- `-t, --no-temp-files`: Do not output intermediate files.
- `-p, --temp-files-dir`: Set the directory for intermediate files.
- `-c, --compact-nc-file`: Output a more compact NetCDF file.

Example:

```bash
TC_Tracker_CLI -z 0 --thread 0 input.nc result.json
```

For WRF output files, the program automatically uses variable names such as `XTIME`, `XLAT`, `XLONG`, `U`, and `V`. Other NetCDF files usually need variable names specified with `-n`.

## Build from Source

The CLI source code is located in `code/projects/TC_Tracker_CLI`. You can refer to the release workflow to build it from source.

## How to Cite

If you use this software in your work, please cite the associated paper:

https://doi.org/10.3389/fenvs.2022.1046890

## Software Copyright

The following software copyright registrations are protected in China. The official registered titles are kept in Chinese:

1. 热带气旋路径识别软件（命令行界面版）V1.0（软著登字第 10410070 号）
2. 热带气旋路径识别软件（Windows 版）V1.0（软著登字第 10410207 号）
3. 热带气旋路径识别软件（Mac 版）V1.0（软著登字第 10415820 号）
