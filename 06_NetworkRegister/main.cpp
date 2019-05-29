#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
using namespace std;
#endif /* __PROGTEST__ */

/*
 * abstract class with the interface of the computer's component
 */
class CComponent {
public:
    CComponent() {}
    virtual ~CComponent() {}
    virtual string print(const string &, bool, bool) const = 0;
    virtual CComponent * clone () const & = 0; //I use this function to get the pointer to a newly created copy of the component
};

/*
 * class to represent computer's component - CPU
 */
class CCPU : public CComponent {
public:
    CCPU (const unsigned int &, const unsigned int &);
    ~CCPU();
    CCPU (const CCPU &);
    CCPU & operator= (const CCPU &);
    virtual string print(const string &, bool, bool) const;
    virtual CCPU * clone () const & override { return new CCPU ( *this ); }
private:
    unsigned int m_numOfCores;
    unsigned int m_frequency;
};

//basic constructor
CCPU::CCPU (const unsigned int & numOfCores, const unsigned int & frequency) :
        m_numOfCores(numOfCores), m_frequency(frequency) {}

//destructor
CCPU::~CCPU () { m_numOfCores = 0; m_frequency = 0; }

//copy constructor
CCPU::CCPU(const CCPU & cpu) { m_numOfCores = cpu.m_numOfCores; m_frequency = cpu.m_frequency; }

//operator of assignment
CCPU & CCPU::operator= (const CCPU & cpu) {
    m_numOfCores = cpu.m_numOfCores; m_frequency = cpu.m_frequency; return *this;
}

/*
 * Method for printing component in the tree.
 * flagLast is used to determine if it is the last component of computer
 * extendedPrint is used to determine if it is printing in a way in the tree
 * beginStr is used to keep a valid structure of a tree
 */
string CCPU::print(const string & beginStr, bool flagLast, bool extendedPrint) const {
    ostringstream os;
    if (extendedPrint) {
        os << beginStr;
        if (flagLast) os << "\\-";
        else os << "+-";
    }
    os << "CPU, " << m_numOfCores << " cores @ " << m_frequency << "MHz" << endl;
    return os.str();
}

/*
 * class to represent computer's component - memory
 */
class CMemory : public CComponent {
public:
    CMemory (const unsigned int &);
    ~CMemory();
    CMemory (const CMemory &);
    CMemory & operator= (const CMemory &);
    virtual string print(const string &, bool, bool) const;
    virtual CMemory * clone () const & override { return new CMemory ( *this ); }
private:
    unsigned int m_size;
};

//basic constructor
CMemory::CMemory(const unsigned int & size) : m_size(size) {}

//destructor
CMemory::~CMemory() { m_size = 0; }

//copy constructor
CMemory::CMemory(const CMemory & memory) { m_size = memory.m_size; }

//operator of assignment
CMemory & CMemory::operator=(const CMemory & memory) { m_size = memory.m_size; return *this; }

/*
 * Method for printing component in the tree.
 * flagLast is used to determine if it is the last component of computer
 * extendedPrint is used to determine if it is printing in a way in the tree
 * beginStr is used to keep a valid structure of a tree
 */
string CMemory::print(const string & beginStr, bool flagLast, bool extendedPrint)  const {
    ostringstream os;
    if (extendedPrint) {
        os << beginStr;
        if (flagLast) os << "\\-";
        else os << "+-";
    }
    os << "Memory, " << m_size << " MiB" << endl;
    return os.str();
}

/*
 * class to represent computer's component - memory
 */
class CDisk : public CComponent {
public:
    CDisk (const string &, const unsigned int &);
    ~CDisk();
    CDisk(const CDisk &);
    CDisk & operator= (const CDisk &);
    CDisk & AddPartition(const unsigned int &, const string &);
    virtual string print(const string &, bool, bool) const;
    virtual CDisk * clone () const & override { return new CDisk ( *this ); }
    static const string MAGNETIC;
    static const string SSD;
private:
    string m_storageType;
    unsigned int m_size;
    unsigned int m_sizeOfPartitions;
    vector<pair<unsigned int, string>> m_partitionList;

};

//constant values to represent type of disc
const string CDisk::MAGNETIC = "HDD";
const string CDisk::SSD = "SSD";

//basic constructor
CDisk::CDisk (const string & storage_type, const unsigned int & size) {
    if (storage_type == MAGNETIC || storage_type == SSD) {
        m_storageType = storage_type;
        m_size = size;
        m_sizeOfPartitions = 0;
    } else {
        m_storageType = "";
        m_size = 0;
        m_sizeOfPartitions = 0;
    }
}

//destructor - we have to clear vector of partitions
CDisk::~CDisk() {
    m_storageType = "";
    m_size = 0;
    m_sizeOfPartitions = 0;
    m_partitionList.clear();
}

//copy constructor
CDisk::CDisk(const CDisk & disc) {
    m_storageType = disc.m_storageType;
    m_size = disc.m_size;
    m_sizeOfPartitions = disc.m_sizeOfPartitions;
    for (auto it = disc.m_partitionList.begin() ; it != disc.m_partitionList.end() ; ++it) {
        m_partitionList.push_back(*it);
    }
}

