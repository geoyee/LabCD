# LabCD

https://user-images.githubusercontent.com/71769312/202847705-eaa97c8b-5a1d-45dc-b6b3-a740cef743b9.mp4

遥感变化检测标注工具，设计参考[EISeg](https://github.com/PaddlePaddle/PaddleSeg/tree/release/2.6/EISeg)，通过左右同步的两张画布来方便快捷地标注两个时段影像的变化信息。

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
   |   └-- 01.bmp				# 单波段标签图
   └-- label.json	 		        # 自动生成，用于自动标签导入
```

## TODO

基础：

- [x] 完成基本界面
- [x] 基本完成数据列表功能，实现加载文件、点击跳转
- [x] 初步完成标签列表，实现标签添加、修改
- [x] 基本实现左右同步画布交互
- [x] 增加选择标签画多边形
- [x] 增加保存标注结果
- [x] 添加qmake
- [x] 增加标签的导入导出
- [ ] 完善工具栏和快捷键
- [ ] 测试和修复明显的bug
  
- [ ] 打包

增强：

- [ ] 增加项目管理功能，可以反复标注一个项目
- [ ] 添加左右画布的定位十字丝
- [ ] 增加对tif的支持，保存结果带有坐标
- [ ] 增加大图自动切块
- [ ] 翻译支持
- [ ] 增加一个变化检测的参考图方便定位

## 参考资源

- 部分图标：[SuperMap GIS产品彩色系功能图标库](https://www.iconfont.cn/collections/detail?spm=a313x.7781069.1998910419.d9df05512&cid=32519)
- 标签预定义颜色：[色板 | AntV](https://antv.vision/zh/docs/specification/language/palette)

## 说明

作为一个C++小白，该项目为个人的第一个C++项目，目前也是一个学习练习项目，因此该项目仍存在较多问题，使用体验不佳，需要慢慢研究，其中写的不好的部分如有机会也希望得到大佬们的一二指点。
