#ifndef TRANSPORT_NETWORK_H
#define TRANSPORT_NETWORK_H

#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <list>
#include <map>
#include <utility>
#include <Structs/Trees/priorityQueue.h>
#include "definitions.h"
#include <Utilities/geographic.h>

template<class T>
struct TimeEvent_
{

  TimeEvent_():timestamp(0), nextIndex(NO_INDEX), sDays(0), next(nullptr)
  {

  }
  TimeEvent_ *next; //8
  T dest; // 8
  Time depTime; //4
  Time arrTime;//4
  Distance validDeparture; // 4
  TripID trId; // 4
  TTL timestamp; // 2
  Index nextIndex; // 2
  unsigned char sDays; // 1

  int getLength() const
  {
   return (arrTime-depTime);
  }

  bool isDiscovered(const TTL *current)
  {
    return (timestamp == *current);
  }

  void setDepartureTime( const Time departureTime)
  {
      depTime = departureTime;
  }

  Time getDepartureTime() const
  {
      return depTime;
  }

  Time getArrivalTime() const
  {
      return arrTime;
  }

  bool operator < (const TimeEvent_& t2) const
  {
     return (depTime < t2.depTime );
  }

  bool operator == (const TimeEvent_& t1) const
  {
    return((depTime == t1.depTime) && (getLength() == t1.getLength()) && (sDays == t1.sDays)) ;
  }


   TimeEvent_& operator = ( const TimeEvent_& t1)
   {
     depTime = t1.depTime;
     arrTime = t1.arrTime;
     trId = t1.trId;
     next = t1.next;
     timestamp = t1.timestamp;
     nextIndex = t1.nextIndex;
     sDays = t1.sDays;
     validDeparture = t1.validDeparture;
     dest = t1.dest;
     return *this;
   }


};

struct TimesContainer
{
     TimesContainer():
     numItems(0)
     {}

     Index numItems;

     Index size() const
     {
       return numItems;
     }


     Time* Fend()
     {
         return ((Time*) &(index[numItems]));
     }

     Time* Fbegin()
     {
         return ((Time*) &(index[0]));
     }

     void emplace_back( Time te)
     {
        index[size()] = te;
        numItems++;
     }

     Time& operator[]( const int i)
     {
       return index[i];
     }

     const Time& operator[]( const int i) const
     {
       return index[i];
     }

     struct Index_
     {
       Time& operator[]( const int i)
       {
         return *(((Time*) this)+i);
       }

       const Time& operator[]( const int i) const
       {
         return *(((Time*) this)+i);
       }

     } index;
};


template <class T>
struct TimeEventContainer
{
     TimeEventContainer():
     numItems(0)
     {}

     Index numItems;

     int size() const
     {
       return numItems;
     }

     void emplace_back( TimeEvent_<T>* te)
     {
        index[size()] = te;
        numItems++;
     }

     TimeEvent_<T>*& operator[]( const int i)
     {
       return index[i];
     }

     const TimeEvent_<T>*& operator[]( const int i) const
     {
       return index[i];
     }

     struct Index_
     {
       TimeEvent_<T>*& operator[]( const int i)
       {
         return *(((TimeEvent_<T>**) this)+i);
       }

       const TimeEvent_<T>*& operator[]( const int i) const
       {
         return *(((TimeEvent_<T>**) this)+i);
       }

     } index;
};

template <class T>
struct TimeSlotItem
{
  TimeSlotItem(Time t,  TimeEventContainer<T> events)
  {
    time = t;
    timeEvents = events;
  }

  TimeSlotItem(Time t)
  {
    time = t;
  }

  bool operator<( const TimeSlotItem& rhs) const
  {
    return time < rhs.time;
  }

  bool operator<( const int rhs) const
  {
    return time < rhs;
  }

  bool operator>( const int rhs) const
  {
    return time > rhs;
  }



  union
  {
      Time time;
      Time first;
  };

  union
  {
    TimeEventContainer<T> timeEvents;
    TimeEventContainer<T> second;
  };
};

