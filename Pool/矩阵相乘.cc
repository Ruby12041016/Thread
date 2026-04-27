#include<iostream>
#include<vector>

class Matrix{
private:
 std::vector<std::vector<int>> data;
 int rows;
 int cols;

public:
 Matrix(int r,int c) : rows(r), cols(c), data(r, std::vector<int>(c, 0)){};
 Matrix(const std::vector<std::vector<int>>& data) : data(data) { rows = data.size();
     cols = rows ? data[0].size() : 0;
 }
 int getrow()const { return rows; }
 int getcol()const { return cols; }
 int& at(int i, int j) { return data[i][j]; }
 const int& at(int i, int j)const { return data[i][j]; }
 bool mulitable(const Matrix& b) {
     if (cols==b.getrow()) {
         return true;
     }
     return false;
 }
 Matrix mulit(const Matrix& b) { Matrix result(rows, b.getcol());
    if(!mulitable(b)){
        std::cout << "维度不匹配无法相乘" << std::endl;
        return Matrix(0, 0);
    }
    for (int i = 0; i < rows;i++){
        for (int j = 0; j < b.getcol();j++){
            int sum = 0;
            for (int k = 0; k < cols;k++){
                sum += at(i, k) * b.at(k, j);
            }
            result.at(i, j) = sum;
        }
    }
    return result;
 }
 void show(){
    for(auto rows:data ){
        for(auto val:rows){
            std::cout << val << " ";            
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
 }
};

int main(){
    Matrix test_a({{2, 3},{2,4},{2,5}});
    Matrix test_b({{2,3,4},{5,6,7},{8,9,0}});
    Matrix test_c(2, 3);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            test_c.at(i, j) = i + j + 1;
        }
    }

    test_a.show();
    test_b.show();
    test_c.show();

    Matrix result=test_b.mulit(test_a);
    result.show();
    result = test_b.mulit(test_c);
    result.show();
    result = test_a.mulit(test_c);
    result.show();
}