#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#endif /* C++ 11 */

using namespace std;

#endif /* __PROGTEST__ */

class NoRouteException{};

template <typename _T, typename _E>
class CRoute
{
public:
    // default constructor
    CRoute () {}
    // destructor
    ~CRoute() {}
    // Add
    CRoute & Add ( const _T & node1, const _T & node2, const _E & edge);
    // Find (with optional filter)
    list<_T> Find ( const _T & node1, const _T & node2) const;
    list<_T> Find ( const _T & node1, const _T & node2, function<bool(_E)> f) const;
private:
    map < _T, vector < pair < _T, _E > > > m_graph; //represented as adjacency list
    void addOneWay (const _T & node1, const _T & node2, const _E & edge);
    list<_T> generalFind(const _T & node1, const _T & node2, bool extendedFind, function<bool(_E)> f) const;
};

template <typename _T, typename _E>
void CRoute<_T,_E>::addOneWay (const _T & node1, const _T & node2, const _E & edge) {
    auto it = m_graph.find(node1);
    if (it != m_graph.end()) it->second.push_back(make_pair(node2, edge));
    else {
        vector < pair < _T, _E > > tmp;
        tmp.push_back(make_pair(node2, edge));
        m_graph.insert(make_pair(node1, tmp));
    }
}

template <typename _T, typename _E>
CRoute<_T,_E> & CRoute<_T,_E>::Add ( const _T & node1, const _T & node2, const _E & edge) {
    addOneWay(node1, node2, edge);
    addOneWay(node2, node1, edge);
    return *this;
}

template <typename _T, typename _E>
list<_T> CRoute<_T,_E>::generalFind(const _T & node1, const _T & node2, bool extendedFind, function<bool(_E)> f) const {
    map <_T, _T> previousInTheShortestPath; //for every node we will just store the previous one from the shortest path
    map <_T, bool> visited; //to mark which node was visited
    map <_T, unsigned int> distances;

    for (const auto & it : m_graph) visited.insert(make_pair(it.first, false)); //fill visited table
    for (const auto & it : m_graph) distances.insert(make_pair(it.first, 0)); //fill table with distances
    for (const auto & it : m_graph) previousInTheShortestPath.insert(make_pair(it.first, node2));

    if (node1 == node2) return list<_T>({node1}); //begin is end so the shortest path is just begin
    auto beginNode = m_graph.find(node1);
    if (beginNode == m_graph.end()) throw NoRouteException(); //there is no such begin node in graph so it is an exception
    auto endNode = m_graph.find(node2);
    if (endNode == m_graph.end()) throw NoRouteException(); //there is no such end node in graph so it is an exception

    queue <_T> q;
    q.push(beginNode->first);
    visited.find(beginNode->first)->second = true; //mark begin node as visited
    distances.find(beginNode->first)->second = 0; //mark distance from the begin node to begin node as 0

    while (!q.empty()) { //main loop of algorithm
        _T ffQ = q.front(); //take first node from queue
        q.pop(); //delete first element from queue

        const vector < pair < _T, _E > > & ffQ_adjList = m_graph.find(ffQ)->second;
        for (const pair<_T,_E> & nodeToCheck : ffQ_adjList ) {
            if (visited.find(nodeToCheck.first)->second) continue; //the node have been already visited

            if (extendedFind) {
                if (!f(nodeToCheck.second)) continue; //the edge doesn't fulfil the filter so skip it
            }
            //update distance
            distances.find(nodeToCheck.first)->second = distances.find(ffQ)->second + 1;

            previousInTheShortestPath.find(nodeToCheck.first)->second = ffQ; //save the previous node for the path

            auto nodeToPush = m_graph.find(nodeToCheck.first)->first;
            q.push(nodeToPush); //we put the current node to the queue

            visited.find(nodeToCheck.first)->second = true; //update visited table
        }
    }

    if (distances.find(node2)->second == 0) throw NoRouteException(); //there is no route between those nodes

    //retrieve the shortest path
    _T tmpListElem = previousInTheShortestPath.find(node2)->second;
    list <_T> listToReturn;
    listToReturn.emplace_front(node2);
    while (tmpListElem != node2) {
        listToReturn.emplace_front(tmpListElem);
        tmpListElem = previousInTheShortestPath.find(tmpListElem)->second;
    }

    return listToReturn;
}

template <typename _T, typename _E>
list<_T> CRoute<_T,_E>::Find ( const _T & node1, const _T & node2) const {
    return generalFind(node1, node2, false, [](_E){return false;});
}

//I just copied the above function and will add something to proceed this parameter f. Later I will simplify this code
template <typename _T, typename _E>
list<_T> CRoute<_T,_E>::Find ( const _T & node1, const _T & node2, function<bool(_E)> f) const {
    return generalFind(node1, node2, true, f);
}