//operator of assignment
CDisk & CDisk::operator= (const CDisk & disc) {
    if (this == &disc) return *this; //protection against self-assignment
    m_partitionList.clear();
    m_storageType = disc.m_storageType;
    m_size = disc.m_size;
    m_sizeOfPartitions = disc.m_sizeOfPartitions;
    for (auto it = disc.m_partitionList.begin() ; it != disc.m_partitionList.end() ; ++it) {
        m_partitionList.push_back(*it);
    }
    return *this;
}

/*
 * method to add the partition. I check if the overal size of disc is not exceeded
 */
CDisk & CDisk::AddPartition(const unsigned int & sizeOfPartition, const string & name) {
    if (m_sizeOfPartitions + sizeOfPartition > m_size)
        return *this;
    m_partitionList.push_back(make_pair(sizeOfPartition, name));
    m_sizeOfPartitions += sizeOfPartition;
    return *this;
}

/*
 * Method for printing component in the tree.
 * flagLast is used to determine if it is the last component of computer
 * extendedPrint is used to determine if it is printing in a way in the tree
 * Partitions of disc are also printeed in a special way
 * beginStr is used to keep a valid structure of a tree
 */
string CDisk::print(const string & beginStr, bool flagLast, bool extendedPrint) const {
    ostringstream os;
    if (extendedPrint) {
        os << beginStr;
        if (flagLast) os << "\\-";
        else os << "+-";
    }
    os << m_storageType << ", " << m_size << " GiB" << endl;
    unsigned int i = 0;
    for (auto it = m_partitionList.begin() ; it != m_partitionList.end() ; ++it) {

        if (extendedPrint) os << beginStr;

        if (flagLast) os << "  ";
        else os << "| ";
        if (it == m_partitionList.end()-1) os << "\\-";
        else os << "+-";
        os << "[" << i << "]: " << it->first << " GiB, " << it->second << endl;
        ++i;
    }
    return os.str();
}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

class CNetwork; //necessary in CComputer

/*
 * it is used inside computer to distinguish if our component is either 'real component' (cpu, disk, memory) or virtualized network
 */
enum ComponentType { COMPONENT, NETWORK };

/*
 * class to represent computer or virtual computer
 */
class CComputer {
public:
    CComputer(const string & name);
    ~CComputer();
    CComputer(const CComputer & computer);
    CComputer & operator= (const CComputer & computer);
    CComputer * clone () const & { return new CComputer ( *this ); }
    CComputer & AddComponent(const CComponent & component);
    CComputer & AddComponent(const CNetwork &);
    CComputer & AddAddress (const string & address);
    CComputer * FindComputer(const string & name);
    CNetwork * FindNetwork(const string & name) ;
    CComputer Duplicate(const list<pair<string,string>> & remap) const;
    string print(const string & , bool, bool) const;
    friend ostream & operator<< (ostream & os, const CComputer &);
    string getName() const { return m_name; }
private:
    string m_name;
    vector< shared_ptr <CComponent> > m_componentList;
    vector< shared_ptr <CNetwork> > m_virtualizedNetworkList;
    vector< ComponentType > m_componentTypeList; //it is used to keep order of added elements while displaying
    vector<string> m_addressList;

};

/*
 * class to represent network or virtual network with computers which it connected
 */
class CNetwork { //: public CComponent {
public:
    CNetwork (const string &);
    ~CNetwork();
    CNetwork(const CNetwork &);
    CNetwork & operator= (const CNetwork &);
    CNetwork * clone () const & { return new CNetwork ( *this ); }
    CNetwork & AddComputer (const CComputer &);
    CComputer * FindComputer (const string &);
    CNetwork * FindNetwork (const string &);
    CNetwork Duplicate(const list<pair<string,string>> & remap) const;
    string print(const string &, bool , bool) const;
    friend ostream & operator<< (ostream &, const CNetwork &);
    string getName() const { return m_name; }
private:
    string m_name;
    vector<shared_ptr <CComputer> > m_computerList;
};

//---------------------------------------------------------------------------------------------------------------------
//-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS--
//---------------------------------------------------------------------------------------------------------------------

//basic constructor
CComputer::CComputer(const string & name) : m_name(name) {}

//destructor - the lsit of components will be clear after deleting the component
CComputer::~CComputer() {
    m_componentList.clear();
    m_virtualizedNetworkList.clear();
    m_componentTypeList.clear();
    m_addressList.clear();
    m_name = "";
}

//copy constructor
CComputer::CComputer(const CComputer & computer) {
    m_name = computer.m_name;
    //copy of list of 'real components'
    for (const auto & it : computer.m_componentList) {
        m_componentList.push_back(shared_ptr<CComponent> (it->clone()));
    }
    //copy of virtualized networks
    for (const auto & it : computer.m_virtualizedNetworkList) {
        m_virtualizedNetworkList.push_back(shared_ptr<CNetwork>(it->clone()));
    }
    //copy of list of type of components
    for (auto it : computer.m_componentTypeList) {
        m_componentTypeList.push_back(it);
    }
    //copy of addresses
    for (const auto & it : computer.m_addressList) {
        m_addressList.push_back(it);
    }
}

