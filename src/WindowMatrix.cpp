#include "../include/WindowMatrix.h"
#include "../include/Matrix.h"
#include "../include/RectangularMatrix.h"
#include "../include/SquareMatrix.h"
#include "../include/TriangleMatrix.h"
#include "../include/Exceptions.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>
#include <QRegularExpression>

// ==================== MatrixWrapper implementation ====================

MatrixWrapper::MatrixWrapper() : dataType(DataType::Int), matrix(std::unique_ptr<Matrix<int>>(nullptr)) {}

MatrixWrapper::MatrixWrapper(Matrix<int>* m) : dataType(DataType::Int), matrix(std::unique_ptr<Matrix<int>>(m)) {}

MatrixWrapper::MatrixWrapper(Matrix<double>* m) : dataType(DataType::Double), matrix(std::unique_ptr<Matrix<double>>(m)) {}

MatrixWrapper::MatrixWrapper(Matrix<std::complex<double>>* m) : dataType(DataType::Complex), matrix(std::unique_ptr<Matrix<std::complex<double>>>(m)) {}

bool MatrixWrapper::isValid() const {
    return std::visit([](const auto& ptr) { return ptr != nullptr; }, matrix);
}

int MatrixWrapper::getRows() const {
    return std::visit([](const auto& ptr) -> int { 
        return ptr ? ptr->getRows() : 0; 
    }, matrix);
}

int MatrixWrapper::getCols() const {
    return std::visit([](const auto& ptr) -> int { 
        return ptr ? ptr->getCols() : 0; 
    }, matrix);
}

QString MatrixWrapper::getTypeString() const {
    QString dataTypeStr;
    switch (dataType) {
        case DataType::Int: dataTypeStr = "int"; break;
        case DataType::Double: dataTypeStr = "double"; break;
        case DataType::Complex: dataTypeStr = "complex"; break;
    }
    
    QString matrixTypeStr = std::visit([this](const auto& ptr) -> QString {
        if (!ptr) return "Unknown";
        
        if (dataType == DataType::Int) {
            auto* mat = dynamic_cast<const RectangularMatrix<int>*>(ptr.get());
            if (mat) return "Rectangular";
            auto* sq = dynamic_cast<const SquareMatrix<int>*>(ptr.get());
            if (sq) return "Square";
            auto* tri = dynamic_cast<const TriangleMatrix<int>*>(ptr.get());
            if (tri) return tri->getType() == MatrixType::Upper ? "UpperTriangular" : "LowerTriangular";
        } else if (dataType == DataType::Double) {
            auto* mat = dynamic_cast<const RectangularMatrix<double>*>(ptr.get());
            if (mat) return "Rectangular";
            auto* sq = dynamic_cast<const SquareMatrix<double>*>(ptr.get());
            if (sq) return "Square";
            auto* tri = dynamic_cast<const TriangleMatrix<double>*>(ptr.get());
            if (tri) return tri->getType() == MatrixType::Upper ? "UpperTriangular" : "LowerTriangular";
        } else {
            auto* mat = dynamic_cast<const RectangularMatrix<std::complex<double>>*>(ptr.get());
            if (mat) return "Rectangular";
            auto* sq = dynamic_cast<const SquareMatrix<std::complex<double>>*>(ptr.get());
            if (sq) return "Square";
            auto* tri = dynamic_cast<const TriangleMatrix<std::complex<double>>*>(ptr.get());
            if (tri) return tri->getType() == MatrixType::Upper ? "UpperTriangular" : "LowerTriangular";
        }
        return "Unknown";
    }, matrix);
    
    return QString("%1 (%2)").arg(matrixTypeStr).arg(dataTypeStr);
}

// ==================== MainWindowMatrix implementation ====================

MainWindowMatrix::MainWindowMatrix(QWidget *parent) 
    : QMainWindow(parent), currentMatrix(nullptr) {
    setupUI();
    updateMatrixList();
}

MainWindowMatrix::~MainWindowMatrix() {
    for (auto* mat : matrices) {
        delete mat;
    }
}

void MainWindowMatrix::showError(const QString& message) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("ОШИБКА");
    msgBox.setText(message);
    msgBox.exec();
}

void MainWindowMatrix::showInfo(const QString& message) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("ИНФОРМАЦИЯ");
    msgBox.setText(message);
    msgBox.exec();
}

