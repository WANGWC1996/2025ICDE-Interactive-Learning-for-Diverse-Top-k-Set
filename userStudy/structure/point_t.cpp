#include <operation.h>
#include "point_t.h"

/**
 * @brief Constructor
 * @param dim   The number of dimensions
 */
point_t::point_t(int d)
{
    this->dim = d;
    attr = new double[dim];
    this->id = -1;
    this->index = -1;
    this->value = 0;
    this->topk = 0;
}

/**
 * @brief Constructor
 * @param d_order   The number of ordered dimensions
 * @param d_unorder The number of unordered dimensions
 * @param id        ID
 */
point_t::point_t(int d, int id)
{
    this->dim = d;
    attr = new double[dim];
    this->id = id;
    this->index = -1;
    this->value = 0;
    this->topk = 0;
}

/**
 * @brief Constructor
 * @param p   The point
 */
point_t::point_t(point_t *p)
{
    dim = p->dim;
    id = p->id;
    attr = new double[dim];
    for(int i = 0;i < dim; ++i)
        attr[i] = p->attr[i];
    cateAttr = p->cateAttr;
    index = p->index;
    value = p->value;
    this->topk = p->topk;
}

/*
 * @brief Constructor
 * @param p1   point
 */
point_t::point_t(point_t *p1, point_t *p2)
{
    dim = p1->dim;
    attr = new double[dim];
    for(int i = 0; i < dim; ++i)
        attr[i] = p1->attr[i] - p2->attr[i];
    cateAttr = p1->cateAttr;
    id = -1;
    index = -1;
    this->value = 0;
    this->topk = 0;
}

/*
 * @brief Destructor
 *        delete the memory of the array
 */
point_t::~point_t()
{
    delete []attr;
}

/*
 * @brief For debug purpose, print the coordinates for a given point
 */
void point_t::print()
{
    std::cout<< id <<"  ";
    for (int i = 0; i < dim; i++)
        std::cout<< attr[i] << "  ";
    std::cout << cateAttr << "  " << place << "  " << topk << "\n";

}

/**
 * @brief       Check whether the point is the same as p
 * @param p     Point
 * @return      1 same
 *              0 different
 */
bool point_t::is_same(point_t *p)
{
    //if(cateAttr != p->cateAttr)
    //    return false;
    if(dim != p->dim)
        return false;
    for (int i = 0; i < dim; ++i)
    {
        if (attr[i] - p->attr[i] < -EQN2 || attr[i] - p->attr[i] > EQN2)
            return false;
    }
    return true;
}

/**
 * @brief   Check whether all the attribute values are 0
 * @return  -true   all attributes values are 0
 *          -false  there exists attribute value which is not 0
 */
bool point_t::is_zero()
{
    for(int i = 0; i < dim; ++i)
    {
        if(attr[i] < -EQN2 || attr[i] > EQN2)
            return false;
    }
    return true;
}

/**
 * @brief	    Calculate the dot product between two points
 * @param p     One point
 */
double point_t::dot_product(point_t *p)
{
    double result = 0;
    for(int i = 0; i < dim; i++)
    {
        result += attr[i] * p->attr[i];
    }
    return result;
}

/**
 * @brief	    Calculate the dot product between two points
 * @param v     One array
 */
double point_t::dot_product(double *v)
{
    double result = 0;
    for(int i = 0; i < dim; i++)
    {
        result += attr[i] * v[i];
    }
    return result;
}

/**
 * @brief	Calculate the subtraction between two points.
 *          Remember to release the returned point to save memory.
 * @param p The subtractor
 * @return  The subtraction(new points)
 */
point_t *point_t::sub(point_t *p)
{
    point_t* result = new point_t(dim);
    for(int i = 0; i < dim; i++)
    {
        result->attr[i] = attr[i] - p->attr[i];
    }
    return result;
}

/**
 * @brief	Calculate the addition between two points.
 *          Remember to release the returned point to save memory.
 * @param p The point
 * @return  The addition(new points)
 */
point_t *point_t::add(point_t *p)
{
    point_t* result = new point_t(dim);
    for(int i = 0; i < dim; i++)
    {
        result->attr[i] = attr[i] + p->attr[i];
    }
    return result;
}

