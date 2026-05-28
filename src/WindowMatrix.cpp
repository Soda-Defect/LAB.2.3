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

MainWindowMatrix::MainWindowMatrix(QWidget *parent) 
    : QMainWindow(parent), currentMatrix(nullptr) {
    setupUI();
    updateMatrixList();
}

MainWindowMatrix::~MainWindowMatrix() {
    for (auto mat : matrices) {
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

void MainWindowMatrix::setupUI() {
    resize(950, 750);
    
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
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
    
    labelCurrent->setMinimumWidth(220);
    labelInfo->setMinimumWidth(150);
    labelNormResult->setMinimumWidth(220);
    
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
    
    // ========== ЧЁРНО-БЕЛАЯ СТИЛИЗАЦИЯ ==========
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
        
        "QScrollArea { background-color: black; border: none; }"
        "QScrollBar:vertical { background-color: black; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background-color: white; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background-color: black; }"
        
        "QMessageBox { background-color: black; }"
        "QMessageBox QLabel { color: white; background-color: black; }"
        "QMessageBox QPushButton { background-color: black; color: white; border: 2px solid white; min-width: 80px; }"
        "QMessageBox QPushButton:hover { background-color: white; color: black; }"
        
        "QInputDialog { background-color: black; }"
        "QInputDialog QLabel { color: white; background-color: black; }"
        "QInputDialog QLineEdit { background-color: black; color: white; border: 2px solid white; }"
        "QInputDialog QPushButton { background-color: black; color: white; border: 2px solid white; }"
        "QInputDialog QPushButton:hover { background-color: white; color: black; }"
    );
}

void MainWindowMatrix::updateMatrixList() {
    listMatrices->clear();
    for (const QString& name : matrices.keys()) {
        listMatrices->addItem(name);
    }
}

void MainWindowMatrix::updateCurrentDisplay() {
    if (!currentMatrix) {
        textDisplay->setPlainText("МАТРИЦА НЕ ВЫБРАНА");
        return;
    }
    
    QString display;
    int rows = currentMatrix->getRows();
    int cols = currentMatrix->getCols();
    
    display += QString("ТИП: %1\n").arg(matrixTypeToString(currentMatrix));
    display += QString("РАЗМЕР: %1 x %2\n\n").arg(rows).arg(cols);
    
    // Находим максимальную ширину числа для выравнивания
    int maxWidth = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            QString val = QString::number(currentMatrix->get(i, j));
            if (val.length() > maxWidth) maxWidth = val.length();
        }
    }
    maxWidth = qMin(maxWidth, 12);
    
    for (int i = 0; i < rows; i++) {
        display += "[ ";
        for (int j = 0; j < cols; j++) {
            display += QString("%1").arg(currentMatrix->get(i, j), maxWidth);
            if (j < cols - 1) display += " ";
        }
        display += " ]\n";
    }
    
    textDisplay->setPlainText(display);
    updateInfo();
}

void MainWindowMatrix::updateInfo() {
    if (!currentMatrix) {
        labelInfo->setText("РАЗМЕР: -");
        return;
    }
    int rows = currentMatrix->getRows();
    int cols = currentMatrix->getCols();
    labelInfo->setText(QString("РАЗМЕР: %1 x %2").arg(rows).arg(cols));
}

QString MainWindowMatrix::matrixTypeToString(Matrix<double>* mat) {
    if (dynamic_cast<SquareMatrix<double>*>(mat)) {
        return "КВАДРАТНАЯ МАТРИЦА";
    } else if (dynamic_cast<TriangleMatrix<double>*>(mat)) {
        TriangleMatrix<double>* tri = dynamic_cast<TriangleMatrix<double>*>(mat);
        if (tri->getType() == MatrixType::Upper) {
            return "ВЕРХНЕТРЕУГОЛЬНАЯ МАТРИЦА";
        } else {
            return "НИЖНЕТРЕУГОЛЬНАЯ МАТРИЦА";
        }
    }
    return "ПРЯМОУГОЛЬНАЯ МАТРИЦА";
}

Matrix<double>* MainWindowMatrix::createMatrixByType(int type, int rows, int cols) {
    switch (type) {
        case 0: return new RectangularMatrix<double>(rows, cols);
        case 1: return new SquareMatrix<double>(rows);
        case 2: return new TriangleMatrix<double>(rows, MatrixType::Upper);
        case 3: return new TriangleMatrix<double>(rows, MatrixType::Lower);
        default: return nullptr;
    }
}

