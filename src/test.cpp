#include "../include/test.h"
#include "../include/DynamicArray.h"
#include "../include/LinkedList.h"
#include "../include/Exceptions.h"
#include "../include/RectangularMatrix.h"
#include "../include/SquareMatrix.h"
#include "../include/TriangleMatrix.h"
#include <QVariant>
#include <QString>
#include <memory>
#include <sstream>

#define TEST(name) \
runTest(name, [&]() -> bool

#define END_TEST() \
        )

#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            output << "  Assertion failed: " << #expr << std::endl; \
            return false; \
    } \
} while(0)

#define ASSERT_THROW(expr, ExceptionType) \
    do { \
        try { \
                expr; \
                output << "  Expected exception " << #ExceptionType << " not thrown" << std::endl; \
                return false; \
        } catch (const ExceptionType&) { \
        } catch (...) { \
                output << "  Wrong exception type for " << #expr << std::endl; \
                return false; \
        } \
    } while(0)

#define ASSERT_NO_THROW(expr) \
        do { \
        try { \
                expr; \
        } catch (const std::exception& e) { \
                output << "  Unexpected exception: " << e.what() << std::endl; \
                return false; \
        } catch (...) { \
                output << "  Unexpected unknown exception" << std::endl; \
                return false; \
        } \
    } while(0)


    void TestRunnerGUI::runTest(const std::string& name, std::function<bool()> test) {
        output << "Тест: " << name << " ... ";
        if (test()) {
            output << "Пройден" << std::endl;
            ++passed;
        } else {
            output << "Ошибка" << std::endl;
            ++failed;
        }
    }

void TestRunnerGUI::printSummary() {
    output << "\n==================\n"
           << "Тесты: " << (passed + failed) << "  "
           << "Пройден: " << passed << "  "
           << "Ошибка: " << failed << "\n"
           << "==================" << std::endl;
}

QString TestRunnerGUI::runAllTests() {
    output.str("");
    output.clear();
    passed = failed = 0;
    output << "Запуск всех тестов \n";

    testDynamicArrayConstructors();
    testDynamicArrayBasicOperations();
    testDynamicArrayInsertErase();
    testDynamicArrayExceptions();
    
    testLinkedListConstructors();
    testLinkedListBasicOperations();
    testLinkedListInsertAt();
    testLinkedListExceptions();
    
    testArraySequenceConstructors();
    testArraySequenceBasicOperations();
    testArraySequenceFunctional();
    testArraySequenceConcatSlice();
    testArraySequenceIterators();
    
    testListSequenceConstructors();
    testListSequenceBasicOperations();
    testListSequenceFunctional();
    testListSequenceConcatSlice();
    testListSequenceIterators();
    
    testSequencePolymorphism();
    
    testZipUnzipInt();
    testZipUnzipDouble();
    testZipUnzipChar();
    testZipUnzipString();
    
    testMapUtils();

    testRectangularMatrix();
    testSquareMatrix();
    testTriangleMatrix();
    
    printSummary();
    return QString::fromStdString(output.str());
}

void TestRunnerGUI::testDynamicArrayConstructors() {
    TEST("DynamicArray: конструктор по умолчанию") {
        DynamicArray<int> arr;
        ASSERT(arr.GetSize() == 0);
        ASSERT(arr.is_empty());
        return true;
    } END_TEST();

    TEST("DynamicArray: конструктор с размером") {
        DynamicArray<int> arr(5);
        ASSERT(arr.GetSize() == 5);
        for (int i = 0; i < 5; i++) ASSERT(arr.Get(i) == 0);
        return true;
    } END_TEST();

    TEST("DynamicArray: конструктор с размером и значением") {
        DynamicArray<int> arr(5, 42);
        ASSERT(arr.GetSize() == 5);
        for (int i = 0; i < 5; i++) ASSERT(arr.Get(i) == 42);
        return true;
    } END_TEST();

    TEST("DynamicArray: конструктор из массива") {
        int items[] = {1, 2, 3, 4, 5};
        DynamicArray<int> arr(items, 5);
        ASSERT(arr.GetSize() == 5);
        for (int i = 0; i < 5; i++) ASSERT(arr.Get(i) == i + 1);
        return true;
    } END_TEST();

    TEST("DynamicArray: конструктор копирования") {
        DynamicArray<int> arr1(5, 10);
        DynamicArray<int> arr2(arr1);
        ASSERT(arr2.GetSize() == 5);
        for (int i = 0; i < 5; i++) ASSERT(arr2.Get(i) == 10);
        arr1.Set(0, 99);
        ASSERT(arr2.Get(0) == 10);
        return true;
    } END_TEST();

    TEST("DynamicArray: оператор присваивания") {
        DynamicArray<int> arr1(3, 5);
        DynamicArray<int> arr2;
        arr2 = arr1;
        ASSERT(arr2.GetSize() == 3);
        for (int i = 0; i < 3; i++) ASSERT(arr2.Get(i) == 5);
        return true;
    } END_TEST();

    TEST("DynamicArray: move конструктор") {
        DynamicArray<int> arr1(3, 7);
        int* oldData = &arr1[0];
        DynamicArray<int> arr2(std::move(arr1));
        ASSERT(arr2.GetSize() == 3);
        ASSERT(arr2[0] == 7);
        ASSERT(arr1.GetSize() == 0);
        ASSERT(arr1.is_empty());
        return true;
    } END_TEST();
}

