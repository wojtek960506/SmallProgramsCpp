#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
using namespace std;

class CTimeStamp
{
public:
    CTimeStamp ( int year, int month, int day, int hour, int minute, double sec ) :
            m_year(year), m_month(month), m_day(day), m_hour(hour), m_minute(minute), m_sec(sec) {}
    int Compare ( const CTimeStamp & x ) const;
    friend ostream & operator << ( ostream & os, const CTimeStamp & x );
private:
    int m_year, m_month, m_day, m_hour, m_minute;
    double m_sec;
};

//positive number for *this > x ; 0 for *this == x ; negative number for *this < x
int CTimeStamp::Compare(const CTimeStamp &x) const {
    if (m_year > x.m_year) return 1;
    else if (m_year < x.m_year) return -1;
    else {
        if (m_month > x.m_month) return 1;
        else if (m_month < x.m_month) return -1;
        else {
            if (m_day > x.m_day) return 1;
            else if (m_day < x.m_day) return -1;
            else {
                if (m_hour > x.m_hour) return 1;
                else if (m_hour < x.m_hour) return -1;
                else {
                    if (m_minute > x.m_minute) return 1;
                    else if (m_minute < x.m_minute) return -1;
                    else {
                        if (m_sec > x.m_sec) return 1;
                        else if (m_sec < x.m_sec) return -1;
                        else return 0;
                    }
                }
            }
        }
    }
}

ostream & operator << (ostream & os, const CTimeStamp & x) {
    std::ios_base::fmtflags f(os.flags());
    os << dec << setfill('0');
    os << setw(4) << setfill('0') << x.m_year << "-";
    os << setw(2) << x.m_month << "-" << setw(2) << x.m_day << " ";
    os << setw(2) << x.m_hour << ":" << setw(2) << x.m_minute << ":" << setw(6) << setprecision(3) << fixed << x.m_sec;
    os.flags(f);
    return os;
}

class CMail
{
public:
    CMail ( const CTimeStamp & timeStamp, const string & from, const string & to, const string & subject ) :
        m_timeStamp(timeStamp), m_from(from), m_to(to), m_subject(subject) {}
    int CompareByTime ( const CTimeStamp & x ) const;
    int CompareByTime ( const CMail & x ) const;
    const string & From ( void ) const { return m_from; }
    const string & To ( void ) const { return m_to; }
    const string & Subject ( void ) const { return m_subject; }
    const CTimeStamp & TimeStamp ( void ) const { return m_timeStamp; }
    friend ostream & operator << ( ostream & os, const CMail & x );
private:
    CTimeStamp m_timeStamp;
    string m_from;
    string m_to;
    string m_subject;
};

int CMail::CompareByTime(const CTimeStamp &x) const {
    if ((m_timeStamp.Compare(x)) > 0) return 1;
    else if ((m_timeStamp.Compare(x)) < 0) return -1;
    else return 0;
}

int CMail::CompareByTime(const CMail &x) const {
    if ((m_timeStamp.Compare(x.m_timeStamp)) > 0) return 1;
    else if ((m_timeStamp.Compare(x.m_timeStamp)) < 0) return -1;
    else return 0;
}

ostream & operator << ( ostream & os, const CMail & x ) {
    std::ios_base::fmtflags f(os.flags());
    os << dec;
    os << x.TimeStamp() << " ";
    os << setw(0);
    os << x.From() << " -> " << x.To() << ", subject: " << x.Subject();
    os.flags(f);
    return os;
}

// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

class CMailLog
{
public:
    CMailLog() {}
    ~CMailLog() {}
    int ParseLog ( istream & in );
    list<CMail> ListMail ( const CTimeStamp & from, const CTimeStamp & to ) const;
    set<string> ActiveUsers ( const CTimeStamp & from, const CTimeStamp & to ) const;
    static const string MONTHS[12];
private:
    struct cmpTimeStamp { //I use this struct as a comparison for multimap in which I store mails
        bool operator()(const CTimeStamp & t1, const CTimeStamp & t2) const{ return t1.Compare(t2) < 0; }
    };
    multimap<CTimeStamp, CMail, cmpTimeStamp> m_listMail; //main container to store successfully parsed mails