// ==================== СЛОТЫ ====================

void MainWindowMatrix::onCreateMatrix() {
    QStringList types = {"ПРЯМОУГОЛЬНАЯ МАТРИЦА", "КВАДРАТНАЯ МАТРИЦА", "ВЕРХНЕТРЕУГОЛЬНАЯ МАТРИЦА", "НИЖНЕТРЕУГОЛЬНАЯ МАТРИЦА"};
    bool ok;
    QString typeStr = QInputDialog::getItem(this, "ТИП МАТРИЦЫ", "ВЫБЕРИТЕ ТИП МАТРИЦЫ:", types, 0, false, &ok);
    if (!ok) return;
    int type = types.indexOf(typeStr);
    
    int rows = QInputDialog::getInt(this, "РАЗМЕР", "КОЛИЧЕСТВО СТРОК:", 2, 1, 10, 1, &ok);
    if (!ok) return;
    
    int cols = rows;
    if (type == 0) {
        cols = QInputDialog::getInt(this, "РАЗМЕР", "КОЛИЧЕСТВО СТОЛБЦОВ:", 2, 1, 10, 1, &ok);
        if (!ok) return;
    }
    
    if ((type == 1 || type == 2 || type == 3) && rows != cols) {
        showError("КВАДРАТНАЯ/ТРЕУГОЛЬНАЯ МАТРИЦА ДОЛЖНА БЫТЬ КВАДРАТНОЙ");
        return;
    }
    
    QString name = QInputDialog::getText(this, "ИМЯ МАТРИЦЫ", "ВВЕДИТЕ ИМЯ:");
    if (name.isEmpty() || matrices.contains(name)) {
        showError("НЕВАЛИДНОЕ ИМЯ ИЛИ ИМЯ УЖЕ СУЩЕСТВУЕТ");
        return;
    }
    
    Matrix<double>* mat = createMatrixByType(type, rows, cols);
    if (!mat) return;
    
    int fill = QMessageBox::question(this, "ЗАПОЛНЕНИЕ", "ЗАПОЛНИТЬ МАТРИЦУ ЗНАЧЕНИЯМИ?", QMessageBox::Yes | QMessageBox::No);
    
    if (fill == QMessageBox::Yes) {
        double defValue = QInputDialog::getDouble(this, "ЗНАЧЕНИЕ ПО УМОЛЧАНИЮ", "ВВЕДИТЕ ЗНАЧЕНИЕ:", 0);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                mat->set(i, j, defValue);
            }
        }
    } else {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double val = QInputDialog::getDouble(this, "ВВОД ЭЛЕМЕНТА", QString("ЭЛЕМЕНТ [%1][%2]:").arg(i).arg(j));
                mat->set(i, j, val);
            }
        }
    }
    
    matrices[name] = mat;
    updateMatrixList();
    showInfo("МАТРИЦА СОЗДАНА");
}

void MainWindowMatrix::onDeleteMatrix() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    int reply = QMessageBox::question(this, "ПОДТВЕРЖДЕНИЕ", QString("УДАЛИТЬ МАТРИЦУ '%1'?").arg(currentName), QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        delete currentMatrix;
        matrices.remove(currentName);
        currentMatrix = nullptr;
        currentName.clear();
        updateMatrixList();
        textDisplay->clear();
        labelCurrent->setText("ТЕКУЩАЯ МАТРИЦА: НЕ ВЫБРАНА");
        labelNormResult->setText("НОРМЫ: НЕ ВЫЧИСЛЕНЫ");
    }
}

void MainWindowMatrix::onRefreshList() {
    updateMatrixList();
    if (currentMatrix) {
        updateCurrentDisplay();
    }
}

void MainWindowMatrix::onMatrixClicked(QListWidgetItem *item) {
    QString name = item->text();
    if (matrices.contains(name)) {
        currentName = name;
        currentMatrix = matrices[name];
        labelCurrent->setText(QString("ТЕКУЩАЯ МАТРИЦА: %1").arg(name));
        updateCurrentDisplay();
    }
}

