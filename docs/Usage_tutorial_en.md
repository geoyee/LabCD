# User Guide

Before using the software, it is necessary to understand the organization of files. When not specified, there may only be two folders, `A` and `B`, containing images from two different periods with the same file name. The file organization is as follows:

```
testimgs
   ├-- A					# Used to store images from period one
   |   └-- 01.jpg				# Image from period one
   ├-- B					# Used to store images from period two
   |   └-- 01.jpg				# Image from period two
   ├-- GT					# Automatically generated folder used to store annotation results
   |   ├-- 01_pseudo.png			# Pseudo-colored image
   |   ├-- 01.bmp			        # Single-band annotated image
   |   └-- 01.json				# Custom format annotation file for restoring polygons
   └-- label.json	 		        # Automatically generated file used for importing auto-labels
```

## Large image split/merge

To use the "Large image split" feature under the "Files" menu, select the path of the large image and the splitting size, which can split a large remote sensing image into multiple smaller images. The names of the smaller images contain the coordinates of the upper-left corner of the image in the large image. Therefore, if you need to use the "Large image merge" operation to merge the small images in this folder, you cannot modify the names of the images.

## Clear empty mask

To use the "Clear empty mask" feature under the "Files" menu, select the folder that meets the file organization requirements. This operation will traverse the labels in `GT` and delete the corresponding two-period images, json files, and other files that have no content.

## Create Annotation from mask

To use the "Create Annotation from mask" feature under the "Files" menu, select a folder that contains only image masks. This operation will generate the corresponding json file for the masks in the folder, allowing LabCD to draw polygons for adjustment.

## Change Detection Annotation

The core function of the software is change detection annotation. To use this feature, select a folder that meets the file organization requirements under the "Files" menu, and annotation can be performed.

1. To annotate, first "Add Label" in the "Label List" and set the corresponding color and text content.
2. You can switch to the file that needs to be annotated by paging up/down or directly double-clicking the file name in the "Data List".
3. Click a label, and then annotate in the image. Left-click to create a new point, and right-click to complete the current polygon annotation.
4. After completion, switching to another image or clicking "Save" will save the results, including a mask file, a polygon json file, and a pseudo-colored image.

*It is recommended to right-click the mouse after drawing each polygon on the canvas. If left-clicking suddenly stops working, you can switch to another label and click again.*

Other features include:

- Setting the color of the crosshairs.
- Loading a small image, which is the change intensity image calculated from the two periods of images.
- Adding the function to calculate the best band combination for the first loading of multi-spectral data, and loading the calculated bands for subsequent loading.
- Deleting a specified polygon and clearing all polygons.
- Double-clicking a "point" to delete the point, and double-clicking a "line" to create a new point on the line.