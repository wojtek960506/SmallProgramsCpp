#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#include <chrono>

using namespace std;
#endif /* __PROGTEST__ */

const long long whenToDelete = 500;

class CLand {
public:
    string m_region;
    unsigned int m_id;
    string m_city;
    string m_address;
    string m_owner;
    unsigned long int m_registrationId;
public:
    CLand(string region, unsigned int id, string city, string address,
          unsigned long int registrationId, string owner = "") :
            m_region(region), m_id(id), m_city(city), m_address(address),
            m_owner(owner), m_registrationId(registrationId){}
    ~CLand() {}
    void setOwner(string owner) { m_owner = owner; }
    void setRegistrationId(unsigned long int registrationId) { m_registrationId = registrationId; }
    bool checkIfEqualCA(const string & city, const string & addr);
    bool checkIfEqualRI(const string & region, unsigned int id);
    bool compareOwner(const string owner1) const;
};

bool CLand::checkIfEqualCA(const string & city, const string & addr) {
    return (m_city == city) && (m_address == addr);
}

bool CLand::checkIfEqualRI(const string & region, unsigned int id) {
    return (m_region == region) && (m_id == id);
}

bool CLand::compareOwner(const string owner1) const {
    if (m_owner.length() != owner1.length())
        return false;
    for (unsigned int i = 0 ; i < m_owner.length() ; i++) {
        if (toupper(m_owner[i]) != toupper(owner1[i]))
            return false;
    }
    return true;
}

class COwner {
private:
    string m_name;
    unsigned long int m_numOfLandLots;
    vector<CLand*> m_landLots;
    vector<bool> m_checkIfOwns; //not to delete every time a landlot when we change the owner
public:
    COwner(string name = "", unsigned long int numOfLandLots = 0) : m_name(name), m_numOfLandLots(numOfLandLots) {}
    ~COwner();
    string getName() const { return m_name; }
    unsigned long int getNumOfLandLots() const { return m_numOfLandLots; }
    const vector<CLand*> * getLandLotsPointer() const { return &m_landLots; }
    const vector<bool> * getCheckIfOwnsPointer() const {return &m_checkIfOwns; }
    void addLandLot(CLand* newLandLotPtr);
    void deleteLandLot(unsigned long int index); //to delete land lot by making it unvisible
    void reallyDeleteLandLot(unsigned long int index); //to delete land lot from owners list
    bool compareOwner(const string owner1) const;
    bool slowCompareOwner(const string owner1) const;
    bool isSmallerOwner(const string owner1) const;
    bool slowIsSmallerOwner(const string owner1) const;
    long long int binarySearch(unsigned long int registrationId) const;
    void cleanDeletedOwners();
};

COwner::~COwner() {
    for (auto it = m_landLots.begin() ; it != m_landLots.end() ; it++) {
        delete (*it);
    }
    m_landLots.clear();
    m_name = "";
    m_numOfLandLots = 0;
}

void COwner::cleanDeletedOwners() {
    auto it1 = m_landLots.begin();
    auto it2 = m_checkIfOwns.begin();

    while (it1 != m_landLots.end()) {
        if ((*it2) == false) {
            delete (*it1);
            it1 = m_landLots.erase(it1);
            it2 = m_checkIfOwns.erase(it2);
        } else {
            it1++;
            it2++;
        }
    }
}

void COwner::addLandLot(CLand* newLandLotPtr){
    string region = newLandLotPtr->m_region;
    unsigned int id = newLandLotPtr->m_id;
    string city = newLandLotPtr->m_city;
    string address = newLandLotPtr->m_address;
    unsigned long int registrationId = newLandLotPtr->m_registrationId;
    string owner = newLandLotPtr->m_owner;
    CLand * tmpLandLotPtr = new CLand(region,id, city, address, registrationId, owner);
    ++m_numOfLandLots;
    m_landLots.push_back(tmpLandLotPtr);
    m_checkIfOwns.push_back(true);
}
//actully it just marks that there is now a different owner
void COwner::deleteLandLot(unsigned long int index) {
    --m_numOfLandLots;
    m_checkIfOwns[index] = false; //we don't actually delete the landlot, but we mark that it is not owned by him
}

void COwner::reallyDeleteLandLot(unsigned long int index) {
    --m_numOfLandLots;
    delete m_landLots[index];
    m_landLots.erase(m_landLots.begin()+index);
    m_checkIfOwns.erase(m_checkIfOwns.begin()+index);
}

long long int COwner::binarySearch(unsigned long int registrationId) const{
    long long int i = 0; //to iterate over elements
    long long int size = m_landLots.size()-1;
    while ( i <= size ) {
        //current position to check (the middle)
        unsigned long int m = i + (size - i) / 2;
        //check if the i-th element of an array is equal the arguments
        if (m_landLots[m]->m_registrationId == registrationId)
            return m;
        else if (m_landLots[m]->m_registrationId < registrationId)
            i = m + 1; //if element is greater than middle then ignore left half
        else
            size = m - 1; //if element is smaller than middle then ignore right half
    }
    return -1; //if we are here, it means that element was not found
}

bool COwner::compareOwner(const string owner1) const {
    if (m_name.length() != owner1.length())
        return false;
    for (unsigned int i = 0 ; i < m_name.length() ; i++) {
        if (toupper(m_name[i]) != toupper(owner1[i]))
            return false;
    }
    return true;
}