template <class T>
struct TimeSlotContainer
{
     TimeSlotContainer():
     numItems(0)
     {}

     int numItems;

     int size() const
     {
       return numItems;
     }

      TimeSlotItem<T>* end()
      {
          return (((TimeSlotItem<T>*) this)+numItems);
      }

      TimeSlotItem<T>* begin()
      {
          return ((TimeSlotItem<T>*) this);
      }




     void emplace_back( TimeSlotItem<T> ts)
     {
        index[size()] = ts;
        numItems++;

     }

     TimeSlotItem<T>& operator[]( const int i)
     {
       return index[i];
     }

     const TimeSlotItem<T>& operator[]( const int i) const
     {
       return index[i];
     }

     struct Index
     {
       TimeSlotItem<T>& operator[]( const int i)
       {
         return *(((TimeSlotItem<T>*) this)+i);
       }

       const TimeSlotItem<T>& operator[]( const int i) const
       {
         return *(((TimeSlotItem<T>*) this)+i);
       }

     } index;
};


template <class T>
struct TspNode{



TspNode():stId(0), first(nullptr), second(nullptr){}

void* pred = 0; // 8
Distance dist = INF; // 4
TTL timestamp = 0; // 2
Index IterDist = NO_INDEX; // 2
unsigned short int minTransferTime; //2
StationID stId; // 2
TTL Permant = 0; // 2


union
{
    TimesContainer* times;
    TimesContainer* first;
};

union
{
  TimeEventContainer<T>* timeEvents;
  TimeEventContainer<T>* second;
};

TimeEventContainer<T>& IndexSecond(const int j)
{
 return  *(TimeEventContainer<T>*) (((char*) timeEvents)+(timeEvents->size()*sizeof(TimeEvent_<T>*) + sizeof(int))*j);
}

TimeEventContainer<T>* getSecond(const int j)
{
 return  (TimeEventContainer<T>*) (((char*) timeEvents)+(timeEvents->size()*sizeof(TimeEvent_<T>*) + sizeof(int))*j);
}

Time& IndexFirst(const int j)
{
  return (*times)[j];
}


static char* IndexTablePool;


Distance getDistance() const
{
  return dist;
}

TspNode& operator = ( const TspNode& t)
{
   dist = t.dist;
   pred = t.pred;
   timestamp = t.timestamp;
   stId = t.stId;
   Permant = t.Permant;
   first = t.first;
   second = t.second;
   IterDist = t.IterDist;
   minTransferTime = t.minTransferTime;

   return *this;

}

};

template <class T>
char* TspNode<T>::IndexTablePool;


template<class T>
struct CompareIndex
{
   bool operator () (const std::pair<Time, std::vector<TimeEvent_<T>* > >& t1, const std::pair<Time, std::vector<TimeEvent_<T>* > >& t2) const
   {
      return (t1.first < t2.first );

   }
};

template<class T>
struct EqIndex
{
   bool operator () (const std::pair<Time, std::vector<TimeEvent_<T>* > >& t1, const std::pair<Time, std::vector<TimeEvent_<T>* > >& t2) const
   {
      return (t1.first == t2.first );

   }
};

template<class T>
struct Compare
{
   bool operator () (const TimeEvent_<T>& t1, const TimeEvent_<T>& t2) const
   {
      if(t1.depTime == t2.depTime)
      {
         if(t1.length == t2.length) {return (t1.sDays >t2.sDays); }
         else return (t1.length < t2.length);
      }
      else return (t1.depTime < t2.depTime );

   }
};

template<class T>
struct CompareArr
{
   bool operator () (const TimeEvent_<T>& t1, const TimeEvent_<T>& t2) const
   {
      /*if(t1.arrTime == t2.arrTime) return (t1.validDeparture < t2.validDeparture);
      else */return (t1.arrTime < t2.arrTime );

   }
};


template <class T>
struct TspEdge{


TspEdge(Distance weight = 0){}
Distance weight;
Distance walk;
char label = 'r';
std::vector<TimeEvent_<T> > TimeTable;


TspEdge& operator = ( const TspEdge& t)
{
  TimeTable = t.TimeTable;
  weight = t.weight;
  walk = t.walk;
  label = t.label;

   return *this;
}




};