void TestRunnerGUI::testDynamicArrayBasicOperations() {
    TEST("DynamicArray: push_back") {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(20);
        arr.push_back(30);
        ASSERT(arr.GetSize() == 3);
        ASSERT(arr.Get(0) == 10);
        ASSERT(arr.Get(1) == 20);
        ASSERT(arr.Get(2) == 30);
        return true;
    } END_TEST();

    TEST("DynamicArray: pop_back") {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(20);
        arr.push_back(30);
        int val = arr.pop_back();
        ASSERT(val == 30);
        ASSERT(arr.GetSize() == 2);
        ASSERT(arr.Back() == 20);
        return true;
    } END_TEST();

    TEST("DynamicArray: Resize") {
        DynamicArray<int> arr(3, 5);
        arr.Resize(5);
        ASSERT(arr.GetSize() == 5);
        for (int i = 0; i < 3; i++) ASSERT(arr.Get(i) == 5);
        arr.Resize(2);
        ASSERT(arr.GetSize() == 2);
        return true;
    } END_TEST();

    TEST("DynamicArray: Set и оператор []") {
        DynamicArray<int> arr(3, 0);
        arr.Set(0, 100);
        arr[1] = 200;
        const DynamicArray<int>& constArr = arr;
        ASSERT(arr.Get(0) == 100);
        ASSERT(arr[1] == 200);
        ASSERT(constArr[2] == 0);
        return true;
    } END_TEST();

    TEST("DynamicArray: clear") {
        DynamicArray<int> arr(5, 1);
        arr.clear();
        ASSERT(arr.GetSize() == 0);
        ASSERT(arr.is_empty());
        return true;
    } END_TEST();

    TEST("DynamicArray: Back") {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(20);
        arr.push_back(30);
        ASSERT(arr.Back() == 30);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testDynamicArrayInsertErase() {
    TEST("DynamicArray: insert в начало") {
        DynamicArray<int> arr;
        arr.push_back(20);
        arr.push_back(30);
        arr.insert(0, 10);
        ASSERT(arr.GetSize() == 3);
        ASSERT(arr.Get(0) == 10);
        ASSERT(arr.Get(1) == 20);
        return true;
    } END_TEST();

    TEST("DynamicArray: insert в середину") {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(30);
        arr.insert(1, 20);
        ASSERT(arr.GetSize() == 3);
        ASSERT(arr.Get(1) == 20);
        return true;
    } END_TEST();

    TEST("DynamicArray: insert в конец") {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(20);
        arr.insert(2, 30);
        ASSERT(arr.GetSize() == 3);
        ASSERT(arr.Get(2) == 30);
        return true;
    } END_TEST();

    TEST("DynamicArray: erase из начала") {
        DynamicArray<int> arr;
        for (int i = 1; i <= 5; i++) arr.push_back(i);
        arr.erase(0);
        ASSERT(arr.GetSize() == 4);
        ASSERT(arr.Get(0) == 2);
        return true;
    } END_TEST();

    TEST("DynamicArray: erase из середины") {
        DynamicArray<int> arr;
        for (int i = 1; i <= 5; i++) arr.push_back(i);
        arr.erase(2);
        ASSERT(arr.GetSize() == 4);
        ASSERT(arr.Get(2) == 4);
        return true;
    } END_TEST();

    TEST("DynamicArray: erase из конца") {
        DynamicArray<int> arr;
        for (int i = 1; i <= 5; i++) arr.push_back(i);
        arr.erase(4);
        ASSERT(arr.GetSize() == 4);
        ASSERT(arr.Get(3) == 4);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testDynamicArrayExceptions() {
    TEST("DynamicArray: исключения при доступе") {
        DynamicArray<int> arr;
        arr.push_back(10);
        ASSERT_THROW(arr.Get(-1), IndexOutOFBoundsException);
        ASSERT_THROW(arr.Get(1), IndexOutOFBoundsException);
        ASSERT_THROW(arr.Set(-1, 5), IndexOutOFBoundsException);
        ASSERT_THROW(arr.Set(1, 5), IndexOutOFBoundsException);
        ASSERT_THROW(arr[-1], IndexOutOFBoundsException);
        ASSERT_THROW(arr[1], IndexOutOFBoundsException);
        return true;
    } END_TEST();

    TEST("DynamicArray: исключения при создании") {
        ASSERT_THROW(DynamicArray<int>(-5), SizeException);
        ASSERT_THROW(DynamicArray<int>(-5, 10), SizeException);
        int* items = nullptr;
        ASSERT_THROW(DynamicArray<int>(items, -1), IndexOutOFBoundsException);
        return true;
    } END_TEST();

    TEST("DynamicArray: исключения pop_back/Back из пустого") {
        DynamicArray<int> arr;
        ASSERT_THROW(arr.pop_back(), IndexOutOFBoundsException);
        ASSERT_THROW(arr.Back(), IndexOutOFBoundsException);
        return true;
    } END_TEST();

    TEST("DynamicArray: исключения erase из пустого") {
        DynamicArray<int> arr;
        arr.erase(0);
        ASSERT(arr.GetSize() == 0);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testLinkedListConstructors() {
    TEST("LinkedList: конструктор по умолчанию") {
        LinkedList<int> list;
        ASSERT(list.GetLength() == 0);
        ASSERT(list.IsEmpty());
        return true;
    } END_TEST();

    TEST("LinkedList: конструктор из массива") {
        int items[] = {1, 2, 3, 4};
        LinkedList<int> list(items, 4);
        ASSERT(list.GetLength() == 4);
        ASSERT(list.Get(0) == 1);
        ASSERT(list.Get(3) == 4);
        return true;
    } END_TEST();

    TEST("LinkedList: конструктор копирования") {
        int items[] = {1, 2, 3};
        LinkedList<int> list1(items, 3);
        LinkedList<int> list2(list1);
        ASSERT(list2.GetLength() == 3);
        ASSERT(list2.Get(1) == 2);
        list1.push_back(4);
        ASSERT(list2.GetLength() == 3);
        return true;
    } END_TEST();

    TEST("LinkedList: move конструктор") {
        int items[] = {1, 2, 3};
        LinkedList<int> list1(items, 3);
        LinkedList<int> list2(std::move(list1));
        ASSERT(list2.GetLength() == 3);
        ASSERT(list1.GetLength() == 0);
        ASSERT(list1.IsEmpty());
        return true;
    } END_TEST();

    TEST("LinkedList: оператор присваивания") {
        int items[] = {1, 2, 3};
        LinkedList<int> list1(items, 3);
        LinkedList<int> list2;
        list2 = list1;
        ASSERT(list2.GetLength() == 3);
        ASSERT(list2.Get(0) == 1);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testLinkedListBasicOperations() {
    TEST("LinkedList: push_back") {
        LinkedList<int> list;
        list.push_back(10);
        list.push_back(20);
        list.push_back(30);
        ASSERT(list.GetLength() == 3);
        ASSERT(list.GetFirst() == 10);
        ASSERT(list.GetLast() == 30);
        return true;
    } END_TEST();

    TEST("LinkedList: push_front") {
        LinkedList<int> list;
        list.push_front(30);
        list.push_front(20);
        list.push_front(10);
        ASSERT(list.GetLength() == 3);
        ASSERT(list.GetFirst() == 10);
        ASSERT(list.GetLast() == 30);
        return true;
    } END_TEST();

    TEST("LinkedList: pop_back") {
        LinkedList<int> list;
        list.push_back(10);
        list.push_back(20);
        list.push_back(30);
        list.pop_back();
        ASSERT(list.GetLength() == 2);
        ASSERT(list.GetLast() == 20);
        return true;
    } END_TEST();

    TEST("LinkedList: Get и оператор []") {
        int items[] = {5, 10, 15, 20};
        LinkedList<int> list(items, 4);
        ASSERT(list.Get(0) == 5);
        ASSERT(list.Get(2) == 15);
        ASSERT(list[3] == 20);
        return true;
    } END_TEST();

    TEST("LinkedList: clear") {
        int items[] = {1, 2, 3};
        LinkedList<int> list(items, 3);
        list.clear();
        ASSERT(list.GetLength() == 0);
        ASSERT(list.IsEmpty());
        return true;
    } END_TEST();

    TEST("LinkedList: GetFirst/GetLast") {
        LinkedList<int> list;
        list.push_back(42);
        list.push_back(100);
        ASSERT(list.GetFirst() == 42);
        ASSERT(list.GetLast() == 100);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testLinkedListInsertAt() {
    TEST("LinkedList: insertAt в начало") {
        LinkedList<int> list;
        list.push_back(20);
        list.push_back(30);
        list.insertAt(10, 0);
        ASSERT(list.GetLength() == 3);
        ASSERT(list.GetFirst() == 10);
        return true;
    } END_TEST();

    TEST("LinkedList: insertAt в середину") {
        LinkedList<int> list;
        list.push_back(10);
        list.push_back(30);
        list.insertAt(20, 1);
        ASSERT(list.GetLength() == 3);
        ASSERT(list.Get(1) == 20);
        return true;
    } END_TEST();

    TEST("LinkedList: insertAt в конец") {
        LinkedList<int> list;
        list.push_back(10);
        list.push_back(20);
        list.insertAt(30, 2);
        ASSERT(list.GetLength() == 3);
        ASSERT(list.GetLast() == 30);
        return true;
    } END_TEST();

    TEST("LinkedList: GetSubList") {
        int items[] = {1, 2, 3, 4, 5};
        LinkedList<int> list(items, 5);
        LinkedList<int>* sub = list.GetSubList(1, 3);
        ASSERT(sub->GetLength() == 3);
        ASSERT(sub->Get(0) == 2);
        ASSERT(sub->Get(2) == 4);
        delete sub;
        return true;
    } END_TEST();

    TEST("LinkedList: Concat") {
        int items1[] = {1, 2};
        int items2[] = {3, 4};
        LinkedList<int> list1(items1, 2);
        LinkedList<int> list2(items2, 2);
        LinkedList<int>* result = list1.Concat(&list2);
        ASSERT(result->GetLength() == 4);
        ASSERT(result->Get(0) == 1);
        ASSERT(result->Get(3) == 4);
        delete result;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testLinkedListExceptions() {
    TEST("LinkedList: исключения при доступе") {
        LinkedList<int> list;
        ASSERT_THROW(list.GetFirst(), IndexOutOFBoundsException);
        ASSERT_THROW(list.GetLast(), IndexOutOFBoundsException);
        ASSERT_THROW(list.Get(0), IndexOutOFBoundsException);
        ASSERT_THROW(list[0], IndexOutOFBoundsException);
        return true;
    } END_TEST();

    TEST("LinkedList: исключения при GetSubList") {
        int items[] = {1, 2, 3};
        LinkedList<int> list(items, 3);
        ASSERT_THROW(list.GetSubList(-1, 2), IndexOutOFBoundsException);
        ASSERT_THROW(list.GetSubList(0, 3), IndexOutOFBoundsException);
        ASSERT_THROW(list.GetSubList(2, 1), IndexOutOFBoundsException);
        return true;
    } END_TEST();

    TEST("LinkedList: исключения при insertAt") {
        LinkedList<int> list;
        list.push_back(10);
        ASSERT_THROW(list.insertAt(5, -1), IndexOutOFBoundsException);
        ASSERT_THROW(list.insertAt(5, 2), IndexOutOFBoundsException);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testArraySequenceConstructors() {
    TEST("ArraySequence: конструктор по умолчанию") {
        ArraySequence<int> seq;
        ASSERT(seq.GetLength() == 0);
        return true;
    } END_TEST();

    TEST("ArraySequence: конструктор с размером и значением") {
        ArraySequence<int> seq(5, 42);
        ASSERT(seq.GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(seq.Get(i) == 42);
        return true;
    } END_TEST();

    TEST("ArraySequence: конструктор из массива") {
        int items[] = {1, 2, 3};
        ArraySequence<int> seq(items, 3);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(0) == 1);
        ASSERT(seq.Get(2) == 3);
        return true;
    } END_TEST();

    TEST("ArraySequence: конструктор копирования") {
        ArraySequence<int> seq1(3, 10);
        ArraySequence<int> seq2(seq1);
        seq1.Set(0, 99);
        ASSERT(seq2.Get(0) == 10);
        return true;
    } END_TEST();

    TEST("ArraySequence: конструктор из DynamicArray") {
        DynamicArray<int> arr(3, 7);
        ArraySequence<int> seq(arr);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(0) == 7);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testArraySequenceBasicOperations() {
    TEST("ArraySequence: Append") {
        ArraySequence<int> seq;
        seq.Append(10);
        seq.Append(20);
        seq.Append(30);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(0) == 10);
        ASSERT(seq.Get(2) == 30);
        return true;
    } END_TEST();

    TEST("ArraySequence: Prepend") {
        ArraySequence<int> seq;
        seq.Append(20);
        seq.Append(30);
        seq.Prepend(10);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.GetFirst() == 10);
        return true;
    } END_TEST();

    TEST("ArraySequence: InsertAt") {
        ArraySequence<int> seq;
        seq.Append(10);
        seq.Append(30);
        seq.InsertAt(20, 1);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(1) == 20);
        seq.InsertAt(5, 0);
        ASSERT(seq.GetFirst() == 5);
        seq.InsertAt(40, 4);
        ASSERT(seq.GetLast() == 40);
        return true;
    } END_TEST();

    TEST("ArraySequence: RemoveAt") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        seq.RemoveAt(0);
        ASSERT(seq.GetFirst() == 2);
        seq.RemoveAt(2);
        ASSERT(seq.Get(2) == 5);
        seq.RemoveAt(2);
        ASSERT(seq.GetLength() == 2);
        return true;
    } END_TEST();

    TEST("ArraySequence: Set") {
        ArraySequence<int> seq(3, 0);
        seq.Set(0, 100);
        seq.Set(1, 200);
        seq.Set(2, 300);
        ASSERT(seq.Get(0) == 100);
        ASSERT(seq.Get(2) == 300);
        return true;
    } END_TEST();

    TEST("ArraySequence: GetFirst/GetLast") {
        ArraySequence<int> seq;
        seq.Append(42);
        seq.Append(100);
        ASSERT(seq.GetFirst() == 42);
        ASSERT(seq.GetLast() == 100);
        return true;
    } END_TEST();

    TEST("ArraySequence: GetSubsequence") {
        ArraySequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        Sequence<int>* sub = seq.GetSubsequence(3, 7);
        ASSERT(sub->GetLength() == 5);
        ASSERT(sub->Get(0) == 3);
        ASSERT(sub->Get(4) == 7);
        delete sub;
        return true;
    } END_TEST();

    TEST("ArraySequence: operator[]") {
        ArraySequence<int> seq(3, 5);
        seq[0] = 10;
        seq[1] = 20;
        const ArraySequence<int>& constSeq = seq;
        ASSERT(seq[0] == 10);
        ASSERT(constSeq[2] == 5);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testArraySequenceFunctional() {
    TEST("ArraySequence: Map") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        Sequence<int>* mapped = seq.Map([](const int& x) { return x * x; });
        ASSERT(mapped->GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(mapped->Get(i) == (i + 1) * (i + 1));
        delete mapped;
        return true;
    } END_TEST();

    TEST("ArraySequence: Reduce сумма") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        int sum = seq.Reduce([](const int& a, const int& b) { return a + b; }, 0);
        ASSERT(sum == 15);
        return true;
    } END_TEST();

    TEST("ArraySequence: Reduce произведение") {
        ArraySequence<int> seq;
        seq.Append(1);
        seq.Append(2);
        seq.Append(3);
        seq.Append(4);
        int product = seq.Reduce([](const int& a, const int& b) { return a * b; }, 1);
        ASSERT(product == 24);
        return true;
    } END_TEST();

    TEST("ArraySequence: Reduce строка") {
        ArraySequence<std::string> seq;
        seq.Append("Hello");
        seq.Append(" ");
        seq.Append("World");
        std::string result = seq.Reduce([](const std::string& a, const std::string& b) {
            return a + b;
        }, std::string(""));
        ASSERT(result == "Hello World");
        return true;
    } END_TEST();

    TEST("ArraySequence: Where") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 10; i++) seq.Append(i);
        Sequence<int>* even = seq.Where([](const int& x) { return x % 2 == 0; });
        ASSERT(even->GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(even->Get(i) == (i + 1) * 2);
        delete even;
        return true;
    } END_TEST();

    TEST("ArraySequence: Where с пустым результатом") {
        ArraySequence<int> seq;
        seq.Append(1);
        seq.Append(3);
        seq.Append(5);
        Sequence<int>* result = seq.Where([](const int& x) { return x > 10; });
        ASSERT(result->GetLength() == 0);
        delete result;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testArraySequenceConcatSlice() {
    TEST("ArraySequence: Concat") {
        ArraySequence<int> seq1;
        seq1.Append(1);
        seq1.Append(2);
        ArraySequence<int> seq2;
        seq2.Append(3);
        seq2.Append(4);
        Sequence<int>* result = seq1.Concat(&seq2);
        ASSERT(result->GetLength() == 4);
        ASSERT(result->Get(0) == 1);
        ASSERT(result->Get(3) == 4);
        delete result;
        return true;
    } END_TEST();

    TEST("ArraySequence: Concat с пустыми") {
        ArraySequence<int> seq1;
        ArraySequence<int> seq2;
        seq2.Append(1);
        Sequence<int>* result = seq1.Concat(&seq2);
        ASSERT(result->GetLength() == 1);
        delete result;
        result = seq2.Concat(&seq1);
        ASSERT(result->GetLength() == 1);
        delete result;
        return true;
    } END_TEST();

    TEST("ArraySequence: Slice удаление") {
        ArraySequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(3, 4);
        ASSERT(seq.GetLength() == 6);
        ASSERT(seq.Get(0) == 0);
        ASSERT(seq.Get(1) == 1);
        ASSERT(seq.Get(2) == 2);
        ASSERT(seq.Get(3) == 7);
        ASSERT(seq.Get(4) == 8);
        ASSERT(seq.Get(5) == 9);
        return true;
    } END_TEST();

    TEST("ArraySequence: Slice с отрицательным start") {
        ArraySequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(-3, 2);
        ASSERT(seq.GetLength() == 8);
        ASSERT(seq.Get(0) == 0);
        ASSERT(seq.Get(1) == 1);
        ASSERT(seq.Get(2) == 2);
        ASSERT(seq.Get(3) == 3);
        ASSERT(seq.Get(4) == 4);
        ASSERT(seq.Get(5) == 5);
        ASSERT(seq.Get(6) == 6);
        ASSERT(seq.Get(7) == 9);
        return true;
    } END_TEST();

    TEST("ArraySequence: Slice с вставкой") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        ArraySequence<int> insert;
        insert.Append(99);
        insert.Append(100);
        seq.Slice(2, 1, &insert);
        ASSERT(seq.GetLength() == 6);
        ASSERT(seq.Get(0) == 1);
        ASSERT(seq.Get(1) == 2);
        ASSERT(seq.Get(2) == 99);
        ASSERT(seq.Get(3) == 100);
        ASSERT(seq.Get(4) == 4);
        ASSERT(seq.Get(5) == 5);
        return true;
    } END_TEST();

    TEST("ArraySequence: Slice удаление с начала") {
        ArraySequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(0, 3);
        ASSERT(seq.GetLength() == 7);
        ASSERT(seq.Get(0) == 3);
        ASSERT(seq.Get(6) == 9);
        return true;
    } END_TEST();

    TEST("ArraySequence: Slice удаление с конца") {
        ArraySequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(7, 3);
        ASSERT(seq.GetLength() == 7);
        ASSERT(seq.Get(6) == 6);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testArraySequenceIterators() {
    TEST("ArraySequence: итераторы begin/end") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        int sum = 0;
        for (auto it = seq.begin(); it != seq.end(); ++it) {
            sum += *it;
        }
        ASSERT(sum == 15);
        return true;
    } END_TEST();

    TEST("ArraySequence: const итераторы") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 3; i++) seq.Append(i);
        const ArraySequence<int>& constSeq = seq;
        int product = 1;
        for (auto it = constSeq.begin(); it != constSeq.end(); ++it) {
            product *= *it;
        }
        ASSERT(product == 6);
        return true;
    } END_TEST();

    TEST("ArraySequence: итератор --") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        auto it = seq.end();
        --it;
        ASSERT(*it == 5);
        --it;
        ASSERT(*it == 4);
        return true;
    } END_TEST();

    TEST("ArraySequence: итератор postfix ++/--") {
        ArraySequence<int> seq;
        seq.Append(10);
        seq.Append(20);
        seq.Append(30);
        auto it = seq.begin();
        auto it2 = it++;
        ASSERT(*it2 == 10);
        ASSERT(*it == 20);
        it2 = it--;
        ASSERT(*it2 == 20);
        ASSERT(*it == 10);
        return true;
    } END_TEST();

    TEST("ArraySequence: range-based for") {
        ArraySequence<int> seq;
        for (int i = 1; i <= 4; i++) seq.Append(i);
        int sum = 0;
        for (int x : seq) {
            sum += x;
        }
        ASSERT(sum == 10);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testListSequenceConstructors() {
    TEST("ListSequence: конструктор по умолчанию") {
        ListSequence<int> seq;
        ASSERT(seq.GetLength() == 0);
        return true;
    } END_TEST();

    TEST("ListSequence: конструктор с размером и значением") {
        ListSequence<int> seq(5, 42);
        ASSERT(seq.GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(seq.Get(i) == 42);
        return true;
    } END_TEST();

    TEST("ListSequence: конструктор из массива") {
        int items[] = {1, 2, 3};
        ListSequence<int> seq(items, 3);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(0) == 1);
        ASSERT(seq.Get(2) == 3);
        return true;
    } END_TEST();

    TEST("ListSequence: конструктор копирования") {
        ListSequence<int> seq1(3, 10);
        ListSequence<int> seq2(seq1);
        seq1.Set(0, 99);
        ASSERT(seq2.Get(0) == 10);
        return true;
    } END_TEST();

    TEST("ListSequence: конструктор из LinkedList") {
        LinkedList<int> list;
        list.push_back(5);
        list.push_back(10);
        ListSequence<int> seq(list);
        ASSERT(seq.GetLength() == 2);
        ASSERT(seq.Get(0) == 5);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testListSequenceBasicOperations() {
    TEST("ListSequence: Append") {
        ListSequence<int> seq;
        seq.Append(10);
        seq.Append(20);
        seq.Append(30);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(0) == 10);
        ASSERT(seq.GetLast() == 30);
        return true;
    } END_TEST();

    TEST("ListSequence: Prepend") {
        ListSequence<int> seq;
        seq.Append(20);
        seq.Append(30);
        seq.Prepend(10);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.GetFirst() == 10);
        return true;
    } END_TEST();

    TEST("ListSequence: InsertAt") {
        ListSequence<int> seq;
        seq.Append(10);
        seq.Append(30);
        seq.InsertAt(20, 1);
        ASSERT(seq.GetLength() == 3);
        ASSERT(seq.Get(1) == 20);
        return true;
    } END_TEST();

    TEST("ListSequence: RemoveAt") {
        ListSequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        seq.RemoveAt(0);
        ASSERT(seq.GetFirst() == 2);
        seq.RemoveAt(2);
        ASSERT(seq.Get(2) == 5);
        return true;
    } END_TEST();

    TEST("ListSequence: Set") {
        ListSequence<int> seq(3, 0);
        seq.Set(0, 100);
        seq.Set(1, 200);
        ASSERT(seq.Get(0) == 100);
        ASSERT(seq.Get(1) == 200);
        return true;
    } END_TEST();

    TEST("ListSequence: GetFirst/GetLast") {
        ListSequence<int> seq;
        seq.Append(42);
        seq.Append(100);
        ASSERT(seq.GetFirst() == 42);
        ASSERT(seq.GetLast() == 100);
        return true;
    } END_TEST();

    TEST("ListSequence: GetSubsequence") {
        ListSequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        Sequence<int>* sub = seq.GetSubsequence(3, 7);
        ASSERT(sub->GetLength() == 5);
        ASSERT(sub->Get(0) == 3);
        ASSERT(sub->Get(4) == 7);
        delete sub;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testListSequenceFunctional() {
    TEST("ListSequence: Map") {
        ListSequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        Sequence<int>* mapped = seq.Map([](const int& x) { return x * 2; });
        ASSERT(mapped->GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(mapped->Get(i) == (i + 1) * 2);
        delete mapped;
        return true;
    } END_TEST();

    TEST("ListSequence: Reduce") {
        ListSequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        int sum = seq.Reduce([](const int& a, const int& b) { return a + b; }, 0);
        ASSERT(sum == 15);
        return true;
    } END_TEST();

    TEST("ListSequence: Where") {
        ListSequence<int> seq;
        for (int i = 1; i <= 10; i++) seq.Append(i);
        Sequence<int>* odd = seq.Where([](const int& x) { return x % 2 == 1; });
        ASSERT(odd->GetLength() == 5);
        for (int i = 0; i < 5; i++) ASSERT(odd->Get(i) == i * 2 + 1);
        delete odd;
        return true;
    } END_TEST();

    TEST("ListSequence: Concat") {
        ListSequence<int> seq1;
        seq1.Append(1);
        seq1.Append(2);
        ListSequence<int> seq2;
        seq2.Append(3);
        seq2.Append(4);
        Sequence<int>* result = seq1.Concat(&seq2);
        ASSERT(result->GetLength() == 4);
        delete result;
        return true;
    } END_TEST();

    TEST("ListSequence: Slice") {
        ListSequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(3, 4);
        ASSERT(seq.GetLength() == 6);
        ASSERT(seq.Get(3) == 7);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testListSequenceConcatSlice() {
    TEST("ListSequence: Concat с пустыми") {
        ListSequence<int> seq1;
        ListSequence<int> seq2;
        seq2.Append(5);
        Sequence<int>* result = seq1.Concat(&seq2);
        ASSERT(result->GetLength() == 1);
        delete result;
        return true;
    } END_TEST();

    TEST("ListSequence: Slice с отрицательным start") {
        ListSequence<int> seq;
        for (int i = 0; i < 10; i++) seq.Append(i);
        seq.Slice(-5, 2);
        ASSERT(seq.GetLength() == 8);
        return true;
    } END_TEST();

    TEST("ListSequence: Slice с вставкой") {
        ListSequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        ListSequence<int> insert;
        insert.Append(99);
        insert.Append(100);
        seq.Slice(2, 1, &insert);
        ASSERT(seq.GetLength() == 6);
        ASSERT(seq.Get(2) == 99);
        ASSERT(seq.Get(3) == 100);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testListSequenceIterators() {
    TEST("ListSequence: итераторы") {
        ListSequence<int> seq;
        for (int i = 1; i <= 5; i++) seq.Append(i);
        int sum = 0;
        for (auto it = seq.begin(); it != seq.end(); ++it) {
            sum += *it;
        }
        ASSERT(sum == 15);
        return true;
    } END_TEST();

    TEST("ListSequence: range-based for") {
        ListSequence<int> seq;
        seq.Append(2);
        seq.Append(4);
        seq.Append(6);
        int sum = 0;
        for (int x : seq) {
            sum += x;
        }
        ASSERT(sum == 12);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testSequencePolymorphism() {
    TEST("Sequence полиморфизм: ArraySequence через указатель") {
        Sequence<int>* seq = new ArraySequence<int>();
        seq->Append(10);
        seq->Append(20);
        ASSERT(seq->GetLength() == 2);
        ASSERT(seq->Get(0) == 10);
        delete seq;
        return true;
    } END_TEST();

    TEST("Sequence полиморфизм: ListSequence через указатель") {
        Sequence<int>* seq = new ListSequence<int>();
        seq->Append(5);
        seq->Append(15);
        ASSERT(seq->GetLength() == 2);
        ASSERT(seq->GetFirst() == 5);
        delete seq;
        return true;
    } END_TEST();

    TEST("Sequence полиморфизм: Map через базовый указатель") {
        Sequence<int>* seq = new ArraySequence<int>();
        for (int i = 1; i <= 3; i++) seq->Append(i);
        Sequence<int>* mapped = seq->Map([](const int& x) { return x * 10; });
        ASSERT(mapped->GetLength() == 3);
        ASSERT(mapped->Get(0) == 10);
        ASSERT(mapped->Get(2) == 30);
        delete seq;
        delete mapped;
        return true;
    } END_TEST();

    TEST("Sequence полиморфизм: Concat разных типов") {
        Sequence<int>* seq1 = new ArraySequence<int>();
        seq1->Append(1);
        seq1->Append(2);
        Sequence<int>* seq2 = new ListSequence<int>();
        seq2->Append(3);
        seq2->Append(4);
        Sequence<int>* result = seq1->Concat(seq2);
        ASSERT(result->GetLength() == 4);
        delete seq1;
        delete seq2;
        delete result;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testZipUnzipInt() {
    TEST("ZipSequences: целые числа одинаковой длины") {
        ArraySequence<int> a;
        a.Append(1);
        a.Append(2);
        a.Append(3);
        ArraySequence<int> b;
        b.Append(4);
        b.Append(5);
        b.Append(6);
        
        Sequence<std::tuple<int,int>>* zipped = ZipSequences(&a, &b);
        ASSERT(zipped->GetLength() == 3);
        ASSERT(std::get<0>(zipped->Get(0)) == 1);
        ASSERT(std::get<1>(zipped->Get(0)) == 4);
        ASSERT(std::get<0>(zipped->Get(2)) == 3);
        ASSERT(std::get<1>(zipped->Get(2)) == 6);
        delete zipped;
        return true;
    } END_TEST();

    TEST("ZipSequences: целые числа разной длины") {
        ArraySequence<int> a;
        a.Append(1);
        a.Append(2);
        ArraySequence<int> b;
        b.Append(3);
        b.Append(4);
        b.Append(5);
        
        Sequence<std::tuple<int,int>>* zipped = ZipSequences(&a, &b);
        ASSERT(zipped->GetLength() == 3);
        ASSERT(std::get<0>(zipped->Get(2)) == 0);
        ASSERT(std::get<1>(zipped->Get(2)) == 5);
        delete zipped;
        return true;
    } END_TEST();

    TEST("UnzipSequences: целые числа") {
        ArraySequence<std::tuple<int,int>> zipped;
        zipped.Append(std::make_tuple(1, 10));
        zipped.Append(std::make_tuple(2, 20));
        zipped.Append(std::make_tuple(3, 30));
        
        auto [first, second] = UnzipSequences(&zipped);
        ASSERT(first->GetLength() == 3);
        ASSERT(second->GetLength() == 3);
        ASSERT(first->Get(0) == 1);
        ASSERT(first->Get(2) == 3);
        ASSERT(second->Get(0) == 10);
        ASSERT(second->Get(2) == 30);
        delete first;
        delete second;
        return true;
    } END_TEST();

    TEST("Zip+Unzip: обратимость") {
        ArraySequence<int> a;
        a.Append(1);
        a.Append(2);
        a.Append(3);
        ArraySequence<int> b;
        b.Append(4);
        b.Append(5);
        b.Append(6);
        
        Sequence<std::tuple<int,int>>* zipped = ZipSequences(&a, &b);
        auto [unzippedA, unzippedB] = UnzipSequences(zipped);
        
        for (int i = 0; i < 3; i++) {
            ASSERT(unzippedA->Get(i) == a.Get(i));
            ASSERT(unzippedB->Get(i) == b.Get(i));
        }
        delete zipped;
        delete unzippedA;
        delete unzippedB;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testZipUnzipDouble() {
    TEST("ZipSequences: вещественные числа") {
        ArraySequence<double> a;
        a.Append(1.1);
        a.Append(2.2);
        ArraySequence<double> b;
        b.Append(3.3);
        b.Append(4.4);
        
        Sequence<std::tuple<double,double>>* zipped = ZipSequences(&a, &b);
        ASSERT(zipped->GetLength() == 2);
        ASSERT(std::get<0>(zipped->Get(0)) == 1.1);
        ASSERT(std::get<1>(zipped->Get(0)) == 3.3);
        delete zipped;
        return true;
    } END_TEST();

    TEST("UnzipSequences: вещественные числа") {
        ArraySequence<std::tuple<double,double>> zipped;
        zipped.Append(std::make_tuple(1.5, 2.5));
        zipped.Append(std::make_tuple(3.5, 4.5));
        
        auto [first, second] = UnzipSequences(&zipped);
        ASSERT(first->Get(0) == 1.5);
        ASSERT(second->Get(1) == 4.5);
        delete first;
        delete second;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testZipUnzipChar() {
    TEST("ZipSequences: символы") {
        ArraySequence<char> a;
        a.Append('A');
        a.Append('B');
        ArraySequence<char> b;
        b.Append('1');
        b.Append('2');
        
        Sequence<std::tuple<char,char>>* zipped = ZipSequences(&a, &b);
        ASSERT(std::get<0>(zipped->Get(0)) == 'A');
        ASSERT(std::get<1>(zipped->Get(1)) == '2');
        delete zipped;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testZipUnzipString() {
    TEST("ZipSequences: строки") {
        ArraySequence<std::string> a;
        a.Append("Hello");
        a.Append("World");
        ArraySequence<std::string> b;
        b.Append("Foo");
        b.Append("Bar");
        
        Sequence<std::tuple<std::string,std::string>>* zipped = ZipSequences(&a, &b);
        ASSERT(std::get<0>(zipped->Get(0)) == "Hello");
        ASSERT(std::get<1>(zipped->Get(0)) == "Foo");
        ASSERT(std::get<0>(zipped->Get(1)) == "World");
        ASSERT(std::get<1>(zipped->Get(1)) == "Bar");
        delete zipped;
        return true;
    } END_TEST();
}

void TestRunnerGUI::testMapUtils() {
    TEST("MapUtils: Upper/Lower для char") {
        char c1 = 'a';
        char c2 = 'Z';
        ASSERT(Upper(c1) == 'A');
        ASSERT(Lower(c2) == 'z');
        return true;
    } END_TEST();

    TEST("MapUtils: ToCesarCipher") {
        std::string s = "ABC";
        std::string result = ToCesarСipher(s, 1);
        ASSERT(result == "BCD");
        
        std::string s2 = "xyz";
        result = ToCesarСipher(s2, 1);
        ASSERT(result == "yza");
        return true;
    } END_TEST();

    TEST("MapUtils: CaMeLcAsE") {
        std::string s = "hello";
        std::string result = functions::maper::strings::CaMeLcAsE(s);
        ASSERT(result == "HeLlO");
        return true;
    } END_TEST();

    TEST("MapUtils: UpperCase/LowerCase") {
        std::string s = "Hello World";
        std::string upper = functions::maper::strings::UpperCase(s);
        std::string lower = functions::maper::strings::LowerCase(s);
        ASSERT(upper == "HELLO WORLD");
        ASSERT(lower == "hello world");
        return true;
    } END_TEST();

    TEST("MapUtils: is_even/is_odd") {
        ASSERT(functions::where::is_even(2) == true);
        ASSERT(functions::where::is_even(3) == false);
        ASSERT(functions::where::is_odd(3) == true);
        ASSERT(functions::where::is_odd(4) == false);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testRectangularMatrix() {
    TEST("RectangularMatrix: конструктор по умолчанию") {
        RectangularMatrix<int> mat;
        ASSERT(mat.getRows() == 0);
        ASSERT(mat.getCols() == 0);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: конструктор с размерами") {
        RectangularMatrix<int> mat(3, 4);
        ASSERT(mat.getRows() == 3);
        ASSERT(mat.getCols() == 4);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                ASSERT(mat.get(i, j) == 0);
            }
        }
        return true;
    } END_TEST();

    TEST("RectangularMatrix: конструктор с размерами и значением по умолчанию") {
        RectangularMatrix<int> mat(2, 3, 42);
        ASSERT(mat.getRows() == 2);
        ASSERT(mat.getCols() == 3);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                ASSERT(mat.get(i, j) == 42);
            }
        }
        return true;
    } END_TEST();

    TEST("RectangularMatrix: конструктор копирования") {
        RectangularMatrix<int> mat1(2, 2);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 0, 3); mat1.set(1, 1, 4);
        
        RectangularMatrix<int> mat2(mat1);
        mat1.set(0, 0, 99);
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(0, 1) == 2);
        ASSERT(mat2.get(1, 0) == 3);
        ASSERT(mat2.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: оператор присваивания") {
        RectangularMatrix<int> mat1(2, 2);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 0, 3); mat1.set(1, 1, 4);
        
        RectangularMatrix<int> mat2;
        mat2 = mat1;
        mat1.set(0, 0, 99);
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: move конструктор") {
        RectangularMatrix<int> mat1(2, 2);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 0, 3); mat1.set(1, 1, 4);
        
        RectangularMatrix<int> mat2(std::move(mat1));
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(1, 1) == 4);
        ASSERT(mat1.getRows() == 0);
        ASSERT(mat1.getCols() == 0);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: установка и получение элементов") {
        RectangularMatrix<int> mat(2, 2);
        mat.set(0, 0, 10);
        mat.set(0, 1, 20);
        mat.set(1, 0, 30);
        mat.set(1, 1, 40);
        
        ASSERT(mat.get(0, 0) == 10);
        ASSERT(mat.get(0, 1) == 20);
        ASSERT(mat.get(1, 0) == 30);
        ASSERT(mat.get(1, 1) == 40);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: оператор ()") {
        RectangularMatrix<int> mat(2, 2);
        mat(0, 0) = 10;
        mat(0, 1) = 20;
        mat(1, 0) = 30;
        mat(1, 1) = 40;
        
        ASSERT(mat(0, 0) == 10);
        ASSERT(mat(0, 1) == 20);
        ASSERT(mat(1, 0) == 30);
        ASSERT(mat(1, 1) == 40);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: сложение матриц (int)") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        RectangularMatrix<int> B(2, 2);
        B.set(0, 0, 5); B.set(0, 1, 6);
        B.set(1, 0, 7); B.set(1, 1, 8);
        
        RectangularMatrix<int>* C = A.add(B);
        
        ASSERT(C->get(0, 0) == 6);
        ASSERT(C->get(0, 1) == 8);
        ASSERT(C->get(1, 0) == 10);
        ASSERT(C->get(1, 1) == 12);
        
        delete C;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: сложение матриц (double)") {
        RectangularMatrix<double> A(2, 2);
        A.set(0, 0, 1.5); A.set(0, 1, 2.5);
        A.set(1, 0, 3.5); A.set(1, 1, 4.5);
        
        RectangularMatrix<double> B(2, 2);
        B.set(0, 0, 0.5); B.set(0, 1, 1.5);
        B.set(1, 0, 2.5); B.set(1, 1, 3.5);
        
        RectangularMatrix<double>* C = A.add(B);
        
        ASSERT(std::abs(C->get(0, 0) - 2.0) < 1e-9);
        ASSERT(std::abs(C->get(0, 1) - 4.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 0) - 6.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 1) - 8.0) < 1e-9);
        
        delete C;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: сложение матриц (complex)") {
        using namespace std::complex_literals;
        
        RectangularMatrix<std::complex<double>> A(2, 2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 0, std::complex<double>(3, 2));
        A.set(1, 1, std::complex<double>(4, -2));
        
        RectangularMatrix<std::complex<double>> B(2, 2);
        B.set(0, 0, std::complex<double>(0, -1));
        B.set(0, 1, std::complex<double>(1, 1));
        B.set(1, 0, std::complex<double>(2, -2));
        B.set(1, 1, std::complex<double>(3, 3));
        
        RectangularMatrix<std::complex<double>>* C = A.add(B);
        
        ASSERT(C->get(0, 0) == std::complex<double>(1, 0));
        ASSERT(C->get(0, 1) == std::complex<double>(3, 0));
        ASSERT(C->get(1, 0) == std::complex<double>(5, 0));
        ASSERT(C->get(1, 1) == std::complex<double>(7, 1));
        
        delete C;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: умножение на скаляр (int)") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 2); A.set(0, 1, 4);
        A.set(1, 0, 6); A.set(1, 1, 8);
        
        RectangularMatrix<int>* B = A.multiplyByScalar(3);
        
        ASSERT(B->get(0, 0) == 6);
        ASSERT(B->get(0, 1) == 12);
        ASSERT(B->get(1, 0) == 18);
        ASSERT(B->get(1, 1) == 24);
        
        delete B;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: умножение на скаляр (double)") {
        RectangularMatrix<double> A(2, 2);
        A.set(0, 0, 2.0); A.set(0, 1, 4.0);
        A.set(1, 0, 6.0); A.set(1, 1, 8.0);
        
        RectangularMatrix<double>* B = A.multiplyByScalar(2.5);
        
        ASSERT(std::abs(B->get(0, 0) - 5.0) < 1e-9);
        ASSERT(std::abs(B->get(0, 1) - 10.0) < 1e-9);
        ASSERT(std::abs(B->get(1, 0) - 15.0) < 1e-9);
        ASSERT(std::abs(B->get(1, 1) - 20.0) < 1e-9);
        
        delete B;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: умножение на скаляр (complex)") {
        using namespace std::complex_literals;
        
        RectangularMatrix<std::complex<double>> A(2, 2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 0, std::complex<double>(3, 2));
        A.set(1, 1, std::complex<double>(4, -2));
        
        RectangularMatrix<std::complex<double>>* B = A.multiplyByScalar(std::complex<double>(0, 1));
        
        ASSERT(B->get(0, 0) == std::complex<double>(-1, 1));
        ASSERT(B->get(0, 1) == std::complex<double>(1, 2));
        ASSERT(B->get(1, 0) == std::complex<double>(-2, 3));
        ASSERT(B->get(1, 1) == std::complex<double>(2, 4));
        
        delete B;
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L1 (int)") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normL1() - 6.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L1 (double)") {
        RectangularMatrix<double> A(2, 2);
        A.set(0, 0, 1.5); A.set(0, 1, 2.5);
        A.set(1, 0, 3.5); A.set(1, 1, 4.5);
        
        double expected = std::max(1.5 + 3.5, 2.5 + 4.5);
        ASSERT(std::abs(A.normL1() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L1 (complex)") {
        using namespace std::complex_literals;
        
        RectangularMatrix<std::complex<double>> A(2, 2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, 0));
        A.set(1, 0, std::complex<double>(3, -1));
        A.set(1, 1, std::complex<double>(4, 2));
        
        double expected = std::max(
            std::abs(1.0+1.0i) + std::abs(3.0-1.0i),  
            std::abs(2.0+0.0i) + std::abs(4.0+2.0i)   
        );
        ASSERT(std::abs(A.normL1() - expected) < 1e-5);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L2 (int)") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normInf() - 7.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L2 (double)") {
        RectangularMatrix<double> A(2, 2);
        A.set(0, 0, 1.5); A.set(0, 1, 2.5);
        A.set(1, 0, 3.5); A.set(1, 1, 4.5);
        
        double expected = std::max(1.5 + 2.5, 3.5 + 4.5);
        ASSERT(std::abs(A.normInf() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L2 (complex)") {
        using namespace std::complex_literals;
        
        RectangularMatrix<std::complex<double>> A(2, 2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 0, std::complex<double>(3, 0));
        A.set(1, 1, std::complex<double>(4, 2));
        
        double expected = std::max(
            std::abs(1.0+1.0i) + std::abs(2.0-1.0i),  
            std::abs(3.0+0.0i) + std::abs(4.0+2.0i)   
        );
        ASSERT(std::abs(A.normInf() - expected) < 1e-5);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L2 (Фробениуса) (int)") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        double expected = std::sqrt(1.0 + 4.0 + 9.0 + 16.0);
        ASSERT(std::abs(A.normL2() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: норма L2 (Фробениуса) (complex)") {
        using namespace std::complex_literals;
        
        RectangularMatrix<std::complex<double>> A(2, 2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, 0));
        A.set(1, 0, std::complex<double>(3, -1));
        A.set(1, 1, std::complex<double>(4, 2));
        
        double expected = std::sqrt(
            std::norm(1.0+1.0i) + std::norm(2.0+0.0i) +
            std::norm(3.0-1.0i) + std::norm(4.0+2.0i)
        );
        ASSERT(std::abs(A.normL2() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: swapRows") {
        RectangularMatrix<int> A(2, 3);
        A.set(0, 0, 1); A.set(0, 1, 2); A.set(0, 2, 3);
        A.set(1, 0, 4); A.set(1, 1, 5); A.set(1, 2, 6);
        
        A.swapRows(0, 1);
        
        ASSERT(A.get(0, 0) == 4); ASSERT(A.get(0, 1) == 5); ASSERT(A.get(0, 2) == 6);
        ASSERT(A.get(1, 0) == 1); ASSERT(A.get(1, 1) == 2); ASSERT(A.get(1, 2) == 3);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: multiplyRow") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.multiplyRow(0, 3);
        
        ASSERT(A.get(0, 0) == 3);
        ASSERT(A.get(0, 1) == 6);
        ASSERT(A.get(1, 0) == 3);
        ASSERT(A.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: addRowToRow") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.addRowToRow(1, 0, 2);
        
        ASSERT(A.get(0, 0) == 7);
        ASSERT(A.get(0, 1) == 10);
        ASSERT(A.get(1, 0) == 3);
        ASSERT(A.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: swapCols") {
        RectangularMatrix<int> A(2, 3);
        A.set(0, 0, 1); A.set(0, 1, 2); A.set(0, 2, 3);
        A.set(1, 0, 4); A.set(1, 1, 5); A.set(1, 2, 6);
        
        A.swapCols(0, 2);
        
        ASSERT(A.get(0, 0) == 3); ASSERT(A.get(0, 2) == 1);
        ASSERT(A.get(1, 0) == 6); ASSERT(A.get(1, 2) == 4);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: multiplyCol") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.multiplyCol(1, 2);
        
        ASSERT(A.get(0, 1) == 4);
        ASSERT(A.get(1, 1) == 8);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: addColToCol") {
        RectangularMatrix<int> A(2, 2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.addColToCol(1, 0, 2);
        
        ASSERT(A.get(0, 0) == 5);
        ASSERT(A.get(1, 0) == 11);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: исключения при несовместимых размерах для сложения") {
        RectangularMatrix<int> A(2, 2);
        RectangularMatrix<int> B(2, 3);
        
        bool exceptionThrown = false;
        try {
            A.add(B);
        } catch (SizeException) {
            exceptionThrown = true;
        }
        ASSERT(exceptionThrown);
        return true;
    } END_TEST();

    TEST("RectangularMatrix: исключения при выходе за границы") {
        RectangularMatrix<int> A(2, 2);
        
        ASSERT_THROW(A.get(-1, 0), IndexOutOFBoundsException);
        ASSERT_THROW(A.get(0, -1), IndexOutOFBoundsException);
        ASSERT_THROW(A.get(2, 0), IndexOutOFBoundsException);
        ASSERT_THROW(A.get(0, 2), IndexOutOFBoundsException);
        ASSERT_THROW(A.set(2, 0, 5), IndexOutOFBoundsException);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testSquareMatrix() {
    TEST("SquareMatrix: конструктор по умолчанию") {
        SquareMatrix<int> mat;
        ASSERT(mat.getRows() == 0);
        ASSERT(mat.getCols() == 0);
        ASSERT(mat.getSize() == 0);
        return true;
    } END_TEST();

    TEST("SquareMatrix: конструктор с размером") {
        SquareMatrix<int> mat(3);
        ASSERT(mat.getRows() == 3);
        ASSERT(mat.getCols() == 3);
        ASSERT(mat.getSize() == 3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ASSERT(mat.get(i, j) == 0);
            }
        }
        return true;
    } END_TEST();

    TEST("SquareMatrix: конструктор с размером и значением по умолчанию") {
        SquareMatrix<int> mat(2, 42);
        ASSERT(mat.getRows() == 2);
        ASSERT(mat.getCols() == 2);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                ASSERT(mat.get(i, j) == 42);
            }
        }
        return true;
    } END_TEST();

    TEST("SquareMatrix: конструктор копирования") {
        SquareMatrix<int> mat1(2);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 0, 3); mat1.set(1, 1, 4);
        
        SquareMatrix<int> mat2(mat1);
        mat1.set(0, 0, 99);
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("SquareMatrix: move конструктор") {
        SquareMatrix<int> mat1(2);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 0, 3); mat1.set(1, 1, 4);
        
        SquareMatrix<int> mat2(std::move(mat1));
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(1, 1) == 4);
        ASSERT(mat1.getSize() == 0);
        return true;
    } END_TEST();

    TEST("SquareMatrix: установка и получение элементов") {
        SquareMatrix<int> mat(2);
        mat.set(0, 0, 10);
        mat.set(0, 1, 20);
        mat.set(1, 0, 30);
        mat.set(1, 1, 40);
        
        ASSERT(mat.get(0, 0) == 10);
        ASSERT(mat.get(0, 1) == 20);
        ASSERT(mat.get(1, 0) == 30);
        ASSERT(mat.get(1, 1) == 40);
        return true;
    } END_TEST();

    TEST("SquareMatrix: сложение матриц (int)") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        SquareMatrix<int> B(2);
        B.set(0, 0, 5); B.set(0, 1, 6);
        B.set(1, 0, 7); B.set(1, 1, 8);
        
        SquareMatrix<int>* C = A.add(B);
        
        ASSERT(C->get(0, 0) == 6);
        ASSERT(C->get(0, 1) == 8);
        ASSERT(C->get(1, 0) == 10);
        ASSERT(C->get(1, 1) == 12);
        
        delete C;
        return true;
    } END_TEST();

    TEST("SquareMatrix: сложение матриц (double)") {
        SquareMatrix<double> A(2);
        A.set(0, 0, 1.5); A.set(0, 1, 2.5);
        A.set(1, 0, 3.5); A.set(1, 1, 4.5);
        
        SquareMatrix<double> B(2);
        B.set(0, 0, 0.5); B.set(0, 1, 1.5);
        B.set(1, 0, 2.5); B.set(1, 1, 3.5);
        
        SquareMatrix<double>* C = A.add(B);
        
        ASSERT(std::abs(C->get(0, 0) - 2.0) < 1e-9);
        ASSERT(std::abs(C->get(0, 1) - 4.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 0) - 6.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 1) - 8.0) < 1e-9);
        
        delete C;
        return true;
    } END_TEST();

    TEST("SquareMatrix: сложение матриц (complex)") {
        using namespace std::complex_literals;
        
        SquareMatrix<std::complex<double>> A(2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 0, std::complex<double>(3, 2));
        A.set(1, 1, std::complex<double>(4, -2));
        
        SquareMatrix<std::complex<double>> B(2);
        B.set(0, 0, std::complex<double>(0, -1));
        B.set(0, 1, std::complex<double>(1, 1));
        B.set(1, 0, std::complex<double>(2, -2));
        B.set(1, 1, std::complex<double>(3, 3));
        
        SquareMatrix<std::complex<double>>* C = A.add(B);
        
        ASSERT(C->get(0, 0) == std::complex<double>(1, 0));
        ASSERT(C->get(0, 1) == std::complex<double>(3, 0));
        ASSERT(C->get(1, 0) == std::complex<double>(5, 0));
        ASSERT(C->get(1, 1) == std::complex<double>(7, 1));
        
        delete C;
        return true;
    } END_TEST();

    TEST("SquareMatrix: умножение на скаляр (int)") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 2); A.set(0, 1, 4);
        A.set(1, 0, 6); A.set(1, 1, 8);
        
        SquareMatrix<int>* B = A.multiplyByScalar(3);
        
        ASSERT(B->get(0, 0) == 6);
        ASSERT(B->get(0, 1) == 12);
        ASSERT(B->get(1, 0) == 18);
        ASSERT(B->get(1, 1) == 24);
        
        delete B;
        return true;
    } END_TEST();

    TEST("SquareMatrix: умножение на скаляр (double)") {
        SquareMatrix<double> A(2);
        A.set(0, 0, 2.0); A.set(0, 1, 4.0);
        A.set(1, 0, 6.0); A.set(1, 1, 8.0);
        
        SquareMatrix<double>* B = A.multiplyByScalar(2.5);
        
        ASSERT(std::abs(B->get(0, 0) - 5.0) < 1e-9);
        ASSERT(std::abs(B->get(0, 1) - 10.0) < 1e-9);
        ASSERT(std::abs(B->get(1, 0) - 15.0) < 1e-9);
        ASSERT(std::abs(B->get(1, 1) - 20.0) < 1e-9);
        
        delete B;
        return true;
    } END_TEST();

    TEST("SquareMatrix: умножение на скаляр (complex)") {
        using namespace std::complex_literals;
        
        SquareMatrix<std::complex<double>> A(2);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 0, std::complex<double>(3, 2));
        A.set(1, 1, std::complex<double>(4, -2));
        
        SquareMatrix<std::complex<double>>* B = A.multiplyByScalar(std::complex<double>(0, 1));
        
        ASSERT(B->get(0, 0) == std::complex<double>(-1, 1));
        ASSERT(B->get(0, 1) == std::complex<double>(1, 2));
        ASSERT(B->get(1, 0) == std::complex<double>(-2, 3));
        ASSERT(B->get(1, 1) == std::complex<double>(2, 4));
        
        delete B;
        return true;
    } END_TEST();

    TEST("SquareMatrix: норма L1 (int)") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normL1() - 6.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("SquareMatrix: норма L2 (int)") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normInf() - 7.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("SquareMatrix: норма L2 (Фробениуса) (int)") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        double expected = std::sqrt(1.0 + 4.0 + 9.0 + 16.0);
        ASSERT(std::abs(A.normL2() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("SquareMatrix: swapRows") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.swapRows(0, 1);
        
        ASSERT(A.get(0, 0) == 3);
        ASSERT(A.get(1, 0) == 1);
        return true;
    } END_TEST();

    TEST("SquareMatrix: swapCols") {
        SquareMatrix<int> A(2);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 0, 3); A.set(1, 1, 4);
        
        A.swapCols(0, 1);
        
        ASSERT(A.get(0, 0) == 2);
        ASSERT(A.get(0, 1) == 1);
        return true;
    } END_TEST();

    TEST("SquareMatrix: исключения при несовместимых размерах для сложения") {
        SquareMatrix<int> A(2);
        SquareMatrix<int> B(3);
        
        bool exceptionThrown = false;
        try {
            A.add(B);
        } catch (SizeException) {
            exceptionThrown = true;
        }
        ASSERT(exceptionThrown);
        return true;
    } END_TEST();

    TEST("SquareMatrix: исключения при выходе за границы") {
        SquareMatrix<int> A(2);
        
        ASSERT_THROW(A.get(-1, 0), IndexOutOFBoundsException);
        ASSERT_THROW(A.get(2, 0), IndexOutOFBoundsException);
        return true;
    } END_TEST();
}

void TestRunnerGUI::testTriangleMatrix() {
    TEST("TriangleMatrix: конструктор по умолчанию") {
        TriangleMatrix<int> mat;
        ASSERT(mat.getRows() == 0);
        ASSERT(mat.getCols() == 0);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: конструктор с размером (верхняя)") {
        TriangleMatrix<int> mat(3, MatrixType::Upper);
        ASSERT(mat.getRows() == 3);
        ASSERT(mat.getCols() == 3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ASSERT(mat.get(i, j) == 0);
            }
        }
        return true;
    } END_TEST();

    TEST("TriangleMatrix: конструктор с размером (нижняя)") {
        TriangleMatrix<int> mat(3, MatrixType::Lower);
        ASSERT(mat.getRows() == 3);
        ASSERT(mat.getCols() == 3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ASSERT(mat.get(i, j) == 0);
            }
        }
        return true;
    } END_TEST();

    TEST("TriangleMatrix: конструктор копирования") {
        TriangleMatrix<int> mat1(2, MatrixType::Upper);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 1, 4);
        
        TriangleMatrix<int> mat2(mat1);
        mat1.set(0, 0, 99);
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(0, 1) == 2);
        ASSERT(mat2.get(1, 1) == 4);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: move конструктор") {
        TriangleMatrix<int> mat1(2, MatrixType::Upper);
        mat1.set(0, 0, 1); mat1.set(0, 1, 2);
        mat1.set(1, 1, 4);
        
        TriangleMatrix<int> mat2(std::move(mat1));
        
        ASSERT(mat2.get(0, 0) == 1);
        ASSERT(mat2.get(1, 1) == 4);
        ASSERT(mat1.getRows() == 0);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: установка и получение элементов (верхняя)") {
        TriangleMatrix<int> upper(3, MatrixType::Upper);
        upper.set(0, 0, 1);
        upper.set(0, 1, 2);
        upper.set(0, 2, 3);
        upper.set(1, 1, 4);
        upper.set(1, 2, 5);
        upper.set(2, 2, 6);
        
        ASSERT(upper.get(0, 0) == 1);
        ASSERT(upper.get(0, 1) == 2);
        ASSERT(upper.get(0, 2) == 3);
        ASSERT(upper.get(1, 1) == 4);
        ASSERT(upper.get(1, 2) == 5);
        ASSERT(upper.get(2, 2) == 6);
        ASSERT(upper.get(1, 0) == 0);
        ASSERT(upper.get(2, 0) == 0);
        ASSERT(upper.get(2, 1) == 0);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: установка и получение элементов (нижняя)") {
        TriangleMatrix<int> lower(3, MatrixType::Lower);
        lower.set(0, 0, 1);
        lower.set(1, 0, 2);
        lower.set(1, 1, 3);
        lower.set(2, 0, 4);
        lower.set(2, 1, 5);
        lower.set(2, 2, 6);
        
        ASSERT(lower.get(0, 0) == 1);
        ASSERT(lower.get(1, 0) == 2);
        ASSERT(lower.get(1, 1) == 3);
        ASSERT(lower.get(2, 0) == 4);
        ASSERT(lower.get(2, 1) == 5);
        ASSERT(lower.get(2, 2) == 6);
        ASSERT(lower.get(0, 1) == 0);
        ASSERT(lower.get(0, 2) == 0);
        ASSERT(lower.get(1, 2) == 0);
        return true;
    } END_TEST();

        TEST("TriangleMatrix: сложение матриц (int, верхняя)") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 1, 4);
        
        TriangleMatrix<int> B(2, MatrixType::Upper);
        B.set(0, 0, 5); B.set(0, 1, 6);
        B.set(1, 1, 8);
        
        Matrix<int>* C = A.add(B);  
        
        ASSERT(C->get(0, 0) == 6);
        ASSERT(C->get(0, 1) == 8);
        ASSERT(C->get(1, 1) == 12);
        
        delete C;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: сложение матриц (double, нижняя)") {
        TriangleMatrix<double> A(2, MatrixType::Lower);
        A.set(0, 0, 1.5);
        A.set(1, 0, 2.5);
        A.set(1, 1, 3.5);
        
        TriangleMatrix<double> B(2, MatrixType::Lower);
        B.set(0, 0, 0.5);
        B.set(1, 0, 1.5);
        B.set(1, 1, 2.5);
        
        Matrix<double>* C = A.add(B);  
        
        ASSERT(std::abs(C->get(0, 0) - 2.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 0) - 4.0) < 1e-9);
        ASSERT(std::abs(C->get(1, 1) - 6.0) < 1e-9);
        
        delete C;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: сложение матриц (complex)") {
        using namespace std::complex_literals;
        
        TriangleMatrix<std::complex<double>> A(2, MatrixType::Upper);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 1, std::complex<double>(4, -2));
        
        TriangleMatrix<std::complex<double>> B(2, MatrixType::Upper);
        B.set(0, 0, std::complex<double>(0, -1));
        B.set(0, 1, std::complex<double>(1, 1));
        B.set(1, 1, std::complex<double>(3, 3));
        
        Matrix<std::complex<double>>* C = A.add(B);  
        
        ASSERT(C->get(0, 0) == std::complex<double>(1, 0));
        ASSERT(C->get(0, 1) == std::complex<double>(3, 0));
        ASSERT(C->get(1, 1) == std::complex<double>(7, 1));
        
        delete C;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: умножение на скаляр (int)") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        A.set(0, 0, 2); A.set(0, 1, 4);
        A.set(1, 1, 8);
        
        Matrix<int>* B = A.multiplyByScalar(3);  
        
        ASSERT(B->get(0, 0) == 6);
        ASSERT(B->get(0, 1) == 12);
        ASSERT(B->get(1, 1) == 24);
        
        delete B;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: умножение на скаляр (double)") {
        TriangleMatrix<double> A(2, MatrixType::Upper);
        A.set(0, 0, 2.0); A.set(0, 1, 4.0);
        A.set(1, 1, 8.0);
        
        Matrix<double>* B = A.multiplyByScalar(2.5);  
        
        ASSERT(std::abs(B->get(0, 0) - 5.0) < 1e-9);
        ASSERT(std::abs(B->get(0, 1) - 10.0) < 1e-9);
        ASSERT(std::abs(B->get(1, 1) - 20.0) < 1e-9);
        
        delete B;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: умножение на скаляр (complex)") {
        using namespace std::complex_literals;
        
        TriangleMatrix<std::complex<double>> A(2, MatrixType::Upper);
        A.set(0, 0, std::complex<double>(1, 1));
        A.set(0, 1, std::complex<double>(2, -1));
        A.set(1, 1, std::complex<double>(4, -2));
        
        Matrix<std::complex<double>>* B = A.multiplyByScalar(std::complex<double>(0, 1));  
        
        ASSERT(B->get(0, 0) == std::complex<double>(-1, 1));
        ASSERT(B->get(0, 1) == std::complex<double>(1, 2));
        ASSERT(B->get(1, 1) == std::complex<double>(2, 4));
        
        delete B;
        return true;
    } END_TEST();

    TEST("TriangleMatrix: норма L1 (int, верхняя)") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normL1() - 6.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: норма L2 (int, нижняя)") {
        TriangleMatrix<int> A(2, MatrixType::Lower);
        A.set(0, 0, 1);
        A.set(1, 0, 2);
        A.set(1, 1, 4);
        
        ASSERT(std::abs(A.normInf() - 6.0) < 1e-9);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: норма L2 (Фробениуса) (int)") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        A.set(0, 0, 1); A.set(0, 1, 2);
        A.set(1, 1, 4);
        
        double expected = std::sqrt(1.0 + 4.0 + 16.0);
        ASSERT(std::abs(A.normL2() - expected) < 1e-9);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: исключения при попытке установить ненулевое значение вне треугольника") {
        TriangleMatrix<int> upper(2, MatrixType::Upper);
        
        bool exceptionThrown = false;
        try {
            upper.set(1, 0, 5);
        } catch (IndexOutOFBoundsException) {
            exceptionThrown = true;
        }
        ASSERT(exceptionThrown);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: исключения при несовместимых размерах для сложения") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        TriangleMatrix<int> B(3, MatrixType::Upper);
        
        bool exceptionThrown = false;
        try {
            A.add(B);
        } catch (SizeException) {
            exceptionThrown = true;
        }
        ASSERT(exceptionThrown);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: исключения при несовместимых типах для сложения") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        TriangleMatrix<int> B(2, MatrixType::Lower);
        
        bool exceptionThrown = false;
        try {
            A.add(B);
        } catch (TypeException) {
            exceptionThrown = true;
        }
        ASSERT(exceptionThrown);
        return true;
    } END_TEST();

    TEST("TriangleMatrix: исключения при выходе за границы") {
        TriangleMatrix<int> A(2, MatrixType::Upper);
        
        ASSERT_THROW(A.get(-1, 0), IndexOutOFBoundsException);
        ASSERT_THROW(A.get(2, 0), IndexOutOFBoundsException);
        return true;
    } END_TEST();
}