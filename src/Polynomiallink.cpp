#include <iostream>
#include <cmath> 
using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream&, const Polynomial&);
    friend istream& operator>>(istream&, Polynomial&);
private:
    float coef;
    int exp;
    Term* link; // 指向下一個節點
};

class Polynomial {
public:
    Polynomial(); // 建構函數
    Polynomial(const Polynomial& poly); // 複製建構函數
    ~Polynomial(); // 解構函數
    
    const Polynomial& operator=(const Polynomial& poly); // 賦值運算符
    Polynomial operator+(const Polynomial& b) const; // 加法運算符
    Polynomial operator-(const Polynomial& b) const; // 減法運算符
    Polynomial operator*(const Polynomial& b) const; // 乘法運算符
    float Evaluate(float x) const; // 計算多項式在某個值的結果
    
    friend ostream& operator<<(ostream& os, const Polynomial& poly);
    friend istream& operator>>(istream& is, Polynomial& poly);

private:
    Term* head; // 指向環狀鏈結串列的頭節點
    void insertTerm(float coef, int exp); // 插入新項目
    void clear(); // 清空多項式的所有項目
};

// 無參建構函數
Polynomial::Polynomial() {
    head = new Term; // 建立頭節點
    head->link = head; // 初始化環狀鏈結串列
}

// 複製建構函數
Polynomial::Polynomial(const Polynomial& poly) {
    head = new Term;
    head->link = head;
    Term* current = poly.head->link;
    while (current != poly.head) {
        insertTerm(current->coef, current->exp);
        current = current->link;
    }
}

// 解構函數
Polynomial::~Polynomial() {
    clear();
    delete head;
}

// 清空多項式的所有項目
void Polynomial::clear() {
    Term* current = head->link;
    while (current != head) {
        Term* temp = current;
        current = current->link;
        delete temp;
    }
    head->link = head;
}

// 插入新項目
void Polynomial::insertTerm(float coef, int exp) {
    if (coef == 0) return; // 係數為0不插入
    Term* prev = head;
    Term* current = head->link;
    while (current != head && current->exp > exp) {
        prev = current;
        current = current->link;
    }
    if (current != head && current->exp == exp) {
        current->coef += coef; // 指數相同則係數相加
        if (current->coef == 0) { // 如果係數變為0，刪除該項
            prev->link = current->link;
            delete current;
        }
    } else {
        Term* newTerm = new Term;
        newTerm->coef = coef;
        newTerm->exp = exp;
        newTerm->link = current;
        prev->link = newTerm;
    }
}

// 賦值運算符
const Polynomial& Polynomial::operator=(const Polynomial& poly) {
    if (this != &poly) {
        clear();
        Term* current = poly.head->link;
        while (current != poly.head) {
            insertTerm(current->coef, current->exp);
            current = current->link;
        }
    }
    return *this;
}

// 加法運算符
Polynomial Polynomial::operator+(const Polynomial& b) const {
    Polynomial result;
    Term* aTerm = head->link;
    Term* bTerm = b.head->link;
    while (aTerm != head && bTerm != b.head) {
        if (aTerm->exp == bTerm->exp) {
            result.insertTerm(aTerm->coef + bTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
            bTerm = bTerm->link;
        } else if (aTerm->exp > bTerm->exp) {
            result.insertTerm(aTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
        } else {
            result.insertTerm(bTerm->coef, bTerm->exp);
            bTerm = bTerm->link;
        }
    }
    while (aTerm != head) {
        result.insertTerm(aTerm->coef, aTerm->exp);
        aTerm = aTerm->link;
    }
    while (bTerm != b.head) {
        result.insertTerm(bTerm->coef, bTerm->exp);
        bTerm = bTerm->link;
    }
    return result;
}

// 減法運算符
Polynomial Polynomial::operator-(const Polynomial& b) const {
    Polynomial result;
    Term* aTerm = head->link;
    Term* bTerm = b.head->link;
    while (aTerm != head && bTerm != b.head) {
        if (aTerm->exp == bTerm->exp) {
            result.insertTerm(aTerm->coef - bTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
            bTerm = bTerm->link;
        } else if (aTerm->exp > bTerm->exp) {
            result.insertTerm(aTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
        } else {
            result.insertTerm(-bTerm->coef, bTerm->exp);
            bTerm = bTerm->link;
        }
    }
    while (aTerm != head) {
        result.insertTerm(aTerm->coef, aTerm->exp);
        aTerm = aTerm->link;
    }
    while (bTerm != b.head) {
        result.insertTerm(-bTerm->coef, bTerm->exp);
        bTerm = bTerm->link;
    }
    return result;
}

// 乘法運算符
Polynomial Polynomial::operator*(const Polynomial& b) const {
    Polynomial result;
    for (Term* aTerm = head->link; aTerm != head; aTerm = aTerm->link) {
        for (Term* bTerm = b.head->link; bTerm != b.head; bTerm = bTerm->link) {
            result.insertTerm(aTerm->coef * bTerm->coef, aTerm->exp + bTerm->exp);
        }
    }
    return result;
}

// 計算多項式在某個值的結果
float Polynomial::Evaluate(float x) const {
    float result = 0;
    for (Term* current = head->link; current != head; current = current->link) {
        result += current->coef * pow(x, current->exp);
    }
    return result;
}

// 重載運算符<<，用於輸出多項式
ostream& operator<<(ostream& os, const Polynomial& poly) {
    Term* current = poly.head->link;
    while (current != poly.head) {
        if (current != poly.head->link && current->coef > 0) os << " + ";
        os << current->coef << "x^" << current->exp;
        current = current->link;
    }
    return os;
}

// 重載運算符>>，用於輸入多項式
istream& operator>>(istream& is, Polynomial& poly) {
    int n;
    cout << "輸入項數: ";
    is >> n;
    for (int i = 0; i < n; i++) {
        float coef;
        int exp;
        cout << "輸入係數跟指數: ";
        is >> coef >> exp;
        poly.insertTerm(coef, exp);
    }
    return is;
}

int main() {
    Polynomial p1, p2, p3;
    cout << "輸入第一個多項式:\n";
    cin >> p1;
    
    cout << "輸入第二個多項式:\n";
    cin >> p2;
    
    p3 = p1 + p2;
    cout << "和: " << p3 << endl;
    
    p3 = p1 * p2;
    cout << "積: " << p3 << endl;

    float x;
    cout << "輸入x的值來計算多項式p1在x的值: ";
    cin >> x;
    cout << "p1(" << x << ") = " << p1.Evaluate(x) << endl;

    return 0;
}
