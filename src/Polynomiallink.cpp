#include <iostream>
#include <cmath>
using namespace std;
class Polynomial;////定義類別 Polynomial

class Term //定義類別 Term
{
    friend class Polynomial;//Polynomial可存取Term的私有成員
    friend ostream& operator<<(ostream&, const Polynomial&);// << 可存取 Term的私有成員
    friend istream& operator>>(istream&, Polynomial&);// >> 可存取Term的私有成員
private://的私有成員
    float coef;//多項式係數
    int exp;//多項式指數
    Term* link;//指向下一個 Term 對象的指針，這是用於鏈表結構的一部分
};

class Polynomial //定義類別 Polynomial
{
public://公有成員
    Polynomial();//建構函數，用於初始化資料
    Polynomial(const Polynomial& poly);//複製現有Polynomial物件創建新物件
    ~Polynomial();//解構函數 

    const Polynomial& operator=(const Polynomial& poly);
//首先會清空當前對象中的所有項目，然後複製給定對象 (poly) 的所有項目到當前對象中，並返回當前對象 (*this) 的參考。
    Polynomial operator+(const Polynomial& b) const;//加法
    Polynomial operator-(const Polynomial& b) const;//減法
    Polynomial operator*(const Polynomial& b) const;//乘法
    float Evaluate(float x) const;//計算多項式在給定數值 x 上的值的函數

    friend ostream& operator<<(ostream& os, const Polynomial& poly);
    friend istream& operator>>(istream& is, Polynomial& poly);

private:
    Term* head;
//用來作為多項式項的鏈表的頭部。head 節點本身並不存儲實際的多項式項目，但它用來標記鏈表的開始並提供一個循環結構。
    void insertTerm(float coef, int exp);
//根據係數和指數在鏈表中找到正確的位置來插入新的 Term 對象。這個方法會處理合併相同指數的項，並在需要時刪除係數為零的項。
    void clear();
//清空多項式中所有項目的私有方法。它會遍歷鏈表，刪除所有 Term 對象，然後重設 head 節點的鏈接，使其指向自身。
};

//初始化 Polynomial 對象的成員變量。在這裡主要是用來設置多項式的鏈表頭部節點。
Polynomial::Polynomial() 
{
    head = new Term;
    //這個 Term 對象作為鏈表的頭部節點，用於標記鏈表的起始位置。
    head->link = head;
    //形成一個循環鏈表。這樣設置可以使得當鏈表中沒有其他節點時，head->link 仍然指向自己，簡化了邊界條件處理。
}

//複製一個現有的 Polynomial 對象 poly 的數據到新創建的 Polynomial 對象中。
Polynomial::Polynomial(const Polynomial& poly) 
{
    head = new Term;
    head->link = head;
    Term* current = poly.head->link;
    //設置 current 指針為傳入 poly 對象鏈表中的第一個實際節點。
    while (current != poly.head)//遍歷 poly 對象中的所有 Term 節點（直到返回到 head）。
    {
        insertTerm(current->coef, current->exp);
        //將每個 Term 的係數和指數插入到當前 Polynomial 對象中。
        current = current->link;
        //移動到下一個節點
    }
}

Polynomial::~Polynomial() 
{
    clear();
    //刪除多項式中所有的 Term 對象。這個方法會遍歷鏈表並刪除每個節點，確保沒有內存泄漏。
    delete head;
    //刪除 head 指針所指向的節點。這裡的 head 節點在構造函數中是用 new 分配的，因此需要顯式地刪除來釋放內存。
}

