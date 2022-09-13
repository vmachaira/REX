#ifndef GTFS2_READER_H
#define GTFS2_READER_H
#include <time.h>
#include "../Structs/definitions.h"
#include "netBuilder.h"
#include "macros.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

//STL header files
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

//PGL  header files
#include <Utilities/geographic.h>

#define PI 3.14159265358979323846
#define DIA 1
#define TIMEPERIOD 86400
#define SECONDS 60
#define DAYS 7


template <typename TspNetwork, typename InTspNetwork>
class GTFS2Reader : public TDBuilder<TspNetwork, InTspNetwork>
{

public:

  typedef typename InTspNetwork::GraphImpl                         InGraphType;
  typedef typename InTspNetwork::GraphImpl::SizeType              InSizeType;

  typedef typename InGraphType::NodeIterator                      NodeIterator;
  typedef typename InGraphType::NodeDescriptor                    NodeDescriptor;
  typedef typename InGraphType::EdgeIterator                      EdgeIterator;
  typedef typename InGraphType::EdgeDescriptor                    EdgeDescriptor;
  typedef typename InGraphType::InEdgeIterator                    InEdgeIterator;

  typedef typename InTspNetwork::StationContainer                 StationContainer;
  typedef typename InTspNetwork::Station                          Station;

  typedef typename InTspNetwork::RouteContainer                   RouteContainer;
  typedef typename InTspNetwork::Route                            Route;

  typedef typename InTspNetwork::Calendar                         Calendar;
  typedef typename InTspNetwork::CalendarDay                      CalendarDay;

  typedef typename InTspNetwork::VehicleContainer                 VehicleContainer;
  typedef typename InTspNetwork::Vehicle                          Vehicle;
  typedef typename InTspNetwork::Trip                             Trip;
  typedef typename InTspNetwork::TransferPath                     TransferPath;
  typedef typename InTspNetwork::Connection                       Connection;
  typedef typename InTspNetwork::ConnectionContainer              ConnectionContainer;
  typedef typename InTspNetwork::Date                             Date;



  typedef TimeEvent_<NodeIterator> TimeEvent;

struct eventSave
{
  eventSave(StationID d, StationID a, TimeEvent e)
  {
    event = e;
    depStation = d;
    arrStation = a;
  }

  TimeEvent event;
  StationID depStation;
  StationID arrStation;
};

struct TestDay
{
  TestDay(int myear, int mmonth, int mday, int mweekday, int mweekdaydec, std::string mdate, std::string mweekbin)
  {
    year = myear;
    month = mmonth;
    weekday = mweekday;
    weekdaydec = mweekdaydec;
    date = mdate;
    weekbin = mweekbin;
  }

  TestDay()
  {

  }

  int year;
  int month;
  int day;
  int weekday;
  int weekdaydec;
  std::string date;
  std::string weekbin;
};



  GTFS2Reader( TspNetwork& tspNetwork, InTspNetwork& intspNetwork, const std::string& routesFilename, const std::string& tripsFilename, const std::string& stopFilename,
    const std::string& stopTimesFilename, const std::string& transfersFilename, const std::string& SDaysFilename, const std::string& footpathsFilename, const std::string& cdfilename, bool walk, std::string netname) :
    TDBuilder<TspNetwork, InTspNetwork>( tspNetwork, intspNetwork),
    m_routesFilename( routesFilename), m_tripsFilename( tripsFilename),
    m_stopsFilename(stopFilename), m_stopTimesFilename( stopTimesFilename), m_transfersFilename( transfersFilename),  m_calendarFilename(SDaysFilename), m_footpaths(footpathsFilename), m_calendarDatesFilename(cdfilename), walkEnabled(walk), netName(netname)
    {

      std::cout << "Walk file: " << m_footpaths << std::endl;

      if(netName == "Paris")
      {
        netCalendar = Calendar(2018, 3, 26, 2018, 3, 28, 0);
        netCalendar.workingDays[0] = 1;
        netCalendar.workingDays[1] = 1;
        netCalendar.workingDays[2] = 1;
        netCalendar.workingDays[3] = 1;
        netCalendar.workingDays[4] = 1;
        netCalendar.workingDays[5] = 1;
        netCalendar.workingDays[6] = 1;



        netCalendar.firstDayofTheWeek = 0;
        netCalendar.setBinDays();

       }
       else if(netName == "Rome")
       {
         netCalendar = Calendar(2020, 11, 2, 2020, 11, 8, 0);
         netCalendar.workingDays[0] = 1;
         netCalendar.workingDays[1] = 1;
           netCalendar.workingDays[2] = 1;
           netCalendar.workingDays[3] = 1;
           netCalendar.workingDays[4] = 1;
           netCalendar.workingDays[5] = 1;
           netCalendar.workingDays[6] = 1;


         netCalendar.firstDayofTheWeek = 0;
         netCalendar.setBinDays();

       }
       else if(netName == "Berlin")
       {
         netCalendar = Calendar(2020, 11, 2, 2020, 11, 8, 0);
         netCalendar.workingDays[0] = 1;
         netCalendar.workingDays[1] = 1;
         netCalendar.workingDays[2] = 1;
           netCalendar.workingDays[3] = 1;
           netCalendar.workingDays[4] = 1;
           netCalendar.workingDays[5] = 1;
           netCalendar.workingDays[6] = 1;

         netCalendar.firstDayofTheWeek = 0;
         netCalendar.setBinDays();
       }
       else if(netName == "London")
       {
         netCalendar = Calendar(2015, 11, 2, 2015, 11, 8, 0);
         netCalendar.workingDays[0] = 1;
         netCalendar.workingDays[1] = 1;
         netCalendar.workingDays[2] = 1;
         netCalendar.workingDays[3] = 1;
         netCalendar.workingDays[4] = 1;
         netCalendar.workingDays[5] = 1;
         netCalendar.workingDays[6] = 1;

         netCalendar.firstDayofTheWeek = 0;
         netCalendar.setBinDays();

       }
       else if(netName == "Athens")
       {
         netCalendar = Calendar(2017, 2, 6, 2017, 2, 12, 0);
         netCalendar.workingDays[0] = 1;
         netCalendar.workingDays[1] = 1;
         netCalendar.workingDays[2] = 1;
         netCalendar.workingDays[3] = 1;
         netCalendar.workingDays[4] = 1;
         netCalendar.workingDays[5] = 1;
         netCalendar.workingDays[6] = 1;

         netCalendar.firstDayofTheWeek = 0;
         netCalendar.setBinDays();

       }
       else if(netName == "Switzerland")
       {
         netCalendar = Calendar(2017, 5, 29, 2017, 6, 4, 0);
         netCalendar.workingDays[0] = 1;
         netCalendar.workingDays[1] = 1;
         netCalendar.workingDays[2] = 1;
         netCalendar.workingDays[3] = 1;
         netCalendar.workingDays[4] = 1;
         netCalendar.workingDays[5] = 1;
         netCalendar.workingDays[6] = 1;

         netCalendar.firstDayofTheWeek = 0;
         netCalendar.setBinDays();


       }






    }



