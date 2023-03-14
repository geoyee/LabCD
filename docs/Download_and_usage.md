# 下载和使用

## 直接使用

提供x64版本的Windows exe[下载](https://github.com/geoyee/LabCD/releases/download/0.3/LabCD-0.3-x64-setup.exe)，下载后按照流程安装后即可。打包过程由[Inno Setup](https://jrsoftware.org/)提供。

## 构建使用

1. 克隆项目到本地：

```shell
git clone https://github.com/geoyee/LabCD
```

2. 根据系统下载[Qt 6.4.0](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)版本并安装。Windows用户如需要使用Visual Studio需要下载VS扩展：Qt Visual Studio Tools。 

3. 下载所需的依赖包。目前需要的第三方包有`OpenCV 4.5.5`，`JsonCpp 1.9.5`，`GDAL 3.5.3`和`Eigen 3.4.0`。

   1. `OpenCV 4.5.5`可从官方github仓库进行[下载](https://github.com/opencv/opencv/releases/tag/4.5.5)并使用cmake进行编译。
   2. `JsonCpp 1.9.5`可从官方github仓库进行[下载](https://github.com/open-source-parsers/jsoncpp/releases/tag/1.9.5)并使用cmake进行编译。
   3. `GDAL 3.5.3`可从官方github仓库进行[下载](https://github.com/OSGeo/gdal/releases/tag/v3.5.3)并使用cmake进行编译。
   4. `Eigen 3.4.0`可从官方gitlab仓库直接进行[下载](https://gitlab.com/libeigen/eigen/-/tree/3.4.0)。

   准备好之后需要在`labcd`的文件夹下新建`depends`文件夹，其中包括`include`和`lib`文件夹。将Opencv、JsonCpp和GDAL编译完成对应的include和lib文件放入，将Eigen源代码直接放到`depends`下。完成后的`depends`文件夹结构如下：

   ```
   depends
      ├-- include
      |   ├-- gdal
      |   ├-- json
      |   └-- opencv2
      ├-- lib
      |   ├-- gdal_i.lib
      |   ├-- jsoncpp_static.lib
      |   ├-- jsoncpp_staticd.lib
      |   ├-- opencv_world455.lib
      |   └-- opencv_world455d.lib
      └-- Eigen
   ```

4. 完成后可使用Visual Studio打开对应的`sln`文件构建或其他平台使用Qt Creator打开`pro`文件，建立qmake后构建，即可使用。
5. CMakeLists由[qmake2cmake](https://www.qt.io/blog/introducing-qmake2cmake)生成，如第三方依赖的路径发生变化，无论是Visual Studio还是Qt Creator或是Cmake都需要对依赖的路径进行调整。