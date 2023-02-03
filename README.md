# LabCD

https://user-images.githubusercontent.com/71769312/208237021-7da8c1c2-0d9a-4dbd-9f7d-a41b6dc6202a.mp4

遥感变化检测标注工具，设计参考[EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg)，可以分别在左右两张画布中标注两个时段影像的变化信息，将自动同步在另一画布进行显示，[快捷键](https://github.com/geoyee/LabCD/wiki/%E5%BF%AB%E6%8D%B7%E9%94%AE%E5%88%97%E8%A1%A8)更方便。

**下载：[Windows x64 exe](https://github.com/geoyee/LabCD/releases/download/0.1beta/LabCD-0.1beta-setup.exe)**

## 环境及依赖

- Visual Studio 2019

- Qt 6.4.0

- OpenCV 4.5.5

- JsonCpp 1.9.5

- ~~GDAL 3.5.3~~

## 文件组织

```
testimgs
   ├-- A					# 用于存放时段一的图像
   |   └-- 01.jpg				# 时段一图像
   ├-- B					# 用于存放时段二的图像
   |   └-- 01.jpg				# 时段二图像
   ├-- GT					# 自动生成，用于存放标注结果
   |   ├-- 01_pseudo.jpg			# 伪彩色图
   |   ├-- 01.bmp			        # 单波段标注图
   |   └-- 01.json				# 自定义格式标注，用于恢复多边形
   └-- label.json	 		        # 自动生成，用于自动标签导入
```

## 使用帮助

1. 需要点击标签后才能在画布进行绘制
2. 绘制以鼠标左键开始，鼠标右键结束，最好每次绘制完一个多边形都在画布上点一下鼠标右键
3. 切换图像自动保存
4. 偶见无法绘制，暂时可以拖动一下另一个多边形的顶点，然后就可以绘制了，还没找到问题

## TODO

- [x] 国际化支持，添加英语
- [x] 添加标注进程
- [x] 添加左右画布的定位十字丝
- [x] 增加对tif的支持，保存结果带有坐标
- [x] 增加大图自动切块
- [x] 增加一个变化检测的参考图方便定位

## 参考与辅助资源

- 部分图标：[SuperMap GIS产品彩色系功能图标库](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519)
- 标签预定义颜色：[色板 | AntV](https://antv.vision/zh/docs/specification/language/palette)
- 打包：[Inno Setup](https://jrsoftware.org/)

## 说明

作为一个C++小白，该项目为个人的第一个C++项目，目前也是一个学习练习项目，因此该项目仍存在较多问题，使用体验不佳，需要慢慢研究，其中写的不好的部分如有机会也希望得到大佬们的一二指点。欢迎大佬们提issue或者pr。