template<typename T>
QString MainWindowMatrix::valueToString(const T& value) const {
    if constexpr (std::is_same_v<T, int>) {
        return QString::number(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return QString::number(value, 'g', 6);
    } else if constexpr (std::is_same_v<T, std::complex<double>>) {
        return QString("(%1,%2)").arg(value.real(), 0, 'g', 6).arg(value.imag(), 0, 'g', 6);
    }
    return QString();
}

template<typename T>
T MainWindowMatrix::parseValue(const QString& str) const {
    if constexpr (std::is_same_v<T, int>) {
        return str.toInt();
    } else if constexpr (std::is_same_v<T, double>) {
        return str.toDouble();
    } else if constexpr (std::is_same_v<T, std::complex<double>>) {
        QRegularExpression re1(R"(^\s*\(?\s*([+-]?\d*\.?\d+)\s*[,;]\s*([+-]?\d*\.?\d+)\s*\)?\s*$)");
        QRegularExpression re2(R"(^\s*([+-]?\d*\.?\d+)\s*([+-]\d*\.?\d*)i?\s*$)");
        QRegularExpression re3(R"(^\s*([+-]?\d*\.?\d+)\s*$)");
        
        double re = 0, im = 0;
        
        auto match1 = re1.match(str);
        if (match1.hasMatch()) {
            re = match1.captured(1).toDouble();
            im = match1.captured(2).toDouble();
        } else {
            auto match2 = re2.match(str);
            if (match2.hasMatch()) {
                re = match2.captured(1).toDouble();
                im = match2.captured(2).toDouble();
            } else {
                auto match3 = re3.match(str);
                if (match3.hasMatch()) {
                    re = match3.captured(1).toDouble();
                    im = 0;
                } else {
                    re = str.toDouble();
                    im = 0;
                }
            }
        }
        return std::complex<double>(re, im);
    }
    return T();
}

template<typename T>
void MainWindowMatrix::fillMatrixDialog(Matrix<T>* mat, int rows, int cols) {
    // Пытаемся определить, является ли матрица треугольной
    bool isTriangular = false;
    bool isUpper = false;
    
    if (auto* tri = dynamic_cast<TriangleMatrix<T>*>(mat)) {
        isTriangular = true;
        isUpper = (tri->getType() == MatrixType::Upper);
    }
    
    int fill = QMessageBox::question(this, "ЗАПОЛНЕНИЕ", 
        "ЗАПОЛНИТЬ МАТРИЦУ ЗНАЧЕНИЯМИ ПО УМОЛЧАНИЮ?", 
        QMessageBox::Yes | QMessageBox::No);
    
    if (fill == QMessageBox::Yes) {
        QString defaultStr = QInputDialog::getText(this, "ЗНАЧЕНИЕ ПО УМОЛЧАНИЮ", 
            "ВВЕДИТЕ ЗНАЧЕНИЕ (для комплексных: a+bi или a,b):", QLineEdit::Normal, "0");
        T def = parseValue<T>(defaultStr);
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (isTriangular) {
                    // Для треугольной матрицы заполняем только треугольную область
                    if (isUpper) {
                        if (i <= j) mat->set(i, j, def);
                    } else {
                        if (i >= j) mat->set(i, j, def);
                    }
                } else {
                    mat->set(i, j, def);
                }
            }
        }
        showInfo(isTriangular ? 
            "ЗАПОЛНЕНЫ ТОЛЬКО ЭЛЕМЕНТЫ В ТРЕУГОЛЬНОЙ ОБЛАСТИ" : 
            "МАТРИЦА ЗАПОЛНЕНА");
    } else {
        // Для ручного ввода показываем только доступные для заполнения позиции
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                // Пропускаем недоступные позиции в треугольной матрице
                if (isTriangular) {
                    if (isUpper && i > j) continue;
                    if (!isUpper && i < j) continue;
                }
                
                QString currentValueStr;
                try {
                    currentValueStr = valueToString(mat->get(i, j));
                } catch (...) {
                    currentValueStr = "0";
                }
                
                bool ok;
                QString valStr = QInputDialog::getText(this, "ВВОД ЭЛЕМЕНТА", 
                    QString("ЭЛЕМЕНТ [%1][%2]:").arg(i).arg(j), 
                    QLineEdit::Normal, currentValueStr, &ok);
                if (!ok) continue;
                
                T val = parseValue<T>(valStr);
                
                // Для треугольной матрицы разрешаем вводить только 0 вне треугольника
                if (isTriangular) {
                    if ((isUpper && i <= j) || (!isUpper && i >= j)) {
                        mat->set(i, j, val);
                    } else if (val != T()) {
                        QMessageBox::warning(this, "ПРЕДУПРЕЖДЕНИЕ", 
                            QString("ЭЛЕМЕНТ [%1][%2] НАХОДИТСЯ ВНЕ ТРЕУГОЛЬНОЙ ОБЛАСТИ.\n"
                                    "БУДЕТ УСТАНОВЛЕН В 0.").arg(i).arg(j));
                        mat->set(i, j, T());
                    }
                } else {
                    mat->set(i, j, val);
                }
            }
        }
    }
}

MatrixWrapper* MainWindowMatrix::createMatrixByType(DataType dataType, int matrixType, int rows, int cols) {
    switch (dataType) {
        case DataType::Int: {
            switch (matrixType) {
                case 0: return new MatrixWrapper(new RectangularMatrix<int>(rows, cols));
                case 1: return new MatrixWrapper(new SquareMatrix<int>(rows));
                case 2: return new MatrixWrapper(new TriangleMatrix<int>(rows, MatrixType::Upper));
                case 3: return new MatrixWrapper(new TriangleMatrix<int>(rows, MatrixType::Lower));
            }
            break;
        }
        case DataType::Double: {
            switch (matrixType) {
                case 0: return new MatrixWrapper(new RectangularMatrix<double>(rows, cols));
                case 1: return new MatrixWrapper(new SquareMatrix<double>(rows));
                case 2: return new MatrixWrapper(new TriangleMatrix<double>(rows, MatrixType::Upper));
                case 3: return new MatrixWrapper(new TriangleMatrix<double>(rows, MatrixType::Lower));
            }
            break;
        }
        case DataType::Complex: {
            switch (matrixType) {
                case 0: return new MatrixWrapper(new RectangularMatrix<std::complex<double>>(rows, cols));
                case 1: return new MatrixWrapper(new SquareMatrix<std::complex<double>>(rows));
                case 2: return new MatrixWrapper(new TriangleMatrix<std::complex<double>>(rows, MatrixType::Upper));
                case 3: return new MatrixWrapper(new TriangleMatrix<std::complex<double>>(rows, MatrixType::Lower));
            }
            break;
        }
    }
    return nullptr;
}

void MainWindowMatrix::displayMatrixInt(Matrix<int>* mat, int rows, int cols, QString& display) {
    int maxWidth = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            if (val.length() > maxWidth) maxWidth = val.length();
        }
    }
    maxWidth = qMin(maxWidth, 20);
    
    for (int i = 0; i < rows; i++) {
        display += "[ ";
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            display += QString("%1").arg(val, maxWidth);
            if (j < cols - 1) display += " ";
        }
        display += " ]\n";
    }
}

