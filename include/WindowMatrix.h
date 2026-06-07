#ifndef WINDOWMATRIX_H
#define WINDOWMATRIX_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QMap>
#include <QComboBox>
#include <variant>
#include <complex>
#include <memory>

// Forward declarations
template<typename T> class Matrix;

// Типы данных
enum class DataType {
    Int,
    Double,
    Complex
};

// Обёртка для хранения любой матрицы
struct MatrixWrapper {
    DataType dataType;
    std::variant<
        std::unique_ptr<Matrix<int>>,
        std::unique_ptr<Matrix<double>>,
        std::unique_ptr<Matrix<std::complex<double>>>
    > matrix;

    MatrixWrapper();
    
    explicit MatrixWrapper(Matrix<int>* m);
    explicit MatrixWrapper(Matrix<double>* m);
    explicit MatrixWrapper(Matrix<std::complex<double>>* m);
    
    // Разрешаем перемещение, запрещаем копирование
    MatrixWrapper(const MatrixWrapper&) = delete;
    MatrixWrapper& operator=(const MatrixWrapper&) = delete;
    
    MatrixWrapper(MatrixWrapper&& other) noexcept = default;
    MatrixWrapper& operator=(MatrixWrapper&& other) noexcept = default;
    
    ~MatrixWrapper() = default;
    
    bool isValid() const;
    int getRows() const;
    int getCols() const;
    QString getTypeString() const;
};

class MainWindowMatrix : public QMainWindow {
    Q_OBJECT

public:
    MainWindowMatrix(QWidget *parent = nullptr);
    ~MainWindowMatrix();

private slots:
    void onCreateMatrix();
    void onRefreshList();
    void onDeleteMatrix();
    void onMatrixClicked(QListWidgetItem *item);
    void onEditElement();
    void onShowMatrix();
    void onAdd();
    void onMultiplyByScalar();
    void onNormL1();
    void onNormInf();
    void onNormL2();
    void onSwapRows();
    void onSwapCols();
    void onMultiplyRow();
    void onMultiplyCol();
    void onAddRowToRow();
    void onAddColToCol();

private:
    void setupUI();
    void updateMatrixList();
    void updateCurrentDisplay();
    void updateInfo();
    
    MatrixWrapper* createMatrixByType(DataType dataType, int matrixType, int rows, int cols);
    
    template<typename T>
    void fillMatrixDialog(Matrix<T>* mat, int rows, int cols);
    
    void showError(const QString& message);
    void showInfo(const QString& message);
    
    template<typename T>
    QString valueToString(const T& value) const;
    
    template<typename T>
    T parseValue(const QString& str) const;
    
    // Вспомогательные методы для работы с variant
    void editElementInt(int i, int j, Matrix<int>* mat);
    void editElementDouble(int i, int j, Matrix<double>* mat);
    void editElementComplex(int i, int j, Matrix<std::complex<double>>* mat);
    
    void displayMatrixInt(Matrix<int>* mat, int rows, int cols, QString& display);
    void displayMatrixDouble(Matrix<double>* mat, int rows, int cols, QString& display);
    void displayMatrixComplex(Matrix<std::complex<double>>* mat, int rows, int cols, QString& display);
    
    MatrixWrapper* addMatrices(MatrixWrapper* a, MatrixWrapper* b);
    MatrixWrapper* multiplyByScalar(MatrixWrapper* mat, const QString& scalarStr);

    QListWidget *listMatrices;
    QTextEdit *textDisplay;
    QLabel *labelCurrent;
    QLabel *labelInfo;
    QLabel *labelNormResult;
    QComboBox *comboDataType;
    
    // Храним указатели, а не объекты
    QMap<QString, MatrixWrapper*> matrices;
    QString currentName;
    MatrixWrapper* currentMatrix;
};

#endif // WINDOWMATRIX_H