//operator of assignment
CComputer & CComputer::operator= (const CComputer & computer) {
    if (&computer == this) return *this; //we check if we don't want to copy the same object
    m_componentList.clear();
    m_virtualizedNetworkList.clear();
    m_componentTypeList.clear();
    m_addressList.clear();
    m_name = computer.m_name;
    //copy of list of 'real components'
    for (const auto & it : computer.m_componentList) {
        m_componentList.push_back(shared_ptr<CComponent> (it->clone()));
    }
    //copy of virtualized networks
    for (const auto & it : computer.m_virtualizedNetworkList) {
        m_virtualizedNetworkList.push_back(shared_ptr<CNetwork>(it->clone()));
    }
    //copy of list of type of components
    for (auto it : computer.m_componentTypeList) {
        m_componentTypeList.push_back(it);
    }
    //copy of addresses
    for (const auto & it : computer.m_addressList) {
        m_addressList.push_back(it);
    }
    return *this;
}

/*
 * In this method I use the fact that all components have the same interface from abstract class CComponent
 * Just before adding I clone the instance to let it be changed later.
 * I store pointers to components in a vector to avoid unnecessary function for every component type.
 */
CComputer & CComputer::AddComponent(const CComponent & component) {
    m_componentList.push_back( shared_ptr < CComponent > ( component.clone()));
    m_componentTypeList.push_back(ComponentType::COMPONENT);
    return *this;
}

CComputer & CComputer::AddComponent(const CNetwork & network) {
    m_virtualizedNetworkList.push_back( shared_ptr <CNetwork> (network.clone()));
    m_componentTypeList.push_back(ComponentType::NETWORK);
    return *this;
}

//method to adding address as a string into a vector
CComputer & CComputer::AddAddress (const string & address) {
    m_addressList.push_back(address);
    return *this;
}

/*
 * Method returns a pointer to CComputer object with the given name,
 * or an invalid pointer if the computer does not exist.
 * The computer is searched in virtualized computers as well.
 */
CComputer * CComputer::FindComputer(const string & name) {
    CComputer * tmpComputerPtr;
    for (auto & it : m_virtualizedNetworkList) {
        tmpComputerPtr = it->FindComputer(name);
        if (tmpComputerPtr != nullptr) return tmpComputerPtr;
    }
    return nullptr; //there wasn't virtualized computer so we return an empty pointer
}

/*
 * Method returns a pointer to CNetwork object with the given name,
 * or or an invalid pointer if the network does not exist.
 * Caution: the network is searched in virtualized networks as well.
 */
CNetwork * CComputer::FindNetwork(const string & name) {
    CNetwork * tmpNetworkPtr;
    for (auto & it : m_virtualizedNetworkList) {
        if (name == it->getName()) return it.get(); //first we check virtual networks inside computer
        tmpNetworkPtr = it->FindNetwork(name); //then we check for nested virtual networks
        if (tmpNetworkPtr != nullptr) return tmpNetworkPtr;
    }
    return nullptr; //there wasn't neither virtual network inside the computer nor nested one
}

string searchNames(const string & originalName, const list<pair<string,string>> & remap) {
    for (const auto & it : remap) {
        if (it.first == originalName) return it.second; //for every pair in the list I check with the original name
    }
    return originalName; //in case there isn't a new name we return the orignal one
}

/*
 * Method creates a copy of the computer instance.
 * The copy is identical to the original, including the virtualized computers/networks.
 * The only difference between the original and the copy may be the computer/network names.
 * The method takes remap parameter which lists (original name, new name) pairs.
 * When creating the copy, the method searches the list for each computer/network name.
 * If the original name is found in the list, it is replaced with the new name.
 * Otherwise (not found in the list), the name is copied unchanged.
 */
CComputer CComputer::Duplicate(const list<pair<string,string>> & remap) const {
    string tmpCompName = searchNames(m_name, remap);
    CComputer computerCopy(tmpCompName);
    for (const auto & it : m_addressList) computerCopy.m_addressList.push_back(it);
    for (const auto & it : m_componentList) computerCopy.m_componentList.push_back( shared_ptr < CComponent > ( it->clone()));
    for (const auto & it : m_componentTypeList) computerCopy.m_componentTypeList.push_back(it);
    for (const auto & it : m_virtualizedNetworkList) {
        CNetwork tmpNetworkCopy = it->Duplicate(remap);
        computerCopy.m_virtualizedNetworkList.push_back(shared_ptr<CNetwork>(tmpNetworkCopy.clone()));
    }
    return computerCopy;
}

/*
 * Method for printing all computer's addresses and components in the tree.
 * flagLast is used to determine if it is the last computer in the network
 * extendedPrint is used to determine if it is printing in a way in the tree or just printing the computer
 * beginStr is used to keep a valid structure of a tree
 */
