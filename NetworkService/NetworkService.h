//
// Created by LiMengyu on 16/5/13.
//

#ifndef IMAGEBLURDETECTION_NETWORKSERVICE_H
#define IMAGEBLURDETECTION_NETWORKSERVICE_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "boost/asio.hpp"

using namespace std;

class NetworkService
{
public:
/// GET请求
    static string GetRequest(char *host, char *path);

    static string GetRequest(string url);

/// POST请求
    static string PostRequest(char *host, char *path, string form);
};

#endif //IMAGEBLURDETECTION_NETWORKSERVICE_H
