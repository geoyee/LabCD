# LabCD

简体中文 | [English](./README_EN.md)

[![version](https://img.shields.io/github/release/geoyee/LabCD.svg)](https://github.com/geoyee/LabCD/releases)  [![license](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)  ![Language](https://img.shields.io/badge/language-C++%2017-blue.svg)

https://user-images.githubusercontent.com/71769312/216638896-bd1d5e40-6512-478d-b7b0-927c142aece6.mp4

遥感变化检测标注工具，设计参考[EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg)，可以分别在左右两张画布中标注两个时段影像的变化信息，将自动同步在另一画布进行显示，[快捷键](https://github.com/geoyee/LabCD/wiki/%E5%BF%AB%E6%8D%B7%E9%94%AE%E5%88%97%E8%A1%A8)更方便。

**哔哩哔哩：[https://www.bilibili.com/video/BV11j411T7up?t=23.2](https://www.bilibili.com/video/BV11j411T7up?t=23.2)**

**下载：[Windows x64 exe](https://github.com/geoyee/LabCD/releases/download/0.3/LabCD-0.3-x64-setup.exe)**

## 更新

1. 增加第一次加载多光谱数据计算最佳波段组合，后续加载计算所得的波段
2.  增加切分图像块可以设置大小，并不会舍去不满一个块的部分
4.  增加合并标签图像块
6.  增加过滤空标签和图像和加载推理好的掩膜

## 环境及依赖

- Visual Studio 2019

- Qt 6.4.0

- OpenCV 4.5.5

- JsonCpp 1.9.5

- GDAL 3.5.3

- Eigen 3.4.0

## 文件组织

```
testimgs
   ├-- A					# 用于存放时段一的图像
   |   └-- 01.jpg				# 时段一图像
   ├-- B					# 用于存放时段二的图像
   |   └-- 01.jpg				# 时段二图像
   ├-- GT					# 自动生成，用于存放标注结果
   |   ├-- 01_pseudo.png			# 伪彩色图
   |   ├-- 01.bmp			        # 单波段标注图
   |   └-- 01.json				# 自定义格式标注，用于恢复多边形
   └-- label.json	 		        # 自动生成，用于自动标签导入
```

## 使用帮助

1. 需要点击标签后才能在画布进行绘制
2. 绘制以鼠标左键开始，鼠标右键结束（最好每次绘制完一个多边形都在画布上点一下鼠标右键）
3. 切换图像会自动保存

## 参考与辅助资源

- 部分图标：[SuperMap GIS产品彩色系功能图标库](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519)
- 标签预定义颜色：[色板 | AntV](https://antv.vision/zh/docs/specification/language/palette)
- 打包：[Inno Setup](https://jrsoftware.org/)