bool COwner::slowCompareOwner(const string owner1) const {
    string tmp_owner0 = m_name;
    string tmp_owner1 = owner1;
    for (auto & c : tmp_owner0) c = toupper(c);
    for (auto & c : tmp_owner1) c = toupper(c);
    if (tmp_owner0 == tmp_owner1)
        return true;
    else
        return false;
}

//return true if owner1 with UPPER letters is smaller than owner2 with UPPER letters
bool COwner::isSmallerOwner(const string owner1) const {
    if (m_name.length() == 0 && owner1.length() > 0)
        return true;
    if (owner1.length() == 0)
        return false;
    for (unsigned int i = 0 ; i < m_name.length() ; i++) {
        if (toupper(m_name[i]) < toupper(owner1[i]))
            return true;
        else if (toupper(m_name[i]) > toupper(owner1[i]))
            return false;
        else {
            if (i == owner1.length()-1)
                return false;
        }
    }
    return true;
}

//return true if owner1 with UPPER letters is smaller than owner2 with UPPER letters
bool COwner::slowIsSmallerOwner(const string owner1) const {
    string tmp_owner0 = m_name;
    string tmp_owner1 = owner1;
    for (auto & c : tmp_owner0) c = toupper(c);
    for (auto & c : tmp_owner1) c = toupper(c);
    if (tmp_owner0 < tmp_owner1)
        return true;
    else
        return false;
}

//full declaration is done later but so far I need CLandRegister to know about this class
class CIterator;

class CLandRegister
{
public:
    CLandRegister() { m_registrationId = 0; m_counterToDelete = 0; }
    ~CLandRegister();
    //copying objects of class CLandRegister is not allowed
    CLandRegister(const CLandRegister &) = delete;
    CLandRegister & operator=(const CLandRegister&) = delete;

    bool Add ( const string & city, const string & addr, const string & region, unsigned int id );
    bool Del ( const string & city, const string & addr );
    bool Del ( const string & region, unsigned int id );
    bool GetOwner ( const string & city, const string & addr, string & owner ) const;
    bool GetOwner ( const string & region, unsigned int id, string & owner ) const;
    bool NewOwner ( const string & city, const string & addr, const string & owner );
    bool NewOwner ( const string & region, unsigned int id, const string & owner );
    unsigned Count ( const string & owner ) const;
    CIterator ListByAddr ( void ) const;
    CIterator ListByOwner ( const string & owner ) const;
    const vector<CLand*> * getLandLotsSortedByCAPointer() const { return &m_landLotsSortedByCA; }
private:
    vector<CLand*> m_landLotsSortedByCA; //city and address
    vector<CLand*> m_landLotsSortedByRI; //region and id
    vector<COwner*> m_owners; //list of pointers to objects COwner to
    unsigned long int m_registrationId;
    unsigned long int m_counterToDelete;
    unsigned long int registerLandLot() { m_counterToDelete++; return m_registrationId++; }
public:
    long long int binarySearchCA(const string & city, const string & addr) const;
    long long int binarySearchRI(const string & region, unsigned int id) const;
    long long int binarySearchCOwners(const string & owner) const;
private:
    void pushAndSortCA(CLand * newLandLotPtr);
    void pushAndSortRI(CLand * newLandLotPtr);
    void pushAndSortCOwners(COwner * newOwnerPtr);
    static bool compareCA(const CLand * c1, const CLand * c2);
    static bool compareRI(const CLand * c1, const CLand * c2);
    static bool compareCOwners(const COwner * c1, const COwner * c2);
    void cleanDeletedOwners();
};

class CIterator
{
public:
    CIterator(const vector<CLand*> * landLotsPtr, const vector<bool> * checkIfOwnsPtr = NULL , bool ownerCase = false );
    ~CIterator() { m_index = 0; m_landLotsPtr = NULL; }
    bool AtEnd ( void ) const {if (m_landLotsPtr == NULL) return true; return m_index == m_landLotsPtr->size();}
    void Next ( void );// { ifm_index++; }
    string City ( void )    const   { return m_landLotsPtr->at(m_index)->m_city;}
    string Addr ( void )    const   { return m_landLotsPtr->at(m_index)->m_address; }
    string Region ( void )  const   { return m_landLotsPtr->at(m_index)->m_region; }
    unsigned ID ( void )    const   { return m_landLotsPtr->at(m_index)->m_id; }
    string Owner ( void )   const   { return m_landLotsPtr->at(m_index)->m_owner; }
private:
    const vector<CLand*> * m_landLotsPtr;
    const vector<bool> * m_checkIfOwnsPtr;
    bool m_ownerCase;
    unsigned long int m_index;
};

CIterator::CIterator(const vector<CLand*> * landLotsPtr, const vector<bool> * checkIfOwnsPtr, bool ownerCase) :
        m_landLotsPtr(landLotsPtr), m_checkIfOwnsPtr(checkIfOwnsPtr), m_ownerCase(ownerCase)
{
    m_index = 0;
    if (m_ownerCase) { while (m_checkIfOwnsPtr->at(m_index) == false) m_index++; }
}

void CIterator::Next() {
    m_index++;
    if (m_ownerCase) { //if the first condition will be false then it will automatically close
        while ((m_index < m_checkIfOwnsPtr->size()) && (m_checkIfOwnsPtr->at(m_index) == false))
            m_index++;
    }
}