      // Διαβάζει το GTFS και δημιουργεί το δίκτυο
      void read()
      {
        readStops();
        readRoutes();
        readCalendar();
        readCalendarDates();
      //  readShapes();
        readTrips();
        readTransfers();

        readConnections();

        makeTimetable();
    //    importVehicles();


        if(walkEnabled) importWalkingPaths();
        buildNetwork();

        // Εκτύπωση στατιστικών
        std::cout << "done!"
        << "\n\tstations:" << stations.size() << " connections:" << numConnections
        << "\n\ttransfers:" << numTransfers << " edges:" << G.getNumEdges()
        << "\n\tnodes:" << G.getNumNodes() << std::endl;


      }


      void clear()
      {
        intspNet.clear();
        stopIndex.clear();
        routeIndex.clear();
        tripIndex.clear();
        calendarIndex.clear();
        tripPathIndex.clear();

      }

    private:

      std::string m_routesFilename;
      std::string m_tripsFilename;
      std::string m_stopsFilename;
      std::string m_stopTimesFilename;
      std::string m_transfersFilename;
      std::string m_calendarDatesFilename;
      std::string m_footpaths;
      std::string m_calendarFilename;
      std::map<std::string,Time> tranedges;
      std::string netName;

      int NullId = -1;

      bool walkEnabled;
      std::vector<TestDay> testday;

      Calendar netCalendar;


      //map: external (GTFS) stop ids -> internal stop ids
      std::map<std::string, StationID> stopIndex;
      //map: external (GTFS) route ids -> internal route ids
      std::map<std::string, RouteID> routeIndex;
      //map: external (GTFS) trip ids -> internal trip ids
      std::map<std::string, RouteID> tripIndex;
      //map: external (GTFS) calendar ids -> internal trip ids
      std::map<std::string, RouteID> calendarIndex;
      //map: external (GTFS) shape ids -> internal trip ids
      std::map<std::string, RouteID> tripPathIndex;

      std::map<std::string, RouteID> unknownCalendarIndex;



      //route type id (global) to route type id (local)
      std::map<VehicleTypeID, VehicleTypeID> vehTypes;
      //trip id to its correspoding route id
      std::map<std::string, RouteID> trip2route;
      std::map<std::string, TripID> trip2trip;
      //route id to its correspoding vehicle type id (global)
      std::map<RouteID, VehicleTypeID> route2vehicle;

      std::map<std::string, std::string> servId2days;
      std::map<std::string, std::string> trip2sdays;

      // Εισαγωγή σταθμών
      void readStops()
      {
        std::ifstream in;
        std::string line;
        std::vector<std::string> fields;
        int numStopDuplicates = 0;

        //open the "stops.csv" file
        //ILOG("[*] Reading stop data. (input: %s)\n", m_stopsFilename.c_str());
        in.open( m_stopsFilename.c_str());
        if( in.is_open() == false)
        {
            //DLOG("[-] Error opening the file. Aborting...\n");
            exit(-1);
        }

        CounterStream read_stops;
        read_stops.label() << "\tReading stops";

        //set read exceptions
        in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            //read file's column labels (first line)
            std::getline( in, line);
            fields = explode( line);

            std::size_t stopIdIndex, nameIndex, latIndex, lonIndex;
            std::string stop_id = "stop_id";
            std::string stop_name = "stop_name";
            std::string stop_lat = "stop_lat";
            std::string stop_lon = "stop_lon";

            findField( fields, stop_id, stopIdIndex);
            findField( fields, stop_name, nameIndex);
            findField( fields, stop_lat, latIndex);
            findField( fields, stop_lon, lonIndex);


            //stop id in [0,numStops-1]
            StationID stopId = 0;
            std::string stopName, stopKey;
            double lon, lat;

            std::string coordKey;
            std::map<std::string, StationID> coord2Key;

            //read stops
            while( std::getline( in, line))
            {
                fields = explode( line);

                stopKey = fields[stopIdIndex];
                stopName = fields[nameIndex];
                lat = str2Float(fields[latIndex]);
                lon = str2Float(fields[lonIndex]);

                //check if there is already inserted stop with the same coords
                #if defined(REMOVE_STOP_DUPLICATES)
                coordKey = /*stopName +*/ fields[latIndex] + fields[lonIndex];
                if( coord2Key.find( coordKey) != coord2Key.end())
                {
                    stopIndex[stopKey] = coord2Key[coordKey];
                    numStopDuplicates++;
                    ++read_stops;
                    continue;
                }
                else
                    coord2Key[coordKey] = stopId;
                #endif

                std::replace( stopName.begin(), stopName.end(), ',', ' ');
                stopIndex[stopKey] = stopId;

                // Δημιουργία σταθμού
                Station& station = intspNet.addStation();
                station.id = stopKey;
                station.lon = lon;
                station.lat = lat;
                station.cords = GeoPoint2D(lat, lon);
                station.name = stopName;
                station.minTransferTime = 0;

                ++stopId;
                ++read_stops;

            //    VERBOSE("[*] Added stop (stop_id:" << stopKey << ", stop_name:" << stopName << ", stop_lat:" << lat << ", stop_lon:" << lon << ")\n");
            }
        }

        catch( std::ifstream::failure e)
        {
            if( in.eof() == false)
            {
              ////
            }
        }