void Polynomial::clear() 
//刪除多項式中的每一個 Term 對象，以便釋放佔用的內存並確保鏈表的狀態被重置為空
{
    Term* current = head->link;
    //第一個實際 Term 節點
    while (current != head) //遍歷鏈表中的所有 Term 節點。循環的條件是 current 不等於 head，這樣可以確保循環繼續直到回到 head 節點為止
    {
        Term* temp = current;
        //在刪除當前節點後仍然能夠保留對該節點的指針。這樣可以確保在刪除節點之前能夠正確地移動到下一個節點。
        current = current->link;
        //更新 current 指針，將其設置為當前節點的 link 成員
        delete temp;
        //刪除當前節點以釋放內存
    }
    head->link = head;
    //重新建立一個空的循環鏈表結構。這樣，鏈表的狀態被重置為空，並且 head 節點指向自身。
}
//Polynomial::表示insertTerm 函數是 Polynomial 類的一部分，而不是全局函數或其他類的函數。
void Polynomial::insertTerm(float coef, int exp) 
//用於在多項式中插入一個新的項，或者更新已有項的係數。如果插入或更新後的係數為零，則該項會被刪除。
{
    if (coef == 0) return;//係數 coef 為零，則不需要插入或更新任何項，因此直接返回。
    Term* prev = head;//prev 指向鏈表的頭部節點（head），用來追蹤當前節點的前一個節點
    Term* current = head->link;//current 指向鏈表中的第一個實際節點
    while (current != head && current->exp > exp) //檢查每個節點的指數 (exp)
    {//如果 current 節點的指數大於新項的指數，則繼續前進，直到找到正確的位置。
        prev = current;
        current = current->link;
    }
    if (current != head && current->exp == exp) 
        //找到的節點 (current) 的指數等於新項的指數，則更新該節點的係數。
    {
        current->coef += coef;
        if (current->coef == 0) 
            //更新後的係數為零，則刪除該節點並調整鏈表中的鏈接，以移除該項。
        {
            prev->link = current->link;
            delete current;
        }
    } 
    else //如果 current 節點的指數不等於新項的指數，則創建一個新的 Term 節點。
    {//設置新節點的係數和指數，並將新節點插入到 prev 節點和 current 節點之間。
        Term* newTerm = new Term;
        newTerm->coef = coef;
        newTerm->exp = exp;
        newTerm->link = current;
        prev->link = newTerm;
    }
}

const Polynomial& Polynomial::operator=(const Polynomial& poly) 
//將一個 Polynomial 對象的內容賦值給另一個 Polynomial 對象。
{
    if (this != &poly) //確保 this 指針不等於 poly 的地址。這樣可以防止在賦值時出現自我賦值的情況
    {
        clear();//刪除當前對象中的所有項目，釋放內存。
        Term* current = poly.head->link;//設置 current 指針為 poly 對象的第一個實際節點。
        while (current != poly.head) //遍歷 poly 對象中的所有項，並將每個項插入到當前對象中
        {
            insertTerm(current->coef, current->exp);
            current = current->link;
        }
    }
    return *this;//返回當前對象的參考 (*this)，這樣可以支持連鎖賦值操作（例如 a = b = c;）。
}

