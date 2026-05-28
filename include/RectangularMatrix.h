#ifndef RECTANGULAR_MATRIX_H
#define RECTANGULAR_MATRIX_H

#include "Matrix.h"
#include "DynamicArray.h"
#include <cmath>
#include <complex>

template<typename T>
class RectangularMatrix : public Matrix<T> {
private:
    DynamicArray<DynamicArray<T>> data;
    int rows;
    int cols;
    
    void checkBounds(int i, int j) const {
        if (i < 0 || i >= rows || j < 0 || j >= cols) {
            throw IndexOutOFBoundsException("Matrix indices out of bounds");
        }
    }
    
    void checkSameSize(const RectangularMatrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw SizeException("Matrices must have same dimensions for addition");
        }
    }
    
public:
    RectangularMatrix() : rows(0), cols(0) {}
    
    RectangularMatrix(int rows, int cols) : rows(rows), cols(cols) {
        if (rows < 0 || cols < 0) {
            throw SizeException("Matrix dimensions cannot be negative");
        }
        data = DynamicArray<DynamicArray<T>>(rows);
        for (int i = 0; i < rows; i++) {
            data.Set(i, DynamicArray<T>(cols, T()));
        }
    }
    
    RectangularMatrix(int rows, int cols, const T& defaultValue) : rows(rows), cols(cols) {
        if (rows < 0 || cols < 0) {
            throw SizeException("Matrix dimensions cannot be negative");
        }
        data = DynamicArray<DynamicArray<T>>(rows);
        for (int i = 0; i < rows; i++) {
            data.Set(i, DynamicArray<T>(cols, defaultValue));
        }
    }
    
    RectangularMatrix(const RectangularMatrix<T>& other) : rows(other.rows), cols(other.cols), data(other.data) {}
    
    RectangularMatrix(RectangularMatrix<T>&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) {
        other.rows = other.cols = 0;
    }
    
    RectangularMatrix<T>& operator=(const RectangularMatrix<T>& other) {
        if (this != &other) {
            rows = other.rows;
            cols = other.cols;
            data = other.data;
        }
        return *this;
    }
    
    RectangularMatrix<T>& operator=(RectangularMatrix<T>&& other) noexcept {
        if (this != &other) {
            rows = other.rows;
            cols = other.cols;
            data = other.data;
            other.rows = other.cols = 0;
        }
        return *this;
    }
    
    int getRows() const override { return rows; }
    int getCols() const override { return cols; }
    
    T get(int i, int j) const override {
        checkBounds(i, j);
        return data.Get(i).Get(j);
    }
    
    void set(int i, int j, const T& value) override {
        checkBounds(i, j);
        data[i].Set(j, value);
    }
    
    T& operator()(int i, int j) {
        checkBounds(i, j);
        return data[i][j];
    }
    
    const T& operator()(int i, int j) const {
        checkBounds(i, j);
        return data[i][j];
    }
    
    RectangularMatrix<T>* add(const Matrix<T>& other) const override {
        const RectangularMatrix<T>* otherRect = dynamic_cast<const RectangularMatrix<T>*>(&other);
        if (!otherRect) {
            throw TypeException("Invalid matrix type for addition");
        }
        checkSameSize(*otherRect);
        
        RectangularMatrix<T>* result = new RectangularMatrix<T>(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result->set(i, j, get(i, j) + otherRect->get(i, j));
            }
        }
        return result;
    }
    
    RectangularMatrix<T>* multiplyByScalar(const T& scalar) const override {
        RectangularMatrix<T>* result = new RectangularMatrix<T>(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result->set(i, j, get(i, j) * scalar);
            }
        }
        return result;
    }
    
    double normL1() const override {
        double maxColSum = 0.0;
        for (int j = 0; j < cols; j++) {
            double colSum = 0.0;
            for (int i = 0; i < rows; i++) {
                colSum += std::abs(get(i, j));
            }
            if (colSum > maxColSum) maxColSum = colSum;
        }
        return maxColSum;
    }
    
    double normInf() const override {
        double maxRowSum = 0.0;
        for (int i = 0; i < rows; i++) {
            double rowSum = 0.0;
            for (int j = 0; j < cols; j++) {
                rowSum += std::abs(get(i, j));
            }
            if (rowSum > maxRowSum) maxRowSum = rowSum;
        }
        return maxRowSum;
    }
    
    double normL2() const override {
        double sum = 0.0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                sum += std::abs(get(i, j)) * std::abs(get(i, j));
            }
        }
        return std::sqrt(sum);
    }
    
    void swapRows(int i, int j) override {
        checkBounds(i, 0);
        checkBounds(j, 0);
        if (i == j) return;
        DynamicArray<T> temp = data[i];
        data.Set(i, data[j]);
        data.Set(j, temp);
    }
    
    void multiplyRow(int i, const T& scalar) override {
        checkBounds(i, 0);
        for (int j = 0; j < cols; j++) {
            data[i].Set(j, data[i].Get(j) * scalar);
        }
    }
    
    void addRowToRow(int source, int target, const T& scalar = T(1)) override {
        checkBounds(source, 0);
        checkBounds(target, 0);
        for (int j = 0; j < cols; j++) {
            T newVal = data[target].Get(j) + data[source].Get(j) * scalar;
            data[target].Set(j, newVal);
        }
    }
    
    void swapCols(int i, int j) override {
        checkBounds(0, i);
        checkBounds(0, j);
        if (i == j) return;
        for (int row = 0; row < rows; row++) {
            T temp = data[row].Get(i);
            data[row].Set(i, data[row].Get(j));
            data[row].Set(j, temp);
        }
    }
    
    void multiplyCol(int j, const T& scalar) override {
        checkBounds(0, j);
        for (int i = 0; i < rows; i++) {
            data[i].Set(j, data[i].Get(j) * scalar);
        }
    }
    
    void addColToCol(int source, int target, const T& scalar = T(1)) override {
        checkBounds(0, source);
        checkBounds(0, target);
        for (int i = 0; i < rows; i++) {
            T newVal = data[i].Get(target) + data[i].Get(source) * scalar;
            data[i].Set(target, newVal);
        }
    }
    
    void print() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << get(i, j) << " ";
            }
            std::cout << std::endl;
        }
    }
    
    bool isSquare() const { return rows == cols; }
};

#endif 