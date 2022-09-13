#ifndef TIME_DEPENDENT_DEFS
#define TIME_DEPENDENT_DEFS

#include <climits>

typedef unsigned  int StationID;
typedef unsigned int TripID;
typedef unsigned int StopID;
typedef unsigned int RouteID;

typedef unsigned int NodeID;
typedef unsigned int VehicleID;
typedef unsigned int TrainID;
typedef unsigned int BusID;
typedef unsigned int EventID;
typedef int Distance;
typedef int Time;
typedef unsigned int PQRange;
typedef unsigned int TTL;

typedef int VehicleTypeID; 

const Distance INF = std::numeric_limits<Distance>::max();

#endif // TIME_DEPENDENT_DEFINITIONS_H