#ifndef __PROGTEST__
//=================================================================================================
class CTrain
{
public:
    CTrain                        ( const string    & company,
                                    int               speed )
            : m_Company ( company ),
              m_Speed ( speed )
    {
    }
    //---------------------------------------------------------------------------------------------
    string                   m_Company;
    int                      m_Speed;
};
//=================================================================================================
class TrainFilterCompany
{
public:
    TrainFilterCompany            ( const set<string> & companies )
            : m_Companies ( companies )
    {
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain & train ) const
    {
        return m_Companies . find ( train . m_Company ) != m_Companies . end ();
    }
    //---------------------------------------------------------------------------------------------
private:
    set <string>             m_Companies;
};
//=================================================================================================
class TrainFilterSpeed
{
public:
    TrainFilterSpeed              ( int               min,
                                    int               max )
            : m_Min ( min ),
              m_Max ( max )
    {
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain    & train ) const
    {
        return train . m_Speed >= m_Min && train . m_Speed <= m_Max;
    }
    //---------------------------------------------------------------------------------------------
private:
    int                      m_Min;
    int                      m_Max;
};
//=================================================================================================
bool               NurSchnellzug                           ( const CTrain    & zug )
{
    return ( zug . m_Company == "OBB" || zug . m_Company == "DB" ) && zug . m_Speed > 100;
}
//=================================================================================================
static string      toText                                  ( const list<string> & l )
{
    ostringstream oss;

    auto it = l . cbegin();
    oss << *it;
    for ( ++it; it != l . cend (); ++it )
        oss << " > " << *it;
    return oss . str ();
}
//=================================================================================================
int main ( void )
{
//----------------------------------------------------------------------------------------------------------------------
//---TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE---
//----------------------------------------------------------------------------------------------------------------------

    CRoute<string,CTrain> lines;
    lines . Add ( "Berlin", "Prague", CTrain ( "DB", 120 ) )
            . Add ( "Berlin", "Prague", CTrain ( "CD",  80 ) )
            . Add ( "Berlin", "Dresden", CTrain ( "DB", 160 ) )
            . Add ( "Dresden", "Munchen", CTrain ( "DB", 160 ) )
            . Add ( "Munchen", "Prague", CTrain ( "CD",  90 ) )
            . Add ( "Munchen", "Linz", CTrain ( "DB", 200 ) )
            . Add ( "Munchen", "Linz", CTrain ( "OBB", 90 ) )
            . Add ( "Linz", "Prague", CTrain ( "CD", 50 ) )
            . Add ( "Prague", "Wien", CTrain ( "CD", 100 ) )
            . Add ( "Linz", "Wien", CTrain ( "OBB", 160 ) )
            . Add ( "Paris", "Marseille", CTrain ( "SNCF", 300 ))
            . Add ( "Paris", "Dresden",  CTrain ( "SNCF", 250 ) );

    lines.Find("Berlin", "Linz");


    list<string> r1 = lines . Find ( "Berlin", "Linz" );
    assert ( toText ( r1 ) == "Berlin > Prague > Linz" );

    list<string> r2 = lines . Find ( "Linz", "Berlin" );
    assert ( toText ( r2 ) == "Linz > Prague > Berlin" );

    list<string> r3 = lines . Find ( "Wien", "Berlin" );
    assert ( toText ( r3 ) == "Wien > Prague > Berlin" );


    list<string> r4 = lines . Find ( "Wien", "Berlin", NurSchnellzug );
    assert ( toText ( r4 ) == "Wien > Linz > Munchen > Dresden > Berlin" );

    list<string> r5 = lines . Find ( "Wien", "Munchen", TrainFilterCompany ( set<string> { "CD", "DB" } ) );
    assert ( toText ( r5 ) == "Wien > Prague > Munchen" );

    list<string> r6 = lines . Find ( "Wien", "Munchen", TrainFilterSpeed ( 120, 200 ) );
    assert ( toText ( r6 ) == "Wien > Linz > Munchen" );

    list<string> r7 = lines . Find ( "Wien", "Munchen", [] ( const CTrain & x ) { return x . m_Company == "CD"; } );
    assert ( toText ( r7 ) == "Wien > Prague > Munchen" );

    list<string> r8 = lines . Find ( "Munchen", "Munchen" );
    assert ( toText ( r8 ) == "Munchen" );

    list<string> r9 = lines . Find ( "Marseille", "Prague" );
    assert ( toText ( r9 ) == "Marseille > Paris > Dresden > Berlin > Prague"
             || toText ( r9 ) == "Marseille > Paris > Dresden > Munchen > Prague" );

    try
    {
        list<string> r10 = lines . Find ( "Marseille", "Prague", NurSchnellzug );
        assert ( "Marseille > Prague connection does not exist!!" == NULL );
    }
    catch ( const NoRouteException & e )
    {
    }

    list<string> r11 = lines . Find ( "Salzburg", "Salzburg" );
    assert ( toText ( r11 ) == "Salzburg" );

    list<string> r12 = lines . Find ( "Salzburg", "Salzburg", [] ( const CTrain & x ) { return x . m_Company == "SNCF"; }  );
    assert ( toText ( r12 ) == "Salzburg" );

    try
    {
        list<string> r13 = lines . Find ( "London", "Oxford" );
        assert ( "London > Oxford connection does not exist!!" == NULL );
    }
    catch ( const NoRouteException & e )
    {
    }

//----------------------------------------------------------------------------------------------------------------------
//---TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE--TESTS FROM SAMPLE---
//----------------------------------------------------------------------------------------------------------------------

    cout << "ALL TESTS PASSED" << endl;

    return 0;
}
#endif  /* __PROGTEST__ */
