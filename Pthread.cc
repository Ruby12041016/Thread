#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#define RAND_NUM 1000000

template <typename T>

class Merge {
   private:
   public:
    bool issort(std::vector<T>& arr) {
        for (T i = 0; i < arr.size() - 1; i++) {
            if (arr[i + 1] < arr[i]) {
                return false;
            }
        }
        return true;
    }
    void merge(std::vector<T>& arr, int left, int right, int mid) {
        int len1 = mid - left + 1;
        int len2 = right - mid;
        std::vector<T> leftarr(len1);
        std::vector<T> rightarr(len2);
        for (int i = 0; i < len1; i++) {
            leftarr[i] = arr[left + i];
        }
        for (int i = 0; i < len2; i++) {
            rightarr[i] = arr[mid + i + 1];
        }
        int i = 0, j = 0, k = left;
        while (i < len1 && j < len2) {
            if (leftarr[i] < rightarr[j]) {
                arr[k++] = leftarr[i++];
            } else {
                arr[k++] = rightarr[j++];
            }
        }
        while (i < len1) {
            arr[k++] = leftarr[i++];
        }
        while (j < len2) {
            arr[k++] = rightarr[j++];
        }
    }
    void mergesort(std::vector<T>& arr, int left, int right) {
        if (left >= right) {
            return;
        }
        int mid = left + (right - left) / 2;
        mergesort(arr, left, mid);
        mergesort(arr, mid + 1, right);
        merge(arr, left, right, mid);
    }
    void show(const std::vector<T>& arr) {
        for (T num : arr) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
};
// 获取逻辑cpu核心数
int cpu_n = std::thread::hardware_concurrency();
std::vector<int> getrand(int n) {
    std::vector<int> arr(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10000);
    std::generate(arr.begin(), arr.end(), [&]() { return dis(gen); });
    return arr;
}
void multithread(std::vector<int>& arr) {
    int len = arr.size();
    if (len <= 1) {
        return;
    }
    Merge<int> mysort;
    int avg = arr.size() / cpu_n;
    std::vector<std::thread> threads;
    for (int i = 0; i < cpu_n; i++) {
        int left = i * avg;
        int right = (i == cpu_n - 1) ? len - 1 : (i + 1) * avg - 1;
        threads.push_back(std::thread(&Merge<int>::mergesort, &mysort,
                                      std::ref(arr), left, right));
    }
    for (int i = 0; i < cpu_n; i++) {
        threads[i].join();
    }
    for (int size_current = avg; size_current < len;
         size_current *= 2) {
        for (int left = 0; left < len; left += 2 * size_current) {
            int mid = std::min(left + size_current - 1, len - 1);
            int right = std::min(left + 2 * size_current - 1, len - 1);
            if (mid < right) {  // 只有两个块需要合并时才合并
                mysort.merge(arr, left, right, mid);
            }
        }
    }
}

int main() {
    Merge<int> mysort;
    std::vector<int> arr = getrand(RAND_NUM);
    auto start = std::chrono::high_resolution_clock::now();
    multithread(arr);
    if (mysort.issort(arr)) {
        std::cout << "排序验证: 成功" << std::endl;
    } else {
        std::cout << "排序验证: 失败" << std::endl;
    }
    //mysort.show(arr);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "多线程排序耗时: " << duration.count() << " ms" << std::endl;

    auto start_d = std::chrono::high_resolution_clock::now();
    mysort.mergesort(arr,0,arr.size()-1);
    if (mysort.issort(arr)) {
        std::cout << "排序验证: 成功" << std::endl;
    } else {
        std::cout << "排序验证: 失败" << std::endl;
    }
    //mysort.show(arr);
    auto end_d = std::chrono::high_resolution_clock::now();
    auto duration_d =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_d - start_d);
    std::cout << "单线程排序耗时: " << duration_d.count() << " ms" << std::endl;
    return 0;
}