#ifndef SQUARE_MATRIX_H
#define SQUARE_MATRIX_H

#include "RectangularMatrix.h"

template<typename T>
class SquareMatrix : public RectangularMatrix<T> {
private:
    int size;
    
public:
    SquareMatrix() : RectangularMatrix<T>() {
        size = 0;
    }
    
    SquareMatrix(int size) : RectangularMatrix<T>(size, size), size(size) {
        if (size < 0) {
            throw SizeException("Matrix size cannot be negative");
        }
    }
    
    SquareMatrix(int size, const T& defaultValue) : RectangularMatrix<T>(size, size, defaultValue), size(size) {}
    
    SquareMatrix(const SquareMatrix<T>& other) : RectangularMatrix<T>(other), size(other.size) {}
    
    SquareMatrix(SquareMatrix<T>&& other) noexcept : RectangularMatrix<T>(other), size(other.size) {
        other.size = 0;
    }
    
    SquareMatrix<T>& operator=(const SquareMatrix<T>& other) {
        if (this != &other) {
            RectangularMatrix<T>::operator=(other);
            size = other.size;
        }
        return *this;
    }
    
    SquareMatrix<T>& operator=(SquareMatrix<T>&& other) noexcept {
        if (this != &other) {
            RectangularMatrix<T>::operator=(other);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }
    
    int getSize() const { return size; }
    
    SquareMatrix<T>* add(const Matrix<T>& other) const override {
        const SquareMatrix<T>* otherSquare = dynamic_cast<const SquareMatrix<T>*>(&other);
        if (!otherSquare) {
            throw TypeException("Invalid matrix type for addition");
        }
        if (size != otherSquare->getSize()) {
            throw SizeException("Matrices must have same size for addition");
        }
        
        SquareMatrix<T>* result = new SquareMatrix<T>(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result->set(i, j, this->get(i, j) + otherSquare->get(i, j));
            }
        }
        return result;
    }
    
    SquareMatrix<T>* multiplyByScalar(const T& scalar) const override {
        SquareMatrix<T>* result = new SquareMatrix<T>(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result->set(i, j, this->get(i, j) * scalar);
            }
        }
        return result;
    }
};

#endif 