void MainWindowMatrix::displayMatrixDouble(Matrix<double>* mat, int rows, int cols, QString& display) {
    int maxWidth = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            if (val.length() > maxWidth) maxWidth = val.length();
        }
    }
    maxWidth = qMin(maxWidth, 20);
    
    for (int i = 0; i < rows; i++) {
        display += "[ ";
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            display += QString("%1").arg(val, maxWidth);
            if (j < cols - 1) display += " ";
        }
        display += " ]\n";
    }
}

void MainWindowMatrix::displayMatrixComplex(Matrix<std::complex<double>>* mat, int rows, int cols, QString& display) {
    int maxWidth = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            if (val.length() > maxWidth) maxWidth = val.length();
        }
    }
    maxWidth = qMin(maxWidth, 20);
    
    for (int i = 0; i < rows; i++) {
        display += "[ ";
        for (int j = 0; j < cols; j++) {
            QString val = valueToString(mat->get(i, j));
            display += QString("%1").arg(val, maxWidth);
            if (j < cols - 1) display += " ";
        }
        display += " ]\n";
    }
}

void MainWindowMatrix::editElementInt(int i, int j, Matrix<int>* mat) {
    int currentValue = mat->get(i, j);
    QString currentStr = valueToString(currentValue);
    QString newStr = QInputDialog::getText(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        QString("ВВЕДИТЕ НОВОЕ ЗНАЧЕНИЕ ДЛЯ ЭЛЕМЕНТА [%1][%2]:").arg(i).arg(j),
        QLineEdit::Normal, currentStr);
    if (!newStr.isEmpty()) {
        int newValue = parseValue<int>(newStr);
        mat->set(i, j, newValue);
        updateCurrentDisplay();
        showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
    }
}

void MainWindowMatrix::editElementDouble(int i, int j, Matrix<double>* mat) {
    double currentValue = mat->get(i, j);
    QString currentStr = valueToString(currentValue);
    QString newStr = QInputDialog::getText(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        QString("ВВЕДИТЕ НОВОЕ ЗНАЧЕНИЕ ДЛЯ ЭЛЕМЕНТА [%1][%2]:").arg(i).arg(j),
        QLineEdit::Normal, currentStr);
    if (!newStr.isEmpty()) {
        double newValue = parseValue<double>(newStr);
        mat->set(i, j, newValue);
        updateCurrentDisplay();
        showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
    }
}

void MainWindowMatrix::editElementComplex(int i, int j, Matrix<std::complex<double>>* mat) {
    auto currentValue = mat->get(i, j);
    QString currentStr = valueToString(currentValue);
    QString newStr = QInputDialog::getText(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        QString("ВВЕДИТЕ НОВОЕ ЗНАЧЕНИЕ ДЛЯ ЭЛЕМЕНТА [%1][%2]:").arg(i).arg(j),
        QLineEdit::Normal, currentStr);
    if (!newStr.isEmpty()) {
        auto newValue = parseValue<std::complex<double>>(newStr);
        mat->set(i, j, newValue);
        updateCurrentDisplay();
        showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
    }
}

MatrixWrapper* MainWindowMatrix::addMatrices(MatrixWrapper* a, MatrixWrapper* b) {
    if (a->dataType != b->dataType) return nullptr;
    
    if (a->dataType == DataType::Int) {
        auto* matA = std::get<std::unique_ptr<Matrix<int>>>(a->matrix).get();
        auto* matB = std::get<std::unique_ptr<Matrix<int>>>(b->matrix).get();
        if (matA && matB) {
            return new MatrixWrapper(matA->add(*matB));
        }
    } else if (a->dataType == DataType::Double) {
        auto* matA = std::get<std::unique_ptr<Matrix<double>>>(a->matrix).get();
        auto* matB = std::get<std::unique_ptr<Matrix<double>>>(b->matrix).get();
        if (matA && matB) {
            return new MatrixWrapper(matA->add(*matB));
        }
    } else {
        auto* matA = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(a->matrix).get();
        auto* matB = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(b->matrix).get();
        if (matA && matB) {
            return new MatrixWrapper(matA->add(*matB));
        }
    }
    return nullptr;
}

MatrixWrapper* MainWindowMatrix::multiplyByScalar(MatrixWrapper* mat, const QString& scalarStr) {
    if (mat->dataType == DataType::Int) {
        auto* m = std::get<std::unique_ptr<Matrix<int>>>(mat->matrix).get();
        if (m) {
            int scalar = parseValue<int>(scalarStr);
            return new MatrixWrapper(m->multiplyByScalar(scalar));
        }
    } else if (mat->dataType == DataType::Double) {
        auto* m = std::get<std::unique_ptr<Matrix<double>>>(mat->matrix).get();
        if (m) {
            double scalar = parseValue<double>(scalarStr);
            return new MatrixWrapper(m->multiplyByScalar(scalar));
        }
    } else {
        auto* m = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(mat->matrix).get();
        if (m) {
            auto scalar = parseValue<std::complex<double>>(scalarStr);
            return new MatrixWrapper(m->multiplyByScalar(scalar));
        }
    }
    return nullptr;
}