        in.close();
        read_stops.stop();

      }

      void readRoutes()
      {
          std::ifstream in;
          std::string line;
          std::vector<std::string> fields;

          ILOG("[*] Reading route data. (input: %s)\n", m_routesFilename.c_str());
          in.open( m_routesFilename.c_str());
          if( in.is_open() == false)
          {
              DLOG("[-] Error opening the file. Aborting...\n");
              return;
          }

          CounterStream read_routes;
          read_routes.label() << "\tReading routes";

          //set read exceptions
          in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
          try
          {
              //read file's column labels (first line)
              std::getline( in, line);
              fields = explode( line);

              std::size_t routeIdIndex, routeLongNameIndex, routeTypeIndex;
              std::string route_id = "route_id";
              std::string route_short_name = "route_short_name";
              std::string route_type = "route_type";

              findField( fields, route_id, routeIdIndex);
              findField( fields, route_short_name, routeLongNameIndex);
              findField( fields, route_type, routeTypeIndex);

              ILOG("[*] Filtering fields: (%s, %s, %s)\n", route_id.c_str(), route_short_name.c_str(), route_type.c_str());

              //route id in [0,numRoutes-1]
              RouteID routeId = 0;
              VehicleTypeID tspModeID;
              std::string routeKey, routeName;

              //map GTFS route id -> GTFS vehicle type id
              while( std::getline( in, line))
              {
                  //skip when line is empty
                  if( line == "") continue;

                  fields = explode( line);
                  fields.push_back("");

                  routeKey = fields[routeIdIndex];
                  routeName = fields[routeLongNameIndex];
                  tspModeID = str2Int(fields[routeTypeIndex]);

                  std::replace( routeName.begin(), routeName.end(), ',', ' ');
                  routeIndex[routeKey] = routeId;

                  Route& r = intspNet.addRoute();
                  r.routeId = routeKey;
                  r.rId = routeId;

                  Vehicle& vehicle = intspNet.addVehicle();

                  vehicle.typeId = tspModeID;



                  ++routeId;
                  ++read_routes;
                  VERBOSE("[*] Added route (route_id:" << routeKey << ", route_long_name:" << routeName << ", route_type:" << tspModeID << ")\n");
              }
          }

          catch( std::ifstream::failure e)
          {
              if( in.eof() == false)
              {
                  ILOG("[-] Error reading the file. Aborting...\n");
              }
          }

          in.close();
          read_routes.stop();

          oILOG("[+] Added " << intspNet.getNumRoutes() << " routes with " << intspNet.getNumTspModes() << " different transport modes:\n");
      }


      /**
       * @brief Reads trip data.
       **/
      void readTrips()
      {
          std::ifstream in;
          std::string line;
          std::vector<std::string> fields;

          ILOG("[*] Reading trip data. (input: %s)\n", m_tripsFilename.c_str());
          in.open( m_tripsFilename.c_str());
          if( in.is_open() == false)
          {
              DLOG("[-] Error opening the file. Aborting...\n");
              return;
          }

          CounterStream read_trips;
          read_trips.label() << "\tReading trips";

          //set read exceptions
          in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
          try
          {
              std::getline( in, line);
              fields = explode( line);

              std::size_t routeIdIndex, serviceIdIndex, tripIdIndex, headsignIndex, tripDirectionIndex, shapeIdIndex;
              std::string route_id = "route_id";
              std::string service_id = "service_id";
              std::string trip_id = "trip_id";
              std::string trip_headsign = "trip_headsign";
              std::string direction_id = "direction_id";
              std::string shape_id = "shape_id";

              findField( fields, route_id, routeIdIndex);
              findField( fields, service_id, serviceIdIndex);
              findField( fields, trip_id, tripIdIndex);
              findField( fields, trip_headsign, headsignIndex, true);
              findField( fields, direction_id, tripDirectionIndex, true);
              findField( fields, shape_id, shapeIdIndex, true);

              ILOG("[*] Filtering fields: (%s, %s, %s, %s, %s, %s)\n",
              route_id.c_str(), service_id.c_str(), trip_id.c_str(),
              trip_headsign.c_str(), direction_id.c_str(), shape_id.c_str());

              //trip id in [0,numTrips-1]
              TripID tripId = 0;
              std::string routeKey;
              std::string calendarKey;
              std::string tripKey;
              std::string tripPathKey;
              std::string headsign;
              int tripDirection;

              //read the trips
              while( std::getline( in, line))
              {
                  fields = explode( line);
                  routeKey = fields[routeIdIndex];
                  tripKey = fields[tripIdIndex];

                  if( tripIndex.find(tripKey) != tripIndex.end())
                  {
                      oILOG("[!] Warning trip_id:(" << tripKey << ") is already registered - duplicate keys.\n");
                  }

                  if( serviceIdIndex < fields.size())
                      calendarKey = fields[serviceIdIndex];
                  else
                      calendarKey = "";

                  if( shapeIdIndex < fields.size())
                      tripPathKey = fields[shapeIdIndex];
                  else
                      tripPathKey = "";


                  if( headsignIndex < fields.size())
                      headsign = fields[headsignIndex];
                  else
                      headsign = "";

                  if( tripDirectionIndex < fields.size())
                      tripDirection = str2Int(fields[tripDirectionIndex]);
                  else
                      tripDirection = 0;

                  if( routeIndex.find(routeKey) == routeIndex.end())
                      oILOG("\n[!] Warning route_id:(" << routeKey << ") is not registered.\n");

                  if( calendarIndex.find(calendarKey) == calendarIndex.end())
                  {
                      if( unknownCalendarIndex.find(calendarKey) == unknownCalendarIndex.end())
                      {
                          unknownCalendarIndex[calendarKey] = NullId;
                          oILOG("\n[!] Warning service_id:(" << calendarKey << ") is not registered.\n");
                      }

                      calendarKey = "";
                  }

                  if( tripPathIndex.find(tripPathKey) == tripPathIndex.end())
                  {
                      VERBOSE("\n[!] Warning shape_id:(" << tripPathKey << ") is not registered.\n");
                  }
                  else if( tripPathIndex[tripPathKey] == NullId)
                  {
                      VERBOSE("\n[!] Warning trip has NULL shape_id .\n");
                  }

                  std::replace( headsign.begin(), headsign.end(), ',', ' ');
                  intspNet.addTrip( Trip(routeIndex[routeKey], calendarIndex[calendarKey], tripPathIndex[tripPathKey], headsign, tripDirection));
                  tripIndex[tripKey] = tripId;

                  ++tripId;
                  ++read_trips;

                  VERBOSE("[*] Added trip (route_id:" << routeKey << ", service_id:" << calendarKey << ", trip_id:" << tripKey << ", trip_headsign:" << headsign
                                                      << ", direction:" << tripDirection << ", shape_id:" << tripPathKey << ")\n");
              }
          }

          catch( std::ifstream::failure e)
          {
              if( in.eof() == false)
              {
                  ILOG("[-] Error reading the file. Aborting...\n");
              }
          }

          in.close();
          read_trips.stop();
          ILOG("[+] Network info (%lu trips).\n", tripIndex.size());
      }

      void readConnections()
      {
          std::ifstream in;
          std::string line;
          std::vector<std::string> fields;

          std::map<int, std::vector<Connection> > trip2Connections;
          StationID depStopId, arrStopId;
          Time depTime, arrTime;
          int hr, min, sec;
          TripID tripId;
          int seqId;
          std::string stopKey;
          std::string tripKey="";

          ILOG("[*] Reading stop times data. (input: %s)\n", m_stopTimesFilename.c_str());
          in.open( m_stopTimesFilename.c_str());


          if( in.is_open() == false)
          {
              DLOG("[-] Error opening the file. Aborting...\n");
              return;
          }

          CounterStream read_stopTimes;
          read_stopTimes.label() << "\tReading stop times";

          //set read exceptions
          in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
          try
          {

              std::getline( in, line);
              fields = explode( line);

              std::size_t tripIdIndex, arrTimeIndex, depTimeIndex, stopIdIndex, stopSeqIdIndex;
              std::string trip_id = "trip_id";
              std::string arrival_time = "arrival_time";
              std::string departure_time = "departure_time";
              std::string stop_id = "stop_id";
              std::string stop_sequence_id = "stop_sequence";


              findField( fields, trip_id, tripIdIndex);

              findField( fields, arrival_time, arrTimeIndex);
              findField( fields, departure_time, depTimeIndex);
              findField( fields, stop_id, stopIdIndex);
              findField( fields, stop_sequence_id, stopSeqIdIndex);



              ILOG("[*] Filtering fields: (%s, %s, %s, %s, %s)\n", trip_id.c_str(), arrival_time.c_str(), departure_time.c_str(), stop_id.c_str(), stop_sequence_id.c_str());
              //read connections
              while( std::getline( in, line))
              {
                  fields = explode( line);

                  tripKey = fields[tripIdIndex];
                  stopKey = fields[stopIdIndex];
                  seqId   = str2Int(fields[stopSeqIdIndex]);
                  arrTime = time2secs( fields[arrTimeIndex]);
                  depTime = time2secs( fields[depTimeIndex]);


                  if( stopIndex.find(stopKey) == stopIndex.end())
                  {
                      oILOG("[!] Warning stop_id:(" << stopKey << ") is not registered.\n");
                      continue;
                  }

                  if( tripIndex.find(tripKey) == tripIndex.end())
                  {
                      oILOG("[!] Warning trip_id:(" << tripKey << ") is not registered.\n");
                      continue;
                  }

                  tripId = tripIndex[tripKey];
                  arrStopId = stopIndex[stopKey];



                  trip2Connections[tripId].emplace_back( arrStopId, arrStopId, depTime, arrTime, tripId, seqId);

                  ++read_stopTimes;

                  VERBOSE("[*] Added stop time (trip_id:" << currTripKey
                          << ", arrival_time:" << arrTime << ", departure_time:" << depTime
                          << ", stop_id:" << stopKey << ")\n");
              }
          }

          catch( std::ifstream::failure e)
          {

              if( in.eof() == false)
              {
                  ILOG("[-] Error reading the file. Aborting...\n");
                  return;
              }
          }

          in.close();
          read_stopTimes.stop();
          int numReplacedTimes = 0, numZeroTrvTimes = 0;
          int maxTrvTime = 0, minTrvTime = std::numeric_limits<int>::max();
          int numSelfLoopConnections = 0;
          int numSequenceGaps = 0;

          oILOG("[*] Sorting and inserting connections...\n")

          for( auto it = trip2Connections.begin(); it != trip2Connections.end(); it++)
          {
              tripId = it->first;
              std::vector<Connection>& tripConnections = it->second;
              std::sort( tripConnections.begin(), tripConnections.end(),
                         []( const Connection& a, const Connection& b) -> bool
                         {
                              return a.seqId < b.seqId;
                         });

              double speed, speedSum=0;
              int numSpeeds=0;
              for( int i=0, endTrip=tripConnections.size()-1; i<endTrip; i++)
              {
                  depStopId=tripConnections[i].depStationId;
                  arrStopId=tripConnections[i+1].arrStationId;
                  depTime=tripConnections[i].depTime;
                  arrTime=tripConnections[i+1].arrTime;
                  if( arrTime >= depTime)
                  {
                      double length = GeoPoint2D::distance( intspNet.getStop( depStopId).getPoint(), intspNet.getStop( arrStopId).getPoint());
                      double trvTime = (arrTime-depTime);
                      speed = (length / trvTime);
                      if( 2 < speed && speed < 55)
                      {
                          speedSum += speed;
                          numSpeeds++;
                      }
                  }
              }

              if( numSpeeds != 0)
                  speed = speedSum/numSpeeds;
              else
                  speed = 8;

              for( int i=0, seqNo=0, endTrip=tripConnections.size()-1; i<endTrip; i++)
              {
                  depStopId=tripConnections[i].depStationId;
                  arrStopId=tripConnections[i+1].arrStationId;
                  depTime=tripConnections[i].depTime;
                  arrTime=tripConnections[i+1].arrTime;
                  //tripId=tripConnections[i].tripId;
                  seqId=tripConnections[i].seqId;

                  #define FIX_DEPARTURE_ARRIVAL
                  #if defined(FIX_DEPARTURE_ARRIVAL)
                  if( arrTime <= depTime)
                  {
                      arrTime = depTime;

                      double length = GeoPoint2D::distance( intspNet.getStop( depStopId).getPoint(), intspNet.getStop( arrStopId).getPoint());
                      double trvTime = std::ceil(length/speed);
                      if( trvTime == 0){ trvTime = 1;}
                      arrTime = depTime + trvTime;


                      numReplacedTimes++;

                  }

                  if( tripConnections[i+1].depTime < arrTime)
                  {
                      tripConnections[i+1].depTime = arrTime;
                      numReplacedTimes++;
                  }
                  #endif


                  if( depStopId == arrStopId)
                  {
                      numSelfLoopConnections++;
                      continue;
                  }

                  if( (seqId+1) != tripConnections[i+1].seqId)
                  {
                      numSequenceGaps++;

                  }


                  if( arrTime == depTime)
                      numZeroTrvTimes++;

                  int trvTime =  arrTime - depTime;
                  if( trvTime > maxTrvTime)
                      maxTrvTime = trvTime;
                  else if( trvTime < minTrvTime)
                      minTrvTime = trvTime;

                  intspNet.addConnection( Connection(depStopId, arrStopId, depTime, arrTime, tripId, seqNo));
                  seqNo++;
              }
          }

          if( numReplacedTimes > 0)
          {
              oILOG("[!] Warning data inconsistency, number of arrival_time <= departure_time cases: " << numReplacedTimes << "\n")
          }

          if( numZeroTrvTimes > 0)
          {
              oILOG("[!] Warning data inconsistency, number of zero travel times: " << numZeroTrvTimes << "\n")
          }

          if( numSelfLoopConnections > 0)
          {
              oILOG("[!] Warning data inconsistency, number of self-loop connections: " << numSelfLoopConnections << "\n")
          }

          if( numSequenceGaps > 0)
          {
              oILOG("[!] Warning data inconsistency, number of sequence gaps: " << numSequenceGaps << "\n")
          }


          oILOG("[*] Weight range [" << minTrvTime << ", " << maxTrvTime << "]\n");

          oILOG("[+] Added " << timetable.getNumConnections() << " connections.\n");
      }


      void readCalendar()
      {
        std::ifstream in;

        ILOG("[*] Reading calendar data. (input: %s)\n", m_calendarFilename.c_str());
        in.open( m_calendarFilename.c_str());
        if( in.is_open() == false)
        {
            DLOG("[-] Error opening the file. Aborting...\n");
            return;
        }

        //set read exceptions
        in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            std::string line;
            std::vector<std::string> fields;

            std::getline( in, line);
            fields = explode( line);

            std::size_t serviceIdIndex, firstDayIndex, lastDayIndex, startDateIndex, endDateIndex;
            std::string service_id = "service_id";
            std::string monday = "monday";
            std::string sunday = "sunday";
            std::string start_date = "start_date";
            std::string end_date = "end_date";

            findField( fields, service_id, serviceIdIndex);
            findField( fields, monday, firstDayIndex);
            findField( fields, sunday, lastDayIndex);
            findField( fields, start_date, startDateIndex);
            findField( fields, end_date, endDateIndex);

            ILOG("[*] Filtering fields: (%s, %s - %s, %s, %s)\n", service_id.c_str(), monday.c_str(), sunday.c_str(), start_date.c_str(), end_date.c_str());

            //calendar id in [0,numCalendars-1]
            int calendarId = 0;
            std::string calendarKey;
            std::string startDate, endDate;

            //add default calendar
            intspNet.addCalendar();
            calendarIndex[""] = calendarId;
            ++calendarId;

            //read calendar (depicting the days of week a trip is operational) per service id
            while( std::getline( in, line))
            {
                fields = explode( line);
                calendarKey = fields[serviceIdIndex];

                if( calendarKey != "")
                {
                    Calendar& calendar = intspNet.addCalendar();
                    for( std::size_t i=0; i<7; i++)
                        calendar.workingDays[i] = str2Int(fields[firstDayIndex+i]);

                    startDate = fields[startDateIndex];
                    endDate = fields[endDateIndex];
                    calendar.startYear  =  str2Int( startDate.substr(0,4));
                    calendar.startMonth =  str2Int( startDate.substr(4,2));
                    calendar.startDay   =  str2Int( startDate.substr(6,2));
                    calendar.endYear    =  str2Int( endDate.substr(0,4));
                    calendar.endMonth   =  str2Int( endDate.substr(4,2));
                    calendar.endDay     =  str2Int( endDate.substr(6,2));
                    calendar.setBinDays();

                    calendarIndex[calendarKey] = calendarId;
                    ++calendarId;

                    VERBOSE("[+] Added calendar (service_id:" << calendarKey << ", [sun,sat,..,tue,mon]-bitmask:" << calendar.workingDays.to_string() << ").\n");
                }

                else
                {
                    VERBOSE("[-] Skipped null-key calendar.\n");
                }

            }
        }

        catch( std::ifstream::failure e)
        {
            if( in.eof() == false)
            {
                ILOG("[-] Error reading the file. Aborting...\n");
            }
        }

        in.close();

        if( intspNet.getNumCalendars() == 1)
        {
            oILOG("[*] No input calendar. Added default calendar.\n");
            Calendar& calendar = intspNet.getCalendar(0);
        //    calendar.workingDays.set();
            calendarIndex[""] = 0;
        }
        else
        {
            Calendar& calendar = intspNet.getCalendar(0);
            calendarIndex[""] = 0;
        }


        oILOG("[+] Network info (" << calendarIndex.size() << " trip-calendars).\n");



      }



      void readCalendarDates()
      {
        std::ifstream in;

        ILOG("[*] Reading calendar data. (input: %s)\n", m_calendarDatesFilename.c_str());
        in.open( m_calendarDatesFilename.c_str());
        if( in.is_open() == false)
        {
            DLOG("[-] Error opening the file. Aborting...\n");
            return;
        }

        CounterStream read_calendarDates;
        read_calendarDates.label() << "\tReading calendar dates";

        //set read exceptions
        in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            std::string line;
            std::vector<std::string> fields;

            std::getline( in, line);
            fields = explode( line);

            std::size_t serviceIdIndex, dateIndex, exceptionTypeIndex;
            std::string service_id = "service_id";
            std::string date = "date";
            std::string exception_type = "exception_type";

            findField( fields, service_id, serviceIdIndex);
            findField( fields, date, dateIndex);
            findField( fields, exception_type, exceptionTypeIndex);

            ILOG("[*] Filtering fields: (%s, %s, %s)\n", service_id.c_str(), date.c_str(), exception_type.c_str());

            //calendar id in [0,numCalendars-1]
            int calendarId = intspNet.getNumCalendars();
            std::string calendarKey, dateKey;
            int exceptionType;

            //add default calendar
            if( calendarId == 0)
            {
                intspNet.addCalendar();
                calendarIndex[""] = calendarId;

                ++calendarId;
            }
            int mycount =0;

            //read calendar (depicting the days of week a trip is operational) per service id
            while( std::getline( in, line))
            {
                fields = explode( line);
                calendarKey = fields[serviceIdIndex];

                if( calendarKey != "")
                {
                    CalendarDay& calendarDay = intspNet.addCalendarDay();
                    if( calendarIndex.find(calendarKey) == calendarIndex.end())
                    {
                        dateKey = fields[dateIndex];

                        Calendar& calendar = intspNet.addCalendar();
                        calendarIndex[calendarKey] = calendarId;

                        for( std::size_t i=0; i<7; i++)
                            calendar.workingDays[i] = 0;

                        calendar.startYear  = str2Int( dateKey.substr(0,4));
                        calendar.startMonth = str2Int( dateKey.substr(4,2));
                        calendar.startDay   = str2Int( dateKey.substr(6,2));
                        calendar.endYear    = str2Int( dateKey.substr(0,4));
                        calendar.endMonth   = str2Int( dateKey.substr(4,2));
                        calendar.endDay     = str2Int( dateKey.substr(6,2));
                        calendar.setBinDays();
                        ++calendarId;


                    }

                    Calendar& calendar = intspNet.getCalendar(calendarIndex[calendarKey]);


                    calendarDay.calendarId = calendarIndex[calendarKey];

                    dateKey = fields[dateIndex];
                    calendarDay.year  =  str2Int( dateKey.substr(0,4));
                    calendarDay.month =  str2Int( dateKey.substr(4,2));
                    calendarDay.day   =  str2Int( dateKey.substr(6,2));



                    Date d1(calendarDay.year, calendarDay.month, calendarDay.day);
                    Date d2(calendar.startYear, calendar.startMonth, calendar.startDay);
                    Date d3(calendar.endYear, calendar.endMonth, calendar.endDay);

                    if(intspNet.getDifference(d1, d2) > 0)
                    {
                      calendar.startYear  = str2Int( dateKey.substr(0,4));
                      calendar.startMonth = str2Int( dateKey.substr(4,2));
                      calendar.startDay   = str2Int( dateKey.substr(6,2));
                    }
                    if(intspNet.getDifference(d3, d1) > 0)
                    {
                      calendar.endYear    = str2Int( dateKey.substr(0,4));
                      calendar.endMonth   = str2Int( dateKey.substr(4,2));
                      calendar.endDay     = str2Int( dateKey.substr(6,2));
                    }







                    exceptionType = str2Int( fields[exceptionTypeIndex]);

                    if( exceptionType == 1)
                        calendarDay.isEnabled = true;
                    else
                        calendarDay.isEnabled = false;


                    VERBOSE("[+] Added calendar day (service_id:" << calendarKey << "@" << dateKey << ").\n");
                }

                else
                {
                    VERBOSE("[-] Skipped null-key calendar day.\n");
                }

                ++read_calendarDates;
            }
        }

        catch( std::ifstream::failure e)
        {
            if( in.eof() == false)
            {
                ILOG("[-] Error reading the file. Aborting...\n");
            }
        }

          in.close();
          read_calendarDates.stop();
          oILOG("[+] Network info (" << intspNet.getNumCalendarDays() << " calendar-special-days).\n");
          oILOG("[+] Network info (" << calendarIndex.size() << " trip-calendars).\n");
      }



      TimeEvent makeTimeEvent(Time depTime, Time arrTime, VehicleID vehicleId,  TripID currTripId, RouteID currRouteId, VehicleTypeID vehicleType, StationID arrStationId, int binDays)
      {

        TimeEvent tEv;
        tEv.depTime = depTime;
        tEv.length = (arrTime - depTime); // Κυκλική διαφορά πρέπει!!!!
        tEv.sDays = binDays;
        tEv.sDays = tEv.sDays & netCalendar.binDays;
        tEv.arrTime = arrTime;
        tEv.trId = currTripId;


        return tEv;

      }


