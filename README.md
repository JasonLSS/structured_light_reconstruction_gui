
# structured light reconstruction

Single line structured light reconstruction on QT

## User guide

1. https://github.com/opencv/opencv/releases/ 下source，如果不想自己编译就去官网下exe，https://opencv.org/releases/，我这里下的是4.10.0
2. https://visualstudio.microsoft.com/zh-hans/visual-cpp-build-tools/ 装vs的22的依赖,使用c++的依赖开发勾上，再去单个组件，搜索msbuild，打钩安装所有的
3. https://cmake.org/download/ 去下cmake

4~5为自己编译的步骤：
4. 打开cmake-gui，选择刚下的那个source解压出来的opencv-4.10.0 文件夹，选择编译工具为22，然后build文件夹为 opencv-4.10.0/build。点configure等一会儿,勾上world，然后generate（后续参照https://blog.csdn.net/sinat_41117967/article/details/140087763）
5. 生成方式略有不同如下，在cmd下搞如下命令：
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ALL_BUILD.vcxproj INSTALL.vcxproj

6. 添加D:\opencv\opencv\build\x64\vc16\bin到环境变量，具体路径自己改
7. 添加C:\Qt\6.7.2\mingw_64\bin到环境变量，具体路径自己改
8. 重启电脑
9. 项目配置使用msvc2019编译，jom.exe in D:\structured_light_reconstruction_gui\build\Desktop_Qt_6_7_2_MSVC2019_64bit-Debug，如果没该选项，请把qt重装勾上这个



# Versions

## v2.0

- Complete all mode, add point show

- Modify structured light acquisition process

- [BUG] first collect maybe slow

- [BUG] some file type may lead unexpected error

## v1.5

- complete all projector calibration test

- add stop scan button

## v1.4

- Add scan mode

## v1.3

- change algorithm of projector calibration

## v1.2

- change UI

- add warning message

## v1.1

- add calibration parameter save

- add projector calibration

## v1.0

- First commit.
