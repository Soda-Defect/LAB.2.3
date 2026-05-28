#ifndef TRIANGLE_MATRIX_H
#define TRIANGLE_MATRIX_H

#include "Matrix.h"
#include "DynamicArray.h"
#include <cmath>
#include <complex>

enum class MatrixType {
    Upper,
    Lower
};

template<typename T>
class TriangleMatrix : public Matrix<T> {
private:
    int size;
    MatrixType type;
    DynamicArray<T> elements;
    
    int getIndex(int i, int j) const {
        if (type == MatrixType::Upper) {
            return i * size - i * (i - 1) / 2 + (j - i);
        } else {
            return i * (i + 1) / 2 + j;
        }
    }
    
    bool isInTriangle(int i, int j) const {
        if (type == MatrixType::Upper) {
            return i <= j;
        } else {
            return i >= j;
        }
    }
    
public:
    TriangleMatrix() : size(0), type(MatrixType::Upper), elements() {}
    
    TriangleMatrix(int size, MatrixType type = MatrixType::Upper) 
        : size(size), type(type) {
        if (size < 0) {
            throw SizeException("Matrix size cannot be negative");
        }
        int elemCount = size * (size + 1) / 2;
        elements = DynamicArray<T>(elemCount, T());
    }
    
    TriangleMatrix(const TriangleMatrix<T>& other) : size(other.size), type(other.type), elements(other.elements) {}
    
    TriangleMatrix(TriangleMatrix<T>&& other) noexcept : size(other.size), type(other.type), elements(std::move(other.elements)) {
        other.size = 0;
    }
    
    ~TriangleMatrix() = default;
    
    TriangleMatrix<T>& operator=(const TriangleMatrix<T>& other) {
        if (this != &other) {
            size = other.size;
            type = other.type;
            elements = other.elements;
        }
        return *this;
    }
    
    TriangleMatrix<T>& operator=(TriangleMatrix<T>&& other) noexcept {
        if (this != &other) {
            size = other.size;
            type = other.type;
            elements = other.elements;
            other.size = 0;
        }
        return *this;
    }
    
    int getRows() const override { return size; }
    int getCols() const override { return size; }
    
    T get(int i, int j) const override {
        if (i < 0 || i >= size || j < 0 || j >= size) {
            throw IndexOutOFBoundsException("Index out of bounds");
        }
        if (!isInTriangle(i, j)) {
            return T();  
        }
        int idx = getIndex(i, j);
        return elements.Get(idx);
    }
    
    void set(int i, int j, const T& value) override {
        if (i < 0 || i >= size || j < 0 || j >= size) {
            throw IndexOutOFBoundsException("Index out of bounds");
        }
        if (!isInTriangle(i, j)) {
            if (value != T()) {
                throw IndexOutOFBoundsException(
                    "Cannot set non-zero value outside triangular matrix"
                );
            }
            return;  
        }
        int idx = getIndex(i, j);
        elements.Set(idx, value);
    }
    
    Matrix<T>* add(const Matrix<T>& other) const override {
        const TriangleMatrix<T>* otherTri = dynamic_cast<const TriangleMatrix<T>*>(&other);
        if (!otherTri) {
            throw TypeException("Invalid matrix type for addition");
        }
        if (size != otherTri->size) {
            throw SizeException("Matrices must have same size for addition");
        }
        if (type != otherTri->type) {
            throw TypeException("Cannot add upper and lower triangular matrices");
        }
        
        TriangleMatrix<T>* result = new TriangleMatrix<T>(size, type);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (isInTriangle(i, j)) {
                    result->set(i, j, get(i, j) + otherTri->get(i, j));
                }
            }
        }
        return result;
    }
    
    Matrix<T>* multiplyByScalar(const T& scalar) const override {
        TriangleMatrix<T>* result = new TriangleMatrix<T>(size, type);
        for (int i = 0; i < elements.GetSize(); i++) {
            result->elements.Set(i, elements.Get(i) * scalar);
        }
        return result;
    }
    
    double normL1() const override {
        double maxColSum = 0.0;
        for (int j = 0; j < size; j++) {
            double colSum = 0.0;
            for (int i = 0; i < size; i++) {
                colSum += std::abs(get(i, j));
            }
            if (colSum > maxColSum) maxColSum = colSum;
        }
        return maxColSum;
    }
    
    double normInf() const override {
        double maxRowSum = 0.0;
        for (int i = 0; i < size; i++) {
            double rowSum = 0.0;
            for (int j = 0; j < size; j++) {
                rowSum += std::abs(get(i, j));
            }
            if (rowSum > maxRowSum) maxRowSum = rowSum;
        }
        return maxRowSum;
    }
    
    double normL2() const override {
        double sum = 0.0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                sum += std::abs(get(i, j)) * std::abs(get(i, j));
            }
        }
        return std::sqrt(sum);
    }
    
    void swapRows(int i, int j) override {
        throw TypeException("swapRows is not supported for triangular matrix (would break triangular structure)");
    }
    
    void multiplyRow(int i, const T& scalar) override {
        throw TypeException("multiplyRow is not supported for triangular matrix");
    }
    
    void addRowToRow(int source, int target, const T& scalar = T(1)) override {
        throw TypeException("addRowToRow is not supported for triangular matrix");
    }
    
    void swapCols(int i, int j) override {
        throw TypeException("swapCols is not supported for triangular matrix");
    }
    
    void multiplyCol(int j, const T& scalar) override {
        throw TypeException("multiplyCol is not supported for triangular matrix");
    }
    
    void addColToCol(int source, int target, const T& scalar = T(1)) override {
        throw TypeException("addColToCol is not supported for triangular matrix");
    }

    int getSize() const { return size; }
    MatrixType getType() const { return type; }
    
    void print() const {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                std::cout << get(i, j) << " ";
            }
            std::cout << std::endl;
        }
    }
};

#endif 