double distance( const Station &p1, const Station& p2)
{
    const double& lat1 = p1.lat;
    const double& lon1 = p1.lon;
    const double& lat2 = p2.lat;
    const double& lon2 = p2.lon;

    double dlat1 = lat1*(PI/180.0);
    double dlon1 = lon1*(PI/180.0);
    double dlat2 = lat2*(PI/180.0);
    double dlon2 = lon2*(PI/180.0);

    double dlon = dlon1-dlon2;
    double dlat = dlat1-dlat2;

    double aHarv = pow(sin(dlat/2.0),2.0)+cos(dlat1)*cos(dlat2)*pow(sin(dlon/2.0),2.0);
    double cHarv = 2.0*atan2(sqrt(aHarv),sqrt(1.0-aHarv));

    //earth's radius from wikipedia varies between 6,356.750 km — 6,378.135 km (˜3,949.901 — 3,963.189 miles)
    //The IUGG value for the equatorial radius of the Earth is 6378.137 km (3963.19 mile)
    double distance = cHarv*6378137.0;

    return distance;
}


      void importWalkingPaths()
      {
        std::ifstream in;
        std::string line;
        in.open(m_footpaths.c_str());

        if( !in)
        {
          std::cerr << "\nError opening walk" << m_stopsFilename << std::endl;
          exit(-1);
        }


        // Αρχικοποίηση μεταβλητών
        std::string name, temp;

        StationID station1, station2;
        std::string stop1, stop2;

        Time time;

        // Διάβασμα της πρώτης γραμμής του αρχείου (Κατηγορίες του GTFS)


        // Διάβασμα του υπόλοιπου αρχείου ανα γραμμή
        while( std::getline(in, line))
        {

          std::stringstream data(line);

  //std::cout << line << std::endl;
          data >> stop1 >> stop2 >> time;

          station1 = stopIndex[stop1];
          station2 = stopIndex[stop2];

          time = time/DIA;

          if(station1 == station2) continue;

          std::string tran = std::to_string(station1)+std::to_string(station2);

          WalkData temp(station1, station2, time);


          typename std::vector<WalkData>::iterator it;
          it = std::find(WalkPaths.begin(), WalkPaths.end(), temp);

          if(it != WalkPaths.end())
          {
            if(it->time > time) it->time = time;
            continue;
          }

          WalkPaths.push_back(WalkData(station1, station2, time));

        }

        // Κλείσιμο αρχείου
        in.close();


      }


      void readTransfers()
      {
          //if( useRealisticTransfers == false) return;

          std::ifstream in;
          std::string line;
          std::vector<std::string> fields;

          //open the "transfers.csv" file
          ILOG("[*] Reading transfers data. (input: %s)\n", m_transfersFilename.c_str());
          in.open( m_transfersFilename.c_str());
          if( in.is_open() == false)
          {
              ILOG("[-] Error opening the file. Aborting...\n");
          //    useRealisticTransfers = false;
              return;
          }

          CounterStream read_transferTimes;
          read_transferTimes.label() << "\tReading transfer times";

          //set read exceptions
          in.exceptions( std::ifstream::failbit | std::ifstream::badbit);
          try
          {
              std::getline( in, line);
              fields = explode( line);

              std::string from_stop_id = "from_stop_id";
              std::string to_stop_id = "to_stop_id";
              std::string transfer_type = "transfer_type";
              std::string min_transfer_time = "min_transfer_time";

              std::size_t fromStopIdIndex, toStopIdIndex, tsfTypeIndex, tsfTimeIndex;
              findField( fields, from_stop_id, fromStopIdIndex);
              findField( fields, to_stop_id, toStopIdIndex);
              findField( fields, transfer_type, tsfTypeIndex, true);
              findField( fields, min_transfer_time, tsfTimeIndex);

              ILOG("[*] Filtering fields: (%s, %s, %s, %s)\n", from_stop_id.c_str(), to_stop_id.c_str(), transfer_type.c_str(), min_transfer_time.c_str());

              std::string fromStopKey, toStopKey;
              StopID stopId;
              Time minTransferTime;
              int transferType;

              //read stop transfer times
              while( std::getline( in, line))
              {
                  fields = explode( line);
                  fields.push_back("");

                  fromStopKey = fields[fromStopIdIndex];
                  toStopKey = fields[toStopIdIndex];

                  if( tsfTypeIndex < fields.size())
                      transferType = str2Int(fields[tsfTypeIndex]);
                  else
                      transferType = 2;

                  minTransferTime = str2Int(fields[tsfTimeIndex]);//in secs

                  if( stopIndex.find(fromStopKey) == stopIndex.end())
                      oILOG("[!] Warning from_stop_id:(" << fromStopKey << ") is not registered.\n");
                  if( stopIndex.find(toStopKey) == stopIndex.end())
                      oILOG("[!] Warning to_stop_id:(" << toStopKey << ") is not registered.\n");

                  if( transferType == 2)
                  {
                      if( fromStopKey == toStopKey)
                      {
                          stopId = stopIndex[fromStopKey];
                          Station& stop = intspNet.stations[stopId];
                          stop.minTransferTime = std::max( stop.minTransferTime, minTransferTime);
                      }
                      else
                          intspNet.addTransferPath( TransferPath( stopIndex[fromStopKey], stopIndex[toStopKey], minTransferTime));
                  }

                  ++read_transferTimes;

                  //TODO readed vs added
                  VERBOSE("[*] Added transfer (from_stop_id:" << fromStopKey << ", to_stop_id:" << toStopKey
                                                              << ", transfer_type:" << transferType << ", min_transfer_time:" << minTransferTime << ")\n");
              }
          }

          catch( std::ifstream::failure e)
          {
              if( in.eof() == false)
              {
                  ILOG("[-] Error reading the file. Aborting...\n");
              }
          }

          in.close();
          read_transferTimes.stop();
          oILOG("[+] Added " << timetable.getNumTransferPaths() << " transfer paths.\n");
      }





      void importVehicles()
      {
        std::vector<std::string> category;
        std::map<std::string, std::string> values;


        std::ifstream in;
        std::string line, name, temp;

        int counter;

        in.open( m_routesFilename.c_str());
        std::cout << "\nReading vehicle types from " << m_routesFilename << " ... " << std::flush;
        if( !in)
        {
          std::cerr << "\nError opening " << m_routesFilename << std::endl;
          exit(-1);
        }

        VehicleTypeID vehicleTypeId;
        std::string routeId;

        // Διάβασμα της πρώτης γραμμής του αρχείου (Κατηγορίες του GTFS)
        std::getline( in, line);
        std::stringstream data1(line);

        // Αποθήκευση του ονόματος κάθε κατηγορίας στον vector category
        while(std::getline( data1, temp, ',')) category.push_back(temp);

        // Διάβασμα του υπόλοιπου αρχείου ανα γραμμή
        while( std::getline(in, line))
        {
          counter = 0;

line.erase(remove(line.begin(), line.end(), '"'), line.end());

          std::stringstream data(line);

          // Εκχώρισε κάθε τιμή ��ου GTFS στο map values με κλειδί την κατηγορία που ανήκει
          while(std::getline(data, temp, ','))
          {
            // Έλεγχος αν το , χωριζεί μια τίμή απο κάποια άλλη ή χρησιμοποιείται ως χαρακτήρας
            if( temp[0] == '"' && *(temp.end()-1) != '"')
            {
              name = temp.substr(1);
              std::getline( data, temp, '"');
              name += ","+temp;
              std::getline( data, temp, ',');
              temp = name;
            }

            values[category.at(counter)] = temp;
            counter++;

          }

          vehicleTypeId = atoi( values["route_type"].c_str());
          routeId = values["route_id"].c_str();

          route2vehicle[routeIndex[routeId]] = vehicleTypeId;
          vehTypes[vehicleTypeId];

        }


        // Κλείσιμο αρχείου
        in.close();

        //vehicle type names (basic route types)
        std::map<VehicleTypeID, std::string> vehicleName;
        vehicleName[0] = std::string("light rail");
        vehicleName[1] = std::string("subway");
        vehicleName[2] = std::string("rail");
        vehicleName[3] = std::string("bus");
        vehicleName[4] = std::string("ferry");
        vehicleName[5] = std::string("cable car");
        vehicleName[6] = std::string("gondola");
        vehicleName[7] = std::string("funicular");

        //vehicle type names (extented route types)
        vehicleName[1000] = std::string("water Transport");
        vehicleName[900] = std::string("tram");
        vehicleName[800] = std::string("trolleybus");
        vehicleName[700] = std::string("bus");
        vehicleName[600] = std::string("underground");
        vehicleName[500] = std::string("metro");
        vehicleName[400] = std::string("urban railway");
        vehicleName[300] = std::string("suburban railway");
        vehicleName[200] = std::string("coach");
        vehicleName[100] = std::string("railway");
        vehicleName[109] = std::string("suburban railway");

        //create vehicle types
        unsigned int i = 0;
        std::map<VehicleTypeID, VehicleTypeID>::iterator it;
        for( it = vehTypes.begin(); it != vehTypes.end(); ++it)
        {
          Vehicle& vehicle = intspNet.addVehicle();
          vehicle.name = vehicleName[it->first];
          vehTypes[it->first] = i;
          vehicle.typeId = it->first;
          i++;
        }

        std::cout << "done!"<<std::endl;

        std::cout << "\nVehicle types:\n";
        for(int i = 0; i < intspNet.vehicles.size(); i++)
        std::cout << "TypeID:" << intspNet.vehicles[i].typeId  << " Name:" << intspNet.vehicles[i].name << "\n";

      }

      void findField( std::vector<std::string>& fields, std::string& fieldName, std::size_t& fieldIndex, bool isOptional=false)
      {
          fieldIndex = std::distance( fields.begin(), std::find( fields.begin(), fields.end(), fieldName));

          if( fieldIndex == fields.size())
          {
              if( isOptional == true)
              {
                  fieldName = fieldName + ":N/A";
                  fieldIndex = std::numeric_limits<std::size_t>::max();
              }
              else
              {
                  oDLOG("[-] Data inconsistency. " << fieldName <<" is not found. Exiting...\n");
                  exit(-1);
              }
          }
          else
              fieldName = fieldName + ":" + std::to_string( fieldIndex);
      }



      /**
       * @brief Returns a vector of string-tokens, each of which is
       * formed by splitting the input string on boundaries formed by the delimiter.
       * @param str The input string.
       * @param firstDelim The first delimiter-character.
       * @param secondDelim The second delimiter-character.
       * @return A vector separating each string token by the specified delimiter.
       **/
      std::vector<std::string> explode( std::string& str, const char firstDelim='"', const char secondDelim=',')
      {
          if( str.back() == '\r')
              str.pop_back();
          if( str[0] == '\r')
              str.erase(0, 1);

          std::vector<std::string> tokens;
          std::istringstream iss(str);

          for( std::string token; iss.rdbuf()->in_avail();)
          {
              if( iss.peek() == firstDelim)
              {
                   iss.ignore(1);
                   std::getline(iss, token, firstDelim);
                   iss.ignore(1);
              }
              else
                   std::getline(iss, token, secondDelim);

              tokens.push_back(std::move(token));
          }

          //special case: at the end of line there is a bad (non-printable) character
          if( !tokens.back().empty() && !isprint((tokens.back()).back()))
              tokens.back().pop_back();

          //special case: lastToken,[eof] VS lastToken[eof]
          if( !iss.eof())
              tokens.push_back(std::string(""));

         return tokens;
      }


      double str2Float( const std::string& str)
      {
          return atof( str.c_str());
      }

      int str2Int( const std::string& str)
      {
          return atoi( str.c_str());
      }

      int time2secs( std::string& timeStr)
      {
          std::vector<std::string> timeFields = explode( timeStr, ':', ':');
          int hr = str2Int(timeFields[0]);
          int min = str2Int(timeFields[1]);
          int sec = str2Int(timeFields[2]);
          return ( hr * 3600 + min * 60 + sec);
      }

      void makeTimetable()
      {
        validateCalendarDay();
        validateTimePeriod();


        for(int i = 0; i < intspNet.connections.size(); ++i)
        {
          Connection& conn = intspNet.connections[i];

          Trip& trip = intspNet.trips[conn.tripId];

          Calendar& cal = intspNet.calendar[trip.calendarId];

          Calendar cal2 = cal;
          int days=DAYS-1;
          if(conn.depTime >= 86400)
          {
            if(cal.workingDays[days] == 1)
            {
              cal2.workingDays[days] = 0;
            }

            for(int j = days-2; j >= 0; --j)
            {
              if(cal.workingDays[j] == 1)
              {
                cal2.workingDays[j] = 0;
                cal2.workingDays[j+1] = 1;
              }

            }

            if(days == 6 && cal.workingDays[6] == 1)
            {
              cal2.workingDays[0] = 1;
            }
            cal2.setBinDays();

          }

          conn.depTime = conn.depTime%86400;
          conn.arrTime = conn.arrTime%86400;
          if(conn.arrTime < conn.depTime) conn.arrTime+=86400;


          if(!cal.valid) continue;

          TimeEvent tEv = makeTimeEvent(conn.depTime, conn.arrTime, 0, conn.tripId, 0, 0, conn.arrStationId, cal2.binDays);
         if((tEv.sDays & netCalendar.binDays) == 0) continue;

          Station& depStation = intspNet.stations[conn.depStationId];
          depStation.addAdjacentStation( conn.arrStationId);

          addEvent(conn.depStationId, conn.arrStationId, tEv);
          numConnections++;

        }

      }

      void validateTimePeriod()
      {
        //int netCalstart = netCalendar.startYear*10000+netCalendar.startMonth*100+netCalendar.startDay;
        //int netCalend = netCalendar.endYear*10000+netCalendar.endMonth*100+netCalendar.endDay;

        Date netCalstart(netCalendar.startYear, netCalendar.startMonth, netCalendar.startDay);
        Date netCalend(netCalendar.endYear, netCalendar.endMonth, netCalendar.endDay);


            for(int i = 1; i < intspNet.calendar.size(); ++i)
            {
              Calendar& cal = intspNet.calendar[i];

              Date calstart(cal.startYear, cal.startMonth, cal.startDay);
              Date calend(cal.endYear, cal.endMonth, cal.endDay);

              if(( intspNet.getDifference(netCalstart, calend) >= 0 && intspNet.getDifference(calend, netCalend) >= 0) ||
                 (intspNet.getDifference(netCalstart, calstart) >= 0 && intspNet.getDifference(calstart, netCalend) >= 0 ) ||
                 (intspNet.getDifference(calstart, netCalstart) >= 0 && intspNet.getDifference(netCalstart, calend) >= 0 )
                 )
                 {
                   cal.valid = true;
                 }
                 else cal.valid = false;

             }
      }

      void validateCalendarDay()
      {

        Date netCalstart(netCalendar.startYear, netCalendar.startMonth, netCalendar.startDay);
        Date netCalend(netCalendar.endYear, netCalendar.endMonth, netCalendar.endDay);


        for(int i = 0 ; i < intspNet.calendarDay.size(); ++i)
        {
          CalendarDay& cal = intspNet.calendarDay[i];
          Date calday(cal.year, cal.month, cal.day);

          if(intspNet.getDifference(netCalstart, calday) >= 0 && intspNet.getDifference(calday, netCalend) >=0)
          {
            int offset = intspNet.getDifference(netCalstart, calday)%7;
            Calendar& calendar = intspNet.calendar[cal.calendarId];
            if(cal.isEnabled)
            {
               calendar.workingDays[(offset+netCalendar.firstDayofTheWeek)%7] = 1;
               calendar.setBinDays();

            }
            else
            {
              calendar.workingDays[(offset+netCalendar.firstDayofTheWeek)%7] = 0;
              calendar.setBinDays();

            }

          }

        }

      }




    protected:


      typedef TDBuilder<TspNetwork, InTspNetwork> Base;
      // Στατιστικά
      using Base::numRoute2Route;
      using Base::numRoute2Central;
      using Base::numCentral2Route;


      using Base::G;

      using Base::intspNet;

      using Base::stations;

      using Base::routes;

      using Base::buildNetwork;
      using Base::numConnections;
      using Base::numTransfers;

      using Base::numRouteNodes;
      using Base::numEdges;

      using Base::addEvent;

      using typename Base::EdgeData;


      using typename Base::EdgeList;
      using Base::WalkPaths;

      using typename Base::WalkData;

      void AddToTimetable(EdgeData e, TimeEvent tEv)
      {

        // Εκχώριση TimeEvent στον πίνακα δρομολογίων της ακμής
        e.TimeTable.push_back(tEv);

        // Ταξινόμιση του πίνακα δρομολογίων
        std::sort(e.TimeTable.begin(), e.TimeTable.end(), Compare<NodeIterator>());

        if(e.minLength > tEv.length ) e.minLength = tEv.length;

      }


    };







    #endif