//const 是函數修飾符，表示這個函數不會修改其成員變量。
Polynomial Polynomial::operator+(const Polynomial& b) const//計算當前多項式 (*this) 和另一個多項式 (b) 的和
{
    Polynomial result;//對象 result，用來存儲加法結果。
    Term* aTerm = head->link;//aTerm 指向當前多項式的第一個實際節點。
    Term* bTerm = b.head->link;//bTerm 指向另一個多項式 b 的第一個實際節點。
    while (aTerm != head && bTerm != b.head) //循環遍歷兩個多項式，直到其中一個多項式遍歷完成。
    {
        if (aTerm->exp == bTerm->exp)//aTerm 和 bTerm 的指數相同，係數相加，並插入到 result 中。然後更新 aTerm 和 bTerm 指針。
        {
            result.insertTerm(aTerm->coef + bTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
            bTerm = bTerm->link;
        } 
        else if (aTerm->exp > bTerm->exp)//aTerm 的指數大於 bTerm 的指數，則將 aTerm 的項插入到 result 中，並更新 aTerm 指針。
        {
            result.insertTerm(aTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
        } 
        else //bTerm 的指數大於 aTerm 的指數，則將 bTerm 的項插入到 result 中，並更新 bTerm 指針。
        {
            result.insertTerm(bTerm->coef, bTerm->exp);
            bTerm = bTerm->link;
        }
    }
    //在前面的 while 循環中可能有一個多項式還有剩餘的項未被處理。這些剩餘的項會被單獨處理並插入到 result 中。
    while (aTerm != head)//aTerm 還剩下的項。
    {
        result.insertTerm(aTerm->coef, aTerm->exp);
        aTerm = aTerm->link;
    }
    while (bTerm != b.head)//bTerm 還剩下的項。
    {
        result.insertTerm(bTerm->coef, bTerm->exp);
        bTerm = bTerm->link;
    }
    return result;
}

//計算當前多項式 (*this) 和另一個多項式 (b) 的差
Polynomial Polynomial::operator-(const Polynomial& b) const
{
    Polynomial result;
    Term* aTerm = head->link;
    Term* bTerm = b.head->link;
    while (aTerm != head && bTerm != b.head)
    {
        if (aTerm->exp == bTerm->exp) //指數相同，係數進行相減，結果插入到 result 中。然後更新 aTerm 和 bTerm 指針。
        {
            result.insertTerm(aTerm->coef - bTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
            bTerm = bTerm->link;
        } 
        else if (aTerm->exp > bTerm->exp) //aTerm 的指數大於 bTerm 的指數，則將 aTerm 的項插入到 result 中，並更新 aTerm 指針。
        {
            result.insertTerm(aTerm->coef, aTerm->exp);
            aTerm = aTerm->link;
        } 
        else //bTerm 的指數大於 aTerm 的指數，則將 -bTerm 的項插入到 result 中（這裡的 -bTerm->coef 表示 bTerm 的項係數取反），並更新 bTerm 指針。
        {
            result.insertTerm(-bTerm->coef, bTerm->exp);
            bTerm = bTerm->link;
        }
    }
    while (aTerm != head) //第一個 while 循環處理 aTerm 還剩下的項，直接插入到 result。
    {
        result.insertTerm(aTerm->coef, aTerm->exp);
        aTerm = aTerm->link;
    }
    while (bTerm != b.head) //處理 bTerm 還剩下的項，將這些項的係數取反後插入 result。
    {
        result.insertTerm(-bTerm->coef, bTerm->exp);
        bTerm = bTerm->link;
    }
    return result;
}

Polynomial Polynomial::operator*(const Polynomial& b) const //當前多項式 (*this) 和參數 b 的乘積。
{
    Polynomial result;
    for (Term* aTerm = head->link; aTerm != head; aTerm = aTerm->link) //外層 for 循環遍歷當前多項式 (*this) 的所有項（由 aTerm 指針遍歷）
    {
        for (Term* bTerm = b.head->link; bTerm != b.head; bTerm = bTerm->link) //內層 for 循環遍歷參數多項式 (b) 的所有項（由 bTerm 指針遍歷）。       
        {
            //對於每對項 aTerm 和 bTerm，計算它們的乘積 aTerm->coef * bTerm->coef，並將它們的指數相加 aTerm->exp + bTerm->exp。
            result.insertTerm(aTerm->coef * bTerm->coef, aTerm->exp + bTerm->exp);
            //result.insertTerm 函數將這些計算出的項插入到結果多項式 result 中。
        }
    }
    return result;
}

//計算多項式在給定點 x 的值。它返回多項式在 x 位置的結果。
float Polynomial::Evaluate(float x) const 
{
    float result = 0;//變量 result 用於累加多項式在 x 點的計算結果，初始值為 0。
    for (Term* current = head->link; current != head; current = current->link) //遍歷多項式的所有項（由 current 指針遍歷）
    {//對於每個項 current，計算 current->coef * pow(x, current->exp)
        result += current->coef * pow(x, current->exp);
    }
    return result;
}

ostream& operator<<(ostream& os, const Polynomial& poly)//將 Polynomial 對象輸出到 ostream 的功能
{
    Term* current = poly.head->link;//指針 current 指向多項式的第一個實際節點
    while (current != poly.head) //遍歷多項式的所有項，直到達到頭節點（poly.head）。
    {
        //如果不是第一個項且係數大於 0，則先輸出 " + " 來分隔項。
        if (current != poly.head->link && current->coef > 0) os << " + ";
        os << current->coef << "x^" << current->exp;
        current = current->link;
        //輸出當前項的係數和指數。格式是 係數x^指數。
        //更新 current 指針，指向下一個項。
    }
    return os;
}

//讀取數據來初始化 Polynomial 對象的功能
istream& operator>>(istream& is, Polynomial& poly) 
{//從輸入流中讀取整數 n，表示多項式的項數。
    int n;
    cout << "輸入項數: ";
    is >> n;
    for (int i = 0; i < n; i++) //根據項數 n 讀取每一項的係數和指數。
    {
        float coef;
        int exp;
        cout << "輸入係數跟指數: ";
        is >> coef >> exp;
        poly.insertTerm(coef, exp);//讀取到的係數和指數插入到 Polynomial 對象中。
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
    cout << "加法結果: " << p3 << endl;

    p3 = p1 - p2;
    cout << "減法結果: " << p3 << endl;
    
    p3 = p1 * p2;
    cout << "乘法結果: " << p3 << endl;

    float x;
    cout << "輸入x的值來計算多項式p1在x的值: ";
    cin >> x;
    float result = p1.Evaluate(x);//計算多項式 p1 在 x 點的值，結果存儲在 result 中。這是通過 Evaluate 成員函數來實現的。
    cout << "p1(" << x << ") = " << result << endl;

    return 0;
}
