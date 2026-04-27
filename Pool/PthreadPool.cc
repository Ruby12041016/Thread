#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <vector>
#define PTHREAD_NUM 10
#define JOB_NUM 10

// 添加输出互斥锁，避免输出混乱
std::mutex output_mutex;

class Matrix {
   private:
    std::vector<std::vector<int>> data;
    int rows;
    int cols;

   public:
    Matrix(int r, int c) : rows(r), cols(c), data(r, std::vector<int>(c, 0)) {};
    Matrix(const std::vector<std::vector<int>>& data) : data(data) {
        rows = data.size();
        cols = rows ? data[0].size() : 0;
    }
    int getrow() const { return rows; }
    int getcol() const { return cols; }
    int& at(int i, int j) { return data[i][j]; }
    const int& at(int i, int j) const { return data[i][j]; }
    bool mulitable(const Matrix& b) {
        if (cols == b.getrow()) {
            return true;
        }
        return false;
    }
    Matrix getrand(int row, int col) {
        Matrix result(row, col);
        std::random_device rd;                        // 硬件随机数种子
        std::mt19937 gen(rd());                       // Mersenne Twister 生成器
        std::uniform_int_distribution<> dis(1, 100);  // 1-100的均匀分布
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                result.at(i, j) = dis(gen);
            }
        }
        return result;
    }
    Matrix mulit(const Matrix& b) {
        Matrix result(rows, b.getcol());
        if (!mulitable(b)) {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "维度不匹配无法相乘" << std::endl;
            return Matrix(0, 0);
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < b.getcol(); j++) {
                int sum = 0;
                for (int k = 0; k < cols; k++) {
                    sum += at(i, k) * b.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "结果";
            result.show();
        }
        return result;
    }
    void show() {
        std::cout << "矩阵：" << std::endl;
        for (auto rows : data) {
            for (auto val : rows) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};
class PthreadPool {
   private:
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> works;
    std::mutex mutex;
    std::condition_variable con;
    bool stop = false;
    int finish_num = 0;
    int jobs_num = 0;
    std::function<void()> func;
    void do_job() {
       // std::this_thread::sleep_for(std::chrono::seconds(1));
        while (!stop) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                // 等待任务或停止信号
                con.wait(lock, [this]() { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) {
                    return;
                }
                if (!tasks.empty()) {
                    task = tasks.front();
                    tasks.pop();
                } else {
                    continue;
                }
            }
            // 执行任务
            try {
                if (task) {
                    task();
                }
            } catch (const std::exception& e) {
                std::cerr << "任务执行异常: " << e.what() << std::endl;
            }
            {
                std::unique_lock<std::mutex> lock(mutex);
                finish_num++;
                {
                    std::lock_guard<std::mutex> output_lock(output_mutex);
                    std::cout << "当前完成 " << finish_num << " 个任务"
                              << std::endl
                              << std::endl;
                }
            }
        }
    }

   public:
    PthreadPool(int num = PTHREAD_NUM) {
        for (int i = 0; i < num; i++) {
            works.emplace_back([this] { do_job(); });
        }
        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "创建了 " << num << " 个线程" << std::endl;
            std::cout << "----------------- " << std::endl;
        }
    }
    void add_job(std::function<void()>& func) {
        std::unique_lock<std::mutex> lock(mutex);
        if (stop) {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "线程池已退出" << std::endl;
            return;
        }
        tasks.push(func);
        jobs_num++;
        // 通知一个线程来执行任务
        con.notify_one();
    }
    void close() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        con.notify_all();
        for (auto& worker : works) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "线程池已关闭，共完成 " << finish_num << " 个任务"
                      << std::endl;
        }
    }
    void wait() { std::this_thread::sleep_for(std::chrono::seconds(10)); }
};

int main() {
    PthreadPool pool{};
    std::vector<std::function<void()>> jobs;
    for (int i = 0; i < JOB_NUM; i++) {
        int job_id = i + 1;
        auto job = [job_id]() {
            Matrix a(0, 0);
            Matrix mat_a = a.getrand(2, 3);
            {
                std::lock_guard<std::mutex> output_lock(output_mutex);
                std::cout << "任务 " << job_id << " 生成矩阵A：" << std::endl;
                mat_a.show();
            }
            Matrix b(0, 0);
            Matrix mat_b = b.getrand(3, 2);
            {
                std::lock_guard<std::mutex> output_lock(output_mutex);
                std::cout << "任务 " << job_id << " 生成矩阵B：" << std::endl;
                mat_b.show();
            }
            // 执行矩阵乘法
            Matrix result = mat_a.mulit(mat_b);
            {
                std::lock_guard<std::mutex> output_lock(output_mutex);
                std::cout << "任务 " << job_id << " 执行完成" << std::endl;
            }
        };
        jobs.push_back(job);
    }
    for (auto& job : jobs) {
        pool.add_job(job);
    }
    pool.wait();
    {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "程序结束" << std::endl;
    }
    return 0;
}