void MainWindowMatrix::setupUI() {
    resize(1000, 800);
    
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Панель выбора типа данных
    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(new QLabel("Тип данных:"));
    comboDataType = new QComboBox;
    comboDataType->addItems({"int (целые)", "double (вещественные)", "complex (комплексные)"});
    typeLayout->addWidget(comboDataType);
    typeLayout->addStretch();
    mainLayout->addLayout(typeLayout);
    
    // Верхняя панель - список и отображение
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setSpacing(10);
    
    // Левая панель - список матриц
    QGroupBox *listGroup = new QGroupBox("МАТРИЦЫ");
    listGroup->setFixedWidth(280);
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);
    
    listMatrices = new QListWidget;
    listMatrices->setMinimumHeight(350);
    listLayout->addWidget(listMatrices);
    
    QPushButton *btnCreate = new QPushButton("СОЗДАТЬ МАТРИЦУ");
    QPushButton *btnDelete = new QPushButton("УДАЛИТЬ МАТРИЦУ");
    QPushButton *btnRefresh = new QPushButton("ОБНОВИТЬ");
    
    btnCreate->setMinimumHeight(35);
    btnDelete->setMinimumHeight(35);
    btnRefresh->setMinimumHeight(35);
    
    listLayout->addWidget(btnCreate);
    listLayout->addWidget(btnDelete);
    listLayout->addWidget(btnRefresh);
    
    topLayout->addWidget(listGroup);
    
    // Правая панель - отображение матрицы
    QGroupBox *displayGroup = new QGroupBox("ОТОБРАЖЕНИЕ МАТРИЦЫ");
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    
    textDisplay = new QTextEdit;
    textDisplay->setReadOnly(true);
    textDisplay->setFont(QFont("Courier New", 11));
    textDisplay->setMinimumHeight(350);
    displayLayout->addWidget(textDisplay);
    
    topLayout->addWidget(displayGroup);
    mainLayout->addLayout(topLayout);
    
    // Информационная панель
    QHBoxLayout *infoLayout = new QHBoxLayout;
    infoLayout->setSpacing(20);
    
    labelCurrent = new QLabel("ТЕКУЩАЯ МАТРИЦА: НЕ ВЫБРАНА");
    labelInfo = new QLabel("РАЗМЕР: -");
    labelNormResult = new QLabel("НОРМЫ: НЕ ВЫЧИСЛЕНЫ");
    
    labelCurrent->setMinimumWidth(280);
    labelInfo->setMinimumWidth(150);
    labelNormResult->setMinimumWidth(250);
    
    infoLayout->addWidget(labelCurrent);
    infoLayout->addWidget(labelInfo);
    infoLayout->addWidget(labelNormResult);
    infoLayout->addStretch();
    
    mainLayout->addLayout(infoLayout);
    
    // Панель операций
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumHeight(280);
    
    QWidget *operationsWidget = new QWidget;
    QVBoxLayout *operationsLayout = new QVBoxLayout(operationsWidget);
    
    QTabWidget *operationsTabs = new QTabWidget;
    
    // ========== ВКЛАДКА "РЕДАКТИРОВАНИЕ" ==========
    QWidget *editTab = new QWidget;
    QHBoxLayout *editLayout = new QHBoxLayout(editTab);
    QPushButton *btnEdit = new QPushButton("ИЗМЕНИТЬ ЭЛЕМЕНТ");
    QPushButton *btnShow = new QPushButton("ПОКАЗАТЬ МАТРИЦУ");
    btnEdit->setMinimumHeight(40);
    btnShow->setMinimumHeight(40);
    editLayout->addWidget(btnEdit);
    editLayout->addWidget(btnShow);
    editLayout->addStretch();
    operationsTabs->addTab(editTab, "РЕДАКТИРОВАНИЕ");
    
    // ========== ВКЛАДКА "АРИФМЕТИКА" ==========
    QWidget *arithTab = new QWidget;
    QGridLayout *arithLayout = new QGridLayout(arithTab);
    arithLayout->setSpacing(10);
    
    QPushButton *btnAdd = new QPushButton("СЛОЖЕНИЕ МАТРИЦ");
    QPushButton *btnMulScalar = new QPushButton("УМНОЖИТЬ НА СКАЛЯР");
    QPushButton *btnNormL1 = new QPushButton("ВЫЧИСЛИТЬ НОРМУ L1");
    QPushButton *btnNormInf = new QPushButton("ВЫЧИСЛИТЬ НОРМУ L∞");
    QPushButton *btnNormL2 = new QPushButton("ВЫЧИСЛИТЬ НОРМУ L2");
    
    btnAdd->setMinimumHeight(40);
    btnMulScalar->setMinimumHeight(40);
    btnNormL1->setMinimumHeight(40);
    btnNormInf->setMinimumHeight(40);
    btnNormL2->setMinimumHeight(40);
    
    arithLayout->addWidget(btnAdd, 0, 0);
    arithLayout->addWidget(btnMulScalar, 0, 1);
    arithLayout->addWidget(btnNormL1, 1, 0);
    arithLayout->addWidget(btnNormInf, 1, 1);
    arithLayout->addWidget(btnNormL2, 2, 0, 1, 2);
    arithLayout->setColumnStretch(2, 1);
    operationsTabs->addTab(arithTab, "АРИФМЕТИКА");
    
    // ========== ВКЛАДКА "СТРОКИ" ==========
    QWidget *rowTab = new QWidget;
    QVBoxLayout *rowMainLayout = new QVBoxLayout(rowTab);
    
    QHBoxLayout *rowLayout1 = new QHBoxLayout;
    QHBoxLayout *rowLayout2 = new QHBoxLayout;
    
    QPushButton *btnSwapRows = new QPushButton("ПОМЕНЯТЬ СТРОКИ");
    QPushButton *btnMulRow = new QPushButton("УМНОЖИТЬ СТРОКУ");
    QPushButton *btnAddRow = new QPushButton("ПРИБАВИТЬ СТРОКУ");
    
    btnSwapRows->setMinimumHeight(40);
    btnMulRow->setMinimumHeight(40);
    btnAddRow->setMinimumHeight(40);
    
    rowLayout1->addWidget(btnSwapRows);
    rowLayout1->addWidget(btnMulRow);
    rowLayout1->addStretch();
    
    rowLayout2->addWidget(btnAddRow);
    rowLayout2->addStretch();
    
    rowMainLayout->addLayout(rowLayout1);
    rowMainLayout->addLayout(rowLayout2);
    rowMainLayout->addStretch();
    
    operationsTabs->addTab(rowTab, "СТРОКИ");
    
    // ========== ВКЛАДКА "СТОЛБЦЫ" ==========
    QWidget *colTab = new QWidget;
    QVBoxLayout *colMainLayout = new QVBoxLayout(colTab);
    
    QHBoxLayout *colLayout1 = new QHBoxLayout;
    QHBoxLayout *colLayout2 = new QHBoxLayout;
    
    QPushButton *btnSwapCols = new QPushButton("ПОМЕНЯТЬ СТОЛБЦЫ");
    QPushButton *btnMulCol = new QPushButton("УМНОЖИТЬ СТОЛБЕЦ");
    QPushButton *btnAddCol = new QPushButton("ПРИБАВИТЬ СТОЛБЕЦ");
    
    btnSwapCols->setMinimumHeight(40);
    btnMulCol->setMinimumHeight(40);
    btnAddCol->setMinimumHeight(40);
    
    colLayout1->addWidget(btnSwapCols);
    colLayout1->addWidget(btnMulCol);
    colLayout1->addStretch();
    
    colLayout2->addWidget(btnAddCol);
    colLayout2->addStretch();
    
    colMainLayout->addLayout(colLayout1);
    colMainLayout->addLayout(colLayout2);
    colMainLayout->addStretch();
    
    operationsTabs->addTab(colTab, "СТОЛБЦЫ");
    
    operationsLayout->addWidget(operationsTabs);
    scrollArea->setWidget(operationsWidget);
    mainLayout->addWidget(scrollArea);
    
    // ========== ПОДКЛЮЧЕНИЕ СИГНАЛОВ ==========
    connect(listMatrices, &QListWidget::itemClicked, this, &MainWindowMatrix::onMatrixClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowMatrix::onCreateMatrix);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowMatrix::onDeleteMatrix);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindowMatrix::onRefreshList);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindowMatrix::onEditElement);
    connect(btnShow, &QPushButton::clicked, this, &MainWindowMatrix::onShowMatrix);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindowMatrix::onAdd);
    connect(btnMulScalar, &QPushButton::clicked, this, &MainWindowMatrix::onMultiplyByScalar);
    connect(btnNormL1, &QPushButton::clicked, this, &MainWindowMatrix::onNormL1);
    connect(btnNormInf, &QPushButton::clicked, this, &MainWindowMatrix::onNormInf);
    connect(btnNormL2, &QPushButton::clicked, this, &MainWindowMatrix::onNormL2);
    connect(btnSwapRows, &QPushButton::clicked, this, &MainWindowMatrix::onSwapRows);
    connect(btnSwapCols, &QPushButton::clicked, this, &MainWindowMatrix::onSwapCols);
    connect(btnMulRow, &QPushButton::clicked, this, &MainWindowMatrix::onMultiplyRow);
    connect(btnMulCol, &QPushButton::clicked, this, &MainWindowMatrix::onMultiplyCol);
    connect(btnAddRow, &QPushButton::clicked, this, &MainWindowMatrix::onAddRowToRow);
    connect(btnAddCol, &QPushButton::clicked, this, &MainWindowMatrix::onAddColToCol);
    
    // Стилизация
    this->setStyleSheet(
        "QMainWindow { background-color: black; }"
        "QGroupBox { color: white; border: 2px solid white; border-radius: 5px; margin-top: 12px; font-weight: bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; color: white; }"
        "QPushButton { background-color: black; color: white; border: 2px solid white; border-radius: 5px; padding: 6px; font-weight: bold; }"
        "QPushButton:hover { background-color: white; color: black; }"
        "QPushButton:pressed { background-color: #666666; color: white; }"
        "QLabel { color: white; background-color: black; font-weight: bold; }"
        "QTextEdit { background-color: black; color: white; border: 2px solid white; border-radius: 3px; font-family: 'Courier New'; }"
        "QListWidget { background-color: black; color: white; border: 2px solid white; border-radius: 3px; }"
        "QListWidget::item:selected { background-color: white; color: black; }"
        "QListWidget::item:hover { background-color: #333333; color: white; }"
        "QTabWidget::pane { background-color: black; border: 2px solid white; border-radius: 3px; }"
        "QTabBar::tab { background-color: black; color: white; padding: 8px 15px; margin-right: 2px; border: 2px solid white; border-bottom: none; }"
        "QTabBar::tab:selected { background-color: white; color: black; }"
        "QTabBar::tab:hover { background-color: #333333; color: white; }"
        "QComboBox { background-color: black; color: white; border: 2px solid white; border-radius: 3px; padding: 5px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox::down-arrow { image: none; border: none; }"
        "QComboBox QAbstractItemView { background-color: black; color: white; border: 2px solid white; }"
    );
}