CLandRegister::~CLandRegister() {
    m_registrationId = 0;
    m_counterToDelete = 0;
    for (auto it = m_landLotsSortedByCA.begin() ; it != m_landLotsSortedByCA.end() ; it++ ) {
        delete (*it);
    }
    m_landLotsSortedByCA.clear();
    m_landLotsSortedByRI.clear();
    for (auto it = m_owners.begin() ; it != m_owners.end() ; it++ ) {
        delete (*it);
    }
    m_owners.clear();
}

void CLandRegister::cleanDeletedOwners() {
    auto it = m_owners.begin();
    while (it != m_owners.end()) {
        (*it)->cleanDeletedOwners();
        if ((*it)->getNumOfLandLots() == 0) {
            delete (*it);
            it = m_owners.erase(it);
        } else {
            it ++;
        }
    }

}

void CLandRegister::pushAndSortCA(CLand * newLandLotPtr) {
    for (auto it = m_landLotsSortedByCA.begin() ; it != m_landLotsSortedByCA.end() ; it++) {
        if(compareCA(newLandLotPtr,*it)) {
            m_landLotsSortedByCA.insert(it, newLandLotPtr);
            return;
        }
    }
    m_landLotsSortedByCA.push_back(newLandLotPtr);
}

void CLandRegister::pushAndSortRI(CLand * newLandLotPtr) {
    for (auto it = m_landLotsSortedByRI.begin() ; it != m_landLotsSortedByRI.end() ; it++) {
        if(compareRI(newLandLotPtr,*it)) {
            m_landLotsSortedByRI.insert(it, newLandLotPtr);
            return;
        }
    }
    m_landLotsSortedByRI.push_back(newLandLotPtr);
}

void CLandRegister::pushAndSortCOwners(COwner * newOwnerPtr) {
    for (auto it = m_owners.begin() ; it != m_owners.end() ; it++) {
        if(compareCOwners(newOwnerPtr, *it)) {
            m_owners.insert(it, newOwnerPtr);
            return;
        }
    }
    m_owners.push_back(newOwnerPtr);
}

long long int CLandRegister::binarySearchCA(const string & city, const string & addr) const{
    long long int i = 0; //to iterate over elements
    long long int size = m_landLotsSortedByCA.size()-1;
    CLand * tmp = new CLand("",0,city,addr,0);
    while ( i <= size ) {
        //current position to check (the middle)
        unsigned long int m = i + (size - i) / 2;
        //check if the i-th element of an array is equal the arguments
        if(m_landLotsSortedByCA[m]->checkIfEqualCA(city,addr)) { delete tmp; return m; }
            //if element is greater than middle then ignore left half
        else if(compareCA(m_landLotsSortedByCA[m], tmp)) { i = m + 1; }
            //if element is smaller than middle then ignore right half
        else { size = m-1; }
    }
    delete tmp;
    return -1; //if we are here, it means that element was not found
}

long long int CLandRegister::binarySearchRI(const string & region, unsigned int id)  const{
    long long int i = 0; //to iterate over elements
    long long int size = m_landLotsSortedByRI.size()-1;
    CLand * tmp = new CLand(region,id,"","",0);
    while ( i <= size ) {
        //current position to check (the middle)
        unsigned long int m = i + (size - i) / 2;
        //check if the i-th element of an array is equal the arguments
        if(m_landLotsSortedByRI[m]->checkIfEqualRI(region,id)) { delete tmp; return m; }
            //if element is greater than middle then ignore left half
        else if(compareRI(m_landLotsSortedByRI[m], tmp)) { i = m + 1; }
            //if element is smaller than middle then ignore right half
        else { size = m-1; }
    }
    delete tmp;
    return -1; //if we are here, it means that element was not found
}

long long int CLandRegister::binarySearchCOwners(const string & owner) const{
    long long int i = 0; //to iterate over elements
    long long int size = m_owners.size()-1;
    COwner * tmp = new COwner(owner);
    while ( i <= size ) {
        //current position to check (the middle)
        unsigned long int m = i + (size - i) / 2;
        //check if the i-th element of an array is equal the arguments
        if(m_owners[m]->compareOwner(owner)) { delete tmp; return m; }
            //if element is greater than middle then ignore left half
        else if(compareCOwners(m_owners[m], tmp)) { i = m + 1; }
            //if element is smaller than middle then ignore right half
        else { size = m-1; }
    }
    delete tmp;
    return -1; //if we are here, it means that element was not found
}

//first check city1 < city2 and if the city is the same then check address1 < address2
bool CLandRegister::compareCA(const CLand * c1, const CLand * c2) {
    if(c1->m_city < c2->m_city) { return true; }
    else if (c1->m_city > c2->m_city) { return false; }
    else { if (c1->m_address < c2->m_address) return true; else return false; }
}

//first check id1 < id2 and if the id is the same then check region1 < region2
bool CLandRegister::compareRI(const CLand * c1, const CLand * c2) {
    if(c1->m_id < c2->m_id) return true;
    else if (c1->m_id > c2->m_id) return false;
    else { if (c1->m_region < c2->m_region) return true; else return false; }
}

//just compare owner names in alphabetic order (preserve the name - UPPER use only to compare)
bool CLandRegister::compareCOwners(const COwner * c1, const COwner * c2) {
    if(c1->isSmallerOwner(c2->getName())) return true;
    else return false;
}

