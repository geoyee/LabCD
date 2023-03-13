# LabCD

Englis | [简体中文](./README.md)

[![version](https://img.shields.io/github/release/geoyee/LabCD.svg)](https://github.com/geoyee/LabCD/releases)  [![license](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)  ![Language](https://img.shields.io/badge/language-C++%2017-blue.svg)

https://user-images.githubusercontent.com/71769312/216638896-bd1d5e40-6512-478d-b7b0-927c142aece6.mp4

Remote sensing change detection annotation tool, it was designed with reference to [EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg). It can mark the change information of images in two periods in the left and right canvases respectively, and automatically display it in the other canvas synchronously. It makes it easy to use [shortcut keys](https://github.com/geoyee/LabCD/wiki/%E5%BF%AB%E6%8D%B7%E9%94%AE%E5%88%97%E8%A1%A8).

**bilibili: [https://www.bilibili.com/video/BV11j411T7up?t=23.2](https://www.bilibili.com/video/BV11j411T7up?t=23.2)**

**download: [Windows x64 exe](https://github.com/geoyee/LabCD/releases/download/0.3/LabCD-0.3-x64-setup.exe)**

## News

1. Add calculation of OIF for the first time loading multi-spectral data, and subsequent loading of calculated bands.
2. Add the ability to set the size of the split image blocks, without discarding any portions that do not fill a complete block.
3. Add merging of labeled image blocks.
4. Add filtering of empty labels and images, and loading of pre-inferred masks.

## Environment and dependence

- Visual Studio 2019

- Qt 6.4.0

- OpenCV 4.5.5

- JsonCpp 1.9.5

- GDAL 3.5.3

- Eigen 3.4.0

## Data organization

```
testimgs
   ├-- A					# time1 image folder
   |   └-- 01.jpg				# time1 image
   ├-- B					# time2 image folder
   |   └-- 01.jpg				# time2 image
   ├-- GT					# automatically generated for save annotation
   |   ├-- 01_pseudo.png			# pseudo-color map
   |   ├-- 01.bmp			        # annotation with single band
   |   └-- 01.json				# custom format used to restore polygons
   └-- label.json	 		        # automatically generated for label import
```

## Help

1. You need to click the label to draw on the canvas
2. The starts with the left mouse button and ends with the right mouse button (It's best to right-click on the canvas every time you finish drawing a polygon)
3.  Automatically saved after switching images

## Reference and resources

- Icon：[SuperMap GIS产品彩色系功能图标库](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519&lang=en-us)
- Color：[Color Palette | AntV](https://antv.vision/en/docs/specification/language/palette)
- Packaging：[Inno Setup](https://jrsoftware.org/)
