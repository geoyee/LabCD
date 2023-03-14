# LabCD

Englis | [简体中文](./README.md)

[![version](https://img.shields.io/github/release/geoyee/LabCD.svg)](https://github.com/geoyee/LabCD/releases)  [![license](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)  ![Language](https://img.shields.io/badge/language-C++%2017-blue.svg)

https://user-images.githubusercontent.com/71769312/216638896-bd1d5e40-6512-478d-b7b0-927c142aece6.mp4

Remote sensing change detection annotation tool, it was designed with reference to [EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg). It can mark the change information of images in two periods in the left and right canvases respectively, and automatically display it in the other canvas synchronously.

## News

1. Add calculation of OIF for the first time loading multi-spectral data, and subsequent loading of calculated bands.
2. Add the ability to set the size of the split image blocks, without discarding any portions that do not fill a complete block.
3. Add merging of labeled image blocks.
4. Add filtering of empty labels and images, and loading of pre-inferred masks.

## Dependences

- Qt 6.4.0

- OpenCV 4.5.5

- JsonCpp 1.9.5

- GDAL 3.5.3

- Eigen 3.4.0

## Tutorials and Documents

- [Download and usage](./docs/Download_and_usage_en.md)

- [Keyboard shortcuts](./docs/Keyboard_shortcuts_en.md)
- [Usage tutorial](./docs/Usage_tutorial_en.md)

## Resources

- Icon：[SuperMap GIS Product color system function icon library](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519&lang=en-us)
- Color：[Color Palette | AntV](https://antv.vision/en/docs/specification/language/palette)
