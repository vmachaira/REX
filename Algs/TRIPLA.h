#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <Utilities/timer.h>
#include <Structs/Graphs/dynamicGraph.h>
#include <Structs/Graphs/adjacencyListImpl.h>
#include <Structs/Graphs/forwardStarImpl.h>
#include <iomanip>
#include <Structs/Graphs/dynamicGraph.h>
#include <queue>
#include <Structs/Arrays/nodeArray.h>
#include <Structs/Trees/priorityQueue.h>
#include <Structs/Graphs/nodeSelection.h>
#include <time.h>
#include <boost/foreach.hpp>
#include <Structs/Trees/seq/knheap.c>

// SECONDS
#define TIMEPERIOD 86400

template <typename TspNetwork>
class TRIPLA
{

public:

  typedef typename TspNetwork::GraphImpl                        GraphType;
  typedef typename TspNetwork::GraphImpl::SizeType              SizeType;

  typedef typename GraphType::NodeIterator                      NodeIterator;
  typedef typename GraphType::NodeDescriptor                    NodeDescriptor;
  typedef typename GraphType::EdgeIterator                      EdgeIterator;
  typedef typename GraphType::EdgeDescriptor                    EdgeDescriptor;
  typedef typename GraphType::InEdgeIterator                    InEdgeIterator;

  typedef typename TspNetwork::StationContainer                 StationContainer;
  typedef typename TspNetwork::Station                          Station;

  typedef typename TspNetwork::RouteContainer                   RouteContainer;
  typedef typename TspNetwork::Route                            Route;
  typedef TimeEvent_<NodeIterator>  TimeEvent;

  TRIPLA(TspNetwork& tspNetwork, TTL *timestamp, std::string filename): tspNet(tspNetwork), stations(tspNet.stations), G(tspNet.G),  m_timestamp(timestamp), pq(8556000, 0)
  {
    ReadHeuristics(filename);
    pq.reserve( stations.size());
  }

  bool runQuery(StationID depStation, StationID arrStation, Time depTime) noexcept
  {
//    sum = 0;

  //  TargetFound = false;



    // Αύξηση timestamp
    ++(*m_timestamp);

    // Settled nodes
    //snodes = 0;

    // Άδειασμα ουράς
    pq.clear();

    // Σταθμός άφιξης
    arrStat = arrStation;

    // Ώρα Άφιξης
    Time arrTime;

    // Κόμβος αναχώρισης
    depSt = G.getNodeIterator(stations[depStation].stNode);

    // Ώρα αναχώρισης
    Time t = depTime;

    // Βοηθητικές μεταβλητές
    NodeIterator u;
    Distance newDistance;


    // Μηδενισμός του χρόνου μετεπιβίβασης του σταθμού αφετηρίας
    depSt->IterDist = NO_INDEX;

    // Αρχικοποίηση της απόστασης του σταθμού αναχώρισης
    depSt->dist = depTime;
    depSt->timestamp = *m_timestamp;


    // Εισαγωγή του κόμβου αναχώρισης στην ουρά
    depSt->pred = G.nilNodeDescriptor();
    pq.insert( depTime, depSt);


    const int inf = INF;
    const Index no_index = NO_INDEX;



    // Όσο η ουρά δεν είναι άδεια
    while (!pq.empty())
    {


      // Αύξηση του αριθμού των κόμβων που έχουν προσπελαστεί ως τότε

      // Έξοδος του στοιχείου με την το μικρότερο κλειδί απο την ουρά
      u = getMinItem();
     pq.popMin();

  //   if(u->dist <=350) std::cout << u->stId << " has dist:" << u->dist << std::endl;


     if(u->Permant == *(m_timestamp)) continue;

  //   ++snodes;

     u->Permant = *(m_timestamp);

      // Αν βρεθεί ο κόμβος προορισμού βγες απο την ουρά
      if(u->stId == arrStation) {found = 1; arrSt = u; /*if(u->dist >(3*depSt->stLowBnds[arrStat]+depSt->dist)) std::cout << "wrong" << std::endl;*/ break;}

      EdgeIterator e;



      // Χαλάρωσε τις εξερχόμενες του u
      if(u->first == nullptr) e = G.beginEdges(u);
      else RelaxEdges(u, e);


      for(EdgeIterator lastEdge = G.endEdges(u); e != lastEdge; ++e)
      {
        NodeIterator v = G.target(e);
        Distance kk = u->dist+e->walk;

       UpdateDistanceWithQueue(u, v, kk, no_index);

      }


    } // end while



    return found;


  }