bool CLandRegister::Add ( const string & city, const string & addr, const string & region, unsigned int id ) {
    if (binarySearchCA(city,addr) != -1)
        return false;
    if (binarySearchRI(region, id) != -1)
        return false;
    unsigned long int registrationId = registerLandLot();
    CLand* newLandLotPtr = new CLand(region, id, city, addr, registrationId);
    COwner* newOwnerPtr = new COwner(); //by default name of owner is "" and numOfLandLots is 0
    long long int resultCOwners = binarySearchCOwners("");
    //there are no landlots without an owner
    if (resultCOwners == -1) {
        newOwnerPtr->addLandLot(newLandLotPtr); //
        pushAndSortCOwners(newOwnerPtr);
    } else {
        m_owners[resultCOwners]->addLandLot(newLandLotPtr);
        delete newOwnerPtr;
    }
    pushAndSortCA(newLandLotPtr);
    pushAndSortRI(newLandLotPtr);
    return true;
}

bool CLandRegister::Del( const string & city, const string & addr ) {
    long long int resultCA = binarySearchCA(city, addr); //find an element to delete in first vector
    if(resultCA == -1) //element not found
        return false;
    else {
        const string region = m_landLotsSortedByCA[resultCA]->m_region;
        unsigned int id = m_landLotsSortedByCA[resultCA]->m_id;
        const string owner = m_landLotsSortedByCA[resultCA]->m_owner;
        unsigned long int registrationId = m_landLotsSortedByCA[resultCA]->m_registrationId;
        long long int resultRI = binarySearchRI(region,id); //find an element to delete in second vector
        long long int resultCOwners = binarySearchCOwners(owner);
        long long int resultOwnerLandLot = m_owners[resultCOwners]->binarySearch(registrationId);
        m_owners[resultCOwners]->reallyDeleteLandLot(resultOwnerLandLot);
        if(m_owners[resultCOwners]->getNumOfLandLots() == 0) {
            //we have to delete this owner from our register
            auto iteratorCOwners = m_owners.begin()+resultCOwners;
            delete *iteratorCOwners;
            //delete m_owners[resultCOwners];
            m_owners.erase(iteratorCOwners);
        }
        auto iteratorCA = m_landLotsSortedByCA.begin()+resultCA;
        auto iteratorRI = m_landLotsSortedByRI.begin()+resultRI;
        delete m_landLotsSortedByCA[resultCA]; //delete a land lot
        m_landLotsSortedByCA.erase(iteratorCA); //delete a pointer to a deleted land lot from a list sorted by city and address
        m_landLotsSortedByRI.erase(iteratorRI); //delete a pointer to a deleted land lot from a list sorted by region and id
        return true;
    }
}

bool CLandRegister::Del( const string & region, unsigned int id ) {
    long long int resultRI = binarySearchRI(region, id); //find an element to delete in first vector
    if(resultRI == -1) //element not found
        return false;
    else {
        const string city = m_landLotsSortedByRI[resultRI]->m_city;
        const string address = m_landLotsSortedByRI[resultRI]->m_address;
        const string owner = m_landLotsSortedByRI[resultRI]->m_owner;
        unsigned long int registrationId = m_landLotsSortedByRI[resultRI]->m_registrationId;
        long long int resultCA = binarySearchCA(city,address); //find an element to delete in second vector
        long long int resultCOwners = binarySearchCOwners(owner);
        long long int resultOwnerLandLot = m_owners[resultCOwners]->binarySearch(registrationId);
        m_owners[resultCOwners]->reallyDeleteLandLot(resultOwnerLandLot);
        if(m_owners[resultCOwners]->getNumOfLandLots() == 0) {
            //we have to delete this owner from our register
            auto iteratorCOwners = m_owners.begin()+resultCOwners;
            delete *iteratorCOwners;
            m_owners.erase(iteratorCOwners);
        }
        auto iteratorCA = m_landLotsSortedByCA.begin()+resultCA;
        auto iteratorRI = m_landLotsSortedByRI.begin()+resultRI;
        delete m_landLotsSortedByRI[resultRI]; //delete a land lot
        m_landLotsSortedByCA.erase(iteratorCA); //delete a pointer to a deleted land lot from a list sorted by city and address
        m_landLotsSortedByRI.erase(iteratorRI); //delete a pointer to a deleted land lot from a list sorted by region and id
        return true;
    }
}

bool CLandRegister::GetOwner ( const string & city, const string & addr, string & owner ) const {
    long long int resultCA = binarySearchCA(city, addr);
    if (resultCA == -1) return false;
    else { owner = m_landLotsSortedByCA[resultCA]->m_owner; return true; }
}

bool CLandRegister::GetOwner ( const string & region, unsigned int id, string & owner ) const {
    long long int resultRI = binarySearchRI(region, id);
    if (resultRI == -1) return false;
    else { owner = m_landLotsSortedByRI[resultRI]->m_owner; return true; }
}

