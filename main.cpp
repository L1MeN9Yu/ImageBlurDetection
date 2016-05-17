#include <iostream>
#include <boost/asio.hpp>
#include "NetworkService/NetworkService.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "ImageWriter/ImageWriter.h"
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost::asio;

static string const videoURL = "http://7xqdbz.com1.z0.glb.clouddn.com/06DA9F7B-64AB-4D92-A99E-8828E4B02A09.mp4";
static string const avInfo = "?avinfo";
static string const frameInfo = "?vframe/jpg/offset/";
static string const tempDirectory = "./temp/";

void print(boost::property_tree::ptree const &pt)
{
    boost::property_tree::ptree format;
    format = pt.get_child("format");
    string duration = format.get<string>("duration");
    int durationInt = std::stoi(duration);
    //todo 目录创建
    boost::filesystem::path tempPath(tempDirectory);
    if (boost::filesystem::exists(tempPath))
    {
        if (boost::filesystem::is_directory(tempPath))
        {
            printf("directory exists \n");
        }
        else
        {
            boost::filesystem::create_directory(tempPath);
        }
    }
    else
    {
        boost::filesystem::create_directory(tempPath);
    }

    //todo 时间间隔截图逻辑
    for (unsigned int j = 0; j < durationInt; ++j)
    {
        string number = to_string(j);
        string imageURL;
        imageURL += videoURL;
        imageURL += frameInfo;
        imageURL += number;
        string response = NetworkService::GetRequest(imageURL);
        string path;
        path += tempDirectory;
        path += number;
        path += ".jpg";
        ImageWriter::WriteImageLocal(path, response);
    }
}

void imageBlurDetection()
{
    boost::filesystem::path path(tempDirectory);
    try
    {
        if (boost::filesystem::exists(path))
        {
            if (boost::filesystem::is_directory(path))
            {
                for (boost::filesystem::directory_entry &x : boost::filesystem::directory_iterator(path))
                {
                    string imagePathString = x.path().string();
                    cv::Mat cvImage = cv::imread(imagePathString);

                    //如果读入图像失败
                    if (cvImage.empty())
                    {
                        continue;
                    }
                    else
                    {
                        printf("cvImage is OK!\n");
                        cv::Mat grayCVImage;
                        cv::cvtColor(cvImage, grayCVImage, cv::COLOR_BGR2GRAY);
                        if(!grayCVImage.empty())
                        {
                            printf("grayCVImage is OK!\n");
                            cv::Mat laplacianImage;
                            cv::Laplacian(grayCVImage,laplacianImage,CV_64F);
                            cv::Scalar mu, sigma;
                            cv::meanStdDev(laplacianImage, mu, sigma);

                            double focusMeasure = sigma.val[0]*sigma.val[0];

                            printf(" %s------------ %lf \n",x.path().string().c_str(),focusMeasure);
                        }
                    }
                }
            }
        }
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        cout << ex.what() << '\n' << endl;
    }

}

int main()
{
    string avInfoURL;
    avInfoURL += videoURL;
    avInfoURL += avInfo;
    cout << "avInfoURL = " << avInfoURL << endl;
    string response = NetworkService::GetRequest(avInfoURL);

    std::stringstream stringstream;
    // send your JSON above to the parser below, but populate ss first

    stringstream.str(response);

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(stringstream, pt);

    print(pt);
    imageBlurDetection();
    return 0;
}