  inline void RelaxEdges(const NodeIterator& u, EdgeIterator& eM) noexcept
  {


     Distance const pureDay = u->getDistance()-u->getDistance()%TIMEPERIOD;
     Distance const DistanceInADay = u->getDistance()-pureDay;
     Distance const DistTran = u->getDistance()+u->minTransferTime;
     Distance const pureTranDay = DistTran-DistTran%TIMEPERIOD;
     Distance const DistTranInADay = DistTran-pureTranDay;

     unsigned int myday = (pureTranDay/TIMEPERIOD)%(timetable);


    Time *up ;



    switch (u->IterDist) {
      case NO_INDEX:
              {
                Time tempPair(u->getDistance()-pureDay-1);
              // Δυαδική αναζήτηση για τον υπολογισμό της επόμενης αναχώρισης
              up = std::upper_bound(u->first->Fbegin(), u->first->Fend(), tempPair); // -1 για να γίνει απο > σε >=
              break;}
      default:
          {up = u->first->Fbegin();
          std::advance(up, u->IterDist);}
    }

    EdgeIterator eI  = G.beginEdges(u);



    // Αν η εφικτή ώρα αναχώρησης είναι μετά το τέλος του πίνακα βάλτο στην αρχή
    if(up == u->first->Fend()) up = u->first->Fbegin();

    TimeEventContainer<NodeIterator>&  second = (u->IndexSecond(up-u->first->Fbegin()));


    unsigned short  const upsize = u->second->size();


    // Για κάθε δείκτη προς εξερχόμενη ακμή
    for(unsigned short int i = 0; i < upsize; ++i)
    {

      Distance minDist =INF;
      int indexDist;
      NodeIterator v = G.target(eI);


      Distance lowerWeight = u->dist+eI->weight;


      if(v->timestamp == *(m_timestamp))
      {
         minDist = v->dist;//-u->dist;
         indexDist  = v->IterDist;
      }

      if(eI->label == 'w' && minDist > u->dist + eI->walk)
      {
        minDist = u->dist + eI->walk;
        indexDist  = NO_INDEX;
        lowerWeight = minDist;
      }



      TimeEvent *eventPointer = second[i];
      TimeEvent *eventPointerStart =  second[i];



      if(eventPointer == nullptr)
      {
        eventPointer = &(eI->TimeTable[0]);
        eventPointerStart = &(eI->TimeTable[0]);

      }



    if(v->timestamp == *(m_timestamp) && (/*u->dist+eI->weight+v->stLowBnds[arrStat]>=TargetDist ||*/lowerWeight >= v->dist+v->minTransferTime))
    {

      ++eI;
      continue;
    }

    if(v->Permant == *(m_timestamp))
    {
      LabelCorrectingPhase(eventPointerStart, eventPointer, eI, v, u, DistTranInADay, pureTranDay, pureDay, DistanceInADay, myday);
      ++eI;
      continue;
    }




    while(1)
    {

      if(minDist != INF)
      {
        if((DistanceInADay <= eventPointer->getArrivalTime()))
        {

          if((*eventPointer).arrTime+pureDay> minDist+v->minTransferTime) break;
        }
        else if((*eventPointer).arrTime+TIMEPERIOD+pureDay > minDist+v->minTransferTime) break;
      }



      if(!eventPointer->isDiscovered(m_timestamp))
      {
        relaxUndiscoveredEvent(eventPointer, u, v, minDist, indexDist, DistanceInADay, pureDay, pureTranDay, DistTranInADay, myday);

      }
      else // Αν δεν έχει ανακαλυφθει αυτή η διαδρομή -> χρειάζεται χρόνος μετεπιβίβασης
      {
        relaxDiscoveredEvent(eventPointer, u, v, minDist, indexDist, DistanceInADay, pureTranDay, DistTranInADay, DistTran, myday);

      }



      if(eventPointer != &(eI->TimeTable.back())) ++eventPointer;
      else eventPointer = &(eI->TimeTable[0]);

      if(eventPointer == eventPointerStart) break;


    } // end while


    if(minDist != INF )
    {

      UpdateDistanceWithQueue(u, v, minDist, indexDist);


    }

    ++eI;

  }

  eM = eI;


}



inline void  UpdateDistanceWithQueue(const NodeIterator& u, NodeIterator& v, const Distance& newdist, const Index& indexDist) noexcept
{

  if(v->timestamp != *(m_timestamp))
  {

    v->dist = newdist;

    v->timestamp = *(m_timestamp);

    v->pred= G.getNodeDescriptor(u);

    v->IterDist = indexDist;


    pq.insert(  v->dist +stLowBnds(v->stId, arrStat), v);


  }
  else if(newdist < v->dist )
  {
    // Ενημέρωση απόστασης
    v->dist = newdist;

    v->pred = G.getNodeDescriptor(u);

    v->IterDist = indexDist;

    pq.insert( v->dist+stLowBnds(v->stId, arrStat), v);



  }


}

inline void MarkNext(TimeEvent*& event, const Distance& temp) noexcept
{
  if(event->next != nullptr)
  {
    event->next->timestamp = (*m_timestamp);
    event->next->validDeparture = temp;
  }
}


inline void PrintShortestPath()
{

  // Αν βρέθηκε εκτύπωση μηνυμάτων για έλεγχο
  if(found)
  {
    NodeIterator temp =  arrSt;

    std::cout << "-> Arrival ";

    std::cout <<"|Station:"<< stations[temp->stId].name <<"("<<temp->stId<<")"<<"|Time:"<< temp->dist << "("<< temp->dist%TIMEPERIOD<<")"<<"|\n"<<std::endl;

    std::cout << "continue?" << std::endl;
    char y;


    NodeIterator p;
std::cin >> y;
if(y == 'y')
{
    while(temp->pred != G.nilNodeDescriptor())
    {
      p = G.getNodeIterator(temp->pred);
      std::cout <<"\t     |Station:"<< stations[temp->stId].name <<"("<<temp->stId<<")"<<"|Time:"<< temp->dist<<"("<< temp->dist%TIMEPERIOD<<")"<<"|"<<std::endl;
      temp = p;
      std::cin >> y;

    }
  }
}
  std::cout << "\nSettled Nodes: " << snodes << std::endl;
  std::cout << "Travel Time: " << (arrSt->dist-depSt->dist) << " mins" <<std::endl;



}

const NodeIterator& getMinItem() const{ return pq.minItem(); }




inline void ReadHeuristics(std::string filename) noexcept
{

  std::ifstream in;
  std::string line;
  in.open(filename.c_str());

std::cout << filename << std::endl;


  if( !in)
  {
    std::cerr << "\nError opening " << filename << std::endl;
    exit(-1);
  }


  // Αρχικοποίηση μεταβλητών
  std::string name, temp;

  StationID station1, station2;
  Distance time;


  numStations = stations.size();
  stLowBndsContainer.resize( numStations*numStations);

std::vector< std::vector<Distance> > tempContainer;
tempContainer.resize( numStations);


  for(unsigned short int i = 0; i< numStations; i++) tempContainer[i].resize( numStations);

  ProgressStream stLowBnds_progress( numStations*numStations);
  stLowBnds_progress.label() << "Reading ALT heuristics for " << numStations << " stations";





  int counter=0;
  // Διάβασμα του υπόλοιπου αρχείου ανα γραμμή
  while(std::getline(in, line))
  {

    std::stringstream data(line);

    data >> station1 >> station2 >> time;

    tempContainer[station1][station2] = time;

    counter++;
    ++stLowBnds_progress;

  }

counter = 0;
  for(int j = 0; j < numStations; ++j)
  {
    for(int i = 0; i < numStations; ++i)
    {
      stLowBndsContainer[j*numStations+i] = (tempContainer[i][j]);
      ++counter;
    }

  }


  // Κλείσιμο αρχείου
  in.close();


}


inline bool relaxDiscoveredEventChain(TimeEvent*& eventPointer,  const NodeIterator &u,  const NodeIterator &v, const Distance &vMinDist, Distance& tempDist, const Distance& DistanceInADay, const Distance& pureTranDay, const Distance& DistTranInADay, const unsigned int& myday) noexcept
{
  if(eventPointer->validDeparture <= u->getDistance()+u->minTransferTime)
  {
    Distance op = eventPointer->validDeparture-eventPointer->validDeparture%TIMEPERIOD + eventPointer->depTime;
    tempDist = op + eventPointer->getLength();

    if(tempDist < u->dist) tempDist+=86400;

  }
  else
  {
    //  unsigned int myday = (pureTranDay/TIMEPERIOD)%(timetable+1); //πχ TIMEPERIOD = 1
      unsigned int currday = (eventPointer->sDays  << (myday+firstday)) & 0b1111111; // 0011000 -> 1000000 αν firstday=2 και myday=1

      if(DistTranInADay <= eventPointer->getDepartureTime() && currday >= 64)
      {
         tempDist = pureTranDay+eventPointer->arrTime;
         if(tempDist < u->dist) tempDist+=86400;


      }
      else // Aν δεν έχει σήμερα δρομολόγιο ή εχει αλλά δεν το προλαβαίνω
      {
          Distance extra = (((currday << 1)&0b1111111)  == 0)?((timetable-myday+6-(int)log2((eventPointer->sDays << firstday)&0b1111111))*TIMEPERIOD):((1+6-(int)log2((currday << 1)&0b1111111))*TIMEPERIOD); //1100000

          tempDist = pureTranDay+eventPointer->arrTime+extra;
          if(tempDist < u->dist) tempDist+=86400;


      }
  }

  if( eventPointer->next != nullptr && tempDist >= eventPointer->next->validDeparture) return false;



  //Μαρκάρισμα του επόμενου ταξιδιού
  MarkNext(eventPointer, tempDist);

  // Ενημέρωση απόστασης αν απατείται
  return (tempDist < vMinDist+v->minTransferTime);
}

inline void relaxDiscoveredEvent(TimeEvent*& eventPointer, const NodeIterator &u, const NodeIterator &v,  Distance &minDist, int& indexDist, const Distance& DistanceInADay, const Distance& pureTranDay, const Distance& DistTranInADay, const Distance& DistTran, const unsigned int& myday) noexcept
{

  if(eventPointer->validDeparture <= DistTran)
  {
     Distance op = eventPointer->validDeparture-eventPointer->validDeparture%TIMEPERIOD + eventPointer->depTime;
     Distance tempDist = op + eventPointer->getLength();

     if(tempDist < u->dist) tempDist+=86400;


      //Μαρκάρισμα του επόμενου ταξιδιού
      MarkNext(eventPointer, tempDist);

      // Ενημέρωση απόστασης αν απατείται
      if(tempDist < minDist)
      {
        minDist = tempDist;
        indexDist = eventPointer->nextIndex;
      }
  }
  else
  {
  //  unsigned int myday = (pureTranDay/TIMEPERIOD)%(timetable+1); //πχ TIMEPERIOD = 1
    unsigned int currday = (eventPointer->sDays  << (myday+firstday)) & 0b1111111; // 0011000 -> 1000000 αν firstday=2 και myday=1

    if(DistTranInADay <= eventPointer->getDepartureTime() && currday >= 64)
    {
      Distance tempDist = pureTranDay+eventPointer->arrTime;
      if(tempDist < u->dist) tempDist+=86400;


      //Μαρκάρισμα του επόμενου ταξιδιού
      MarkNext(eventPointer, tempDist);

      // Ενημέρωση απόστασης αν απατείται
      if(tempDist < minDist)
      {
        minDist = tempDist;
        indexDist = eventPointer->nextIndex;
      }

    }
    else // Aν δεν έχει σήμερα δρομολόγιο ή εχει αλλά δεν το προλαβαίνω
    {
        Distance extra = (((currday << 1) & 0b1111111)  == 0)?((timetable-myday+6-(int)log2((eventPointer->sDays << firstday) & 0b1111111))*TIMEPERIOD):((1+6-(int)log2((currday << 1)&0b1111111))*TIMEPERIOD); //1100000

        Distance tempDist = pureTranDay+eventPointer->arrTime+extra;
        if(tempDist < u->dist) tempDist+=86400;


        //Μαρκάρισμα του επόμενου ταξιδιού
        MarkNext(eventPointer, tempDist);

        // Ενημέρωση απόστασης αν απατείται
        if(tempDist < minDist)
        {
          minDist = tempDist;
          indexDist = eventPointer->nextIndex;
        }

    }

  }

}


inline void relaxUndiscoveredEvent(TimeEvent*& eventPointer, const NodeIterator &u,  const NodeIterator &v,  Distance &minDist, int& indexDist, const Distance& DistanceInADay, const Distance& pureDay, const Distance& pureTranDay, const Distance& DistTranInADay, const unsigned int& myday) noexcept
{

    unsigned int currday = (eventPointer->sDays  << (myday+firstday)) & 0b1111111; // 0011000 -> 1000000 αν firstday=2 και myday=1

//std::cout << "sDays:" << eventPointer->sDays <<  " pureDay: " << pureDay << " myday: "<< myday << " currday: " << currday << " DistanceInADay: " << DistTranInADay <<std::endl;
//int k; std::cin >> k;
    // Aν έχει σήμερα δρομολόγιο και το προλαβαίνω
    if(DistTranInADay<= eventPointer->getDepartureTime() && currday >= 64)
    {
      //Distance clearDist = pureDay + eventPointer->arrTime;
      Distance tempDist = pureTranDay+eventPointer->arrTime;
      if(tempDist < u->dist) tempDist+=86400;

//      std::cout << "PROLAVA udist: "<< u->dist << " depTime: " << eventPointer->depTime << " arrTime: "<< eventPointer->arrTime << " tempDist:" << tempDist << std::endl;
  //    std::cin >> k;
      //Μαρκάρισμα του επόμενου ταξιδιού
      MarkNext(eventPointer, tempDist);

      // Ενημέρωση απόστασης αν απατείται
      if(tempDist < minDist)
      {
        minDist = tempDist;
        indexDist = eventPointer->nextIndex;
      }

      eventPointer->validDeparture = pureTranDay+eventPointer->depTime;

      //if(clearDist < tempDist && clearDist<minDist+v->minTransferTime)  checkEdgeToRevisit(eventPointer, u);



    }
    else // Aν δεν έχει σήμερα δρομολόγιο ή εχει αλλά δεν το προλαβαίνω
    {
        Distance extra = (((currday << 1) & 0b1111111)  == 0)?((timetable-myday+6-(int)log2((eventPointer->sDays << firstday)&0b1111111))*TIMEPERIOD):((1+6-(int)log2((currday << 1)&0b1111111))*TIMEPERIOD); //1100000

        //Distance clearDist = pureDay + eventPointer->arrTime+extra;
        Distance tempDist = pureTranDay+eventPointer->arrTime+extra;
        if(tempDist < u->dist) tempDist+=86400;

//std::cout << "/* message */" << (((currday << 1) & 0b1111111)  == 0) << '\n';
  //      std::cout << "OXI udist: "<< u->dist << " depTime: " << eventPointer->depTime << " arrTime: "<< eventPointer->arrTime << " tempDist:" << tempDist << " extra: "<< extra << " pureTranDay: "<< pureTranDay << " log2((currday << 1): "<< (1+6-(int)log2((currday << 1)&0b1111111)) <<std::endl;
    //    std::cin >> k;

        //Μαρκάρισμα του επόμενου ταξιδιού
        MarkNext(eventPointer, tempDist);

        // Ενημέρωση απόστασης αν απατείται
        if(tempDist < minDist)
        {
          minDist = tempDist;
          indexDist = eventPointer->nextIndex;
        }

        eventPointer->validDeparture = pureTranDay+eventPointer->depTime+extra;

        //if(clearDist < tempDist && clearDist<minDist+v->minTransferTime)  checkEdgeToRevisit(eventPointer, u);


    }

}

inline bool relaxUndiscoveredEventChain(TimeEvent*& eventPointer,  const NodeIterator &u, const NodeIterator &v,  const Distance &vMinDist, Distance& tempDist, const Distance& DistanceInADay, const Distance& pureDay, const Distance& pureTranDay, const Distance& DistTranInADay, const unsigned int& myday) noexcept
{

  unsigned int currday = (eventPointer->sDays  << (myday+firstday)) & 0b1111111; // 0011000 -> 1000000 αν firstday=2 και myday=1

  // Aν έχει σήμερα δρομολόγιο και το προλαβαίνω
  if(DistTranInADay<= eventPointer->getDepartureTime() && currday >= 64)
  {
    //Distance clearDist = pureDay + eventPointer->arrTime;
    Distance tempDist = pureTranDay+eventPointer->arrTime;
    if(tempDist < u->dist) tempDist+=86400;


    eventPointer->validDeparture = pureTranDay+eventPointer->depTime;

  //  if(clearDist < tempDist && clearDist < vMinDist+v->minTransferTime) checkEdgeToRevisit(eventPointer, u);

  if(eventPointer->next != nullptr && tempDist >= eventPointer->next->validDeparture) return false;

    //Μαρκάρισμα του επόμενου ταξιδιού
    MarkNext(eventPointer, tempDist);

    // Ενημέρωση απόστασης αν απατείται
    return (tempDist < vMinDist+v->minTransferTime);


  }
  else // Aν δεν έχει σήμερα δρομολόγιο ή εχει αλλά δεν το προλαβαίνω
  {
      Distance extra = (((currday << 1)&0b1111111)  == 0)?((timetable-myday+6-(int)log2((eventPointer->sDays << firstday)&0b1111111))*TIMEPERIOD):((1+6-(int)log2((currday << 1)&0b1111111))*TIMEPERIOD); //1100000

      //Distance clearDist = pureDay + eventPointer->arrTime+extra;
      Distance tempDist = pureTranDay+eventPointer->arrTime+extra;
      if(tempDist < u->dist) tempDist+=86400;


      eventPointer->validDeparture = pureTranDay+eventPointer->depTime+extra;
      //if(clearDist < tempDist && clearDist < vMinDist+v->minTransferTime) checkEdgeToRevisit(eventPointer, u);

      if(eventPointer->next != nullptr && tempDist >= eventPointer->next->validDeparture) return false;

      //Μαρκάρισμα του επόμενου ταξιδιού
      MarkNext(eventPointer, tempDist);

      // Ενημέρωση απόστασης αν απατείται
      return (tempDist < vMinDist+v->minTransferTime);

  }

}

inline void LabelCorrectingPhase(TimeEvent*& eventPointerStart, TimeEvent*& eventPointer, EdgeIterator& eI, NodeIterator& v, NodeIterator u, const Distance& DistTranInADay, const Distance& pureTranDay, const Distance& pureDay, const Distance& DistanceInADay, const unsigned int& myday) noexcept
{

  while(1)
  {

    if((DistanceInADay <= eventPointer->getArrivalTime()))
    {
      if((*eventPointer).arrTime+pureDay> v->dist+v->minTransferTime) return;
    }
    else if((*eventPointer).arrTime+TIMEPERIOD+pureDay > v->dist+v->minTransferTime) return;



    if(eventPointer->next == nullptr)
    {

          if(eventPointer != &(eI->TimeTable.back())) ++eventPointer;
          else eventPointer = &(eI->TimeTable[0]);

          if(eventPointer == eventPointerStart) return;
    }
    else if (eventPointer->isDiscovered(m_timestamp))
    {

      NodeIterator mynode = v;
      NodeIterator myprevnode = u;
      TimeEvent *myevent = eventPointer;


      bool continueRelax=true;

      Distance vMinDist;
      Distance tempDist=INF;


      while(continueRelax)
      {

        vMinDist = mynode->dist;
        continueRelax = relaxDiscoveredEventChain( myevent, myprevnode, mynode, vMinDist, tempDist, DistanceInADay, pureTranDay, DistTranInADay, myday);

        if(mynode->Permant != (*m_timestamp)) break;

        if(!continueRelax) break;
        else if(myevent->next != nullptr)
        {
          myprevnode = mynode;
          mynode = myevent->next->dest;
          myevent = myevent->next;
        }
        else continueRelax = false;
      }


      if(mynode->Permant != (*m_timestamp))  UpdateDistanceWithQueue(myprevnode, mynode, tempDist, myevent->nextIndex);

    }
    else
    {

      NodeIterator mynode = v;
      NodeIterator myprevnode = u;
      TimeEvent *myevent = eventPointer;

      bool continueRelax=true;

      Distance vMinDist;
      Distance tempDist;

      bool myflag = true;


      while(continueRelax)
      {
        vMinDist = mynode->dist;

        if(myflag) continueRelax = relaxUndiscoveredEventChain( myevent, myprevnode, mynode, vMinDist, tempDist, DistanceInADay, pureDay, pureTranDay, DistTranInADay, myday);
        else continueRelax = relaxDiscoveredEventChain( myevent, myprevnode, mynode, vMinDist, tempDist, DistanceInADay, pureTranDay, DistTranInADay, myday);

        myflag = false;


        if(mynode->Permant != (*m_timestamp)) break;

        if(!continueRelax) break;
        else if(myevent->next != nullptr)
        {


          myprevnode = mynode;
          mynode = myevent->next->dest;

          myevent = myevent->next;

        }
        else continueRelax = false;



      }

      if(mynode->Permant != (*m_timestamp)) UpdateDistanceWithQueue(myprevnode, mynode, tempDist, myevent->nextIndex);

    }


    if(eventPointer != &(eI->TimeTable.back())) ++eventPointer;
    else eventPointer = &(eI->TimeTable[0]);


    if(eventPointer == eventPointerStart) return;



  } // end while





}

inline Distance& stLowBnds(const int& i, const int& j)
{
  return stLowBndsContainer[j*(numStations)+i];
}

inline unsigned int msbBitValue(unsigned int n) {
   unsigned int k = (unsigned int)(log2(n));
   return (unsigned int)(pow(2, k));
}

inline float returnSum()
{
//  return sum;
}
public:
  unsigned short int snodes;

protected:
  TspNetwork& tspNet;
  StationContainer& stations;
  GraphType& G;


  NodeIterator arrSt, depSt;
  bool found = 0;
  TTL *m_timestamp;
  StationID arrStat;
  typedef KNHeap<Distance, NodeIterator> PriorityQueueType;
  PriorityQueueType pq;
  std::vector<Distance> stLowBndsContainer;
  int numStations;
  unsigned int firstday = 0; unsigned int timetable = 7;

};
