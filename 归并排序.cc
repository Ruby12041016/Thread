#include <iostream>
#include <vector>

using namespace std;

bool issort(vector<int>& arr) {
    for (int i = 0; i < arr.size() - 1; i++) {
        if (arr[i + 1] < arr[i]) {
            return false;
        }
    }
    return true;
}

void merge(vector<int>& arr, int left, int right, int mid) {
    int len1 = mid - left + 1;
    int len2 = right - mid;
    vector<int> leftarr(len1);
    vector<int> rightarr(len2);
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

void mergesort(vector<int>& arr, int left, int right) {
    if (issort(arr)) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergesort(arr, left, mid);
    mergesort(arr, mid + 1, right);
    merge(arr, left, right, mid);
}

void mysort(vector<int>& arr) {
    int len = arr.size();
    if (len <= 1) {
        return;
    }
    mergesort(arr, 0, len - 1);
}

// 打印数组函数
void printArray(const vector<int>& arr) {
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;
}

int main() {
    // 测试用例
    vector<int> arr1 = {12, 11, 13, 5, 6, 7};
    vector<int> arr2 = {64, 34, 25, 12, 22, 11, 90};
    vector<int> arr3 = {5, 2, 4, 6, 1, 3};

    cout << "原始数组1: ";
    printArray(arr1);
    mysort(arr1);
    cout << "排序后数组1: ";
    printArray(arr1);
    cout << endl;

    cout << "原始数组2: ";
    printArray(arr2);
    mysort(arr2);
    cout << "排序后数组2: ";
    printArray(arr2);
    cout << endl;

    cout << "原始数组3: ";
    printArray(arr3);
    mysort(arr3);
    cout << "排序后数组3: ";
    printArray(arr3);

    return 0;
}