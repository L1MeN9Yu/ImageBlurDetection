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
#include <boost/regex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace boost::asio;

//static string const videoURL = "http://7xqdbz.com1.z0.glb.clouddn.com/06DA9F7B-64AB-4D92-A99E-8828E4B02A09.mp4";
static string const urlRegexString = "^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$";
static string const avInfo = "?avinfo";
static string const frameInfo = "?vframe/jpg/offset/";

string videoURLString;

void imageBlurDetection(string &tempDirectory)
{
    boost::property_tree::ptree array;

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
                        if (!grayCVImage.empty())
                        {
                            printf("grayCVImage is OK!\n");
                            cv::Mat laplacianImage;
                            cv::Laplacian(grayCVImage, laplacianImage, CV_64F);
                            cv::Scalar mu, sigma;
                            cv::meanStdDev(laplacianImage, mu, sigma);

                            double focusMeasure = sigma.val[0] * sigma.val[0];

                            printf(" %s ------------ %lf \n", x.path().string().c_str(), focusMeasure);

                            boost::property_tree::ptree vframeInfo;
                            vframeInfo.put("vframe", x.path().stem());
                            vframeInfo.put("value", std::to_string(focusMeasure));
                            array.push_back(std::make_pair("", vframeInfo));
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

    try
    {
        boost::filesystem::remove_all(path);
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        cout << ex.what() << '\n' << endl;
    }

    printf("videoURLString = %s", videoURLString.c_str());

    boost::property_tree::ptree postJson;
    postJson.put("video_path", videoURLString);
    postJson.add_child("result", array);

    std::stringstream buf;
    boost::property_tree::write_json(buf, postJson, false);
    std::string postJsonString = buf.str();

    printf("post json string === > %s", postJsonString.c_str());

    using boost::posix_time::ptime;
    using boost::posix_time::second_clock;
    using boost::posix_time::to_simple_string;
    using boost::gregorian::day_clock;

    ptime todayUtc(day_clock::universal_day(), second_clock::universal_time().time_of_day());
    string resultFilePath = "./";
    resultFilePath += to_simple_string(todayUtc);
    resultFilePath += ".json";

    printf("%s", resultFilePath.c_str());
    ImageWriter::WriteImageLocal(resultFilePath, postJsonString);
}

void downloadImages(boost::property_tree::ptree const &pt, string &videoURL)
{
    boost::property_tree::ptree format;
    format = pt.get_child("format");
    string duration = format.get<string>("duration");
    int durationInt = std::stoi(duration);

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    string tempDirectory = "./";
    tempDirectory += boost::uuids::to_string(uuid);
    tempDirectory += "/";

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

    imageBlurDetection(tempDirectory);
}

int main(int argc, char *argv[])
{
    printf("参数个数 = %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("参数序号=%d ", i);
        printf("参数值=%s\n", argv[i]);
    }

    if (argc != 2)
    {
        throw runtime_error("usage : ./ImageBlurDetection http://xxx.mp4");
    }

    string tempVideoURLString(argv[1]);

    videoURLString = tempVideoURLString;

    boost::regex urlRegex(urlRegexString);
    if (!boost::regex_match(videoURLString, urlRegex))
    {
        throw runtime_error("Your URL is not formatted correctly!");
    }

    string avInfoURL;
    avInfoURL += videoURLString;
    avInfoURL += avInfo;
    cout << "avInfoURL = " << avInfoURL << endl;
    string response = NetworkService::GetRequest(avInfoURL);

    std::stringstream stringstream;
    // send your JSON above to the parser below, but populate ss first

    stringstream.str(response);

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(stringstream, pt);

    downloadImages(pt, videoURLString);

    return 0;
}