void MainWindowMatrix::updateMatrixList() {
    listMatrices->clear();
    for (auto it = matrices.begin(); it != matrices.end(); ++it) {
        listMatrices->addItem(it.key());
    }
}

void MainWindowMatrix::updateCurrentDisplay() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        textDisplay->setPlainText("МАТРИЦА НЕ ВЫБРАНА");
        return;
    }
    
    int rows = currentMatrix->getRows();
    int cols = currentMatrix->getCols();
    
    QString display;
    display += QString("ТИП: %1\n").arg(currentMatrix->getTypeString());
    display += QString("РАЗМЕР: %1 x %2\n\n").arg(rows).arg(cols);
    
    // Отображаем матрицу в зависимости от типа
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (mat) displayMatrixInt(mat, rows, cols, display);
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (mat) displayMatrixDouble(mat, rows, cols, display);
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (mat) displayMatrixComplex(mat, rows, cols, display);
    }
    
    textDisplay->setPlainText(display);
    updateInfo();
}

void MainWindowMatrix::updateInfo() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        labelInfo->setText("РАЗМЕР: -");
        return;
    }
    int rows = currentMatrix->getRows();
    int cols = currentMatrix->getCols();
    labelInfo->setText(QString("РАЗМЕР: %1 x %2").arg(rows).arg(cols));
}

// ==================== СЛОТЫ ====================