/**
 * @brief	Scale the point
 *          Remember to release the returned point to save memory.
 * @param c The scaled coefficient
 * @return  The scaled point
 */
point_t *point_t::scale(double c)
{
    point_t* result = new point_t(dim);
    for(int i = 0; i < dim; i++)
    {
        result->attr[i] = attr[i] * c;
    }
    return result;
}

/**
 * @brief Calculate the length of a vector
 * @return The length
 */
double point_t::cal_len()
{
    double diff = 0;
    for(int i = 0; i < dim; i++)
    {
        diff += (double) pow(attr[i], 2);
    }
    return sqrt(diff);
}


/**
 * @brief       Calculate the distance between two points
 * @param p     The points
 * @return      The distance
 */
double point_t::distance(point_t *p)
{
    double diff = 0;
    for(int i = 0; i < dim; i++)
    {
        diff += (double) pow(attr[i]/100 - p->attr[i]/100, 2);
    }
    return 100 * sqrt(diff);
}

/**
 * @brief       Calculate the distance between a points and a vector
 * @param p     The vector
 * @return      The distance
 */
double point_t::distance(double *p)
{
    double diff = 0;
    for(int i = 0; i < dim; i++)
    {
        diff += (double) pow(attr[i] - p[i], 2);
    }
    return sqrt(diff);
}

/**
 * @brief Print the result of the algorithm
 * @param out_cp    The name of the output file
 * @param name      The name of the algorithm
 * @param Qcount    The number of question asked
 * @param t1        The start time
 */
void point_t::printResult(char *name, int Qcount, timeval t1)
{
    timeval t2;
    std::ofstream out_cp("../../result.txt");
    gettimeofday(&t2, 0);
    double time_cost = (double) t2.tv_sec + (double) t2.tv_usec / 1000000 - (double) t1.tv_sec - (double) t1.tv_usec / 1000000;
    std::cout << "-----------------------------------------------------------------\n";
    printf("|%15s |%15d |%15lf |%10d |\n", name, Qcount, time_cost, id);
    std::cout << "-----------------------------------------------------------------\n";
    out_cp << Qcount << "       " << time_cost << "\n";
    out_cp.close();
}


/**
 * @brief Print the result of the algorithm
 * @param out_cp    The name of the output file
 * @param name      The name of the algorithm
 * @param Qcount    The number of question asked
 * @param t1        The start time
 * @param preTime   The preprocessing time cost
 */
void point_t::printResult(char *name, int Qcount, timeval t1, double preTime, long mem_baseline)
{
    timeval t2; gettimeofday(&t2, 0);
    double time_cost = (double) t2.tv_sec + (double) t2.tv_usec / 1000000 - (double) t1.tv_sec - (double) t1.tv_usec / 1000000;
    std::cout << "-----------------------------------------------------------------------------------\n";
    //printf("|%15s |%15d |%15lf |%15lf |%15d |%10d |\n", name, Qcount, preTime, time_cost - preTime, get_mem_usage() - mem_baseline, id);
    printf("|%15s |%15d |%15lf |%15lf |%10d |\n", name, Qcount, preTime, time_cost - preTime, id);
    std::cout << "-----------------------------------------------------------------------------------\n";
    //out_cp << Qcount << "       " << preTime << "       " << time_cost - preTime << "      " << get_mem_usage() - mem_baseline << "\n";
}


/**
 * @brief calculate the place where the order of the two points change
 * @param p  The second point
 * @return
 */
double point_t::bound(point_t *p, double x)
{
    double sum = 0;
    sum =  (this->attr[0] * this->attr[0]) - (p->attr[0] * p->attr[0])
             + (this->attr[1] * this->attr[1]) - (p->attr[1] * p->attr[1]);
    sum = sum / 2 - x * (this->attr[0] - p->attr[0]);
    sum = sum /  (this->attr[1] - p->attr[1]);
    return sum;
}

/**
 * @brief Check whether the point dominates p
 * @param p     The point p
 * @return
 */
bool point_t::dominate(point_t *p)
{
    int dnum = 0;
    for (int i = 0; i < dim; i++)
    {
        if (attr[i] < p->attr[i])
            return false;
        if (attr[i] == p->attr[i])
        {
            dnum++;
        }
    }
    if(dnum >= dim)
    {
        return false;
    }
    return true;
}