string CComputer::print(const string & beginStr, bool flagLast, bool extendedPrint) const {
    ostringstream os;
    string newBeginStr = beginStr;
    if (extendedPrint) {
        os << beginStr;
        if (flagLast) { os << "\\-"; newBeginStr += "  "; }
        else { os << "+-"; newBeginStr += "| "; }
    }
    os << "Host: " << m_name << endl;
    for (auto it = m_addressList.begin() ; it != m_addressList.end() ; ++it) {
        os << newBeginStr;
        if (it == m_addressList.end()-1 && m_componentList.size() == 0) os << "\\-";
        else os << "+-";
        os << (*it) << endl;
    }

    auto itC = m_componentList.begin();
    auto itN = m_virtualizedNetworkList.begin();

    for (auto it = m_componentTypeList.begin() ; it != m_componentTypeList.end() ; ++it) {
        if (*it == ComponentType::COMPONENT) {
            if (it == m_componentTypeList.end()-1) {
                os << (*itC)->print(newBeginStr, true, true);
                ++itC;
            } else {
                os << (*itC)->print(newBeginStr, false, true);
                ++itC;
            }
        } else { //*it == ComponentType::NETWORK
            if (it == m_componentTypeList.end() - 1) {
                os << (*itN)->print(newBeginStr, true, true);
                ++itN;
            } else {
                os << (*itN)->print(newBeginStr, false, true);
                ++itN;
            }
        }
    }
    return os.str();
}

/*
 * I use method print implemented above with the flagLast and extendedPrint set to false to achieve
 * just a normal printing of computer and its addresses and components
 */
ostream & operator<< (ostream & os, const CComputer & computer) {
    os << computer.print("", false, false);
    return os;
}

//---------------------------------------------------------------------------------------------------------------------
//-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS-----COMPUTER'S METHODS--
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//--NETWORK'S METHODS--NETWORK'S METHODs--NETWORK'S METHODS--NETWORK'S METHODS--NETWORK'S METHODS--NETWORK'S METHODS---
//---------------------------------------------------------------------------------------------------------------------

//basic constructor
CNetwork::CNetwork (const string & name) : m_name(name) {}

//destructor - I make sure to have an empty list of computers after destruction
CNetwork::~CNetwork() { m_computerList.clear(); }

//copy constructor
CNetwork::CNetwork(const CNetwork & network) {
    m_name = network.m_name;
    m_computerList.clear(); //just to be sure that the list is empty at the beginning
    for (const auto & it : network.m_computerList) {
        m_computerList.push_back(shared_ptr<CComputer>(it->clone())); //we copy every computer one by one
    }
}

//operator of assignment
CNetwork & CNetwork::operator= (const CNetwork & network) {
    if (&network == this) return *this; //we check if we don't want to copy the same object
    m_name = network.m_name;
    m_computerList.clear(); //just to be sure that the list is empty at the beginning
    for (const auto & it : network.m_computerList) {
        m_computerList.push_back(shared_ptr<CComputer>(it->clone())); //we copy every computer one by one
    }
    return *this;
}

//simple adding a computer to a vector of computers
CNetwork & CNetwork::AddComputer (const CComputer & computer) {
    m_computerList.push_back(shared_ptr<CComputer>(computer.clone()));
    return *this;
}

/*
 * Method returns a pointer to CComputer object with the given name,
 * or an invalid pointer if the computer does not exist.
 * The computer is searched in virtualized computers as well.
 */
CComputer * CNetwork::FindComputer (const string & name) {
    CComputer * tmpComputerPtr;

    for (auto & it : m_computerList) {
        if (name == it->getName()) return it.get();
        tmpComputerPtr = it->FindComputer(name);
        if (tmpComputerPtr != nullptr) return tmpComputerPtr;
    }
    return nullptr; //if there wasn't neither computer in the list nor virtual computer then we return an empty pointer
}

/*
 * Method returns a pointer to CNetwork object with the given name,
 * or or an invalid pointer if the network does not exist.
 * Caution: the network is searched in virtualized networks as well.
 */
CNetwork * CNetwork::FindNetwork (const string & name) {
    CNetwork * tmpNetworkPtr;
    for (auto & it : m_computerList) {
        tmpNetworkPtr = it->FindNetwork(name);
        if (tmpNetworkPtr != nullptr) return tmpNetworkPtr;
    }
    return nullptr; //if there wasn't virtual network then we return an empty pointer
}

CNetwork CNetwork::Duplicate(const list<pair<string,string>> & remap) const {
    string tmpNetName = searchNames(m_name, remap);
    CNetwork networkCopy(tmpNetName);

    for (const auto & it : m_computerList) {
        CComputer tmpComputerCopy = it->Duplicate(remap);
        networkCopy.m_computerList.push_back(shared_ptr<CComputer>(tmpComputerCopy.clone()));
    }
    return networkCopy;
}

/*
 * Method for printing network and its computers in the tree.
 * flagLast is used to determine if it is the last virtual network (I just wrote it in a way to extend it later)
 * extendedPrint is used to determine if it is printing in a way in the tree or just printing the network
 * Flag extendedPrint is also used there to be used in extended version of this task)
 * beginStr is used to keep a valid structure of a tree
 */
