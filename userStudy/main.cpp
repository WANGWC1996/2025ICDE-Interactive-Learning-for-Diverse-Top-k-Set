#include "structure/data_utility.h"
#include "structure/data_struct.h"
#include "structure/point_set.h"
#include "structure/define.h"
#include <vector>
#include <ctime>
#include <sys/time.h>
#include "UH/UH.h"
#include "GroundTruth/Groundtruth.h"
#include "RH/RH.h"
#include "ActiveRanking/ActiveRanking.h"
#include "PreferenceLearning/preferenceLearning.h"
#include "HDIA/Piecewise_exhaustion.h"
#include "SinglePass/singlePass.h"
#ifdef DEBUG
#undef DEBUG
#endif

#include <iostream>
#include <thread>
#include <map>
#include "libs/boost_1_82_0/boost/asio.hpp"
#include "libs/websocketpp/config/asio_no_tls.hpp"
#include "libs/websocketpp/server.hpp"
#include <mutex>
#include <random>
#include <sstream>

std::string generate_uuid() {
    std::random_device rd;  // 使用随机设备生成种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister随机数引擎
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t random_value1 = dis(gen);
    uint64_t random_value2 = dis(gen);

    std::stringstream ss;
    ss << std::hex << random_value1 << random_value2;
    return ss.str();
}

typedef websocketpp::server<websocketpp::config::asio> server;

// 维护每个客户端的线程和交互状态
std::map<websocketpp::connection_hdl, std::thread, std::owner_less<websocketpp::connection_hdl>> client_threads;
std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> client_info;

// 模拟客户端程序，等待多次输入
void client_program(server* s, websocketpp::connection_hdl hdl) {
    try {
        int input_count = 0;  // 跟踪输入次数
        std::string user_input;

        while (input_count < 3) {  // 假设程序需要 3 次输入
            // 提示用户输入
            s->send(hdl, "Enter value " + std::to_string(input_count + 1) + ": ", websocketpp::frame::opcode::text);

            // 阻塞等待用户输入或退出信号
            {
                std::unique_lock<std::mutex> lock(client_info[hdl].mtx);
                client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

                // 检查是否收到退出信号
                if (client_info[hdl].should_exit) {
                    std::cerr << "Thread for client is exiting." << std::endl;
                    return;  // 线程退出
                }

                user_input = client_info[hdl].user_input;  // 获取输入
                client_info[hdl].data_ready = false;  // 重置数据状态
            }

            // 处理输入
            s->send(hdl, "You entered: " + user_input, websocketpp::frame::opcode::text);
            input_count++;  // 增加输入计数
        }

        // 程序结束
        s->send(hdl, "Program completed.", websocketpp::frame::opcode::text);

        // 关闭 WebSocket 连接
        s->close(hdl, websocketpp::close::status::normal, "Program finished");

    } catch (const std::exception &e) {
        std::cerr << "Error in client program thread: " << e.what() << std::endl;
    }
}


