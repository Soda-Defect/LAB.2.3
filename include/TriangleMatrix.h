#ifndef TRIANGLERMATRIX_H
#define TRIANGLERMATRIX_H

#include "DynamicArray.h"
#include <complex>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <type_traits>

enum class MatrixType {
    Upper,  
    Lower
};

template<typename T>
class TriangleMatrix {
private:
    DynamicArray<T> data;     
    int n;                    
    MatrixType type;          
    
    int getIndex(int row, int col) const;
    
    int getNumElements() const { return n * (n + 1) / 2; }
    
public:
    TriangleMatrix();
    TriangleMatrix(int size, MatrixType t = MatrixType::Upper);
    TriangleMatrix(int size, const T& defaultValue, MatrixType t = MatrixType::Upper);
    TriangleMatrix(const TriangleMatrix<T>& other);
    TriangleMatrix(TriangleMatrix<T>&& other) noexcept;
    
    ~TriangleMatrix() = default;
    
    TriangleMatrix<T>& operator=(const TriangleMatrix<T>& other);
    TriangleMatrix<T>& operator=(TriangleMatrix<T>&& other) noexcept;
    
    T get(int row, int col) const;
    void set(int row, int col, const T& value);
    
    T operator()(int row, int col) const;
    
    int size() const { return n; }
    MatrixType getType() const { return type; }
    bool isEmpty() const { return n == 0; }
    
    TriangleMatrix<T> add(const TriangleMatrix<T>& other) const;
    TriangleMatrix<T> multiplyByScalar(const T& scalar) const;
    
    double normL1() const;      // (максимум суммы по столбцам)
    double normL2() const;      // Евклидова норма (Frobenius norm)
    double normInf() const;      // (максимум суммы по строкам) 
    
    void print() const;
    bool isSquare() const { return n > 0; }
    
    static TriangleMatrix<T> identity(int size);
    static TriangleMatrix<T> zeros(int size, MatrixType t = MatrixType::Upper);
    static TriangleMatrix<T> ones(int size, MatrixType t = MatrixType::Upper);
};

template<typename T>
int TriangleMatrix<T>::getIndex(int row, int col) const {
    if (row < 0 || row >= n || col < 0 || col >= n) {
        throw IndexOutOFBoundsException("Matrix index out of range");
    }
    
    if (type == MatrixType::Upper) {
        if (row > col){
            return -1; 
        }
        int idx = 0;
        for (int k = 0; k < row; k++) {
            idx += (n - k);
        }
        return idx + (col - row);
    } else { 
        if (row < col){
            return -1; 
        }
        int idx = 0;
        for (int k = 0; k < row; k++) {
            idx += (k + 1);
        }
        return idx + col;
    }
}

template<typename T>
TriangleMatrix<T>::TriangleMatrix() : n(0), type(MatrixType::Upper), data(DynamicArray<T>()) {}

template<typename T>
TriangleMatrix<T>::TriangleMatrix(int size, MatrixType t) : n(size), type(t) {
    if (size < 0) {
        throw SizeException("Matrix size cannot be negative");
    }
    int numElements = getNumElements();
    data = DynamicArray<T>(numElements);
}

template<typename T>
TriangleMatrix<T>::TriangleMatrix(int size, const T& defaultValue, MatrixType t) : n(size), type(t) {
    if (size < 0) {
        throw SizeException("Matrix size cannot be negative");
    }
    int numElements = getNumElements();
    data = DynamicArray<T>(numElements, defaultValue);
}

template<typename T>
TriangleMatrix<T>::TriangleMatrix(const TriangleMatrix<T>& other) : n(other.n), type(other.type), data(other.data) {}

template<typename T>
TriangleMatrix<T>::TriangleMatrix(TriangleMatrix<T>&& other) noexcept: n(other.n), type(other.type), data(std::move(other.data)) {
    other.n = 0;
}

template<typename T>
TriangleMatrix<T>& TriangleMatrix<T>::operator=(const TriangleMatrix<T>& other) {
    if (this != &other) {
        n = other.n;
        type = other.type;
        data = other.data;
    }
    return *this;
}

template<typename T>
TriangleMatrix<T>& TriangleMatrix<T>::operator=(TriangleMatrix<T>&& other) noexcept {
    if (this != &other) {
        n = other.n;
        type = other.type;
        data = std::move(other.data);
        other.n = 0;
    }
    return *this;
}

template<typename T>
T TriangleMatrix<T>::get(int row, int col) const {
    int idx = getIndex(row, col);
    if (idx == -1){
        return T();
    }
    return data.Get(idx);
}

template<typename T>
void TriangleMatrix<T>::set(int row, int col, const T& value) {
    int idx = getIndex(row, col);
    if (idx == -1) {
        if (value != T()) {
            throw IndexOutOFBoundsException("Cannot set non-zero value in zero position of triangular matrix");
        }
        return;
    }
    data.Set(idx, value);
}

template<typename T>
T TriangleMatrix<T>::operator()(int row, int col) const {
    return get(row, col);
}

template<typename T>
TriangleMatrix<T> TriangleMatrix<T>::add(const TriangleMatrix<T>& other) const {
    if (n != other.n) {
        throw SizeException("Matrix sizes do not match for addition");
    }
    if (type != other.type) {
        throw TypeException("Matrix types (upper/lower) do not match for addition");
    }
    
    TriangleMatrix<T> result(n, type);
    for (int k = 0; k < data.GetSize(); k++) {
        result.data.Set(k, data.Get(k) + other.data.Get(k));
    }
    return result;
}

template<typename T>
TriangleMatrix<T> TriangleMatrix<T>::multiplyByScalar(const T& scalar) const {
    TriangleMatrix<T> result(n, type);
    for (int k = 0; k < data.GetSize(); k++) {
        result.data.Set(k, data.Get(k) * scalar);
    }
    return result;
}

template<typename T>
double TriangleMatrix<T>::normL1() const {
    double maxSum = 0.0;
    for (int j = 0; j < n; j++) {
        double colSum = 0.0;
        for (int i = 0; i < n; i++) {
            T val = get(i, j);
            if constexpr (IsComplex<T>::value) {
                colSum += std::abs(val);
            } else {
                colSum += std::abs(static_cast<double>(val));
            }
        }
        if (colSum > maxSum){
            maxSum = colSum;
        }
    }
    return maxSum;
}


#endif 