template<class GraphType>
class TspNetwork
{

public:

  typedef typename GraphType::NodeIterator                        NodeIterator;
typedef typename GraphType::NodeDescriptor                      NodeDescriptor;
typedef typename GraphType::EdgeIterator                        EdgeIterator;
typedef typename GraphType::EdgeDescriptor                      EdgeDescriptor;
typedef typename GraphType::InEdgeIterator                      InEdgeIterator;
typedef typename GraphType::SizeType                            SizeType;
typedef GraphType                                               GraphImpl;

struct Station;
typedef typename std::vector<Station> StationContainer;

struct Vehicle;
typedef typename std::vector<Vehicle> VehicleContainer;

struct Route;
typedef typename std::vector<Route> RouteContainer;

struct Calendar;
typedef typename std::vector<Calendar> CalendarContainer;

struct CalendarDay;
typedef typename std::vector<CalendarDay> CalendarDayContainer;

struct Trip;
typedef typename std::vector<Trip> TripsContainer;

struct TransferPath;
typedef typename std::vector<TransferPath> TransferPathContainer;

struct Connection;
typedef typename std::vector<Connection> ConnectionContainer;

struct Date;



TspNetwork()
{};

~TspNetwork(){};

StationContainer stations;
VehicleContainer vehicles;
RouteContainer routes;
CalendarContainer calendar;

CalendarDayContainer calendarDay;
TripsContainer trips;
TransferPathContainer transferPaths;
ConnectionContainer connections;

GraphType G;

/* Adds a Station to the StationContainer */
Station& addStation(){
    stations.resize( stations.size()+1);
    return stations.back();
}

Station& getStop(int pos)
{
  return stations[pos];
}


Vehicle& addVehicle()
{
    vehicles.resize( vehicles.size()+1);
    return vehicles.back();
}

/* Adds a Route to the RouteContainer */
Route& addRoute(){
    routes.resize( routes.size()+1);
    return routes.back();
}

Calendar& addCalendar(){
    calendar.resize( calendar.size()+1);
    return calendar.back();
}

int getNumCalendars()
{
  return calendar.size();
}

Calendar& getCalendar(int pos)
{
  return calendar[pos];
}

CalendarDay& addCalendarDay(){
    calendarDay.resize( calendarDay.size()+1);
    return calendarDay.back();
}

int getNumCalendarDays()
{
  return calendarDay.size();
}

void addTrip(Trip t){
    trips.resize( trips.size()+1);
    trips.back() = t;
}

void addTransferPath(TransferPath t)
{
  transferPaths.resize( transferPaths.size()+1);
  transferPaths.back() = t;
}

void addConnection(Connection t){
    connections.resize( connections.size()+1);
    connections.back() = t;
}






struct Calendar
{
  Calendar():valid(false){
    for(int i = 0; i < 7; ++i) workingDays[i] = 0;
  }

  Calendar(int sy, int sm, int sd, int ey, int em, int ed, int firstDay)
  {
    startYear = sy;
    startMonth = sm;
    startDay = sd;
    endYear = ey;
    endMonth = em;
    endDay = ed;
    firstDayofTheWeek = firstDay;
    for(int i = 0; i < 7; ++i) workingDays[i] = 0;

  }

  int startYear;
  int startMonth;
  int startDay;
  int endYear;
  int endMonth;
  int endDay;
  int binDays;
  int workingDays[7];
  bool valid;
  int firstDayofTheWeek;

  void setBinDays()
  {
    char * Pend;
    std::string temp = std::to_string(workingDays[0])+std::to_string(workingDays[1])+std::to_string(workingDays[2])+std::to_string(workingDays[3])+std::to_string(workingDays[4])+std::to_string(workingDays[5])+std::to_string(workingDays[6]);
    binDays = strtol(temp.c_str(),&Pend,2);

    for(int i = 0; i < 7; ++i)
    {
      if(workingDays[i] == 1)
      {
        firstDayofTheWeek = i;
        break;
      }
    }

  }

};

struct CalendarDay
{
  int year;
  int month;
  int day;
  RouteID calendarId;
  bool isEnabled;


};

struct Trip
{
  Trip(){}

