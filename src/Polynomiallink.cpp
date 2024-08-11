#include <iostream>
#include <cmath>
#include <chrono> // 用於效能量測
using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream&, const Polynomial&);
    friend istream& operator>>(istream&, Polynomial&);
private:
    float coef;
    int exp;
    Term* link;
};

class Polynomial {
public:
    Polynomial();
    Polynomial(const Polynomial& poly);
    ~Polynomial();

    const Polynomial& operator=(const Polynomial& poly);
    Polynomial operator+(const Polynomial& b) const;
    Polynomial operator-(const Polynomial& b) const;
    Polynomial operator*(const Polynomial& b) const;
    float Evaluate(float x) const;

    friend ostream& operator<<(ostream& os, const Polynomial& poly);
    friend istream& operator>>(istream& is, Polynomial& poly);

private:
    Term* head;
    void insertTerm(float coef, int exp);
    void clear();
};

Polynomial::Polynomial() {
    head = new Term;
    head->link = head;
}

Polynomial::Polynomial(const Polynomial& poly) {
    head = new Term;
    head->link = head;
    Term* current = poly.head->link;
    while (current != poly.head) {
        insertTerm(current->coef, current->exp);
        current = current->link;
    }
}

Polynomial::~Polynomial() {
    clear();
    delete head;
}

void Polynomial::clear() {
    Term* current = head->link;
    while (current != head) {
        Term* temp = current;
        current = current->link;
        delete temp;
    }
    head->link = head;
}

void Polynomial::insertTerm(float coef, int exp) {
    if (coef == 0) return;
    Term* prev = head;
    Term* current = head->link;
    while (current != head && current->exp > exp) {
        prev = current;
        current = current->link;
    }
    if (current != head && current->exp == exp) {
        current->coef += coef;
        if (current->coef == 0) {
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

Polynomial Polynomial::operator*(const Polynomial& b) const {
    Polynomial result;
    for (Term* aTerm = head->link; aTerm != head; aTerm = aTerm->link) {
        for (Term* bTerm = b.head->link; bTerm != b.head; bTerm = bTerm->link) {
            result.insertTerm(aTerm->coef * bTerm->coef, aTerm->exp + bTerm->exp);
        }
    }
    return result;
}

float Polynomial::Evaluate(float x) const {
    float result = 0;
    for (Term* current = head->link; current != head; current = current->link) {
        result += current->coef * pow(x, current->exp);
    }
    return result;
}

ostream& operator<<(ostream& os, const Polynomial& poly) {
    Term* current = poly.head->link;
    while (current != poly.head) {
        if (current != poly.head->link && current->coef > 0) os << " + ";
        os << current->coef << "x^" << current->exp;
        current = current->link;
    }
    return os;
}

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
    
    // 測量加法運算時間
    auto start = chrono::high_resolution_clock::now();
    p3 = p1 + p2;
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "加法結果: " << p3 << endl;
    cout << "加法運算時間: " << duration.count() << " 秒" << endl;
    
    // 測量乘法運算時間
    start = chrono::high_resolution_clock::now();
    p3 = p1 * p2;
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << "乘法結果: " << p3 << endl;
    cout << "乘法運算時間: " << duration.count() << " 秒" << endl;

    // 測量多項式求值時間
    float x;
    cout << "輸入x的值來計算多項式p1在x的值: ";
    cin >> x;
    start = chrono::high_resolution_clock::now();
    float result = p1.Evaluate(x);
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << "p1(" << x << ") = " << result << endl;
    cout << "求值運算時間: " << duration.count() << " 秒" << endl;

    return 0;
}
