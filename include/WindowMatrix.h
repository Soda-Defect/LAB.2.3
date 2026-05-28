#ifndef WINDOWMATRIX_H
#define WINDOWMATRIX_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QMap>

class QListWidgetItem;
template<typename T> class Matrix;

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
    Matrix<double>* createMatrixByType(int type, int rows, int cols);
    QString matrixTypeToString(Matrix<double>* mat);
    void showError(const QString& message);
    void showInfo(const QString& message);

    QListWidget *listMatrices;
    QTextEdit *textDisplay;
    QLabel *labelCurrent;
    QLabel *labelInfo;
    QLabel *labelNormResult;
    
    QMap<QString, Matrix<double>*> matrices;
    QString currentName;
    Matrix<double>* currentMatrix;
};

#endif // WINDOWMATRIX_H