  Trip(int rId, int callId, int tpId, std::string hs, int tD)
  {
    routeId = rId;
    calendarId = callId;
    tripPathId = tpId;
    headsign = hs;
    tripDirection = tD;
  }

   int routeId;
   int calendarId;
   int tripPathId;
   std::string headsign;
   int tripDirection;
};

struct TransferPath
{
  TransferPath(){}

  TransferPath(StationID f, StationID t, Time m)
  {
    from = f;
    to = t;
    minTransferTime = m;
  }

  StationID from;
  StationID to;
  Time minTransferTime;

};

struct Connection
{
  Connection(){}

  Connection(StationID depStopId, StationID arrStopId, Time depT, Time arrT, TripID trId, int seq)
  {
    depStationId = depStopId;
    arrStationId = arrStopId;
    depTime = depT;
    arrTime = arrT;
    tripId = trId;
    seqId = seq;
  }

  StationID depStationId;
  StationID arrStationId;
  Time depTime;
  Time arrTime;
  TripID tripId;
  int seqId;
};

struct Date {

Date(int year, int month, int day)
{
  d = day;
  m = month;
  y = year;
}

int d, m, y;
};



    const int monthDays[12]
        = { 31, 28, 31, 30, 31, 30,
           31, 31, 30, 31, 30, 31 };

    // This function counts number of
    // leap years before the given date
    int countLeapYears(Date d)
    {
        int years = d.y;

        // Check if the current year needs to be
        //  considered for the count of leap years
        // or not
        if (d.m <= 2)
            years--;

        // An year is a leap year if it
        // is a multiple of 4,
        // multiple of 400 and not a
         // multiple of 100.
        return years / 4
               - years / 100
               + years / 400;
    }

    // This function returns number of
    // days between two given dates
    int getDifference(Date dt1, Date dt2)
    {
        // COUNT TOTAL NUMBER OF DAYS
        // BEFORE FIRST DATE 'dt1'

        // initialize count using years and day
        long int n1 = dt1.y * 365 + dt1.d;

        // Add days for months in given date
        for (int i = 0; i < dt1.m - 1; i++)
            n1 += monthDays[i];

        // Since every leap year is of 366 days,
        // Add a day for every leap year
        n1 += countLeapYears(dt1);

        // SIMILARLY, COUNT TOTAL NUMBER OF
        // DAYS BEFORE 'dt2'

        long int n2 = dt2.y * 365 + dt2.d;
        for (int i = 0; i < dt2.m - 1; i++)
            n2 += monthDays[i];
        n2 += countLeapYears(dt2);

        // return difference between two counts
        return (n2 - n1);
    }





/* Station Information */
struct Station
{

    Station():id(""),name("undefined"){};


    int RouteNodes = 0;
    Time minTransferTime = 0;
    double lat,lon;
    std::string id;    //stationId is the index for the StationContainer
    std::string name;
    std::vector<StationID> adjStations;
    GeoPoint2D cords;



    NodeDescriptor stNode;


    inline void addAdjacentStation( StationID id)
    {
        if( std::find( adjStations.begin(), adjStations.end(), id) == adjStations.end())
            adjStations.push_back( id);
    }

    const GeoPoint2D& getPoint()
    {
        return cords;
    }




};


//public transport vehicles
struct Vehicle
{
    Vehicle(): typeId(0), name("undefined")
    {}

    VehicleTypeID typeId;
    std::string name;

    inline bool operator==( const Vehicle& veh) const
    {
        if( typeId == veh.typeId)
            return true;
        else
            return false;
    }
};


/* Route Information */
struct Route{

    Route(): routeId("undefined"), rId(INF) {};

    std::string routeId;
    RouteID rId;

};





private:


};


#endif //TRANSPORT_NETWORK_Hconst