/**
 * @brief Check whether the location of two points has already changed
 * @param p Point 2
 * @return  true it changes
 *          false it does not change
 */
bool point_t::is_changed(point_t *p)
{
    for (int i = 0; i < surpass.size(); ++i)
    {
        if (surpass[i]->id == p->id)
            return true;
    }

    for (int i = 0; i < p->surpass.size(); ++i)
    {
        if (p->surpass[i]->id == id)
            return true;
    }
    return false;
}

/**
 * @brief Count the number of points which has surpassed the point
 * @param ut
 * @return
 */
int point_t::countPassed(double ut)
{
    int count = 0;
    for(int i = 0; i < surpassVector.size(); ++i)
    {
        if(surpassVector[i] <= ut)
            count++;
    }
    return count;
}

/**
 * @brief Check whether each p[i] >= 0
 * @return  1 Each dimension is positive
 *          -1 Exist dimension which is negative
 */
bool point_t::is_positive()
{
    for(int i = 0; i < dim; ++i)
    {
        if(attr[i] < 0)
            return false;
    }
    return true;
}


std::string center(const std::string& str, int width)
{
    int padding = width - str.length();
    // cout << str.length() << " " << width << " " << padding << endl;
    if (padding <= 0) return str;  // 如果字符串已经超过宽度，直接返回

    int pad_left = padding / 2;
    int pad_right = padding - pad_left;

    return std::string(pad_left, ' ') + str + std::string(pad_right, ' ');
}

int interaction_compare_org(point_t *p1, point_t *p2, server* s, websocketpp::connection_hdl hdl,
                                 std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
                                 std::ofstream &out_cp)
{
    std::string user_input;
    int option = 0;

    std::string present;
    present += "Reference Information: Mileage 1000-595k Power 41-612 Price 15k-12100k\n";
    present += "参考信息：行驶里程 1000-595k 马力 41-612 价格 15k-12100k\n\n";
    present += "Please choose the car you favor more. \n请选择您更加喜欢的一辆车\n";
    present += "------------------------------------------------------------------------\n";
    // 使用 stringstream 来格式化字符串
    std::stringstream ss1, ss2, ss3;
    ss1 << "|" << center("Car 汽车", 13)
        << "|" << center("Mileage 已行驶里程", 25)
        << "|" << center("Power 马力", 22)
        << "|" << center("Price 价格",22) << "|\n";
    present += ss1.str();
    present += "------------------------------------------------------------------------\n";
    // point p1
    ss2 << "|" << center("1", 10);
    ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                            (p1->attr[0]/1000) << "k").str(), 19);

    ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                            p1->attr[1]).str(), 19);

    ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                            (p1->attr[2]/10000) << "k").str(), 19);
    present += ss2.str();
    present += "|\n------------------------------------------------------------------------\n";
    // point p2
    ss3 << "|" << center("2", 10);
    ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p2->attr[0]/1000) << "k").str(), 19);

    ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              p2->attr[1]).str(), 19);

    ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p2->attr[2]/10000) << "k").str(), 19);present += ss3.str();
    present += "|\n------------------------------------------------------------------------\n";
    present += "Please enter your choice (1 or 2) in the following box.\n请在下方输入框内输入您更喜欢的车辆编号(1 或 2)\n";

    s->send(hdl, present, websocketpp::frame::opcode::text);

    // 阻塞等待用户输入或退出信号
    {
        std::unique_lock<std::mutex> lock(client_info[hdl].mtx);
        client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

        // 检查是否收到退出信号
        if (client_info[hdl].should_exit) {
            std::cerr << "Thread for client is exiting." << std::endl;
            return 888;  // 线程退出
        }

        user_input = client_info[hdl].user_input;  // 获取输入
        option = std::stoi(user_input);
        while (option != 1 && option != 2)
        {
            client_info[hdl].data_ready = false;
            client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

            // 检查是否收到退出信号
            if (client_info[hdl].should_exit) {
                std::cerr << "Thread for client is exiting." << std::endl;
                return 888;  // 线程退出
            }
            user_input = client_info[hdl].user_input;  // 获取输入
            option = std::stoi(user_input);
        }
        client_info[hdl].data_ready = false;  // 重置数据状态
    }

    out_cp << present;
    out_cp << "choice: " << option << "\n\n";

    return option;
}