bool CLandRegister::NewOwner ( const string & city, const string & addr, const string & owner ) {
    long long int resultCA = binarySearchCA(city, addr);
    if (resultCA == -1)
        return false;
    else {
        if (m_landLotsSortedByCA[resultCA]->compareOwner(owner))
            return false;
        else {
            if (m_counterToDelete > whenToDelete) {
                m_counterToDelete = 0;
                cleanDeletedOwners();
            }

            const string oldOwner = m_landLotsSortedByCA[resultCA]->m_owner;
            unsigned long int registrationId = m_landLotsSortedByCA[resultCA]->m_registrationId;
            long long int resultOldCOwners = binarySearchCOwners(oldOwner);
            long long int resultOwnerLandLot = m_owners[resultOldCOwners]->binarySearch(registrationId);
            m_owners[resultOldCOwners]->deleteLandLot(resultOwnerLandLot);
            unsigned long int newRegistrationId = registerLandLot();
            m_landLotsSortedByCA[resultCA]->setOwner(owner);
            m_landLotsSortedByCA[resultCA]->setRegistrationId(newRegistrationId);
            long long int resultNewCOwners = binarySearchCOwners(owner);
            if (resultNewCOwners == -1) {
                COwner* newOwnerPtr = new COwner(owner); //by default name of owner is "" and numOfLandLots is 0
                newOwnerPtr->addLandLot(m_landLotsSortedByCA[resultCA]); //
                pushAndSortCOwners(newOwnerPtr);
            } else {
                m_owners[resultNewCOwners]->addLandLot(m_landLotsSortedByCA[resultCA]);
            }
            return true;
        }
    }
}

bool CLandRegister::NewOwner ( const string & region, unsigned int id, const string & owner ) {
    long long int resultRI = binarySearchRI(region, id);
    if (resultRI == -1)
        return false;
    else {
        if (m_landLotsSortedByRI[resultRI]->compareOwner(owner))
            return false;
        else {
            if (m_counterToDelete > whenToDelete) {
                m_counterToDelete = 0;
                cleanDeletedOwners();
            }

            const string oldOwner = m_landLotsSortedByRI[resultRI]->m_owner;
            unsigned long int registrationId = m_landLotsSortedByRI[resultRI]->m_registrationId;
            long long int resultOldCOwners = binarySearchCOwners(oldOwner);
            long long int resultOwnerLandLot = m_owners[resultOldCOwners]->binarySearch(registrationId);
            m_owners[resultOldCOwners]->deleteLandLot(resultOwnerLandLot);
            unsigned long int newRegistrationId = registerLandLot();
            m_landLotsSortedByRI[resultRI]->setOwner(owner);
            m_landLotsSortedByRI[resultRI]->setRegistrationId(newRegistrationId);
            long long int resultNewCOwners = binarySearchCOwners(owner);
            if (resultNewCOwners == -1) {
                COwner* newOwnerPtr = new COwner(owner); //by default name of owner is "" and numOfLandLots is 0
                newOwnerPtr->addLandLot(m_landLotsSortedByRI[resultRI]); //
                pushAndSortCOwners(newOwnerPtr);
            } else {
                m_owners[resultNewCOwners]->addLandLot(m_landLotsSortedByRI[resultRI]);
            }
            return true;
        }
    }
}

// constant complexity - I just go through all list and compare owners with my 'owner' variable
unsigned CLandRegister::Count ( const string & owner ) const {
    unsigned long int count = 0;
    long long int resultCOwners = binarySearchCOwners(owner);
    if(resultCOwners != -1) //we found an owner
        count = m_owners[resultCOwners]->getNumOfLandLots();

    return count;
}

CIterator CLandRegister::ListByAddr ( void ) const {
    return CIterator(getLandLotsSortedByCAPointer());
}

CIterator CLandRegister::ListByOwner ( const string & owner ) const {
    long long int result = binarySearchCOwners(owner);
    if (result == -1) {
        return CIterator(NULL); //this iterator is empty because of size = 0
    }
    return CIterator(m_owners[result]->getLandLotsPointer(), m_owners[result]->getCheckIfOwnsPointer(), true);

}