void interaction_cpp(server* s, websocketpp::connection_hdl hdl)
{
    bool englishChinese = false;
    std::string user_name = generate_uuid();

    // 阻塞等待用户输入或退出信号
    {
        std::unique_lock<std::mutex> lock(client_info[hdl].mtx);
        client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

        // 检查是否收到退出信号
        if (client_info[hdl].should_exit) {
            std::cerr << "Thread for client is exiting." << std::endl;
            return;  // 线程退出
        }

        englishChinese = std::stoi(client_info[hdl].user_input);  // 获取输入
        client_info[hdl].data_ready = false;  // 重置数据状态
    }


    std::string file_name = "../result/Re_" + user_name + ".txt";
    std::ofstream out_cp(file_name);
    string data_name = "all_regions1000processed.txt";
    string real_data_name = "all_regions1000.txt";
    int k = 10;
    point_set *pSet = new point_set(data_name);
    point_set *realDataset = new point_set(real_data_name);

    //skyband
    vector<string> category; vector<vector<double>> bound;
    pSet->skyband_with_clusters(category, k, bound);

    //utility vector
    int dim = pSet->points[0]->dim;
    point_t *u = new point_t(dim);
    point_set *resultSet = ground_truth(pSet, u, k, category, bound);

    int numOfQuestion;
    numOfQuestion = RH(pSet, realDataset, u, k, resultSet, category, bound, 0, s, hdl, client_info, out_cp, englishChinese);
    if(numOfQuestion == 888)
        return;

    point_set *pSet1 = new point_set(data_name);
    vector<string> category1; vector<vector<double>> bound1;
    pSet1->skyband_with_clusters(category1, k, bound1);
    numOfQuestion = Piecewise_exhaustion(pSet1, realDataset, u, k, resultSet, category1, bound1, s, hdl, client_info, out_cp, englishChinese);
    if(numOfQuestion == 888)
        return;

    point_set *pSet2 = new point_set(data_name);
    vector<string> category2; vector<vector<double>> bound2;
    pSet2->skyband_with_clusters(category2, k, bound2);
    numOfQuestion = Piecewise_exhaustion_diveristy_results(pSet2, realDataset, u, k, resultSet, category2, s, hdl, client_info, out_cp, englishChinese);
    if(numOfQuestion == 888)
        return;

    point_set *pSet3 = new point_set(data_name);
    vector<string> category3; vector<vector<double>> bound3;
    pSet3->skyband_with_clusters(category3, k, bound3);
    numOfQuestion = Piecewise_exhaustion_bound_diveristy(pSet3, realDataset, u, k, resultSet, category3, s, hdl, client_info, out_cp, englishChinese);
    if(numOfQuestion == 888)
        return;

    point_set *pSet4 = new point_set(data_name);
    vector<string> category4; vector<vector<double>> bound4;
    pSet4->skyband_with_clusters(category4, k, bound4);
    numOfQuestion = max_utility(pSet4, realDataset, u, k, resultSet, category4, bound4, 2, SIMPLEX, 0, s, hdl, client_info, out_cp, englishChinese);
    if(numOfQuestion == 888)
        return;

    // 程序结束
    s->send(hdl, "Thank you!", websocketpp::frame::opcode::text);

    // 关闭 WebSocket 连接
    s->close(hdl, websocketpp::close::status::normal, "Program finished");


    delete pSet;
}

// 当收到客户端消息时的处理函数
void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::string user_input = msg->get_payload();  // 获取用户输入

    // 存储客户端输入，并唤醒等待该输入的线程
    {
        std::lock_guard<std::mutex> lock(client_info[hdl].mtx);
        client_info[hdl].user_input = user_input;
        client_info[hdl].data_ready = true;
    }
    client_info[hdl].cv.notify_one();  // 通知等待输入的线程继续运行
}

// 当客户端连接时，启动一个新线程来执行客户端程序
void on_open(server* s, websocketpp::connection_hdl hdl) {
    std::cerr << "Client connected." << std::endl;
    client_info.emplace(std::piecewise_construct,
                        std::forward_as_tuple(hdl),  // 构造键
                        std::forward_as_tuple());   // 构造值
    client_threads[hdl] = std::thread(interaction_cpp, s, hdl);  // 启动线程运行客户端程序

    std::cerr << "Active clients: " << client_info.size() << std::endl;
}

// 当连接关闭时，清理线程和数据
void on_close(server* s, websocketpp::connection_hdl hdl) {
    std::cerr << "Client closed." << std::endl;

    // 通知线程应该退出
    {
        std::lock_guard<std::mutex> lock(client_info[hdl].mtx);
        client_info[hdl].should_exit = true;  // 设置退出标志
    }

    // 唤醒线程，确保它可以检测到 should_exit 变化
    client_info[hdl].cv.notify_all();

    // 等待线程结束
    if (client_threads[hdl].joinable()) {
        client_threads[hdl].join();
    }

    // 清理线程和客户端信息
    client_threads.erase(hdl);
    client_info.erase(hdl);

    std::cerr << "Active clients: " << client_info.size() << std::endl;
}


int main() {
    server ws_server;

    try {
        ws_server.set_access_channels(websocketpp::log::alevel::all);
        ws_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        ws_server.init_asio();
        ws_server.set_open_handler(std::bind(&on_open, &ws_server, std::placeholders::_1));
        ws_server.set_close_handler(std::bind(&on_close, &ws_server, std::placeholders::_1));
        ws_server.set_message_handler(std::bind(&on_message, &ws_server, std::placeholders::_1, std::placeholders::_2));

        // 监听端口
        ws_server.listen(9002);
        ws_server.start_accept();
        ws_server.run();

    } catch (websocketpp::exception const& e) {
        std::cerr << "WebSocket++ exception: " << e.what() << std::endl;
    } catch (boost::system::system_error const& e) {
        std::cerr << "Boost system error: " << e.what() << std::endl;
    }

    return 0;
}


