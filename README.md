# LabCD

简体中文 | [English](./README_EN.md)

[![version](https://img.shields.io/github/release/geoyee/LabCD.svg)](https://github.com/geoyee/LabCD/releases)  [![license](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)  ![Language](https://img.shields.io/badge/language-C++%2017-blue.svg)

https://user-images.githubusercontent.com/71769312/216638896-bd1d5e40-6512-478d-b7b0-927c142aece6.mp4

遥感变化检测标注工具，设计参考[EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg)，可以分别在左右两张画布中标注两个时段影像的变化信息，将自动同步在另一画布进行显示。

## 更新

1. 增加第一次加载多光谱数据计算最佳波段组合，后续加载计算所得的波段
2.  增加切分图像块可以设置大小，并不会舍去不满一个块的部分
4.  增加合并标签图像块
6.  增加过滤空标签和图像和加载推理好的掩膜

## 依赖

- Qt 6.4.0

- OpenCV 4.5.5

- JsonCpp 1.9.5

- GDAL 3.5.3

- Eigen 3.4.0

## 教程与文档

- [下载与使用](./docs/Download_and_usage.md)
- [快捷键](./docs/Keyboard_shortcuts.md)
- [使用教程](./docs/Usage_tutorial.md)

## 资源

- 部分图标：[SuperMap GIS产品彩色系功能图标库](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519)
- 标签预定义颜色：[色板 | AntV](https://antv.vision/zh/docs/specification/language/palette)