void MainWindowMatrix::onEditElement() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2, ok3;
    int i = QInputDialog::getInt(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ (I):", 0, 0, currentMatrix->getRows() - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА (J):", 0, 0, currentMatrix->getCols() - 1, 1, &ok2);
    if (!ok2) return;
    
    double value = QInputDialog::getDouble(this, "ИЗМЕНИТЬ ЭЛЕМЕНТ", 
        QString("ВВЕДИТЕ НОВОЕ ЗНАЧЕНИЕ ДЛЯ ЭЛЕМЕНТА [%1][%2]:").arg(i).arg(j), 
        currentMatrix->get(i, j), -1000000, 1000000, 2, &ok3);
    if (!ok3) return;
    
    try {
        currentMatrix->set(i, j, value);
        updateCurrentDisplay();
        showInfo(QString("ЭЛЕМЕНТ [%1][%2] ИЗМЕНЁН НА %3").arg(i).arg(j).arg(value));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onShowMatrix() {
    updateCurrentDisplay();
}

void MainWindowMatrix::onAdd() {
    if (!currentMatrix) {
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
    QString otherName = QInputDialog::getItem(this, "ВЫБОР МАТРИЦЫ", "ВЫБЕРИТЕ МАТРИЦУ ДЛЯ СЛОЖЕНИЯ:", names, 0, false, &ok);
    if (!ok) return;
    
    try {
        Matrix<double>* result = currentMatrix->add(*matrices[otherName]);
        QString newName = QInputDialog::getText(this, "СОХРАНИТЬ РЕЗУЛЬТАТ", "ВВЕДИТЕ ИМЯ НОВОЙ МАТРИЦЫ:");
        
        if (!newName.isEmpty() && !matrices.contains(newName)) {
            matrices[newName] = result;
            updateMatrixList();
            showInfo("РЕЗУЛЬТАТ СЛОЖЕНИЯ СОХРАНЁН");
        } else {
            delete result;
            showError("НЕВАЛИДНОЕ ИМЯ");
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyByScalar() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok;
    double scalar = QInputDialog::getDouble(this, "УМНОЖИТЬ НА СКАЛЯР", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):", 1.0, -1000000, 1000000, 2, &ok);
    if (!ok) return;
    
    try {
        Matrix<double>* result = currentMatrix->multiplyByScalar(scalar);
        QString newName = QInputDialog::getText(this, "СОХРАНИТЬ РЕЗУЛЬТАТ", 
            "ВВЕДИТЕ ИМЯ НОВОЙ МАТРИЦЫ:");
        
        if (!newName.isEmpty() && !matrices.contains(newName)) {
            matrices[newName] = result;
            updateMatrixList();
            showInfo("РЕЗУЛЬТАТ УМНОЖЕНИЯ СОХРАНЁН");
        } else {
            delete result;
            showError("НЕВАЛИДНОЕ ИМЯ");
        }
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onNormL1() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    try {
        double norm = currentMatrix->normL1();
        labelNormResult->setText(QString("L1 НОРМА = %1").arg(norm));
        showInfo(QString("L1 НОРМА = %1").arg(norm));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onNormInf() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    try {
        double norm = currentMatrix->normInf();
        labelNormResult->setText(QString("L∞ НОРМА = %1").arg(norm));
        showInfo(QString("L∞ НОРМА = %1").arg(norm));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onNormL2() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    try {
        double norm = currentMatrix->normL2();
        labelNormResult->setText(QString("L2 НОРМА (ФРОБЕНИУСА) = %1").arg(norm));
        showInfo(QString("L2 НОРМА = %1").arg(norm));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onSwapRows() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТРОКИ", 
        "ВВЕДИТЕ НОМЕР ПЕРВОЙ СТРОКИ:", 0, 0, currentMatrix->getRows() - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТРОКИ", 
        "ВВЕДИТЕ НОМЕР ВТОРОЙ СТРОКИ:", 0, 0, currentMatrix->getRows() - 1, 1, &ok2);
    if (!ok2) return;
    
    try {
        currentMatrix->swapRows(i, j);
        updateCurrentDisplay();
        showInfo(QString("СТРОКИ %1 И %2 ПОМЕНЯНЫ МЕСТАМИ").arg(i).arg(j));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onSwapCols() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТОЛБЦЫ", 
        "ВВЕДИТЕ НОМЕР ПЕРВОГО СТОЛБЦА:", 0, 0, currentMatrix->getCols() - 1, 1, &ok1);
    if (!ok1) return;
    
    int j = QInputDialog::getInt(this, "ПОМЕНЯТЬ СТОЛБЦЫ", 
        "ВВЕДИТЕ НОМЕР ВТОРОГО СТОЛБЦА:", 0, 0, currentMatrix->getCols() - 1, 1, &ok2);
    if (!ok2) return;
    
    try {
        currentMatrix->swapCols(i, j);
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЦЫ %1 И %2 ПОМЕНЯНЫ МЕСТАМИ").arg(i).arg(j));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyRow() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2;
    int i = QInputDialog::getInt(this, "УМНОЖИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ:", 0, 0, currentMatrix->getRows() - 1, 1, &ok1);
    if (!ok1) return;
    
    double scalar = QInputDialog::getDouble(this, "УМНОЖИТЬ СТРОКУ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):", 1.0, -1000000, 1000000, 2, &ok2);
    if (!ok2) return;
    
    try {
        currentMatrix->multiplyRow(i, scalar);
        updateCurrentDisplay();
        showInfo(QString("СТРОКА %1 УМНОЖЕНА НА %2").arg(i).arg(scalar));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onMultiplyCol() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2;
    int j = QInputDialog::getInt(this, "УМНОЖИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА:", 0, 0, currentMatrix->getCols() - 1, 1, &ok1);
    if (!ok1) return;
    
    double scalar = QInputDialog::getDouble(this, "УМНОЖИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):", 1.0, -1000000, 1000000, 2, &ok2);
    if (!ok2) return;
    
    try {
        currentMatrix->multiplyCol(j, scalar);
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЕЦ %1 УМНОЖЕН НА %2").arg(j).arg(scalar));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onAddRowToRow() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2, ok3;
    int source = QInputDialog::getInt(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ-ИСТОЧНИКА:", 0, 0, currentMatrix->getRows() - 1, 1, &ok1);
    if (!ok1) return;
    
    int target = QInputDialog::getInt(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ НОМЕР СТРОКИ-ПРИЁМНИКА:", 0, 0, currentMatrix->getRows() - 1, 1, &ok2);
    if (!ok2) return;
    
    double scalar = QInputDialog::getDouble(this, "ПРИБАВИТЬ СТРОКУ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):\n(СТРОКА_ПРИЁМНИК = СТРОКА_ПРИЁМНИК + СКАЛЯР * СТРОКА_ИСТОЧНИК)", 
        1.0, -1000000, 1000000, 2, &ok3);
    if (!ok3) return;
    
    try {
        currentMatrix->addRowToRow(source, target, scalar);
        updateCurrentDisplay();
        showInfo(QString("СТРОКА %1 = СТРОКА %1 + %2 * СТРОКА %3").arg(target).arg(scalar).arg(source));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void MainWindowMatrix::onAddColToCol() {
    if (!currentMatrix) {
        showError("НЕТ ВЫБРАННОЙ МАТРИЦЫ");
        return;
    }
    
    bool ok1, ok2, ok3;
    int source = QInputDialog::getInt(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА-ИСТОЧНИКА:", 0, 0, currentMatrix->getCols() - 1, 1, &ok1);
    if (!ok1) return;
    
    int target = QInputDialog::getInt(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ НОМЕР СТОЛБЦА-ПРИЁМНИКА:", 0, 0, currentMatrix->getCols() - 1, 1, &ok2);
    if (!ok2) return;
    
    double scalar = QInputDialog::getDouble(this, "ПРИБАВИТЬ СТОЛБЕЦ", 
        "ВВЕДИТЕ МНОЖИТЕЛЬ (СКАЛЯР):\n(СТОЛБЕЦ_ПРИЁМНИК = СТОЛБЕЦ_ПРИЁМНИК + СКАЛЯР * СТОЛБЕЦ_ИСТОЧНИК)", 
        1.0, -1000000, 1000000, 2, &ok3);
    if (!ok3) return;
    
    try {
        currentMatrix->addColToCol(source, target, scalar);
        updateCurrentDisplay();
        showInfo(QString("СТОЛБЕЦ %1 = СТОЛБЕЦ %1 + %2 * СТОЛБЕЦ %3").arg(target).arg(scalar).arg(source));
    } catch (const std::exception& e) {
        showError(e.what());
    }
}