    pair<string, string> parseMessage(const string & message) const;
    void parseMonth(const string & month_a, unsigned int & month) const;
    void parseLine( const string & daemonLog, unsigned int & month, unsigned int & day, unsigned int & year,
                    unsigned int & hour, unsigned int & minute, double & sec, string & relay,
                    string & mailID, string & message) const;
    void insertMail(const CTimeStamp & timeStamp,const string & from, const string to, const string & subject);

};

/*
 * array of months' acronyms used for parsing the mail acronym into number
 */
const string CMailLog::MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/*
 * auxiliary function to insert an email when we have its at least sender and receiver
 */
void CMailLog::insertMail(const CTimeStamp & timeStamp,const string & from, const string to, const string & subject) {
    m_listMail.insert(pair<CTimeStamp, CMail>(timeStamp,CMail(timeStamp,from, to, subject)));
}

/*
 * only messages with beginning either 'from=' or 'subject=' or 'to=' have some meaning for parsing the log
 * for those messages the first element of returned pair is respectively 'from', 'subject' and 'or'
 * second element of pair is what is after the first equal sign in the message
 * for other messages I return a pair of 2 empty strings
 */
pair<string, string> CMailLog::parseMessage(const string & message) const{
    vector<string> fields;
    if (message.length() > 3 && message.substr(0,3) == "to=") {
        return pair<string,string>("to",message.substr(3,message.size()-3));
    } else if (message.length() > 5 && message.substr(0,5) == "from=") {
        return pair<string,string>("from",message.substr(5,message.size()-5));
    } else if (message.length() >= 8 && message.substr(0,8) == "subject=") {
        if (message.length() == 8) return pair<string,string>("subject","");
        else return pair<string,string>("subject",message.substr(8,message.size()-8));
    } else {
        return pair<string,string>("","");
    }
}

/*
 * Parse 3 letter acronym of month into the number from 1 to 12.
 * Reference to unsigned int is used to store this value
 */
void CMailLog::parseMonth(const string & month_a, unsigned int & month) const{
    unsigned int i = 0;
    string tmp_m;
    const unsigned int NUM_OF_MONTHS = 12;
    for (; i < NUM_OF_MONTHS ; ++i) {
        tmp_m = CMailLog::MONTHS[i];
        if (month_a == tmp_m)
            break;
    }
    month = i+1;
}

/*
 * Parsing the line of the following format: month day year hour:minute:sec relay_name mailID: message
 * I assume that there is no need to validate this format.
 * All non-const references passed as arguments are used to store every single element of the line
 *
 */
void CMailLog::parseLine(   const string & daemonLog, unsigned int & month, unsigned int & day, unsigned int & year,
                            unsigned int & hour, unsigned int & minute, double & sec, string & relay,
                            string & mailID, string & message) const {
    char c;
    string month_a;
    istringstream iss(daemonLog);

    iss >> month_a; iss >> day; iss >> year; iss >> hour; iss >> c; iss >> minute; iss >> c; iss >> sec;
    iss >> relay; iss >> mailID;

    char a[10000]; //10000 just in case of very long message
    iss.getline(a, 10000, '\n');
    message = a;

    //I have to get rid of spaces which are between 'mailID:' and 'message'
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t start = message.find_first_not_of(WHITESPACE);
    message = message.substr(start);

    parseMonth(month_a, month);
}

/*
 * main function of parsing the mail log
 */