string CNetwork::print(const string & beginStr, bool flagLast, bool extendedPrint) const {
    ostringstream os;
    string newBeginStr = beginStr;
    if (extendedPrint) {
        os << beginStr;
        if (flagLast) { os << "\\-"; newBeginStr += "  "; }
        else { os << "+-"; newBeginStr += "| "; }
    }
    os << "Network: " << m_name << endl;
    for (auto it = m_computerList.begin() ; it != m_computerList.end() ; ++it) {
        //if (extendedPrint) os << beginStr;
        if (it == m_computerList.end()-1) os << (*it)->print(newBeginStr, true, true);
        else os << (*it)->print(newBeginStr, false, true);
    }
    return os.str();
}

/*
 * I use method print implemented above with the flagLast and extendedPrint set to false to achieve
 * just a basic printing of network and computers connected within it
 */
ostream & operator<< (ostream & os, const CNetwork & network) {
    os << network.print("",false, false);
    return os;
}

//---------------------------------------------------------------------------------------------------------------------
//--NETWORK'S METHODS--NETWORK'S METHODs--NETWORK'S METHODS--NETWORK'S METHODS--NETWORK'S METHODS--NETWORK'S METHODS---
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef __PROGTEST__
template<typename _T>
string toString ( const _T & x )
{
    ostringstream oss;
    oss << x;
    return oss . str ();
}

