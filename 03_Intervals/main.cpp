#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;
class InvalidRangeException
{
};
#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
#define EXTENDED_SYNTAX

class CRange
{
public:
    // constructor
    CRange(const long long &, const long long &);
    ~CRange() {}
    long long getLow() const { return m_low; }
    long long getHigh() const { return m_high; }
    bool operator == (const CRange&) const;
    bool operator != (const CRange&) const;
    friend ostream & operator << (ostream &, const CRange&); // operator <<
private:
    long long m_low;
    long long m_high;
};
CRange::CRange(const long long & lo, const long long & hi) {
    if (lo > hi) throw InvalidRangeException(); else { m_low = lo; m_high = hi; }
}
bool CRange::operator == (const CRange& range) const {
    if (m_low == range.m_low && m_high == range.m_high) return true; else return false;
}
bool CRange::operator != (const CRange& range) const {
    return !(*this == range);
}
ostream& operator<<(ostream & os, const CRange & x) {
    std::ios_base::fmtflags f(os.flags());
    os << dec;
    if (x.getLow() == x.getHigh()) os  << x.getLow();
    else os << "<" << x.getLow() << ".." << x.getHigh() << ">";
    os.flags(f);
    return os;
}

class CRangeListIterator : public std::iterator<std::random_access_iterator_tag, CRange, CRange*, CRange&> {
public:
    CRangeListIterator(CRange* ptr = nullptr) { m_ptr = ptr; }
    CRangeListIterator& operator=(const CRangeListIterator & iterator) = default;
    CRangeListIterator& operator=(CRange* ptr) { m_ptr = ptr; return (*this); }
    bool operator==(const CRangeListIterator& iterator) const { return (m_ptr == iterator.m_ptr); }
    bool operator!=(const CRangeListIterator& iterator) const { return (m_ptr != iterator.m_ptr); }
    CRangeListIterator& operator++() { ++m_ptr; return (*this); }
    CRangeListIterator operator++(int) { CRangeListIterator tmp; tmp.m_ptr = m_ptr++; return tmp; }
    CRange& operator*(){return *m_ptr;}
    const CRange& operator*()const{return *m_ptr;}
    CRange* operator->(){return m_ptr;}
private:
    CRange* m_ptr;
};

class CRangeList
{
public:
    CRangeList() = default; // constructor
    CRangeList(initializer_list<CRange>);
    CRangeList(initializer_list<long long>);
    bool Includes (const long long &) const;// Includes long long / range
    bool Includes (const CRange&) const;
    friend CRangeList operator + (const CRange&, const CRange&); // += range / range list
    CRangeList operator + (const CRange&) const;
    CRangeList operator + (const CRangeList&) const;
    CRangeList & operator += (const CRange&);
    CRangeList & operator += (const CRangeList&);
    friend CRangeList operator - (const CRange&, const CRange&); // -= range / range list
    CRangeList operator - (const CRange&) const;
    CRangeList operator - (const CRangeList&) const;
    CRangeList & operator -= (const CRange&);
    CRangeList & operator -= (const CRangeList&);
    CRangeList & operator = (const CRange&); // = range / range list
    bool operator == (const CRangeList&) const; // operator ==
    bool operator != (const CRangeList&) const; // operator !=
    friend ostream & operator << (ostream &, const CRangeList&); // operator <<

    void pushRange(const CRange & range) { m_rangeList.push_back(range); }
    CRangeListIterator begin() {return &m_rangeList[0];}
    CRangeListIterator end() {return &m_rangeList[m_rangeList.size()];}
private:
    vector<CRange> m_rangeList;

    CRangeList deleteWhenSingleRange(const CRange & range, CRangeList & x) const;
    CRangeList deleteWhenBiggerRange(const CRange & range, CRangeList & x) const;
        long long binarySearch(const CRange & range) const;
    bool insideRange(const long long & m, const CRange & range) const;
    bool biggerRange(const long long & m, const CRange & range) const;
};

CRangeList::CRangeList(initializer_list<CRange> initializerList) {
    for (const auto & range : initializerList) {
        CRangeList tmp;
        tmp.m_rangeList.push_back(range);
        *this += tmp;
    }
}

CRangeList::CRangeList(initializer_list<long long> initializerList) {
    vector<long long> myList;
    for (const auto & i : initializerList) myList.push_back(i);
    std::sort(myList.begin(),myList.end());
    vector<long long> myNewList; //without multiple values
    for (auto it = myList.begin() ; it != myList.end() ; it++) {
        if (it != myList.end()-1) { if ((*it) != *(it+1)) myNewList.push_back(*it); }
        else myNewList.push_back(*it);
    }
    long long tmpLow = myNewList[0];
    long long tmpHigh = myNewList[0];
    for (auto it = myNewList.begin() ; it != myNewList.end() ; it++) {
        if (it != myNewList.end()-1) {
            if (*it == *(it+1)) { tmpHigh = *(it+1); continue; }
            else { *this += CRange(tmpLow, tmpHigh); tmpLow = *(it+1); tmpHigh = *(it+1); continue; }
        } else *this += CRange(tmpLow,tmpHigh);
    }
}