int CMailLog::ParseLog(istream &in) {
    string daemonLog;
    string relay, mailID, message;
    unsigned int month, day, year, hour, minute;
    double sec;
    //this map is used to keep the sender and subject to add those values to the mail when we have reveiver of this mail
    map<string, pair<string,string>> fromAndSubject; //<mailID, <from, subject>>
    unsigned int counter = 0;
    while(getline(in, daemonLog)) {

        parseLine(daemonLog, month, day, year, hour, minute, sec, relay, mailID, message);
        CTimeStamp tmpTimeStamp(year, month, day, hour, minute, sec);
        pair<string, string> msg = parseMessage(message);
        if (msg.first == "") continue; //there was neither 'from' nor 'subject' nor 'to'
        else if (msg.first == "from") {
            //by default subject is set to an empty string
            fromAndSubject.insert(pair<string,pair<string,string>>(mailID,pair<string,string>(msg.second,"")));
            continue;
        } else if (msg.first == "subject") {
            auto it = fromAndSubject.find(mailID);
            if (it == fromAndSubject.end()) continue; //there cannot be subject for an mail without the sender
            it->second.second = msg.second;
        } else if (msg.first == "to"){
            auto it = fromAndSubject.find(mailID);
            if (it == fromAndSubject.end()) continue; //there cannot be a receiver of an mail without the sender
            string from = it->second.first;
            string subject = it->second.second;
            string to = msg.second;
            insertMail(tmpTimeStamp, from, to, subject);
            ++counter; //we only count how much mail was was saved (we need at least sender and receiver
        } else {
            continue; //just in case. Actually program should never come to this if
        }
    }
    return counter;
}

/*
 * This method returns a list of e-mail records that were delivered in the time interval specified (both boundaries inclusive).
 * The list returned is sorted in an ascending order by delivery timestamp,
 * becuase key of multimap for storing mails is a delivery timestamp
 * If there are two mails with identical delivery timestamp, then the relative order from the log is preserved,
 * because of the structure of multimap.
 */
list<CMail> CMailLog::ListMail(const CTimeStamp &from, const CTimeStamp &to) const {
    list<CMail> valuesToReturn;
    if (from.Compare(to) > 0) return valuesToReturn; //if the beginning of range is bigger then end of range then fail
    auto firstEqualOrBiggerThanFrom = m_listMail.find(from); //check if lower bound is equal to some element
    if (firstEqualOrBiggerThanFrom == m_listMail.end()) { //there is no element equal so I have to find first bigger
        firstEqualOrBiggerThanFrom = m_listMail.upper_bound(from);
    }
    if (firstEqualOrBiggerThanFrom == m_listMail.end()) {
        return valuesToReturn; //all elements stored are smaller than lower bound of range
    }
    //pointer to the first element bigger than upper bound of range. It is also fine when 'to' is equal to some element
    auto firstBiggerThanTo = m_listMail.upper_bound(to);

    for (; firstEqualOrBiggerThanFrom != firstBiggerThanTo ; ++firstEqualOrBiggerThanFrom) {
        valuesToReturn.push_back(firstEqualOrBiggerThanFrom->second);
    }
    return valuesToReturn;
}

/*
 * This method considers the mail records that were delivered in the time interval specified (both boundaries inclusive).
 * The return value is the set of all e-mail addresses that participated in these e-mails (both senders and receivers).
 * As it is set then there cannot be a value stored twice and values are sorted.
 */