void MainWindowMatrix::onCreateMatrix() {
    // Выбор типа матрицы
    QStringList matrixTypes = {"ПРЯМОУГОЛЬНАЯ", "КВАДРАТНАЯ", "ВЕРХНЕТРЕУГОЛЬНАЯ", "НИЖНЕТРЕУГОЛЬНАЯ"};
    bool ok;
    QString typeStr = QInputDialog::getItem(this, "ТИП МАТРИЦЫ", "ВЫБЕРИТЕ ТИП МАТРИЦЫ:", matrixTypes, 0, false, &ok);
    if (!ok) return;
    
    int matrixType = matrixTypes.indexOf(typeStr);
    
    // Выбор типа данных
    DataType dataType;
    int dataTypeIndex = comboDataType->currentIndex();
    switch (dataTypeIndex) {
        case 0: dataType = DataType::Int; break;
        case 1: dataType = DataType::Double; break;
        default: dataType = DataType::Complex; break;
    }
    
    int rows = QInputDialog::getInt(this, "РАЗМЕР", "КОЛИЧЕСТВО СТРОК:", 2, 1, 10, 1, &ok);
    if (!ok) return;
    
    int cols = rows;
    if (matrixType == 0) {
        cols = QInputDialog::getInt(this, "РАЗМЕР", "КОЛИЧЕСТВО СТОЛБЦОВ:", 2, 1, 10, 1, &ok);
        if (!ok) return;
    }
    
    if ((matrixType == 1 || matrixType == 2 || matrixType == 3) && rows != cols) {
        showError("КВАДРАТНАЯ/ТРЕУГОЛЬНАЯ МАТРИЦА ДОЛЖНА БЫТЬ КВАДРАТНОЙ");
        return;
    }
    
    QString name = QInputDialog::getText(this, "ИМЯ МАТРИЦЫ", "ВВЕДИТЕ ИМЯ:");
    if (name.isEmpty() || matrices.contains(name)) {
        showError("НЕВАЛИДНОЕ ИМЯ ИЛИ ИМЯ УЖЕ СУЩЕСТВУЕТ");
        return;
    }
    
    try {
        MatrixWrapper* wrapper = createMatrixByType(dataType, matrixType, rows, cols);
        if (wrapper && wrapper->isValid()) {
            // Заполняем матрицу значениями
            if (dataType == DataType::Int) {
                auto* mat = std::get<std::unique_ptr<Matrix<int>>>(wrapper->matrix).get();
                fillMatrixDialog(mat, rows, cols);
            } else if (dataType == DataType::Double) {
                auto* mat = std::get<std::unique_ptr<Matrix<double>>>(wrapper->matrix).get();
                fillMatrixDialog(mat, rows, cols);
            } else {
                auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(wrapper->matrix).get();
                fillMatrixDialog(mat, rows, cols);
            }
            
            matrices.insert(name, wrapper);
            updateMatrixList();
            showInfo("МАТРИЦА СОЗДАНА");
        } else {
            delete wrapper;
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onDeleteMatrix() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int reply = QMessageBox::question(this, "ПОДТВЕРЖДЕНИЕ", 
        QString("УДАЛИТЬ МАТРИЦУ '%1'?").arg(currentName), 
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        delete currentMatrix;
        matrices.remove(currentName);
        currentMatrix = nullptr;
        currentName.clear();
        updateMatrixList();
        textDisplay->clear();
        labelCurrent->setText("ТЕКУЩАЯ МАТРИЦА: НЕ ВЫБРАНА");
        labelNormResult->setText("НОРМЫ: НЕ ВЫЧИСЛЕНЫ");
        showInfo("МАТРИЦА УДАЛЕНА");
    }
}

void MainWindowMatrix::onRefreshList() {
    updateMatrixList();
    if (currentMatrix && currentMatrix->isValid()) {
        updateCurrentDisplay();
    }
}

void MainWindowMatrix::onMatrixClicked(QListWidgetItem *item) {
    QString name = item->text();
    auto it = matrices.find(name);
    if (it != matrices.end()) {
        currentName = name;
        currentMatrix = it.value();
        labelCurrent->setText(QString("ТЕКУЩАЯ МАТРИЦА: %1").arg(name));
        updateCurrentDisplay();
    }
}

void MainWindowMatrix::onEditElement() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    // Проверяем, является ли матрица треугольной
    bool isTriangular = false;
    bool isUpper = false;
    int size = currentMatrix->getRows();
    
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (auto* tri = dynamic_cast<TriangleMatrix<int>*>(mat)) {
            isTriangular = true;
            isUpper = (tri->getType() == MatrixType::Upper);
        }
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (auto* tri = dynamic_cast<TriangleMatrix<double>*>(mat)) {
            isTriangular = true;
            isUpper = (tri->getType() == MatrixType::Upper);
        }
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (auto* tri = dynamic_cast<TriangleMatrix<std::complex<double>>*>(mat)) {
            isTriangular = true;
            isUpper = (tri->getType() == MatrixType::Upper);
        }
    }
    
    int rows = currentMatrix->getRows();
    int cols = currentMatrix->getCols();
    
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ (I):", 0, 0, rows - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА (J):", 0, 0, cols - 1, 1, &ok2);
    if (!ok2) return;
    
    // Для треугольной матрицы проверяем доступность позиции
    if (isTriangular) {
        bool isInTriangle = isUpper ? (i <= j) : (i >= j);
        
        if (!isInTriangle) {
            QMessageBox::warning(this, "НЕДОСТУПНАЯ ПОЗИЦИЯ", 
                QString("ЭЛЕМЕНТ [%1][%2] НАХОДИТСЯ ВНЕ ТРЕУГОЛЬНОЙ ОБЛАСТИ.\n"
                        "ЭТА ПОЗИЦИЯ ВСЕГДА РАВНА 0 И НЕ МОЖЕТ БЫТЬ ИЗМЕНЕНА.")
                        .arg(i).arg(j));
            return;
        }
    }
    
    // Получаем текущее значение
    QString currentStr;
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (mat) {
            try {
                currentStr = valueToString(mat->get(i, j));
            } catch (const std::exception& e) {
                currentStr = "0";
            }
        }
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (mat) {
            try {
                currentStr = valueToString(mat->get(i, j));
            } catch (const std::exception& e) {
                currentStr = "0";
            }
        }
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (mat) {
            try {
                currentStr = valueToString(mat->get(i, j));
            } catch (const std::exception& e) {
                currentStr = "(0,0)";
            }
        }
    }
    
    QString newStr = QInputDialog::getText(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        QString("ВВЕДИТЕ НОВОЕ ЗНАЧЕНИЕ ДЛЯ ЭЛЕМЕНТА [%1][%2]:").arg(i).arg(j),
        QLineEdit::Normal, currentStr);
    if (newStr.isEmpty()) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) {
                int newValue = parseValue<int>(newStr);
                mat->set(i, j, newValue);
                updateCurrentDisplay();
                showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
            }
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) {
                double newValue = parseValue<double>(newStr);
                mat->set(i, j, newValue);
                updateCurrentDisplay();
                showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
            }
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) {
                auto newValue = parseValue<std::complex<double>>(newStr);
                mat->set(i, j, newValue);
                updateCurrentDisplay();
                showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН").arg(i).arg(j));
            }
        }
    } catch (const IndexOutOFBoundsException& e) {
        // Специальная обработка для треугольной матрицы
        QString errorMsg = e.what();
        if (errorMsg.contains("Cannot set non-zero value outside triangular matrix")) {
            QMessageBox::warning(this, "ОШИБКА", 
                QString("НЕЛЬЗЯ УСТАНОВИТЬ НЕНУЛЕВОЕ ЗНАЧЕНИЕ ВНЕ ТРЕУГОЛЬНОЙ ОБЛАСТИ.\n"
                        "ПОЗИЦИЯ [%1][%2] ВСЕГДА ДОЛЖНА БЫТЬ РАВНА 0.\n"
                        "ПОЖАЛУЙСТА, ВВЕДИТЕ 0 ИЛИ ВЫБЕРИТЕ ДРУГУЮ ПОЗИЦИЮ.")
                        .arg(i).arg(j));
        } else {
            showError(e.what());
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onShowMatrix() {
    updateCurrentDisplay();
}

void MainWindowMatrix::onAdd() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    QStringList names = matrices.keys();
    names.removeOne(currentName);
    
    if (names.isEmpty()) {
        showError("НЕТ ДРУГИХ МАТРИЦ ДЛЯ СЛОЖЕНИЯ");
        return;
    }
    
    bool ok;
    QString otherName = QInputDialog::getItem(this, "ВЫБОР МАТРИЦЫ", 
        "ВЫБЕРИТЕ МАТРИЦУ ДЛЯ СЛОЖЕНИЯ:", names, 0, false, &ok);
    if (!ok) return;
    
    MatrixWrapper* other = matrices[otherName];
    
    // Проверяем совместимость типов
    if (currentMatrix->dataType != other->dataType) {
        showError("НЕЛЬЗЯ СКЛАДЫВАТЬ МАТРИЦЫ РАЗНЫХ ТИПОВ ДАННЫХ");
        return;
    }
    
    try {
        MatrixWrapper* result = addMatrices(currentMatrix, other);
        
        if (result && result->isValid()) {
            QString newName = QInputDialog::getText(this, "СОХРАНИТЬ РЕЗУЛЬТАТ", 
                "ВВЕДИТЕ ИМЯ НОВОЙ МАТРИЦЫ:");
            
            if (!newName.isEmpty() && !matrices.contains(newName)) {
                matrices.insert(newName, result);
                updateMatrixList();
                showInfo("РЕЗУЛЬТАТ СЛОЖЕНИЯ СОХРАНЁН");
            } else {
                delete result;
            }
        } else {
            delete result;
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyByScalar() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    QString scalarStr = QInputDialog::getText(this, "УМНОЖИТЬ НА СКАЛЯР", 
        "ВВЕДИТЕ СКАЛЯР:");
    if (scalarStr.isEmpty()) return;
    
    try {
        MatrixWrapper* result = multiplyByScalar(currentMatrix, scalarStr);
        
        if (result && result->isValid()) {
            QString newName = QInputDialog::getText(this, "СОХРАНИТЬ РЕЗУЛЬТАТ", 
                "ВВЕДИТЕ ИМЯ НОВОЙ МАТРИЦЫ:");
            
            if (!newName.isEmpty() && !matrices.contains(newName)) {
                matrices.insert(newName, result);
                updateMatrixList();
                showInfo("РЕЗУЛЬТАТ УМНОЖЕНИЯ СОХРАНЁН");
            } else {
                delete result;
            }
        } else {
            delete result;
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onNormL1() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    double norm = 0;
    
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL1();
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL1();
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL1();
    }
    
    labelNormResult->setText(QString("L1 НОРМА = %1").arg(norm, 0, 'g', 10));
    showInfo(QString("L1 НОРМА = %1").arg(norm, 0, 'g', 10));
}

void MainWindowMatrix::onNormInf() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    double norm = 0;
    
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normInf();
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normInf();
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normInf();
    }
    
    labelNormResult->setText(QString("L∞ НОРМА = %1").arg(norm, 0, 'g', 10));
    showInfo(QString("L∞ НОРМА = %1").arg(norm, 0, 'g', 10));
}

void MainWindowMatrix::onNormL2() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    double norm = 0;
    
    if (currentMatrix->dataType == DataType::Int) {
        auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL2();
    } else if (currentMatrix->dataType == DataType::Double) {
        auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL2();
    } else {
        auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
        if (mat) norm = mat->normL2();
    }
    
    labelNormResult->setText(QString("L2 НОРМА (ФРОБЕНИУСА) = %1").arg(norm, 0, 'g', 10));
    showInfo(QString("L2 НОРМА = %1").arg(norm, 0, 'g', 10));
}

void MainWindowMatrix::onSwapRows() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int rows = currentMatrix->getRows();
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТРОКИ", 
        "ВВЕДИТЕ НОМЕР ПЕРВОЙ СТРОКИ:", 0, 0, rows - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТРОКИ", 
        "ВВЕДИТЕ НОМЕР ВТОРОЙ СТРОКИ:", 0, 0, rows - 1, 1, &ok2);
    if (!ok2) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) mat->swapRows(i, j);
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) mat->swapRows(i, j);
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) mat->swapRows(i, j);
        }
        updateCurrentDisplay();
        showInfo(QString("СТРОКИ %1 И %2 ПОМЕНЯНЫ МЕСТАМИ").arg(i).arg(j));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onSwapCols() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int cols = currentMatrix->getCols();
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТОЛБЦЫ", 
        "ВВЕДИТЕ НОМЕР ПЕРВОГО СТОЛБЦА:", 0, 0, cols - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТОЛБЦЫ", 
        "ВВЕДИТЕ НОМЕР ВТОРОГО СТОЛБЦА:", 0, 0, cols - 1, 1, &ok2);
    if (!ok2) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) mat->swapCols(i, j);
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) mat->swapCols(i, j);
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) mat->swapCols(i, j);
        }
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЦЫ %1 И %2 ПОМЕНЯНЫ МЕСТАМИ").arg(i).arg(j));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyRow() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int rows = currentMatrix->getRows();
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "УМНОЖИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ:", 0, 0, rows - 1, 1, &ok1);
    if (!ok1) return;
    
    QString scalarStr = QInputDialog::getText(this, "УМНОЖИТЬ СТРОКУ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ:");
    if (scalarStr.isEmpty()) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) {
                int scalar = parseValue<int>(scalarStr);
                mat->multiplyRow(i, scalar);
            }
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) {
                double scalar = parseValue<double>(scalarStr);
                mat->multiplyRow(i, scalar);
            }
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) {
                auto scalar = parseValue<std::complex<double>>(scalarStr);
                mat->multiplyRow(i, scalar);
            }
        }
        updateCurrentDisplay();
        showInfo(QString("СТРОКА %1 УМНОЖЕНА").arg(i));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyCol() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int cols = currentMatrix->getCols();
    bool ok1, ok2;
    int j = QInputDialog::getInt(this, "УМНОЖИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА:", 0, 0, cols - 1, 1, &ok1);
    if (!ok1) return;
    
    QString scalarStr = QInputDialog::getText(this, "УМНОЖИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ:");
    if (scalarStr.isEmpty()) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) {
                int scalar = parseValue<int>(scalarStr);
                mat->multiplyCol(j, scalar);
            }
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) {
                double scalar = parseValue<double>(scalarStr);
                mat->multiplyCol(j, scalar);
            }
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) {
                auto scalar = parseValue<std::complex<double>>(scalarStr);
                mat->multiplyCol(j, scalar);
            }
        }
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЕЦ %1 УМНОЖЕН").arg(j));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onAddRowToRow() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int rows = currentMatrix->getRows();
    bool ok1, ok2;
    int source = QInputDialog::getInt(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ-ИСТОЧНИКА:", 0, 0, rows - 1, 1, &ok1);
    if (!ok1) return;
    
    int target = QInputDialog::getInt(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ-ПРИЁМНИКА:", 0, 0, rows - 1, 1, &ok2);
    if (!ok2) return;
    
    QString scalarStr = QInputDialog::getText(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):");
    if (scalarStr.isEmpty()) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) {
                int scalar = parseValue<int>(scalarStr);
                mat->addRowToRow(source, target, scalar);
            }
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) {
                double scalar = parseValue<double>(scalarStr);
                mat->addRowToRow(source, target, scalar);
            }
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) {
                auto scalar = parseValue<std::complex<double>>(scalarStr);
                mat->addRowToRow(source, target, scalar);
            }
        }
        updateCurrentDisplay();
        showInfo(QString("СТРОКА %1 = СТРОКА %1 + СКАЛЯР * СТРОКА %2").arg(target).arg(source));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onAddColToCol() {
    if (!currentMatrix || !currentMatrix->isValid()) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int cols = currentMatrix->getCols();
    bool ok1, ok2;
    int source = QInputDialog::getInt(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА-ИСТОЧНИКА:", 0, 0, cols - 1, 1, &ok1);
    if (!ok1) return;
    
    int target = QInputDialog::getInt(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА-ПРИЁМНИКА:", 0, 0, cols - 1, 1, &ok2);
    if (!ok2) return;
    
    QString scalarStr = QInputDialog::getText(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):");
    if (scalarStr.isEmpty()) return;
    
    try {
        if (currentMatrix->dataType == DataType::Int) {
            auto* mat = std::get<std::unique_ptr<Matrix<int>>>(currentMatrix->matrix).get();
            if (mat) {
                int scalar = parseValue<int>(scalarStr);
                mat->addColToCol(source, target, scalar);
            }
        } else if (currentMatrix->dataType == DataType::Double) {
            auto* mat = std::get<std::unique_ptr<Matrix<double>>>(currentMatrix->matrix).get();
            if (mat) {
                double scalar = parseValue<double>(scalarStr);
                mat->addColToCol(source, target, scalar);
            }
        } else {
            auto* mat = std::get<std::unique_ptr<Matrix<std::complex<double>>>>(currentMatrix->matrix).get();
            if (mat) {
                auto scalar = parseValue<std::complex<double>>(scalarStr);
                mat->addColToCol(source, target, scalar);
            }
        }
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЕЦ %1 = СТОЛБЕЦ %1 + СКАЛЯР * СТОЛБЕЦ %2").arg(target).arg(source));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}