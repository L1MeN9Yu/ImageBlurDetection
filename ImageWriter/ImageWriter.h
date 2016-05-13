//
// Created by LiMengyu on 16/5/13.
//

#ifndef IMAGEBLURDETECTION_IMAGEWRITER_H
#define IMAGEBLURDETECTION_IMAGEWRITER_H

#include <iostream>
#include <fstream>

using namespace std;

class ImageWriter
{
    static void WriteImageLocal(const char *path, string imageDataString);
};


#endif //IMAGEBLURDETECTION_IMAGEWRITER_H
