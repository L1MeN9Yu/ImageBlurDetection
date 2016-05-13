//
// Created by LiMengyu on 16/5/13.
//

#include "ImageWriter.h"

void ImageWriter::WriteImageLocal(const char *path, string imageDataString)
{
    long size = imageDataString.length();

    // 1. 定义保存图片的文件
    FILE *ofile = fopen(path, "wb");

    // 2. 再将string类型转换为char数组
    char *imgCharArray = new char[size];
    for (int i = 0; i <= size; i++)
    {
        imgCharArray[i] = imageDataString[i];
    }

    // 3. 最后将char数组输出保存为图片文件
    fwrite(imgCharArray, sizeof(char), size, ofile);

    fclose(ofile); // 关闭文件
}