int main ( void )
{

    CNetwork n ( "FIT network" );
  n . AddComputer (
      CComputer ( "progtest.fit.cvut.cz" ) .
        AddAddress ( "147.32.232.142" ) .
        AddComponent ( CCPU ( 8, 2400 ) ) .
        AddComponent ( CCPU ( 8, 1200 ) ) .
        AddComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
          AddPartition ( 50, "/" ) .
          AddPartition ( 5, "/boot" ).
          AddPartition ( 1000, "/var" ) ) .
        AddComponent ( CDisk ( CDisk::SSD, 60 ) .
          AddPartition ( 60, "/data" )  ) .
        AddComponent ( CMemory ( 2000 ) ).
        AddComponent ( CMemory ( 2000 ) ) ) .
    AddComputer (
      CComputer ( "edux.fit.cvut.cz" ) .
        AddAddress ( "147.32.232.158" ) .
        AddComponent ( CCPU ( 4, 1600 ) ) .
        AddComponent ( CMemory ( 4000 ) ).
        AddComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
          AddPartition ( 100, "/" )   .
          AddPartition ( 1900, "/data" ) ) ) .
    AddComputer (
      CComputer ( "imap.fit.cvut.cz" ) .
        AddAddress ( "147.32.232.238" ) .
        AddComponent ( CCPU ( 4, 2500 ) ) .
        AddAddress ( "2001:718:2:2901::238" ) .
        AddComponent ( CMemory ( 8000 ) ) );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "\\-Host: imap.fit.cvut.cz\n"
    "  +-147.32.232.238\n"
    "  +-2001:718:2:2901::238\n"
    "  +-CPU, 4 cores @ 2500MHz\n"
    "  \\-Memory, 8000 MiB\n" );
  CNetwork x = n;
  auto c = x . FindComputer ( "imap.fit.cvut.cz" );
  assert ( toString ( *c ) ==
    "Host: imap.fit.cvut.cz\n"
    "+-147.32.232.238\n"
    "+-2001:718:2:2901::238\n"
    "+-CPU, 4 cores @ 2500MHz\n"
    "\\-Memory, 8000 MiB\n" );
  c -> AddComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
            AddPartition ( 100, "system" ) .
            AddPartition ( 200, "WWW" ) .
            AddPartition ( 700, "mail" ) );
  assert ( toString ( x ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "\\-Host: imap.fit.cvut.cz\n"
    "  +-147.32.232.238\n"
    "  +-2001:718:2:2901::238\n"
    "  +-CPU, 4 cores @ 2500MHz\n"
    "  +-Memory, 8000 MiB\n"
    "  \\-HDD, 1000 GiB\n"
    "    +-[0]: 100 GiB, system\n"
    "    +-[1]: 200 GiB, WWW\n"
    "    \\-[2]: 700 GiB, mail\n" );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "\\-Host: imap.fit.cvut.cz\n"
    "  +-147.32.232.238\n"
    "  +-2001:718:2:2901::238\n"
    "  +-CPU, 4 cores @ 2500MHz\n"
    "  \\-Memory, 8000 MiB\n" );
  n . AddComputer ( CComputer ( "vm01.fit.cvut.cz" ) .
    AddAddress ( "147.32.232.232" ) .
      AddComponent ( CCPU ( 32, 4000 ) ) .
      AddComponent ( CMemory ( 32768 ) ) .
      AddComponent ( CDisk ( CDisk::MAGNETIC, 3000 ) .
        AddPartition ( 500, "SYSTEM" ) .
        AddPartition ( 2000, "DATA" ) .
        AddPartition ( 500, "BACKUP" ) ) .
      AddComponent ( CNetwork ( "dummy00" ) .
        AddComputer ( CComputer ( "testing.fit.cvut.cz" ) .
          AddAddress ( "192.168.1.1" ) .
          AddComponent ( CCPU ( 1, 300 ) ) .
          AddComponent ( CMemory ( 256 ) ) ) ) .
      AddComponent ( CMemory ( 16384 ) ) .
      AddComponent ( CNetwork ( "vnet00" ) ) );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "\\-Host: vm01.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n" );
  auto vmnet = n . FindNetwork ( "vnet00" );
  vmnet -> AddComputer ( CComputer ( "monitor1.fit.cvut.cz" ) .
    AddAddress ( "147.32.232.254" ) .
    AddComponent ( CCPU ( 2, 1500 ) ) .
    AddComponent ( CMemory ( 4096 ) ) .
    AddComponent ( CDisk ( CDisk::MAGNETIC, 750 ) .
      AddPartition ( 100, "root" ) .
      AddPartition ( 600, "log" ) ) );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "\\-Host: monitor1.fit.cvut.cz\n"
    "  +-147.32.232.254\n"
    "  +-CPU, 2 cores @ 1500MHz\n"
    "  +-Memory, 4096 MiB\n"
    "  \\-HDD, 750 GiB\n"
    "    +-[0]: 100 GiB, root\n"
    "    \\-[1]: 600 GiB, log\n" );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "\\-Host: vm01.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    \\-Host: monitor1.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );
  vmnet -> AddComputer ( n . FindComputer ( "monitor1.fit.cvut.cz" )
           -> Duplicate ( { pair<string,string>("monitor1.fit.cvut.cz","monitor2.fit.cvut.cz") } ) );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "+-Host: monitor1.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "\\-Host: monitor2.fit.cvut.cz\n"
    "  +-147.32.232.254\n"
    "  +-CPU, 2 cores @ 1500MHz\n"
    "  +-Memory, 4096 MiB\n"
    "  \\-HDD, 750 GiB\n"
    "    +-[0]: 100 GiB, root\n"
    "    \\-[1]: 600 GiB, log\n" );
  n . AddComputer ( n . FindComputer ( "vm01.fit.cvut.cz" )
        -> Duplicate (
             {
               pair<string,string>("monitor1.fit.cvut.cz","monitor3.fit.cvut.cz"),
               pair<string,string>("monitor2.fit.cvut.cz","monitor4.fit.cvut.cz"),
               pair<string,string>("vm01.fit.cvut.cz","vm02.fit.cvut.cz")
             } ) );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "+-Host: vm01.fit.cvut.cz\n"
    "| +-147.32.232.232\n"
    "| +-CPU, 32 cores @ 4000MHz\n"
    "| +-Memory, 32768 MiB\n"
    "| +-HDD, 3000 GiB\n"
    "| | +-[0]: 500 GiB, SYSTEM\n"
    "| | +-[1]: 2000 GiB, DATA\n"
    "| | \\-[2]: 500 GiB, BACKUP\n"
    "| +-Network: dummy00\n"
    "| | \\-Host: testing.fit.cvut.cz\n"
    "| |   +-192.168.1.1\n"
    "| |   +-CPU, 1 cores @ 300MHz\n"
    "| |   \\-Memory, 256 MiB\n"
    "| +-Memory, 16384 MiB\n"
    "| \\-Network: vnet00\n"
    "|   +-Host: monitor1.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   \\-Host: monitor2.fit.cvut.cz\n"
    "|     +-147.32.232.254\n"
    "|     +-CPU, 2 cores @ 1500MHz\n"
    "|     +-Memory, 4096 MiB\n"
    "|     \\-HDD, 750 GiB\n"
    "|       +-[0]: 100 GiB, root\n"
    "|       \\-[1]: 600 GiB, log\n"
    "\\-Host: vm02.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    +-Host: monitor3.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: monitor4.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );
  vmnet -> AddComputer ( n . FindComputer ( "vm02.fit.cvut.cz" )
           -> Duplicate (
                {
                  pair<string,string>("monitor3.fit.cvut.cz","monitor5.fit.cvut.cz"),
                  pair<string,string>("monitor4.fit.cvut.cz","monitor6.fit.cvut.cz"),
                  pair<string,string>("vm02.fit.cvut.cz","vm03.fit.cvut.cz"),
                  pair<string,string>("vnet00","vnet01")
                } ) );
  assert ( toString ( *vmnet ) ==
    "Network: vnet00\n"
    "+-Host: monitor1.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "+-Host: monitor2.fit.cvut.cz\n"
    "| +-147.32.232.254\n"
    "| +-CPU, 2 cores @ 1500MHz\n"
    "| +-Memory, 4096 MiB\n"
    "| \\-HDD, 750 GiB\n"
    "|   +-[0]: 100 GiB, root\n"
    "|   \\-[1]: 600 GiB, log\n"
    "\\-Host: vm03.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet01\n"
    "    +-Host: monitor5.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: monitor6.fit.cvut.cz\n"
    "      +-147.32.232.254\n"
    "      +-CPU, 2 cores @ 1500MHz\n"
    "      +-Memory, 4096 MiB\n"
    "      \\-HDD, 750 GiB\n"
    "        +-[0]: 100 GiB, root\n"
    "        \\-[1]: 600 GiB, log\n" );
  n . FindComputer ( "vm02.fit.cvut.cz" )
      -> FindNetwork ( "vnet00" )
      -> AddComputer ( n . FindComputer ( "vm01.fit.cvut.cz" )
           -> Duplicate (
                {
                  pair<string,string>("monitor1.fit.cvut.cz","monitor11.fit.cvut.cz"),
                  pair<string,string>("monitor2.fit.cvut.cz","monitor12.fit.cvut.cz"),
                  pair<string,string>("vm01.fit.cvut.cz","vm11.fit.cvut.cz"),
                  pair<string,string>("vm03.fit.cvut.cz","vm13.fit.cvut.cz"),
                  pair<string,string>("vnet00","vnet10"),
                  pair<string,string>("vnet01","vnet11")
                } ) );
  assert ( toString ( n ) ==
    "Network: FIT network\n"
    "+-Host: progtest.fit.cvut.cz\n"
    "| +-147.32.232.142\n"
    "| +-CPU, 8 cores @ 2400MHz\n"
    "| +-CPU, 8 cores @ 1200MHz\n"
    "| +-HDD, 1500 GiB\n"
    "| | +-[0]: 50 GiB, /\n"
    "| | +-[1]: 5 GiB, /boot\n"
    "| | \\-[2]: 1000 GiB, /var\n"
    "| +-SSD, 60 GiB\n"
    "| | \\-[0]: 60 GiB, /data\n"
    "| +-Memory, 2000 MiB\n"
    "| \\-Memory, 2000 MiB\n"
    "+-Host: edux.fit.cvut.cz\n"
    "| +-147.32.232.158\n"
    "| +-CPU, 4 cores @ 1600MHz\n"
    "| +-Memory, 4000 MiB\n"
    "| \\-HDD, 2000 GiB\n"
    "|   +-[0]: 100 GiB, /\n"
    "|   \\-[1]: 1900 GiB, /data\n"
    "+-Host: imap.fit.cvut.cz\n"
    "| +-147.32.232.238\n"
    "| +-2001:718:2:2901::238\n"
    "| +-CPU, 4 cores @ 2500MHz\n"
    "| \\-Memory, 8000 MiB\n"
    "+-Host: vm01.fit.cvut.cz\n"
    "| +-147.32.232.232\n"
    "| +-CPU, 32 cores @ 4000MHz\n"
    "| +-Memory, 32768 MiB\n"
    "| +-HDD, 3000 GiB\n"
    "| | +-[0]: 500 GiB, SYSTEM\n"
    "| | +-[1]: 2000 GiB, DATA\n"
    "| | \\-[2]: 500 GiB, BACKUP\n"
    "| +-Network: dummy00\n"
    "| | \\-Host: testing.fit.cvut.cz\n"
    "| |   +-192.168.1.1\n"
    "| |   +-CPU, 1 cores @ 300MHz\n"
    "| |   \\-Memory, 256 MiB\n"
    "| +-Memory, 16384 MiB\n"
    "| \\-Network: vnet00\n"
    "|   +-Host: monitor1.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   +-Host: monitor2.fit.cvut.cz\n"
    "|   | +-147.32.232.254\n"
    "|   | +-CPU, 2 cores @ 1500MHz\n"
    "|   | +-Memory, 4096 MiB\n"
    "|   | \\-HDD, 750 GiB\n"
    "|   |   +-[0]: 100 GiB, root\n"
    "|   |   \\-[1]: 600 GiB, log\n"
    "|   \\-Host: vm03.fit.cvut.cz\n"
    "|     +-147.32.232.232\n"
    "|     +-CPU, 32 cores @ 4000MHz\n"
    "|     +-Memory, 32768 MiB\n"
    "|     +-HDD, 3000 GiB\n"
    "|     | +-[0]: 500 GiB, SYSTEM\n"
    "|     | +-[1]: 2000 GiB, DATA\n"
    "|     | \\-[2]: 500 GiB, BACKUP\n"
    "|     +-Network: dummy00\n"
    "|     | \\-Host: testing.fit.cvut.cz\n"
    "|     |   +-192.168.1.1\n"
    "|     |   +-CPU, 1 cores @ 300MHz\n"
    "|     |   \\-Memory, 256 MiB\n"
    "|     +-Memory, 16384 MiB\n"
    "|     \\-Network: vnet01\n"
    "|       +-Host: monitor5.fit.cvut.cz\n"
    "|       | +-147.32.232.254\n"
    "|       | +-CPU, 2 cores @ 1500MHz\n"
    "|       | +-Memory, 4096 MiB\n"
    "|       | \\-HDD, 750 GiB\n"
    "|       |   +-[0]: 100 GiB, root\n"
    "|       |   \\-[1]: 600 GiB, log\n"
    "|       \\-Host: monitor6.fit.cvut.cz\n"
    "|         +-147.32.232.254\n"
    "|         +-CPU, 2 cores @ 1500MHz\n"
    "|         +-Memory, 4096 MiB\n"
    "|         \\-HDD, 750 GiB\n"
    "|           +-[0]: 100 GiB, root\n"
    "|           \\-[1]: 600 GiB, log\n"
    "\\-Host: vm02.fit.cvut.cz\n"
    "  +-147.32.232.232\n"
    "  +-CPU, 32 cores @ 4000MHz\n"
    "  +-Memory, 32768 MiB\n"
    "  +-HDD, 3000 GiB\n"
    "  | +-[0]: 500 GiB, SYSTEM\n"
    "  | +-[1]: 2000 GiB, DATA\n"
    "  | \\-[2]: 500 GiB, BACKUP\n"
    "  +-Network: dummy00\n"
    "  | \\-Host: testing.fit.cvut.cz\n"
    "  |   +-192.168.1.1\n"
    "  |   +-CPU, 1 cores @ 300MHz\n"
    "  |   \\-Memory, 256 MiB\n"
    "  +-Memory, 16384 MiB\n"
    "  \\-Network: vnet00\n"
    "    +-Host: monitor3.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    +-Host: monitor4.fit.cvut.cz\n"
    "    | +-147.32.232.254\n"
    "    | +-CPU, 2 cores @ 1500MHz\n"
    "    | +-Memory, 4096 MiB\n"
    "    | \\-HDD, 750 GiB\n"
    "    |   +-[0]: 100 GiB, root\n"
    "    |   \\-[1]: 600 GiB, log\n"
    "    \\-Host: vm11.fit.cvut.cz\n"
    "      +-147.32.232.232\n"
    "      +-CPU, 32 cores @ 4000MHz\n"
    "      +-Memory, 32768 MiB\n"
    "      +-HDD, 3000 GiB\n"
    "      | +-[0]: 500 GiB, SYSTEM\n"
    "      | +-[1]: 2000 GiB, DATA\n"
    "      | \\-[2]: 500 GiB, BACKUP\n"
    "      +-Network: dummy00\n"
    "      | \\-Host: testing.fit.cvut.cz\n"
    "      |   +-192.168.1.1\n"
    "      |   +-CPU, 1 cores @ 300MHz\n"
    "      |   \\-Memory, 256 MiB\n"
    "      +-Memory, 16384 MiB\n"
    "      \\-Network: vnet10\n"
    "        +-Host: monitor11.fit.cvut.cz\n"
    "        | +-147.32.232.254\n"
    "        | +-CPU, 2 cores @ 1500MHz\n"
    "        | +-Memory, 4096 MiB\n"
    "        | \\-HDD, 750 GiB\n"
    "        |   +-[0]: 100 GiB, root\n"
    "        |   \\-[1]: 600 GiB, log\n"
    "        +-Host: monitor12.fit.cvut.cz\n"
    "        | +-147.32.232.254\n"
    "        | +-CPU, 2 cores @ 1500MHz\n"
    "        | +-Memory, 4096 MiB\n"
    "        | \\-HDD, 750 GiB\n"
    "        |   +-[0]: 100 GiB, root\n"
    "        |   \\-[1]: 600 GiB, log\n"
    "        \\-Host: vm13.fit.cvut.cz\n"
    "          +-147.32.232.232\n"
    "          +-CPU, 32 cores @ 4000MHz\n"
    "          +-Memory, 32768 MiB\n"
    "          +-HDD, 3000 GiB\n"
    "          | +-[0]: 500 GiB, SYSTEM\n"
    "          | +-[1]: 2000 GiB, DATA\n"
    "          | \\-[2]: 500 GiB, BACKUP\n"
    "          +-Network: dummy00\n"
    "          | \\-Host: testing.fit.cvut.cz\n"
    "          |   +-192.168.1.1\n"
    "          |   +-CPU, 1 cores @ 300MHz\n"
    "          |   \\-Memory, 256 MiB\n"
    "          +-Memory, 16384 MiB\n"
    "          \\-Network: vnet11\n"
    "            +-Host: monitor5.fit.cvut.cz\n"
    "            | +-147.32.232.254\n"
    "            | +-CPU, 2 cores @ 1500MHz\n"
    "            | +-Memory, 4096 MiB\n"
    "            | \\-HDD, 750 GiB\n"
    "            |   +-[0]: 100 GiB, root\n"
    "            |   \\-[1]: 600 GiB, log\n"
    "            \\-Host: monitor6.fit.cvut.cz\n"
    "              +-147.32.232.254\n"
    "              +-CPU, 2 cores @ 1500MHz\n"
    "              +-Memory, 4096 MiB\n"
    "              \\-HDD, 750 GiB\n"
    "                +-[0]: 100 GiB, root\n"
    "                \\-[1]: 600 GiB, log\n" );

    cout << "ALL TESTS PASSED" << endl;
    return 0;
}
#endif /* __PROGTEST__ */