bool valueInRange (const CRange & range, const long long & value) {
    if (value >= range.getLow() && value <= range.getHigh()) return true; else return false;
}
//checks if range1 is included in range2
bool rangeInRange (const CRange & range1, const CRange & range2) {
    if (valueInRange(range2, range1.getLow()) && valueInRange(range2, range1.getHigh())) return true; else return false;
}

bool CRangeList::insideRange(const long long & m, const CRange & range) const{
    return (range.getLow() >= m_rangeList[m].getLow() && range.getHigh() <= m_rangeList[m].getHigh());
}

bool CRangeList::biggerRange(const long long & m, const CRange & range) const {
    return (range.getLow() > m_rangeList[m].getHigh());
}

long long CRangeList::binarySearch(const CRange & range) const {
    long long l = 0;
    long long r = m_rangeList.size()-1;
    while (l <= r) {
        long long m = l + (r - l) / 2;
        if (insideRange(m,range)) return m;
        else if (biggerRange(m, range)) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

bool CRangeList::Includes (const long long & value) const {
    return (binarySearch(CRange(value,value)) != -1);
}

bool CRangeList::Includes (const CRange & range) const {
    return (binarySearch(range) != -1);
}
//it is used in CRange + CRange when low of one CRange is smaller than low of another CRange
void lowSmaller(CRangeList& rangeList, const CRange& smallerLowRange, const CRange& biggerLowRange) {
    if (biggerLowRange.getLow() != LLONG_MIN && smallerLowRange.getHigh() < biggerLowRange.getLow()-1)
    { rangeList.pushRange(smallerLowRange); rangeList.pushRange(biggerLowRange); return; }
    else if (   biggerLowRange.getLow() != LLONG_MIN &&
                smallerLowRange.getHigh() >= biggerLowRange.getLow()-1 &&
                smallerLowRange.getHigh() <= biggerLowRange.getHigh())
        { rangeList.pushRange(CRange(smallerLowRange.getLow(), biggerLowRange.getHigh())); return; }
    else { rangeList.pushRange(smallerLowRange); return; }
}
//it is used in CRange + CRange when low of both CRanges are equal
void lowEqual(CRangeList& rangeList, const CRange& range1, const CRange& range2) {
    if (range1.getHigh() >= range2.getHigh()) rangeList.pushRange(range1); else rangeList.pushRange(range2);
}
CRangeList operator + (const CRange& range1, const CRange& range2) {
    CRangeList x;
    if (range1.getLow() < range2.getLow()) { lowSmaller(x, range1, range2); return x; }
    else if (range1.getLow() == range2.getLow()) { lowEqual(x, range1, range2); return x; }
    else { lowSmaller(x, range2, range1); return x; }
}
CRangeList CRangeList::operator + (const CRange& range) const {
    CRangeList result(*this);
    CRangeList tmp; tmp.m_rangeList.push_back(range);
    result+=tmp; return result;
}
CRangeList CRangeList::operator + (const CRangeList& rangeList) const {
    CRangeList result(*this); result += rangeList; return result;
}
CRangeList & CRangeList::operator += (const CRange & range) {
    CRangeList tmp; tmp.m_rangeList.push_back(range);
    *this += tmp; return *this;
}

void pushRest (vector<CRange> & result, const vector<CRange> & list, long long & index, const long long & size) {
    while(index != size) { result.push_back(list[index]); index++; }
}

void whenOneListIsDone(vector<CRange> & result, const vector<CRange> & list,
        long long & index, const long long & size, const long long & tmpLow, const long long & tmpHigh) {
    if (index == size) return;
    else {
        if (list[index].getLow() != LLONG_MIN && list[index].getLow()-1 > tmpHigh)
        { result.push_back((CRange(tmpLow, tmpHigh))); return;}
        else if (list[index].getLow() != LLONG_MIN && list[index].getLow()-1 <= tmpHigh && list[index].getHigh() >= tmpHigh) {
            result.push_back(CRange(tmpLow, list[index].getHigh())); index++; return;
        } else { index++; whenOneListIsDone(result, list, index, size, tmpLow, tmpHigh); }
    }
}

//first 1 is the one to which we are trying connect so it is smaller
void nestedConnectWithNext(vector<CRange> & result, const vector<CRange> & list1, const vector<CRange> & list2,
        long long & index1, long long & index2, const long long & size1, const long long & size2,
        const long long & tmpLow, long long & tmpHigh) {
    while(true) {
        if(index1 == size1) { whenOneListIsDone(result, list2, index2, size2, tmpLow, tmpHigh); break; }
        if(index2 == size2) { whenOneListIsDone(result, list1, index1, size1, tmpLow, tmpHigh); break; }
        if(list2[index2].getLow() != LLONG_MIN && list1[index1].getHigh() < list2[index2].getLow()-1) {
            result.push_back(CRange(tmpLow, list1[index1].getHigh())); index1++; break;
        }
        if(list1[index1].getLow() != LLONG_MIN && list2[index2].getHigh() < list1[index1].getLow()-1) {
            result.push_back(CRange(tmpLow, list2[index2].getHigh())); index2++; break;
        }
        if (list2[index2].getLow() != LLONG_MIN && list1[index1].getHigh() >= list2[index2].getLow()-1 &&
           list1[index1].getHigh() <= list2[index2].getHigh())
            { index1++; tmpHigh = list2[index2].getHigh(); continue; }
        if (list1[index1].getLow() != LLONG_MIN && list2[index2].getHigh() >= list1[index1].getLow()-1 &&
            list2[index2].getHigh() <= list1[index1].getHigh())
            { index2++; tmpHigh = list1[index1].getHigh(); continue; }
        if ( list1[index1].getHigh() > list2[index2].getHigh())
            { index2++; tmpHigh = list1[index1].getHigh(); continue; }
        if ( list2[index2].getHigh() > list1[index1].getHigh())
            { index1++; tmpHigh = list2[index2].getHigh(); continue; }
        continue;
    }
}

void operatorPlusEqualLowSmaller( vector<CRange> & result, const vector<CRange> & listSmaller,
                                  const vector<CRange> & listBigger, long long & indexSmaller, long long & indexBigger,
                                  const long long & sizeSmaller, const long long & sizeBigger) {
    if (listBigger[indexBigger].getLow() != LLONG_MIN && listSmaller[indexSmaller].getHigh() < listBigger[indexBigger].getLow()-1) {
        result.push_back(listSmaller[indexSmaller]); indexSmaller++;
    } else if ( listBigger[indexBigger].getLow() != LLONG_MIN &&
                listSmaller[indexSmaller].getHigh() >= listBigger[indexBigger].getLow()-1 &&
                listSmaller[indexSmaller].getHigh() <= listBigger[indexBigger].getHigh()) {
        long long tmpLow = listSmaller[indexSmaller].getLow();
        long long tmpHigh = listBigger[indexBigger].getHigh();
        nestedConnectWithNext(result, listSmaller, listBigger, indexSmaller, indexBigger, sizeSmaller, sizeBigger, tmpLow, tmpHigh);
    } else { //bigger is inside smaller;
        indexBigger++; //we have to go to the next one as it is will not affect result
    }
}

void operatorPlusEqualLowEqual( vector<CRange> & result, const vector<CRange> & list1,
                                const vector<CRange> & list2, long long & index1, long long & index2,
                                const long long & size1, const long long & size2) {
    if(list1[index1].getHigh() >= list2[index2].getHigh()) index2++; else index1++;
}

CRangeList & CRangeList::operator += (const CRangeList & rangeList) {
    long long sizeThis = m_rangeList.size(), indexThis = 0, sizeArg = rangeList.m_rangeList.size(), indexArg = 0;
    vector<CRange> result;
    vector<CRange> listThis = m_rangeList;
    vector<CRange> listArg = rangeList.m_rangeList;
    while (indexThis != sizeThis) {
        if (indexArg == sizeArg) { pushRest(result, listThis, indexThis, sizeThis); break; }
        if (listThis[indexThis].getLow() < listArg[indexArg].getLow()) {
            operatorPlusEqualLowSmaller(result, listThis, listArg, indexThis, indexArg, sizeThis, sizeArg);
        } else if (listThis[indexThis].getLow() == listArg[indexArg].getLow()) {
            operatorPlusEqualLowEqual(result, listThis, listArg, indexThis, indexArg, sizeThis, sizeArg);
        } else { //listThis[indexThis].getLow() > listArg[indexArg.getLow())
            operatorPlusEqualLowSmaller(result, listArg, listThis, indexArg, indexThis, sizeArg, sizeThis);
        }
    }
    if (indexArg != sizeArg) pushRest(result, listArg, indexArg, sizeArg);
    m_rangeList = result;
    return *this;
}

CRangeList operator - (const CRange & range1, const CRange & range2) {
    long long low1 = range1.getLow();
    long long low2 = range2.getLow();
    long long high1 = range1.getHigh();
    long long high2 = range2.getHigh();
    CRangeList x;
    if (low1 > high2 || high1 < low2) { x.pushRange(range1); return x; }
    if (low1 >= low2 && high1 <= high2) return x;
    if (low2 > low1 && low2 <= high1) x.pushRange(CRange(low1, low2 - 1));
    if (high2 < high1 && high2 >= low1) x.pushRange(CRange(high2+1, high1));
    return x;
}

CRangeList CRangeList::deleteWhenSingleRange(const CRange & range, CRangeList & x) const {
    long long low = range.getLow();
    long long high = range.getHigh();
    for (auto it = x.m_rangeList.begin() ; it != x.m_rangeList.end() ; it++) {
        if (low == it->getLow() && high == it->getHigh()) { x.m_rangeList.erase(it); return x; }
        if (low < it->getLow()) return x;
        if (low == it->getLow()) { *it = CRange(low+1, it->getHigh()); return x; }
        if (low > it->getLow() && low < it->getHigh()) {
            long long tmpLow = it->getLow();
            long long tmpHigh = it->getHigh();
            it = x.m_rangeList.erase(it);
            it = x.m_rangeList.insert(it, CRange(tmpLow, low-1));
            it++;
            it = x.m_rangeList.insert(it, CRange(high+1, tmpHigh));
            return x;
        }
        if (high == it->getHigh()) { *it = CRange(it->getLow(), high-1); return x;}
    }
    return x;
}

CRangeList CRangeList::deleteWhenBiggerRange(const CRange & range, CRangeList & x) const {
    long long low = range.getLow();
    long long high = range.getHigh();
    for (auto it = x.m_rangeList.begin() ; it != x.m_rangeList.end() ; it++) {
        if (low < it->getLow()) {
            while(true) {
                if (high < it->getLow()) return x;
                if (/*high >= it->getLow() &&*/high < it->getHigh()) {*it = CRange(high+1, it->getHigh()); return x;}
                it = x.m_rangeList.erase(it);
                if (it == x.m_rangeList.end()) return x;
            }
        }
        if (low == it->getLow()) {
            while(true) {
                if (high < it->getLow()) return x;
                if (high < it->getHigh()) { *it = CRange(high+1,it->getHigh()); return x; }
                it = x.m_rangeList.erase(it);
                if (it == x.m_rangeList.end()) return x;
            }
        }

        if (low > it->getLow() && low <= it->getHigh()) {
            long long tmpLow = it->getLow();
            while(true) {
                if (high < it->getLow()) { x.m_rangeList.insert(it,CRange(tmpLow, low-1)); return x;
                }
                if (high < it->getHigh()) {
                    long long tmpHigh = it->getHigh(); //high of the range which I will insert after this one
                    *it = CRange(/*it->getLow()*/tmpLow,low-1);

                    it++;
                    x.m_rangeList.insert(it,CRange(high+1,tmpHigh));
                    return x;
                }
                if (high == it->getHigh()) { *it = CRange(tmpLow,low-1); return x; }
                if (it == x.m_rangeList.end()-1) return x;
                it = x.m_rangeList.erase(it);
            }
        }
    }
    return x;
}

CRangeList CRangeList::operator - ( const CRange& range ) const {
    CRangeList x = *this;
    if (m_rangeList.empty()) return x;
    if (m_rangeList.size() == 1) return *(x.m_rangeList.begin())-range;

    if(range.getLow() == range.getHigh()) return deleteWhenSingleRange(range, x);
    else return deleteWhenBiggerRange(range, x);
}

CRangeList CRangeList::operator - ( const CRangeList& rangeList ) const {
    CRangeList x = *this;
    for (auto it = rangeList.m_rangeList.begin() ; it != rangeList.m_rangeList.end() ; it++ ) x = x - *it;
    return x;
}

CRangeList & CRangeList::operator -= ( const CRange& range) {
    *this = *this - range;
    return *this;
}

CRangeList & CRangeList::operator -= ( const CRangeList& rangeList) {
    for (auto it = rangeList.m_rangeList.begin() ; it != rangeList.m_rangeList.end() ; it++ ) *this -= *it;
    return *this;
}

CRangeList & CRangeList::operator = ( const CRange & range) {
    m_rangeList.clear(); m_rangeList.push_back(range); return *this;
}
bool CRangeList::operator == (const CRangeList& rangeList) const {
    if (m_rangeList.size() != rangeList.m_rangeList.size()) return false;
    for (unsigned long long i = 0 ; i < m_rangeList.size() ; i++)
        if (m_rangeList[i] != rangeList.m_rangeList[i]) return false;
    return true;
}
bool CRangeList::operator != (const CRangeList& rangeList) const {
    if (m_rangeList.size() != rangeList.m_rangeList.size()) return true;
    for (unsigned long long i = 0 ; i < m_rangeList.size() ; i++)
        if (m_rangeList[i] == rangeList.m_rangeList[i]) return false;
    return true;
}

ostream& operator<<(ostream & os, const CRangeList & x) {
    std::ios_base::fmtflags f(os.flags());
    os << dec;
    os << "{";
    for (auto it = x.m_rangeList.begin() ; it != x.m_rangeList.end() ; it++) {
        if (it->getLow() == it->getHigh()) os << it->getLow();
        else os << "<" << it->getLow() << ".." << it->getHigh() << ">";
        if (it != x.m_rangeList.end()-1) os << ",";
    }
    os << "}";
    os.flags(f);
    return os;
}

#ifndef __PROGTEST__
string toString ( const CRangeList& x )
{
    ostringstream oss;
    oss << x;
    return oss . str ();
}

void testCRangePlusCRange() {
    CRangeList a;
    a = CRange(10,20) + CRange(30,40);
    assert( toString(a) == "{<10..20>,<30..40>}");
    a = CRange(10,20) + CRange(22,40);
    assert( toString(a) == "{<10..20>,<22..40>}");
    a = CRange(10,20) + CRange(21,40);
    assert( toString(a) == "{<10..40>}");
    a = CRange(10,20) + CRange(20,40);
    assert( toString(a) == "{<10..40>}");
    a = CRange(10,20) + CRange(11,40);
    assert( toString(a) == "{<10..40>}");
    a = CRange(10,20) + CRange(10,40);
    assert( toString(a) == "{<10..40>}");
    a = CRange(10,20) + CRange(10,20);
    assert( toString(a) == "{<10..20>}");
    a = CRange(10,20) + CRange(10,21);
    assert( toString(a) == "{<10..21>}");
    a = CRange(30,40) + CRange(10,20);
    assert( toString(a) == "{<10..20>,<30..40>}");
    a = CRange(22,40) + CRange(10,20);
    assert( toString(a) == "{<10..20>,<22..40>}");
    a = CRange(21,40) + CRange(10,20);
    assert( toString(a) == "{<10..40>}");
    a = CRange(20,40) + CRange(10,20);
    assert( toString(a) == "{<10..40>}");
    a = CRange(11,40) + CRange(10,20);
    assert( toString(a) == "{<10..40>}");
    a = CRange(LLONG_MIN,999999) + CRange(-333333,LLONG_MAX);
    assert( toString(a) == "{<-9223372036854775808..9223372036854775807>}" );
    assert (a.Includes(20000));
    assert (a.Includes(CRange(1000,5000000)));

}
void testCRangeMinusCRange() {
    CRangeList a;
    a = CRange(LLONG_MIN,LLONG_MAX) - CRange(0,0);
    assert( toString(a) == "{<-9223372036854775808..-1>,<1..9223372036854775807>}" );
    a = CRange(LLONG_MIN,20) - CRange(10,20);
    assert( toString(a) == "{<-9223372036854775808..9>}" );
    a = CRange(LLONG_MIN,20) - CRange(20,20);
    assert( toString(a) == "{<-9223372036854775808..19>}" );
    a = CRange(LLONG_MIN,20) - CRange(21,30);
    assert( toString(a) == "{<-9223372036854775808..20>}" );
    a = CRange(-10000000,LLONG_MAX) - CRange(-20000000,-1000000);
    assert( toString(a) == "{<-999999..9223372036854775807>}" );
    a = CRange(30000,LLONG_MAX) - CRange(10000,35555);
    assert( toString(a) == "{<35556..9223372036854775807>}" );
    a = CRange(100,LLONG_MAX) - CRange(0,99);
    assert( toString(a) == "{<100..9223372036854775807>}" );
    a = CRange(LLONG_MIN,LLONG_MAX) - CRange(LLONG_MIN,LLONG_MAX);
    assert( toString(a) == "{}" );
    a = CRange(LLONG_MIN,LLONG_MAX) - CRange(LLONG_MIN+1,LLONG_MAX-1);
    assert( toString(a) == "{-9223372036854775808,9223372036854775807}" );

}
void testCRangeListPlusEqualsCRangeList() {
    CRangeList a,b,c,d,e,f,g,h;
    a += CRange(10,20) + CRange(30,40) + CRange(42,42) + CRange(51,60)+ CRange(62,62) + CRange(64,64) + CRange(66,66) + CRange(68,68) + CRange(71,80);
    b = CRange(25,29) + CRange(43,70);
    a+=b;
    assert ( toString(a) == "{<10..20>,<25..40>,<42..80>}");
    b = CRange(0,100);
    c = a;
    a+=b;
    assert( toString(a) == "{<0..100>}");
    b = CRange(100,150) + CRange(200,250) + CRange(300,350);
    d = b + c;
    assert ( toString(d) == "{<10..20>,<25..40>,<42..80>,<100..150>,<200..250>,<300..350>}");
    e = CRange(21,24) + CRange(41,41) + CRange(81,99) + CRange(151,199) + CRange(251,299) + CRange(351,1000) + CRange(2000000,LLONG_MAX);
    f = d + e;
    assert ( toString(f) == "{<10..1000>,<2000000..9223372036854775807>}");
    a = CRange(21,24) + CRange(41,41) + CRange(81,199) + CRange(151,199) + CRange(251,299) + CRange(352,1000) + CRange(2000000,LLONG_MAX);
    f = d + a;
    assert ( toString(f) == "{<10..350>,<352..1000>,<2000000..9223372036854775807>}");
    a = CRange(352,2000007) + CRange(-100, 250) + CRange(-1000,-1000);
    f += a;
    assert ( toString(f) == "{-1000,<-100..350>,<352..9223372036854775807>}");
    g = CRange(-2,-2) + CRange(0,0) + CRange(2,2) + CRange(4,4);
    h = CRange(-5,-1) + CRange(3,3) + CRange(5,10) + CRange(20,30) + CRange(3000, 3500);
    g += h;
    assert ( toString(g) == "{<-5..0>,<2..10>,<20..30>,<3000..3500>}");

    a = CRange(LLONG_MIN, LLONG_MAX) - CRange(LLONG_MAX,LLONG_MAX);
    b = CRange(LLONG_MIN, LLONG_MAX-1);
    c = a + a + a + a + a;
    a = CRange(LLONG_MIN,LLONG_MAX-1) + CRange(LLONG_MIN, LLONG_MAX-1);
}
static void myTest0() {
    CRangeList a;
    assert(sizeof (CRange) <= 2*sizeof(long long));
    a = CRange(5,10);
    assert ( toString ( a ) == "{<5..10>}" );
    a = CRange(1,1);
    assert ( toString ( a ) == "{1}" );
    a = CRange(1,1) + CRange(1,2);
    assert ( toString ( a ) == "{<1..2>}" );
    a = CRange(1,2) + CRange(1,1);
    assert ( toString ( a ) == "{<1..2>}" );

    //2 ranges of more than one element
    a = CRange(1,5) + CRange(5,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,5) + CRange(6,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,5) + CRange(7,10);
    assert ( toString ( a ) == "{<1..5>,<7..10>}" );
    a = CRange(1,10) + CRange(7,9);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,10) + CRange(7,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,10) + CRange(7,11);
    assert ( toString ( a ) == "{<1..11>}" );

    a = CRange(1,5) + CRange(1,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,10) + CRange(1,5);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,10) + CRange(1,10);
    assert ( toString ( a ) == "{<1..10>}" );

    a = CRange(5,10) + CRange(1,5);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(6,10) + CRange(1,5);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(7,10) + CRange(1,5);
    assert ( toString ( a ) == "{<1..5>,<7..10>}" );
    a = CRange(7,9) + CRange(1,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(7,10) + CRange(1,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(7,11) + CRange(1,10);
    assert ( toString ( a ) == "{<1..11>}" );

    //one range with one element, one range with more than one element
    a = CRange(1,1) + CRange(1,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,1) + CRange(2,10);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(1,1) + CRange(3,10);
    assert ( toString ( a ) == "{1,<3..10>}" );
    a = CRange(1,10) + CRange(1,1);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(2,10) + CRange(1,1);
    assert ( toString ( a ) == "{<1..10>}" );
    a = CRange(3,10) + CRange(1,1);
    assert ( toString ( a ) == "{1,<3..10>}" );

    a = CRange(3,9) + CRange(10,10);
    assert ( toString ( a ) == "{<3..10>}" );
    a = CRange(100,500) + CRange(105,105);
    assert ( toString ( a ) == "{<100..500>}" );

    //two ranges with one element
    a = CRange(1,1) + CRange(1,1);
    assert ( toString ( a ) == "{1}" );
    a = CRange(1,1) + CRange(2,2);
    assert ( toString ( a ) == "{<1..2>}" );
    a = CRange(1,1) + CRange(3,3);
    assert ( toString ( a ) == "{1,3}" );
    a = CRange(2,2) + CRange(1,1);
    assert ( toString ( a ) == "{<1..2>}" );
    a = CRange(3,3) + CRange(1,1);
    assert ( toString ( a ) == "{1,3}" );
}
static void myTest1() {
    CRangeList a;

    a = CRange(1,1) + CRange(1,2) + CRange(-1,-1);
    assert ( toString(a) == "{-1,<1..2>}");
    a = CRange(0,0) + CRange(0,0) + CRange(1,2);
    assert ( toString(a) == "{<0..2>}");
    a = CRange(1,1) + CRange(1,2) + CRange(1,1);
    assert ( toString(a) == "{<1..2>}");
    a = CRange(1,1) + CRange(1,2) + CRange(2,2);
    assert ( toString(a) == "{<1..2>}");
    a = CRange(1,5) + CRange(7,10) + CRange(6,6);
    assert ( toString(a) == "{<1..10>}");
    a = CRange(1,5) + CRange(8,10) + CRange(6,6);
    assert ( toString(a) == "{<1..6>,<8..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(6,6);
    assert ( toString(a) == "{<1..4>,6,<8..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(7,7);
    assert ( toString(a) == "{<1..4>,<7..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(8,8);
    assert ( toString(a) == "{<1..4>,<8..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(9,9);
    assert ( toString(a) == "{<1..4>,<8..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(10,10);
    assert ( toString(a) == "{<1..4>,<8..10>}");
    a = CRange(1,4) + CRange(8,10) + CRange(11,11);
    assert ( toString(a) == "{<1..4>,<8..11>}");
    a = CRange(1,4) + CRange(8,10) + CRange(12,12);
    assert ( toString(a) == "{<1..4>,<8..10>,12}");

}
static void myTest2() {
    CRangeList a;

    //I - low < first
    a = CRange(10, 15) + CRange(20,25) + CRange(0,5);
    assert ( toString ( a ) == "{<0..5>,<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,8);
    assert ( toString ( a ) == "{<5..8>,<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,10);
    assert ( toString ( a ) == "{<5..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,12);
    assert ( toString ( a ) == "{<5..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,15);
    assert ( toString ( a ) == "{<5..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,16);
    assert ( toString ( a ) == "{<5..16>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,19);
    assert ( toString ( a ) == "{<5..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,20);
    assert ( toString ( a ) == "{<5..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,25);
    assert ( toString ( a ) == "{<5..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(5,26);
    assert ( toString ( a ) == "{<5..26>}" );
    a = CRange(55,60) + CRange(42,50)  + CRange(30,40) + CRange(5,26) +  CRange(3,53);

    //II - inside the previous
    a = CRange(10, 15) + CRange(20,25) + CRange(10,15);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(10,14);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(11,14);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(11,15);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );

    //III - go from inside of the previous and finish after its end
    a = CRange(10, 15) + CRange(20,25) + CRange(14,16);
    assert ( toString ( a ) == "{<10..16>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(14,19);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(14,20);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(10,17);
    assert ( toString ( a ) == "{<10..17>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(10,19);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(10,20);
    assert ( toString ( a ) == "{<10..25>}" );

    //IV - connect on the border with the previous
    a = CRange(10, 15) + CRange(20,25) + CRange(15,16);
    assert ( toString ( a ) == "{<10..16>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(15,19);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(15,20);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(15,26);
    assert ( toString ( a ) == "{<10..26>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(16,19);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(16,20);
    assert ( toString ( a ) == "{<10..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(16,26);
    assert ( toString ( a ) == "{<10..26>}" );

    //V - try to connect with the next
    a = CRange(10, 15) + CRange(20,25) + CRange(17,18);
    assert ( toString ( a ) == "{<10..15>,<17..18>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(17,19);
    assert ( toString ( a ) == "{<10..15>,<17..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(17,20);
    assert ( toString ( a ) == "{<10..15>,<17..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(17,24);
    assert ( toString ( a ) == "{<10..15>,<17..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(17,27);
    assert ( toString ( a ) == "{<10..15>,<17..27>}" );

    //VI - inside the next
    a = CRange(10, 15) + CRange(20,25) + CRange(20,25);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(20,24);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(21,24);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(21,25);
    assert ( toString ( a ) == "{<10..15>,<20..25>}" );

    //VII - go from inside of the next
    a = CRange(10, 15) + CRange(20,25) + CRange(23,35);
    assert ( toString ( a ) == "{<10..15>,<20..35>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(25,35);
    assert ( toString ( a ) == "{<10..15>,<20..35>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(26,35);
    assert ( toString ( a ) == "{<10..15>,<20..35>}" );
    a = CRange(10, 15) + CRange(20,25) + CRange(27,35);
    assert ( toString ( a ) == "{<10..15>,<20..25>,<27..35>}" );

    //VIII - some other
    a = CRange(10, 20) + CRange(30,40) + CRange(50,60) + CRange(70,80) + CRange (43,47);
    assert ( toString ( a ) == "{<10..20>,<30..40>,<43..47>,<50..60>,<70..80>}" );

    //IX - adding to empty list or list with one element
    CRangeList b;
    a = b + CRange(1,1);
    assert ( toString(a) == "{1}");
    assert ( toString(b) == "{}");
    a = b + CRange(1,5);
    assert ( toString(a) == "{<1..5>}");
    assert ( toString(b) == "{}");
    a = CRange(1,5);
    a = a + CRange(10,10);
    assert ( toString(a) == "{<1..5>,10}");
    a = CRange(1,5);
    a = a + CRange(10,15);
    assert ( toString(a) == "{<1..5>,<10..15>}");
}
static void myTest3() {
    CRangeList a;
    CRangeList b;
    CRangeList c;

    a = CRange(1,4) + CRange(8,12);
    b = CRange(5,11) + CRange(13,18);
    c = a + b;
    assert ( toString(a) == "{<1..4>,<8..12>}");
    assert ( toString(b) == "{<5..11>,<13..18>}");
    assert ( toString(c) == "{<1..18>}");

    a += CRange(75,80);
    assert ( toString(a) == "{<1..4>,<8..12>,<75..80>}");

    a += CRange(-1,90) + CRange(95,100);
    assert ( toString(a) == "{<-1..90>,<95..100>}");

    assert ( !a.Includes(CRange(-2,91)));
    assert ( !a.Includes(CRange(-2,90)));
    assert ( !a.Includes(CRange(-1,91)));
    assert ( !a.Includes(CRange(-5,-1)));
    assert ( a.Includes(CRange(-1,90)));
    assert ( a.Includes(CRange(0,89)));
    assert ( !a.Includes(CRange(-1,100)));
    assert ( !a.Includes(-10));
    assert ( !a.Includes(-2));
    assert ( !a.Includes(91));
    assert ( !a.Includes(94));
    assert ( !a.Includes(101));
    assert ( !a.Includes(200));
    assert ( a.Includes(0));
    assert ( a.Includes(88));
    assert ( a.Includes(43));
    assert ( a.Includes(90));
    assert ( a.Includes(95));
    assert ( a.Includes(98));
    assert ( a.Includes(100));
    b = a;
    assert ( b == a);
    assert ( !(b != a) );
    b = CRange(1,1);
    assert (b != a);
    assert (!(b == a));
}

int                main                                    ( void )
{
    CRangeList a, b;
    testCRangePlusCRange();
    testCRangeMinusCRange();
    testCRangeListPlusEqualsCRangeList();
    myTest0();
    myTest1();
    myTest2();
    myTest3();

    assert ( sizeof ( CRange ) <= 2 * sizeof ( long long ) );
    a = CRange ( 5, 10 );
    a += CRange ( 25, 100 );
    assert ( toString ( a ) == "{<5..10>,<25..100>}" );
    a += CRange ( -5, 0 );
    a += CRange ( 8, 50 );
    assert ( toString ( a ) == "{<-5..0>,<5..100>}" );
    a += CRange ( 101, 105 ) + CRange ( 120, 150 ) + CRange ( 160, 180 ) + CRange ( 190, 210 );
    assert ( toString ( a ) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}" );
    a += CRange ( 106, 119 ) + CRange ( 152, 158 );
    assert ( toString ( a ) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}" );
    a += CRange ( -3, 170 );
    a += CRange ( -30, 1000 );
    assert ( toString ( a ) == "{<-30..1000>}" );
    b = CRange ( -500, -300 ) + CRange ( 2000, 3000 ) + CRange ( 700, 1001 );
    a += b;
    assert ( toString ( a ) == "{<-500..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( -400, -400 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( 10, 20 ) + CRange ( 900, 2500 ) + CRange ( 30, 40 ) + CRange ( 10000, 20000 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    try
    {
        a += CRange ( 15, 18 ) + CRange ( 10, 0 ) + CRange ( 35, 38 );
        assert ( "Exception not thrown" == NULL );
    }
    catch ( const InvalidRangeException & e )
    {
    }
    catch ( ... )
    {
        assert ( "Invalid exception thrown" == NULL );
    }
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    b = a;
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 2600, 2700 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 15, 15 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}" );
    assert ( !( a == b ) );
    assert ( a != b );
    assert ( b . Includes ( 15 ) );
    assert ( b . Includes ( 2900 ) );
    assert ( b . Includes ( CRange ( 15, 15 ) ) );
    assert ( b . Includes ( CRange ( -350, -350 ) ) );
    assert ( b . Includes ( CRange ( 100, 200 ) ) );
    assert ( !b . Includes ( CRange ( 800, 900 ) ) );
    assert ( !b . Includes ( CRange ( -1000, -450 ) ) );
    assert ( !b . Includes ( CRange ( 0, 500 ) ) );
    a += CRange ( -10000, 10000 ) + CRange ( 10000000, 1000000000 );
    assert ( toString ( a ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b += a;
    assert ( toString ( b ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b -= a;
    assert ( toString ( b ) == "{}" );
    b += CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 );
    assert ( toString ( b ) == "{<0..100>,<160..169>,<171..180>,<251..300>}" );
    b -= CRange ( 10, 90 ) - CRange ( 20, 30 ) - CRange ( 40, 50 ) - CRange ( 60, 90 ) + CRange ( 70, 80 );
    assert ( toString ( b ) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}" );
#ifdef EXTENDED_SYNTAX
    CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
    assert ( toString ( x ) == "{<-9..20>,<48..93>,<150..200>}" );
    ostringstream oss;
    oss << setfill ( '=' ) << hex << left;
    for ( const auto & v : x + CRange ( -100, -100 ) )
        oss << v << endl;
    oss << setw ( 10 ) << 1024;
    assert ( oss . str () == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======" );

    CRangeList y {};
    assert (toString(y) == "{}");

    CRangeList z { {10,40}, {35,38}, {50,70}, {-100,-29} };
    ostringstream oss2;
    oss2 << oct;
    for (auto it = z.begin(); it != z.end() ; it++) {
        oss2 << (*it) << endl;
    }
    oss2 << 1024;
    assert ( oss2.str() == "<-100..-29>\n<10..40>\n<50..70>\n2000" );

    CRangeList g { {10,10} };
    g += CRange(20,20);
    ostringstream oss3;
    oss3 << dec;
    for (auto it = g.begin() ; it != g.end() ; ++it) {
        oss3 << (*it) << endl;
    }
    assert ( oss3.str() == "10\n20\n");

    cout << "ALL TESTS PASSED" << endl;
#endif /* EXTENDED_SYNTAX */
    return 0;
}
#endif /* __PROGTEST__ */