int point_t::interaction_compare(point_t *p1, point_t *p2, server* s, websocketpp::connection_hdl hdl,
                            std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
                            std::ofstream &out_cp, bool EnglishChinese)
{
    std::string user_input;
    int option = 0;
    std::string present;

    present += "Please choose the car you favor more.\n";
    present += "------------------------------------------------------------------------\n";
    // 使用 stringstream 来格式化字符串
    std::stringstream ss4, ss5, ss6;
    ss4 << "|" << center("Car", 10)
        << "|" << center("Mileage", 19)
        << "|" << center("Power", 19)
        << "|" << center("Price", 19) << "|\n";
    present += ss4.str();
    present += "------------------------------------------------------------------------\n";
    // point p1
    ss5 << "|" << center("1", 10);
    ss5 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p1->attr[0] / 1000) << "k").str(), 19);

    ss5 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              p1->attr[1]).str(), 19);

    ss5 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p1->attr[2] / 10000) << "k").str(), 19);
    present += ss5.str();
    present += "|\n------------------------------------------------------------------------\n";
    // point p2
    ss6 << "|" << center("2", 10);
    ss6 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p2->attr[0] / 1000) << "k").str(), 19);

    ss6 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              p2->attr[1]).str(), 19);

    ss6 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                              (p2->attr[2] / 10000) << "k").str(), 19);
    present += ss6.str();
    present += "|\n------------------------------------------------------------------------\n";
    present += "Please click the button to choose.\n";

    if (EnglishChinese == 0)
    {
        s->send(hdl, present, websocketpp::frame::opcode::text);
    }
    else
    {
        std::string presentC;
        presentC += "请选择您更加喜欢的一辆车\n";
        presentC += "------------------------------------------------------------------------\n";
        // 使用 stringstream 来格式化字符串
        std::stringstream ss1, ss2, ss3;
        ss1 << "|" << center("汽车", 13)
            << "|" << center("已行驶里程", 25)
            << "|" << center("马力", 22)
            << "|" << center("价格", 22) << "|\n";
        presentC += ss1.str();
        presentC += "------------------------------------------------------------------------\n";
        // point p1
        ss2 << "|" << center("1", 10);
        ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  (p1->attr[0] / 1000) << "k").str(), 19);

        ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  p1->attr[1]).str(), 19);

        ss2 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  (p1->attr[2] / 10000) << "k").str(), 19);
        presentC += ss2.str();
        presentC += "|\n------------------------------------------------------------------------\n";
        // point p2
        ss3 << "|" << center("2", 10);
        ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  (p2->attr[0] / 1000) << "k").str(), 19);

        ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  p2->attr[1]).str(), 19);

        ss3 << "|" << center((std::stringstream() << std::fixed << std::setprecision(1) <<
                                                  (p2->attr[2] / 10000) << "k").str(), 19);
        presentC += ss3.str();
        presentC += "|\n------------------------------------------------------------------------\n";
        presentC += "请根据您的选择按按钮\n";
        s->send(hdl, presentC, websocketpp::frame::opcode::text);
    }


    // 阻塞等待用户输入或退出信号
    {
        std::unique_lock<std::mutex> lock(client_info[hdl].mtx);
        client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

        // 检查是否收到退出信号
        if (client_info[hdl].should_exit) {
            std::cerr << "Thread for client is exiting." << std::endl;
            return 888;  // 线程退出
        }

        user_input = client_info[hdl].user_input;  // 获取输入
        option = std::stoi(user_input);
        while (option != 1 && option != 2)
        {
            client_info[hdl].data_ready = false;
            client_info[hdl].cv.wait(lock, [&]() { return client_info[hdl].data_ready || client_info[hdl].should_exit; });  // 等待输入或退出信号

            // 检查是否收到退出信号
            if (client_info[hdl].should_exit) {
                std::cerr << "Thread for client is exiting." << std::endl;
                return 888;  // 线程退出
            }
            user_input = client_info[hdl].user_input;  // 获取输入
            option = std::stoi(user_input);
        }
        client_info[hdl].data_ready = false;  // 重置数据状态
    }

    out_cp << present;
    out_cp << "choice: " << option << "\n\n";

    return option;
}