set<string> CMailLog::ActiveUsers(const CTimeStamp &from, const CTimeStamp &to) const {
    set<string> valuesToReturn;
    if (from.Compare(to) > 0) return valuesToReturn; //if the beginning of range is bigger then end of range then fail
    auto firstEqualOrBiggerThanFrom = m_listMail.find(from); //check if lower bound is equal to some element
    if (firstEqualOrBiggerThanFrom == m_listMail.end()) { //there is no element equal so I have to find first bigger
        firstEqualOrBiggerThanFrom = m_listMail.upper_bound(from);
    }
    if (firstEqualOrBiggerThanFrom == m_listMail.end()) {
        return valuesToReturn; //all elements stored are smaller than lower bound of range
    }
    //pointer to the first element bigger than upper bound of range. It is also fine when 'to' is equal to some element
    auto firstBiggerThanTo = m_listMail.upper_bound(to);
    for (; firstEqualOrBiggerThanFrom != firstBiggerThanTo ; ++firstEqualOrBiggerThanFrom) {
        valuesToReturn.insert(firstEqualOrBiggerThanFrom->second.From());
        valuesToReturn.insert(firstEqualOrBiggerThanFrom->second.To());
    }
    return valuesToReturn;
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
string             printMail                               ( const list<CMail> & all )
{
    ostringstream oss;
    for ( const auto & mail : all )
        oss << mail << endl;
    return oss . str ();
}
string             printUsers                              ( const set<string> & all )
{
    ostringstream oss;
    bool first = true;
    for ( const auto & name : all )
    {
        if ( ! first )
            oss << ", ";
        else
            first = false;
        oss << name;
    }
    return oss . str ();
}
void testFromSample() {
    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;

    iss . clear ();
    iss . str (
            "Mar 29 2019 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
            "Mar 29 2019 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
            "Mar 29 2019 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
            "Mar 29 2019 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
            "Mar 29 2019 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
            "Mar 29 2019 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
            "Mar 29 2019 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
            "Mar 29 2019 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
            "Mar 29 2019 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
    assert ( m . ParseLog ( iss ) == 8 );
    mailList = m . ListMail ( CTimeStamp ( 2019, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 23, 59, 59 ) );
    assert ( printMail ( mailList ) ==
             "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
             "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
             "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n"
             "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner\n"
             "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner\n"
             "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner\n"
             "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner\n"
             "2019-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner\n" );
    mailList = m . ListMail ( CTimeStamp ( 2019, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 14, 58, 32 ) );
    assert ( printMail ( mailList ) ==
             "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
             "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
             "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n" );
    mailList = m . ListMail ( CTimeStamp ( 2019, 3, 30, 0, 0, 0 ),
                              CTimeStamp ( 2019, 3, 30, 23, 59, 59 ) );
    assert ( printMail ( mailList ) == "" );
    users = m . ActiveUsers ( CTimeStamp ( 2019, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 23, 59, 59 ) );
    assert ( printUsers ( users ) == "CEO@fit.cvut.cz, CIO@fit.cvut.cz, HR-department@fit.cvut.cz, PR-department@fit.cvut.cz, archive@fit.cvut.cz, boss13@fit.cvut.cz, dean@fit.cvut.cz, office13@fit.cvut.cz, person3@fit.cvut.cz, user76@fit.cvut.cz, vice-dean@fit.cvut.cz" );
    users = m . ActiveUsers ( CTimeStamp ( 2019, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 13, 59, 59 ) );
    assert ( printUsers ( users ) == "person3@fit.cvut.cz, user76@fit.cvut.cz" );

    users = m.ActiveUsers( CTimeStamp (2019, 3, 29, 15, 2, 34.230), CTimeStamp ( 2019, 3, 29, 15, 2, 34.231));
    assert (printUsers (users) == "CEO@fit.cvut.cz, CIO@fit.cvut.cz, PR-department@fit.cvut.cz, archive@fit.cvut.cz, dean@fit.cvut.cz, vice-dean@fit.cvut.cz" );

    users = m.ActiveUsers( CTimeStamp (2019, 3, 29, 15, 2, 34.231), CTimeStamp ( 2019, 3, 29, 15, 2, 34.231) );
    assert (printUsers (users) == "CIO@fit.cvut.cz, PR-department@fit.cvut.cz");
}
void compareTimeStampTest() {
    CTimeStamp a (2004,11,5,9,21,254.33);
    CTimeStamp b (2004,11,5,9,21,254.33);
    CTimeStamp c (2005,11,5,9,21,254.33);
    CTimeStamp d (2003,11,5,9,21,254.33);
    CTimeStamp e (2004,12,5,9,21,254.33);
    CTimeStamp f (2004,10,5,9,21,254.33);
    CTimeStamp g (2004,11,6,9,21,254.33);
    CTimeStamp h (2004,11,4,9,21,254.33);
    CTimeStamp i (2004,11,5,10,21,254.33);
    CTimeStamp j (2004,11,5,8,21,254.33);
    CTimeStamp k (2004,11,5,9,22,254.33);
    CTimeStamp l (2004,11,5,9,20,254.33);
    CTimeStamp m (2004,11,5,9,21,254.34);
    CTimeStamp n (2004,11,5,9,21,254.32);
    CTimeStamp o (2004,11,5,9,21,255.33);
    CTimeStamp p (2004,11,5,9,21,253.33);
    assert( a.Compare(b) == 0);
    assert( a.Compare(c) == -1);
    assert( a.Compare(d) == 1);
    assert( a.Compare(e) == -1);
    assert( a.Compare(f) == 1);
    assert( a.Compare(g) == -1);
    assert( a.Compare(h) == 1);
    assert( a.Compare(i) == -1);
    assert( a.Compare(j) == 1);
    assert( a.Compare(k) == -1);
    assert( a.Compare(l) == 1);
    assert( a.Compare(m) == -1);
    assert( a.Compare(n) == 1);
    assert( a.Compare(o) == -1);
    assert( a.Compare(p) == 1);
}
void printMailTest() {
    CTimeStamp a (2019,03,29,13,36,13.023);
    CTimeStamp p (2004,11,5,9,21,253.33);
    CMail m (a,"person3@fit.cvut.cz","user76@fit.cvut.cz", "New progtest homework!");
    CMail m1 (a,"person3@fit.cvut.cz","user77@fit.cvut.cz", "New progtest homework!");
    list<CMail> l;
    l.push_back(m);
    l.push_back(m1);
    assert ( printMail(l) ==
             "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
             "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user77@fit.cvut.cz, subject: New progtest homework!\n");

    assert (m.CompareByTime(m1) == 0);
    assert (m.CompareByTime(p) == 1);
}
void myTest1() {
    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;

    iss . clear ();
    iss . str (
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz AAA: from=aaa\n"
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz BBB: from=bbb\n"
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz CCC: from=ccc\n"
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz DDD: from=ddd\n"
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz EEE: from=eee\n"
            "Mar 29 2019 12:35:32.000 relay.fit.cvut.cz FFF: from=fff\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz BBB: to=BoB\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz DDD: to=DoD\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz CCC: to=CoC\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz A: to=CIO@fit.cvut.cz\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz A: to=CEO@fit.cvut.cz\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz A: to=dean@fit.cvut.cz\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz A: to=vice-dean@fit.cvut.cz\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz AAA: to=AoA\n"
            "Mar 29 2019 13:00:00.000 relay.fit.cvut.cz EEE: to=EoE\n");
    assert ( m . ParseLog ( iss ) == 5 );
    mailList = m . ListMail ( CTimeStamp ( 2019, 3, 29, 13, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 13, 0, 1 ) );
    assert ( printMail ( mailList ) ==
             "2019-03-29 13:00:00.000 bbb -> BoB, subject: \n"
             "2019-03-29 13:00:00.000 ddd -> DoD, subject: \n"
             "2019-03-29 13:00:00.000 ccc -> CoC, subject: \n"
             "2019-03-29 13:00:00.000 aaa -> AoA, subject: \n"
             "2019-03-29 13:00:00.000 eee -> EoE, subject: \n" );

    mailList = m.ListMail ( CTimeStamp ( 2019, 3, 29, 12, 00, 59.999 ),
                            CTimeStamp ( 2019, 3, 29, 12, 59, 59.999 ) );
    assert ( printMail (mailList) == "");

    users = m . ActiveUsers ( CTimeStamp ( 2019, 3, 29, 13, 0, 0 ),
                              CTimeStamp ( 2019, 3, 29, 13, 0, 0 ) );
    assert ( printUsers ( users ) == "AoA, BoB, CoC, DoD, EoE, aaa, bbb, ccc, ddd, eee");

    iss.clear();
    iss.str("Mar 3 2019 12:35:32.000 relay.fit.cvut.cz AAA: from=abc\n"
            "Mar 3 2019 12:35:32.000 relay.fit.cvut.cz AAA: subject=abc\n"
            "Mar 3 2019 12:35:32.000 relay.fit.cvut.cz AAA: to=def\n");
    assert (m.ParseLog(iss) == 1);

}
void myTest2() {
    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;
    iss.clear();
    iss.str(
            "Jun 29 2019 14:00:35.230 relay ID1: from=mymail1@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID1: from=mymail2@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID2: from=mymail3@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID2: from=mymail4@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID3: from=mymail5@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID3: from=mymail6@wp.pl\n"
            "Jun 29 2019 14:00:35.230 relay ID4: from=mymail7@wp.pl\n"
            ""
    );
    assert ( m . ParseLog ( iss ) == 0 );
}
void myTest3() {
    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;
    iss.clear();
    iss.str(
            "Jul 1 2000 00:00:00.000 relay ID1: from=Wojti\n"
            "Jul 1 2000 00:00:00.000 relay ID2: from=Cinio\n"
            "Jul 1 2000 00:00:00.001 relay ID1: subject=Yo bro\n"
            "Jul 1 2000 00:00:00.001 relay ID3: from=Gelo\n"
            "Jul 1 2000 00:00:00.001 relay ID1: to=Cinio\n"
            "Jul 1 2000 00:00:00.001 relay ID1: to=Filip\n"
            "Jul 1 2000 00:00:00.002 relay ID1: to=Lucas\n"
            "Jul 1 2000 00:00:00.005 relay ID3: subject=Dear Sir\n"
            "Jul 1 2000 00:00:00.010 relay ID2: subject=Elo mordy\n"
            "Jul 1 2000 00:00:00.020 relay ID2: to=Alek\n"
            "Jul 1 2000 00:00:01.040 relay ID2: to=Deresz\n"
            "Jul 1 2000 00:00:01.040 relay ID1: to=Jakub\n"
            "Jul 1 2000 00:00:01.040 relay ID2: to=Galson\n"
            "Jul 1 2000 00:00:01.055 relay ID3: to=JanT\n"
            "Jul 1 2000 00:00:01.056 relay ID3: to=MartinS\n"
            "Jul 1 2000 00:00:02.000 relay ID1: to=Michal\n"
            "Jul 1 2000 00:00:05.055 relay ID4: from=Ola\n"
            "Jul 1 2000 00:00:06.055 relay ID4: to=Ala\n"
            "Jul 1 2000 00:00:07.055 relay ID3: to=MartinS\n"
            "Jul 5 2000 23:59:59.999 relay ID3: to=AlexM\n"
            "Aug 1 2000 15:59:59.999 relay ID2: to=Karol\n"
            "Sep 5 2000 23:45:59.999 relay ID3: to=AlexM\n"
            "Jan 1 2001 12:12:12.111 relay ID4: to=Basia\n"
            "Dec 5 2003 23:59:59.999 relay ID5: to=AlexM\n"
            "Dec 6 2003 23:59:59.999 relay ID6: to=AlexM\n"
            "Dec 12 2003 23:59:59.999 relay ID7: subject=123\n"
    );

    assert(m.ParseLog(iss) == 16);
    mailList = m . ListMail ( CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                              CTimeStamp ( 2003, 12, 29, 23, 59, 59 ) );
    assert ( printMail ( mailList ) ==
             "2000-07-01 00:00:00.001 Wojti -> Cinio, subject: Yo bro\n"
             "2000-07-01 00:00:00.001 Wojti -> Filip, subject: Yo bro\n"
             "2000-07-01 00:00:00.002 Wojti -> Lucas, subject: Yo bro\n"
             "2000-07-01 00:00:00.020 Cinio -> Alek, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Cinio -> Deresz, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Wojti -> Jakub, subject: Yo bro\n"
             "2000-07-01 00:00:01.040 Cinio -> Galson, subject: Elo mordy\n"
             "2000-07-01 00:00:01.055 Gelo -> JanT, subject: Dear Sir\n"
             "2000-07-01 00:00:01.056 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-01 00:00:02.000 Wojti -> Michal, subject: Yo bro\n"
             "2000-07-01 00:00:06.055 Ola -> Ala, subject: \n"
             "2000-07-01 00:00:07.055 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-05 23:59:59.999 Gelo -> AlexM, subject: Dear Sir\n"
             "2000-08-01 15:59:59.999 Cinio -> Karol, subject: Elo mordy\n"
             "2000-09-05 23:45:59.999 Gelo -> AlexM, subject: Dear Sir\n"
             "2001-01-01 12:12:12.111 Ola -> Basia, subject: \n"
             );

    mailList = m . ListMail ( CTimeStamp ( 2000, 7, 1, 0, 0, 0.001 ),
                              CTimeStamp ( 2001, 1, 1, 12, 12, 12.111 ) );
    assert ( printMail ( mailList ) ==
             "2000-07-01 00:00:00.001 Wojti -> Cinio, subject: Yo bro\n"
             "2000-07-01 00:00:00.001 Wojti -> Filip, subject: Yo bro\n"
             "2000-07-01 00:00:00.002 Wojti -> Lucas, subject: Yo bro\n"
             "2000-07-01 00:00:00.020 Cinio -> Alek, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Cinio -> Deresz, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Wojti -> Jakub, subject: Yo bro\n"
             "2000-07-01 00:00:01.040 Cinio -> Galson, subject: Elo mordy\n"
             "2000-07-01 00:00:01.055 Gelo -> JanT, subject: Dear Sir\n"
             "2000-07-01 00:00:01.056 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-01 00:00:02.000 Wojti -> Michal, subject: Yo bro\n"
             "2000-07-01 00:00:06.055 Ola -> Ala, subject: \n"
             "2000-07-01 00:00:07.055 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-05 23:59:59.999 Gelo -> AlexM, subject: Dear Sir\n"
             "2000-08-01 15:59:59.999 Cinio -> Karol, subject: Elo mordy\n"
             "2000-09-05 23:45:59.999 Gelo -> AlexM, subject: Dear Sir\n"
             "2001-01-01 12:12:12.111 Ola -> Basia, subject: \n"
    );

    mailList = m . ListMail ( CTimeStamp ( 2000, 7, 1, 0, 0, 0.002 ),
                              CTimeStamp ( 2001, 1, 1, 12, 12, 12.110 ) );
    assert ( printMail ( mailList ) ==

             "2000-07-01 00:00:00.002 Wojti -> Lucas, subject: Yo bro\n"
             "2000-07-01 00:00:00.020 Cinio -> Alek, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Cinio -> Deresz, subject: Elo mordy\n"
             "2000-07-01 00:00:01.040 Wojti -> Jakub, subject: Yo bro\n"
             "2000-07-01 00:00:01.040 Cinio -> Galson, subject: Elo mordy\n"
             "2000-07-01 00:00:01.055 Gelo -> JanT, subject: Dear Sir\n"
             "2000-07-01 00:00:01.056 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-01 00:00:02.000 Wojti -> Michal, subject: Yo bro\n"
             "2000-07-01 00:00:06.055 Ola -> Ala, subject: \n"
             "2000-07-01 00:00:07.055 Gelo -> MartinS, subject: Dear Sir\n"
             "2000-07-05 23:59:59.999 Gelo -> AlexM, subject: Dear Sir\n"
             "2000-08-01 15:59:59.999 Cinio -> Karol, subject: Elo mordy\n"
             "2000-09-05 23:45:59.999 Gelo -> AlexM, subject: Dear Sir\n"

    );


}


int main ( void )
{
    testFromSample();
    compareTimeStampTest();
    printMailTest();
    myTest1();
    myTest2();
    myTest3();

    cout << "ALL TESTS PASSED" << endl;

    return 0;
}
#endif /* __PROGTEST__ */