#ifndef __PROGTEST__
static void test0 ( void )
{
    CLandRegister x;
    string owner;
    assert ( x . Add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert (! x . Add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . Add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . Add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( x . Add ( "Plzen", "Evropska", "Plzen mesto", 78901 ) );
    assert ( x . Add ( "Liberec", "Evropska", "Librec", 4552 ) );
    CIterator i0 = x . ListByAddr ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Liberec"
             && i0 . Addr () == "Evropska"
             && i0 . Region () == "Librec"
             && i0 . ID () == 4552
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Plzen"
             && i0 . Addr () == "Evropska"
             && i0 . Region () == "Plzen mesto"
             && i0 . ID () == 78901
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Evropska"
             && i0 . Region () == "Vokovice"
             && i0 . ID () == 12345
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Technicka"
             && i0 . Region () == "Dejvice"
             && i0 . ID () == 9873
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Thakurova"
             && i0 . Region () == "Dejvice"
             && i0 . ID () == 12345
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( i0 . AtEnd () );
    assert ( x . Count ( "" ) == 5 );
    CIterator i1 = x . ListByOwner ( "" );
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Prague"
             && i1 . Addr () == "Thakurova"
             && i1 . Region () == "Dejvice"
             && i1 . ID () == 12345
             && i1 . Owner () == "" );
    i1 . Next ();
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Prague"
             && i1 . Addr () == "Evropska"
             && i1 . Region () == "Vokovice"
             && i1 . ID () == 12345
             && i1 . Owner () == "" );
    i1 . Next ();
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Prague"
             && i1 . Addr () == "Technicka"
             && i1 . Region () == "Dejvice"
             && i1 . ID () == 9873
             && i1 . Owner () == "" );
    i1 . Next ();
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Plzen"
             && i1 . Addr () == "Evropska"
             && i1 . Region () == "Plzen mesto"
             && i1 . ID () == 78901
             && i1 . Owner () == "" );
    i1 . Next ();
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Liberec"
             && i1 . Addr () == "Evropska"
             && i1 . Region () == "Librec"
             && i1 . ID () == 4552
             && i1 . Owner () == "" );
    i1 . Next ();
    assert ( i1 . AtEnd () );
    assert ( x . Count ( "CVUT" ) == 0 );
    CIterator i2 = x . ListByOwner ( "CVUT" );
    assert ( i2 . AtEnd () );
    assert ( x . NewOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( x . NewOwner ( "Dejvice", 9873, "CVUT" ) );
    assert ( x . NewOwner ( "Plzen", "Evropska", "Anton Hrabis" ) );
    assert ( x . NewOwner ( "Librec", 4552, "Cvut" ) );
    assert ( x . GetOwner ( "Prague", "Thakurova", owner ) && owner == "CVUT" );
    assert ( x . GetOwner ( "Dejvice", 12345, owner ) && owner == "CVUT" );
    assert ( x . GetOwner ( "Prague", "Evropska", owner ) && owner == "" );
    assert ( x . GetOwner ( "Vokovice", 12345, owner ) && owner == "" );
    assert ( x . GetOwner ( "Prague", "Technicka", owner ) && owner == "CVUT" );
    assert ( x . GetOwner ( "Dejvice", 9873, owner ) && owner == "CVUT" );
    assert ( x . GetOwner ( "Plzen", "Evropska", owner ) && owner == "Anton Hrabis" );
    assert ( x . GetOwner ( "Plzen mesto", 78901, owner ) && owner == "Anton Hrabis" );
    assert ( x . GetOwner ( "Liberec", "Evropska", owner ) && owner == "Cvut" );
    assert ( x . GetOwner ( "Librec", 4552, owner ) && owner == "Cvut" );
    CIterator i3 = x . ListByAddr ();
    assert ( ! i3 . AtEnd ()
             && i3 . City () == "Liberec"
             && i3 . Addr () == "Evropska"
             && i3 . Region () == "Librec"
             && i3 . ID () == 4552
             && i3 . Owner () == "Cvut" );
    i3 . Next ();
    assert ( ! i3 . AtEnd ()
             && i3 . City () == "Plzen"
             && i3 . Addr () == "Evropska"
             && i3 . Region () == "Plzen mesto"
             && i3 . ID () == 78901
             && i3 . Owner () == "Anton Hrabis" );
    i3 . Next ();
    assert ( ! i3 . AtEnd ()
             && i3 . City () == "Prague"
             && i3 . Addr () == "Evropska"
             && i3 . Region () == "Vokovice"
             && i3 . ID () == 12345
             && i3 . Owner () == "" );
    i3 . Next ();
    assert ( ! i3 . AtEnd ()
             && i3 . City () == "Prague"
             && i3 . Addr () == "Technicka"
             && i3 . Region () == "Dejvice"
             && i3 . ID () == 9873
             && i3 . Owner () == "CVUT" );
    i3 . Next ();
    assert ( ! i3 . AtEnd ()
             && i3 . City () == "Prague"
             && i3 . Addr () == "Thakurova"
             && i3 . Region () == "Dejvice"
             && i3 . ID () == 12345
             && i3 . Owner () == "CVUT" );
    i3 . Next ();
    assert ( i3 . AtEnd () );
    assert ( x . Count ( "cvut" ) == 3 );
    CIterator i4 = x . ListByOwner ( "cVuT" );
    assert ( ! i4 . AtEnd ()
             && i4 . City () == "Prague"
             && i4 . Addr () == "Thakurova"
             && i4 . Region () == "Dejvice"
             && i4 . ID () == 12345
             && i4 . Owner () == "CVUT" );
    i4 . Next ();
    assert ( ! i4 . AtEnd ()
             && i4 . City () == "Prague"
             && i4 . Addr () == "Technicka"
             && i4 . Region () == "Dejvice"
             && i4 . ID () == 9873
             && i4 . Owner () == "CVUT" );
    i4 . Next ();
    assert ( ! i4 . AtEnd ()
             && i4 . City () == "Liberec"
             && i4 . Addr () == "Evropska"
             && i4 . Region () == "Librec"
             && i4 . ID () == 4552
             && i4 . Owner () == "Cvut" );
    i4 . Next ();
    assert ( i4 . AtEnd () );
    assert ( x . NewOwner ( "Plzen mesto", 78901, "CVut" ) );
    assert ( x . Count ( "CVUT" ) == 4 );
    CIterator i5 = x . ListByOwner ( "CVUT" );
    assert ( ! i5 . AtEnd ()
             && i5 . City () == "Prague"
             && i5 . Addr () == "Thakurova"
             && i5 . Region () == "Dejvice"
             && i5 . ID () == 12345
             && i5 . Owner () == "CVUT" );
    i5 . Next ();
    assert ( ! i5 . AtEnd ()
             && i5 . City () == "Prague"
             && i5 . Addr () == "Technicka"
             && i5 . Region () == "Dejvice"
             && i5 . ID () == 9873
             && i5 . Owner () == "CVUT" );
    i5 . Next ();
    assert ( ! i5 . AtEnd ()
             && i5 . City () == "Liberec"
             && i5 . Addr () == "Evropska"
             && i5 . Region () == "Librec"
             && i5 . ID () == 4552
             && i5 . Owner () == "Cvut" );
    i5 . Next ();
    assert ( ! i5 . AtEnd ()
             && i5 . City () == "Plzen"
             && i5 . Addr () == "Evropska"
             && i5 . Region () == "Plzen mesto"
             && i5 . ID () == 78901
             && i5 . Owner () == "CVut" );
    i5 . Next ();
    assert ( i5 . AtEnd () );
    assert ( x . Del ( "Liberec", "Evropska" ) );
    assert ( x . Del ( "Plzen mesto", 78901 ) );
    assert ( x . Count ( "cvut" ) == 2 );
    CIterator i6 = x . ListByOwner ( "cVuT" );
    assert ( ! i6 . AtEnd ()
             && i6 . City () == "Prague"
             && i6 . Addr () == "Thakurova"
             && i6 . Region () == "Dejvice"
             && i6 . ID () == 12345
             && i6 . Owner () == "CVUT" );
    i6 . Next ();
    assert ( ! i6 . AtEnd ()
             && i6 . City () == "Prague"
             && i6 . Addr () == "Technicka"
             && i6 . Region () == "Dejvice"
             && i6 . ID () == 9873
             && i6 . Owner () == "CVUT" );
    i6 . Next ();
    assert ( i6 . AtEnd () );
    assert ( x . Add ( "Liberec", "Evropska", "Librec", 4552 ) );
}
static void test1 ( void )
{
    CLandRegister x;
    string owner;
    assert ( x . Add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . Add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . Add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( ! x . Add ( "Prague", "Technicka", "Hradcany", 7344 ) );
    assert ( ! x . Add ( "Brno", "Bozetechova", "Dejvice", 9873 ) );
    assert ( !x . GetOwner ( "Prague", "THAKUROVA", owner ) );
    assert ( !x . GetOwner ( "Hradcany", 7343, owner ) );
    CIterator i0 = x . ListByAddr ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Evropska"
             && i0 . Region () == "Vokovice"
             && i0 . ID () == 12345
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Technicka"
             && i0 . Region () == "Dejvice"
             && i0 . ID () == 9873
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( ! i0 . AtEnd ()
             && i0 . City () == "Prague"
             && i0 . Addr () == "Thakurova"
             && i0 . Region () == "Dejvice"
             && i0 . ID () == 12345
             && i0 . Owner () == "" );
    i0 . Next ();
    assert ( i0 . AtEnd () );
    assert ( x . NewOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( ! x . NewOwner ( "Prague", "technicka", "CVUT" ) );
    assert ( ! x . NewOwner ( "prague", "Technicka", "CVUT" ) );
    assert ( ! x . NewOwner ( "dejvice", 9873, "CVUT" ) );
    assert ( ! x . NewOwner ( "Dejvice", 9973, "CVUT" ) );
    assert ( ! x . NewOwner ( "Dejvice", 12345, "CVUT" ) );
    assert ( x . Count ( "CVUT" ) == 1 );
    CIterator i1 = x . ListByOwner ( "CVUT" );
    assert ( ! i1 . AtEnd ()
             && i1 . City () == "Prague"
             && i1 . Addr () == "Thakurova"
             && i1 . Region () == "Dejvice"
             && i1 . ID () == 12345
             && i1 . Owner () == "CVUT" );
    i1 . Next ();
    assert ( i1 . AtEnd () );
    assert ( ! x . Del ( "Brno", "Technicka" ) );
    assert ( ! x . Del ( "Karlin", 9873 ) );
    assert ( x . Del ( "Prague", "Technicka" ) );
    assert ( ! x . Del ( "Prague", "Technicka" ) );
    assert ( ! x . Del ( "Dejvice", 9873 ) );
}
static void test2( void ) {
    CLandRegister x;
    string owner;
    x.Del("Prague","B");
    string * a = new string ("B");
    assert (x . Add ("Prague",*a,"",0));
    delete a;
    assert (!x . Add ("Prague","B","",0));
    assert (x . Add ("Prague","D","",1));
    assert (x . Add ("Prague","A","",2));
    assert (x . Add ("Prague","C","",3));
    assert (x . Del ("Prague", "C"));
    assert (x . Add ("Prague","C","",3));
    assert (x . Del ("Prague", "C"));
    assert (x . Add ("Prague","C","",3));
    assert (x . Del ("Prague", "C"));
    assert (x . Add ("Prague","C","",3));
    assert (x . Del ("Prague", "C"));
    assert (x . Add ("Prague","C","",3));
    assert (x . Del ("Prague", "C"));
    assert (x . Add ("Prague","C","",3));
    assert ( x . NewOwner("Prague","A","a"));
    assert ( x . NewOwner("Prague","B","b"));
    assert ( x . NewOwner("Prague","C","c"));
    assert ( x . NewOwner("Prague","D","d"));
    assert ( x . Count("A") == 1);
    assert ( x . Count("Q") == 0);
    assert ( x . Count("") == 0);
}
static void test3( void ) {

    CLandRegister x;
    string str = "a";
    for (int i = 0 ; i < 100 ; i++) {
        x . Add (str,str,str,0);
        str += "a";
    }
    str = "a";
    string o = "o";
    for(int i = 0 ; i < 50; i++) {
        x. NewOwner(str,str,o);
        str += "a";
    }
    CIterator i1 = x.ListByOwner("o");
}
static void test4( void ) {
    CLandRegister x;
    x.Del("","");
    x.Del("",1);
    CIterator i2 = x.ListByAddr();
    x . Add ("","","",1);
    CIterator i1 = x.ListByOwner("");
    x . Add ("a","a","a",1);
    i1.Next();
    assert (!i1.AtEnd());
    assert(!i2.AtEnd());
    assert(!x.Add("","","",1));
    assert(!x.Add("","","",1));
    assert(!x.Add("","","",1));
    assert(!x.Add("","","",1));
    assert(!x.Add("","","",1));
}
static void test5 ( void ) {
    CLandRegister x;
    x.Add("","","",0);
    CIterator i1 = x.ListByAddr();
}
static void test6(void) {
    CLandRegister x;
    x.Add("M$%^ama","Tata","Baba",1);
    x.Add("Dama","Nata","Naba",1);
    x.Add("Qama","Data","$aba",1);
    x.Add("Wama","Bata","Haba",1);
    x.Add("Eama","Hata","$aba",1);
    x.Add("Rama","Gata","#aba",1);
    x.Add("Tama","Eata","%aba",1);
    x.Add("Yama","Rata","!aba",1);
    x.NewOwner("M$%^ama","Tata","!@#");
    x.NewOwner("$aba", 1, "%CVU^T");
    x.NewOwner("Haba", 1, "%CvU^T");
}

//very simple speed tests
/*
static void test7(void) {
    CLandRegister x;
    string tmp = "a";
    for (int i = 0 ; i < 100 ; i++) {
        x.Add(tmp, tmp, tmp, 1);
        tmp += "a";
    }
    auto start = chrono::steady_clock::now();
    x.binarySearchCA("a", "a");
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << "Binary search CA with 100 elements: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    start = chrono::steady_clock::now();
    unsigned int tmpInt = 100;
    for (unsigned int i = 0 ; i < tmpInt ; i++) {
        x.Add(tmp, tmp, tmp, 1);
        tmp += "a";
    }
    end = chrono::steady_clock::now();
    diff = end - start;
    cout << "Add " << tmpInt << " elements: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    start = chrono::steady_clock::now();
    x.binarySearchCA("a", "a");
    end = chrono::steady_clock::now();
    diff = end - start;
    cout << "Binary search CA with " << tmpInt+100 << " elements: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    cout << endl;
}
static void test8 (void)
{
    CLandRegister x;
    x.Add("a", "a", "a", 1);
    x.Add("b", "b", "b", 1);
    x.Add("c", "c", "c", 1);
    x.Add("d", "d", "d", 1);
    auto start = chrono::steady_clock::now();
    for (int i = 0 ; i < 10000 ; i++) {
        if (i % 2 == 0) {
            x.NewOwner("a", "a", "tata");
            x.NewOwner("b", "b", "mama");
            x.NewOwner("c", "c", "tata");
            x.NewOwner("d", "d", "mama");
        } else {
            x.NewOwner("a", "a", "mama");
            x.NewOwner("b", "b", "tata");
            x.NewOwner("c", "c", "mama");
            x.NewOwner("d", "d", "tata");
        }
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}
static void test9() {
    COwner o1("tata111111111");
    COwner o2("tata111111111");
    unsigned int size = 500;
    cout << "Slow Compare Owner" << endl;
    auto start = chrono::steady_clock::now();
    for (unsigned int i = 0 ; i < size ; i++) {
        o1.slowCompareOwner(o2.getName());
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    cout << endl;
    cout << "Good Compare Owner" << endl;
    start = chrono::steady_clock::now();
    for (unsigned int i = 0 ; i < size ; i++) {
        o1.compareOwner(o2.getName());
    }
    end = chrono::steady_clock::now();
    diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}
static void test10() {
    COwner o1("tata111111111");
    COwner o2("tata111");
    cout << endl << "----------------------" <<endl;
    unsigned int size = 500;
    cout << "Slow isSmallerOwner" << endl;
    auto start = chrono::steady_clock::now();
    for (unsigned int i = 0 ; i < size ; i++) {
        o1.slowIsSmallerOwner(o2.getName());
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    cout << endl;
    cout << "Good isSmallerOwner" << endl;
    start = chrono::steady_clock::now();
    for (unsigned int i = 0 ; i < size ; i++) {
        o1.isSmallerOwner(o2.getName());
    }
    end = chrono::steady_clock::now();
    diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}
*/
static void test11() {
    COwner o1 = COwner("abc");
    COwner o2 = COwner("");
    assert(o1.isSmallerOwner("abcd"));
    assert(!o1.isSmallerOwner("ab"));
    assert(!o1.isSmallerOwner("Abc"));
    assert(o1.isSmallerOwner("abd"));
    assert(!o1.isSmallerOwner(""));
    assert(!o2.isSmallerOwner(""));
    assert(o2.isSmallerOwner("a"));
    assert(!o1.isSmallerOwner("ABB"));
}
int main ( void )
{
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    
    //very simple speed tests
    /*
    test7();
    test8();
    test9();
    test10();
    */

    test11();
    cout << "ALL TESTS PASSED" << endl;
    return 0;
}
#endif /* __PROGTEST__ */