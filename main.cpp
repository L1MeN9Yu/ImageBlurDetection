#include <iostream>
#include <boost/asio.hpp>
#include "NetworkService/NetworkService.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace boost::asio;

static string const videoURL = "http://7xqdbz.com1.z0.glb.clouddn.com/06DA9F7B-64AB-4D92-A99E-8828E4B02A09.mp4";
static string const avInfo = "?avinfo";
static string const frameInfo = "?vframe/jpg/offset/";

void print(boost::property_tree::ptree const &pt)
{
    boost::property_tree::ptree format;
    format = pt.get_child("format");
    string duration = format.get<string>("duration");
    cout << "duration = " << duration << endl;
    int durationInt = std::stoi(duration);
    for (unsigned int j = 0; j < durationInt; ++j)
    {
        string number = to_string(j);
        string imageURL;
        imageURL += videoURL;
        imageURL += frameInfo;
        imageURL += number;
        string response = NetworkService::GetRequest(imageURL);
    }
}

int main()
{
    string avInfoURL;
    avInfoURL += videoURL;
    avInfoURL += avInfo;
    cout << "avInfoURL = " << avInfoURL << endl;
    string response = NetworkService::GetRequest(avInfoURL);
//    std::cout << string << std::endl;

    std::stringstream ss;
    // send your JSON above to the parser below, but populate ss first

    ss.str(response);

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    print(pt);
    return 0;
}

