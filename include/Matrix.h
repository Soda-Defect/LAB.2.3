#ifndef MATRIX_H
#define MATRIX_H

#include <functional>
#include "Exceptions.h"

template<typename T>
class Matrix {
public:
    virtual ~Matrix() = default;
    
    virtual int getRows() const = 0;
    virtual int getCols() const = 0;
    virtual T get(int i, int j) const = 0;
    virtual void set(int i, int j, const T& value) = 0;
    
    virtual Matrix<T>* add(const Matrix<T>& other) const = 0;
    virtual Matrix<T>* multiplyByScalar(const T& scalar) const = 0;
    
    virtual double normL1() const = 0;      
    virtual double normInf() const = 0;    
    virtual double normL2() const = 0;      
    
    virtual void swapRows(int i, int j) = 0;
    virtual void swapCols(int i, int j) = 0;
    virtual void multiplyRow(int i, const T& scalar) = 0;
    virtual void multiplyCol(int j, const T& scalar) = 0;
    virtual void addRowToRow(int source, int target, const T& scalar = T(1)) = 0;
    virtual void addColToCol(int source, int target, const T& scalar = T(1)) = 0;
};

#endif 