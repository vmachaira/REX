
#include <typeinfo>
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <map>

#include <Utilities/progressBar.h>
#include "../Structs/definitions.h"
#include "../Algs/scc.h"

#include <Algorithms/basicGraphAlgorithms.h>

#include <queue>

#include <Structs/Graphs/adjacencyListImpl.h>
#include <Structs/Graphs/adjacencyVectorImpl.h>





template <typename TspNetwork, typename InTspNetwork>
class TDBuilder
{

 public:

    typedef typename InTspNetwork::GraphImpl                        InGraphType;

    typedef typename InTspNetwork::GraphImpl::SizeType              InSizeType;
    typedef typename TspNetwork::GraphImpl                        GraphType;

    typedef typename TspNetwork::GraphImpl::SizeType              SizeType;


    typedef typename GraphType::NodeIterator                      tNodeIterator;
    typedef typename GraphType::NodeDescriptor                    tNodeDescriptor;
    typedef typename GraphType::EdgeIterator                      tEdgeIterator;
    typedef typename GraphType::EdgeDescriptor                    tEdgeDescriptor;
    typedef typename GraphType::InEdgeIterator                    tInEdgeIterator;

    typedef typename InGraphType::NodeIterator                    NodeIterator;
    typedef typename InGraphType::NodeDescriptor                  NodeDescriptor;
    typedef typename InGraphType::EdgeIterator                    EdgeIterator;
    typedef typename InGraphType::EdgeDescriptor                  EdgeDescriptor;
    typedef typename InGraphType::InEdgeIterator                  InEdgeIterator;


    typedef typename InTspNetwork::StationContainer                 StationContainer;
    typedef typename InTspNetwork::Station                          Station;
    typedef typename TspNetwork::StationContainer                 tStationContainer;
    typedef typename TspNetwork::Station                          tStation;
    typedef typename InTspNetwork::RouteContainer                   RouteContainer;
    typedef typename InTspNetwork::Route                            Route;
    //typedef test<NodeIterator> etst;

    typedef TimeEvent_<NodeIterator> TimeEvent;
    typedef TimeEvent_<tNodeIterator> tTimeEvent;





    TDBuilder( TspNetwork& tspNetwork, InTspNetwork& intspNetwork ) :
    tspNet(tspNetwork),intspNet(intspNetwork), stations(intspNet.stations), routes(intspNet.routes),  G(intspNet.G), Gt(tspNet.G)
    {}



 protected:

    TspNetwork& tspNet;
    InTspNetwork& intspNet;

    StationContainer& stations;
    RouteContainer& routes;

    GraphType& Gt;
    InGraphType& G;

    unsigned int numConnections = 0;
    unsigned int numRouteNodes = 0;
    unsigned int numEdges = 0;

    unsigned int numTotalEvents;
    unsigned int numInsertedEvents;

    unsigned int numRoute2Route = 0;
    unsigned int numRoute2Central = 0;
    unsigned int numCentral2Route = 0;
    unsigned int numTransfers = 0;

    std::set<StationID> newStopIds;





    // Πληροφορίες Ακμής
    struct EdgeData
    {

        EdgeData( NodeDescriptor uD, NodeDescriptor vD, Distance weight, std::vector<TimeEvent> EventTable, StationID origin):TimeTable{std::move(EventTable)}
        {
            this->uD = uD;
            this->vD = vD;
            this->weight = weight;

        }

        EdgeData( NodeDescriptor uD, NodeDescriptor vD, Distance weight, std::vector<TimeEvent> EventTable, char l, StationID origin):TimeTable{std::move(EventTable)}
        {
            this->uD = uD;
            this->vD = vD;
            this->weight = weight;
            this->label = l;
            this->origin = origin;

        }

        bool operator==( const EdgeData& other)
        {
            return ( weight == other.weight);
        }

        bool operator<( const EdgeData& other)
        {
            return ( weight < other.weight);
        }

        NodeDescriptor uD;
        NodeDescriptor vD;

        Distance weight;

        std::vector<TimeEvent> TimeTable;

        char label = 'r';

        StationID origin;





    };

    struct CompareEdgesOrigin
    {
           bool operator () (const EdgeData& t1, const EdgeData& t2) const
           {
              return (t1.origin < t2.origin );

           }
    };

    // Πληροφορίες Ακμής
    struct WalkData
    {
        WalkData( StationID st1, StationID st2, Time time)
        {
            this->st1 = st1;
            this->st2 = st2;
            this->time = time;

        }

        StationID st1;
        StationID st2;

        Time time;

        bool operator== ( const WalkData& other)
        {
            return ( st1 == other.st1 && st2 == other.st2);
        }

    };


    std::vector<WalkData> WalkPaths;

    std::map<std::string, std::vector<TimeEvent> > eventsToSave;
    std::string lastEvent = "";
    std::string lastrip = "";
    StationID lastdep = INF;
    StationID lastarr=INF;
    std::map<std::string, std::string> ShowNext;
    std::map<std::string, std::string>::iterator it;

    void copyNode(NodeIterator& u, tNodeIterator& uP)
    {
      uP->dist = u->dist;
      uP->minTransferTime = u->minTransferTime;
      uP->stId = u->stId;
    }

    void copyIndexTable(NodeIterator& u, tNodeIterator& uP)
    {

      for(int i = 0; i<u->IndexTable.size(); ++i)
      {

        std::pair<Time, std::vector<tTimeEvent*> > temppair(u->IndexTable[i].first, std::vector<tTimeEvent*>());
        uP->IndexTable.push_back(temppair);
      }

    }


    void copyEdge(EdgeIterator& e, tEdgeIterator& eP)
    {
      eP->weight = e->weight;
      eP->walk = e->walk;
      eP->label = e->label;
    }

    void copyStation(Station& station1, tStation& tstation)
    {
      tStation tempstation;
      tempstation.id = station1.id;    //stationId is the index for the StationContainer
      tempstation.name = station1.name;
      tempstation.lat = station1.lat;
      tempstation.lon = station1.lon;
      tempstation.adjStations = station1.adjStations;
      tempstation.minTransferTime = station1.minTransferTime;

      tstation = tempstation;

    }

    void CopyEvent(TimeEvent& ev, tTimeEvent& et)
    {
      et.depTime = ev.depTime;
      et.arrTime = ev.arrTime;
      et.trId = ev.trId;
      et.sDays = ev.sDays;

    }

    void addEvent(StationID depStationId, StationID arrStationId, TimeEvent ev)
    {

        if(lastEvent != "")
        {
           if(eventsToSave[lastEvent].back().trId == ev.trId)
           {
             it = ShowNext.find(lastEvent+" "+std::to_string(ev.trId));
             if(it == ShowNext.end()) ShowNext[lastEvent+" "+std::to_string(ev.trId)+" "+lastrip] = std::to_string(depStationId)+" "+std::to_string(arrStationId)+" single "+std::to_string(ev.depTime)+" "+std::to_string(ev.sDays);

           }

        }


        eventsToSave[std::to_string(depStationId)+" "+std::to_string(arrStationId)].push_back(ev);

        lastEvent = std::to_string(depStationId)+" "+std::to_string(arrStationId);
        lastrip = std::to_string(ev.depTime)+" "+std::to_string(ev.sDays);
        lastdep = depStationId;
        lastarr = arrStationId;

    }



    struct EdgeList
    {
        EdgeList( unsigned int init = 0): eData(0)
        {}

        EdgeList( EdgeData* data): eData(data)
        {}

        bool operator== ( const EdgeList& other)
        {
            return ( eData == other.eData);
        }

        bool operator!= ( const EdgeList& other)
        {
            return ( eData != other.eData);
        }

        EdgeData* eData;
    };

    struct TransferEdge
    {
        TransferEdge( EdgeIterator e, Distance weight, unsigned int TimeTableSize)
        {
            this->e = e;
            this->weight = weight;
            this->TimeTableSize = TimeTableSize;

        }

        EdgeIterator e;
        Distance weight;
        unsigned int TimeTableSize;


        bool operator> ( const TransferEdge& a)
        {
            if(weight > a.weight) return true;
            else if(weight == a.weight) return TimeTableSize > a.TimeTableSize;
            else return false;
        }

    };


    void buildNetwork()
    {

       // Vector : δείχνει αν ο σταθμός/κόμβος έχει διαγραφεί
       std::vector<bool> isErased( stations.size(), true);

       // Δομή με τα νέα IDs των σταθμών
       std::vector<StationID> newIds( stations.size(), 0);
       std::vector<StationID> oldIds( stations.size(), 0);
       std::vector<StationID> stationOrder(stations.size(), 0);

       ComputeSCC(isErased, newIds, oldIds, stationOrder);


       CreateGraph(isErased, newIds, oldIds, stationOrder);




    }


    void ComputeSCC(std::vector<bool>& isErased, std::vector<StationID>& newIds, std::vector<StationID>& oldIds, std::vector<StationID>& stationOrder)
    {

        // Δομή με τους εναπομείνοντες σταθμούς
        StationContainer savedStations;
        savedStations.reserve( stations.size());

        // ---- Δημιουργία του γραφήματος των σταθμών -----


        // Δομή με τα NodeDescriptors των σταθμών του stG
        std::vector<StNodeDescriptor> stNode( stations.size());
        stG.clear();
        StationID stId, newStId, stopFromId, stopToId;

        // Δημ������υργία των κόμβων του stG
        for( stId = 0; stId < stations.size(); stId++)
        {
            stNode[stId] = stG.insertNode();
            stG.getNodeIterator( stNode[stId])->stId = stId;
        }

        // Δημιουργία των ακμών του stG
        for( stId = 0; stId < stations.size(); stId++)
        {
            std::vector<StationID>& adjStations = stations[stId].adjStations;
            stopFromId = stId;

            for( SizeType i = 0, endSt = adjStations.size(); i<endSt; i++)
            {

                stopToId = adjStations[i];

                if( !stG.hasEdge( stNode[stopFromId], stNode[stopToId]))
                    stG.insertEdge( stNode[stopFromId], stNode[stopToId]);
            }


        }





       for(int i = 0; i< WalkPaths.size(); i++)
       {
           StationID uid = WalkPaths[i].st1;
           StationID vid = WalkPaths[i].st2;

           if( !stG.hasEdge( stNode[uid], stNode[vid])) stG.insertEdge( stNode[uid], stNode[vid]);

       }





        // Μετατροπή του γραφήματος σε Ισχυρά Συνεκτικό
        SccProcessor<StGraph> scp( stG);
        scp.makeStronglyConnected();

        int mycounter = 0;

        // Ενημέρωση κατάστασης και αποθήκευση των σταθμών που απέμειναν
        for( StNodeIterator u = stG.beginNodes(), endNode = stG.endNodes(); u != endNode; ++u)
        {
            isErased[u->stId] = false;
            savedStations.push_back( stations[u->stId]);
            stationOrder[mycounter] = u->stId;
            mycounter++;

        }

        // Καταχώριση των νέων IDs
        for( newStId = 0, stId = 0; stId < stations.size(); stId++)
        {
            if( isErased[stId] == false)
            {
                newIds[stId] = newStId;


                oldIds[newStId] = stId;
                newStId++;
            }
        }

        for(int i=0; i<newIds.size(); i++)
        {
          StationID temp = stationOrder[i];
          stationOrder[i] = newIds[temp];
        }


        stations.swap(savedStations);


    }

    void InsertEdges()
    {

        EdgeDescriptor eD;
        EdgeIterator e;

        ProgressStream edge_progress( netEdges.size());
        edge_progress.label() << "\tInserting " << netEdges.size() << "route edges";

    //    std::sort(netEdges.begin(), netEdges.end(), CompareEdgesOrigin());



        //insert the edges
        for( SizeType i=0, size=netEdges.size(); i<size; i++)
        {
            eD = G.insertEdge( netEdges[i].uD, netEdges[i].vD);
            e = G.getEdgeIterator( eD);
            e->weight = netEdges[i].weight;
            e->TimeTable.reserve(netEdges[i].TimeTable.size());
            e->TimeTable.insert(e->TimeTable.end(), netEdges[i].TimeTable.begin(), netEdges[i].TimeTable.end() );
            e->label = netEdges[i].label;
            NodeIterator temp = G.getNodeIterator(netEdges[i].vD);
            NodeIterator temp1 = G.getNodeIterator(netEdges[i].uD);

            ++edge_progress;
        }


        ProgressStream edge_progress2( walkEdges.size());
        edge_progress2.label() << "\tInserting " << walkEdges.size() << "walk edges";


        std::sort(walkEdges.begin(), walkEdges.end());

        numTransfers = walkEdges.size();


        //insert the edges
        for( SizeType i=0, size=walkEdges.size(); i<size; i++)
        {
            if(G.hasEdge(walkEdges[i].uD, walkEdges[i].vD))
            {
              eD = G.getEdgeDescriptor(walkEdges[i].uD, walkEdges[i].vD);
              e = G.getEdgeIterator( eD);
              e->walk = walkEdges[i].weight;
              e->label = 'w';

            }
            else
            {
              eD = G.insertEdge( walkEdges[i].uD, walkEdges[i].vD);
              e = G.getEdgeIterator( eD);
              e->walk = walkEdges[i].weight;
              // = walkEdges[i].weight;
              e->label = 'w';

              NodeIterator node = G.getNodeIterator(walkEdges[i].uD);


            }

            ++edge_progress2;
        }



        //delete container
        {netEdges.clear(); std::vector<EdgeData> empty; netEdges.swap( empty);}

        //delete container
        {walkEdges.clear(); std::vector<EdgeData> empty; walkEdges.swap( empty);}


    }


    void InsertPMGEdges()
    {

        EdgeDescriptor eD;
        EdgeIterator e, a;

        ProgressStream edge_progress( netEdges.size());
        edge_progress.label() << "\tSorting and inserting " << netEdges.size() << " edges";

        PackedMemoryArray<EdgeList> edgeLists;
        typename PackedMemoryArray<EdgeList>::Iterator it;
        edgeLists.reserve( netEdges.size());

        //sort the first edges
        for( SizeType i=0, size=netEdges.size(); i<size; i++)
            edgeLists.optimalInsert( EdgeList( &(netEdges[i])));

        //insert the first edges
        while( !edgeLists.empty())
        {
            it = edgeLists.chooseCell();

            if( it->eData == 0)
            {
                continue;
            }

            const EdgeData& edge = *(it->eData);

            if( G.hasEdge( edge.uD, edge.vD) == true)
            {
                //warning: there are duplicates
                std::cout << "\nwarning: there are duplicates" << std::endl;
                continue;
            }

            eD = G.insertEdge( edge.uD, edge.vD);
            e = G.getEdgeIterator( eD);
            e->weight = edge.weight;
            e->TimeTable.reserve(edge.TimeTable.size());
            e->TimeTable.insert(e->TimeTable.end(), edge.TimeTable.begin(), edge.TimeTable.end() );

            edgeLists.erase( it);
            ++edge_progress;
        }

        edgeLists.clear();

        //delete container
        {netEdges.clear(); std::vector<EdgeData> empty; netEdges.swap( empty);}



    }




    void CreateGraph(std::vector<bool>& isErased, std::vector<StationID>& newIds, std::vector<StationID>& oldIds, std::vector<StationID>& stationOrder)
    {

        numConnections = 0;
        netEdges.reserve(numEdges+WalkPaths.size());



        unsigned int nodes = numRouteNodes+stations.size();

        std::cout << "nodes: " << nodes << ", other:"<<numEdges+WalkPaths.size()<< '\n';

        G.reserve(128380, 348940);


        // Δημιουργία κόμβων Σταθμών
        for(int i = 0; i < stations.size(); i++)
        {
           stations[i].stNode = G.insertNode();
           NodeIterator st = G.getNodeIterator(stations[i].stNode);
           st->stId = i;
           st->minTransferTime = stations[i].minTransferTime;
        }



              int numTimeEvents = 0;
              int numTimeSlots = 0;
              std::map<int, int> OutDegree;
              std::map<StationID, std::vector<TimeEvent> > stToEv;
              for(typename std::map<std::string,std::vector<TimeEvent> >::iterator it=eventsToSave.begin(); it!=eventsToSave.end(); ++it)
              {
                  StationID depStation, arrStation;
                  std::stringstream data(it->first);
                  data >> depStation >> arrStation;
                  if( (!isErased[depStation]) && (!isErased[ arrStation]))
                  {
                      std::vector<TimeEvent>& events = it->second;
                      numTimeSlots++;
                      numTimeEvents+= events.size();
                      OutDegree[depStation]++;
                      stToEv[newIds[depStation]].insert(stToEv[newIds[depStation]].end(), events.begin(), events.end());
                  }
              }



              int maxPoolSize = G.getNumNodes() *(sizeof(int) + 1440*sizeof(TimeSlotItem<tNodeIterator>)) + (1440*numTimeSlots)*sizeof(TimeEvent_<tNodeIterator>*);
              TspNode<tNodeIterator>::IndexTablePool = new char[maxPoolSize];
              char* tdata = TspNode<tNodeIterator>::IndexTablePool;

              int whr = 0;


        for(typename std::map<std::string,std::vector<TimeEvent> >::iterator it=eventsToSave.begin(); it!=eventsToSave.end(); ++it)
        {

            StationID depStation, arrStation;

            std::stringstream data(it->first);

            data >> depStation >> arrStation;

            if( (!isErased[depStation]) && (!isErased[ arrStation]))
            {
              //TODO! ENA PERASMA  GIA KATHE ENAN KOMVO
                StationID uid = newIds[depStation];
                StationID vid = newIds[arrStation];
                std::vector<TimeEvent>& events = it->second;
                std::sort(events.begin(), events.end(), CompareArr<NodeIterator>());


                newStopIds.insert(uid);

                NodeIterator tempNode = G.getNodeIterator(stations[uid].stNode);


                if(stToEv[uid].size() > 0 )
                {
                  std::sort( stToEv[uid].begin(), stToEv[uid].end());

                  tempNode->first = (TimesContainer*) tdata;
                  TimesContainer& IndexTable = *(tempNode->first);

                  Time MaxDep = stToEv[uid][0].depTime;
                  IndexTable.emplace_back(stToEv[uid][0].depTime);

                  for(int i = 1; i < stToEv[uid].size(); ++i)
                  {
                      if(stToEv[uid][i].depTime > MaxDep)
                      {

                        IndexTable.emplace_back(stToEv[uid][i].depTime);
                        MaxDep = stToEv[uid][i].depTime;
                      }
                  }


                  tdata += sizeof(int) + IndexTable.size()* sizeof(Time);
                  whr+= sizeof(int) + IndexTable.size()* sizeof(Time);


                  tempNode->second = ((TimeEventContainer<NodeIterator>*) tdata);
                  TimeEventContainer<NodeIterator>& eventsSecond = *(tempNode->second);
                  tdata +=  IndexTable.size()*(sizeof(int) + OutDegree[depStation]*sizeof(TimeEvent_<NodeIterator>*) );
                  stToEv[uid].clear();

                }

                //minLength
                auto obj = *std::min_element(events.begin(), events.end(), [](const TimeEvent& t1, const TimeEvent& t2){return(t1.getLength()<t2.getLength());});

                netEdges.push_back(EdgeData(stations[uid].stNode, stations[vid].stNode, obj.getLength(), events, uid/*idToVector[uid])*/));

                numConnections+=events.size();

            }

        }


       for(int i = 0; i< WalkPaths.size(); i++)
       {

           if( (!isErased[ WalkPaths[i].st1 ]) && (!isErased[ WalkPaths[i].st2]))
           {
              StationID uid = newIds[ WalkPaths[i].st1];
              StationID vid = newIds[ WalkPaths[i].st2];

              walkEdges.push_back(EdgeData(stations[uid].stNode, stations[vid].stNode, WalkPaths[i].time, std::vector<TimeEvent>() , 'w'));

           }
       }






        const std::string graphType( typeid(G).name());
        InsertEdges();



        NodeIterator u = G.beginNodes();

       G.getImpl().dfsCompress();


       Gt.load(G);

       // Copy Stations
        tStationContainer& tstations(tspNet.stations);
        tstations.reserve( stations.size());
        for(int i = 0; i<stations.size(); ++i)
        {
          tStation& station = tspNet.addStation();
          copyStation(stations[i], station);
        }

        tNodeIterator uP = Gt.beginNodes();
        tNodeIterator endNodeP = Gt.beginNodes();

        // Δημιουργία κόμβν Σταθμών
        for(NodeIterator u = G.beginNodes(), lastNode = G.endNodes(); u != lastNode; ++u, ++uP)
        {
           copyNode(u, uP);
           tstations[uP->stId].stNode = Gt.getNodeDescriptor(uP);

           stations[u->stId].stNode = G.getNodeDescriptor(u);


          // copyIndexTable(u, uP);

           tEdgeIterator eP= Gt.beginEdges(uP);
           for(EdgeIterator mye = G.beginEdges(u), lastEdge = G.endEdges(u); mye != lastEdge; ++mye, ++eP)
           {

             copyEdge(mye, eP);

             for(int i = 0; i<mye->TimeTable.size(); i++)
             {

               NodeIterator temp = G.target(mye);
               (mye->TimeTable[i]).dest = temp;


               tNodeIterator ttemp = Gt.target(eP);
               tTimeEvent tempevent;
               tempevent.dest = ttemp;


               CopyEvent(mye->TimeTable[i], tempevent);
               eP->TimeTable.push_back(tempevent);

             }

           }

        }


      //  stations.swap(savedStations);


      u = G.getNodeIterator(stations[0].stNode);


        Timer t1;


        MakeNext(oldIds, newIds, isErased);

        MakeIndex();



    }

    void MakeIndex()
    {
       // Για όλους τους κόμβους του γραφήματος
       for(tNodeIterator uP = Gt.beginNodes(), lastNode = Gt.endNodes(); uP != lastNode; ++uP)
       {
          // Για όλες  τις ακμές
          for(tEdgeIterator eP = Gt.beginEdges(uP), lastEdge = Gt.endEdges(uP); eP != lastEdge; ++eP)
          {


             tNodeIterator vP = Gt.target(eP);

             if(eP->TimeTable.size() == 0 || vP->first ==  nullptr) continue;

             for(int i = 0; i<eP->TimeTable.size(); ++i)
             {

                Time temp2((eP->TimeTable[i].arrTime)%86400-1);
                Time *tup = std::upper_bound(vP->first->Fbegin(), vP->first->Fend(),temp2);

                // Δυαδική αναζήτηση για τον υπολογισμό της επόμενης αναχώρισης
                eP->TimeTable[i].nextIndex = tup-vP->first->Fbegin();

                if(tup == vP->first->Fend()) eP->TimeTable[i].nextIndex = 0;

             }

          }

        }
    }

    void MakeNext(std::vector<StationID>& oldIds, std::vector<StationID>& newIds, std::vector<bool>& isErased)
    {


        tStationContainer& tstations(tspNet.stations);




    for( auto it = newStopIds.begin(); it != newStopIds.end(); it++)
    {
          NodeIterator u = G.getNodeIterator(stations[*it].stNode);
          tNodeIterator ut = Gt.getNodeIterator(tstations[*it].stNode);
          ut->first =  (TimesContainer*) u->first;
          ut->second =  (TimeEventContainer<tNodeIterator>*) u->second;
     }
      //for loop σε σχλέση με την σειρά της πρώτης φάσης (γέμισμα με timeslots)


        // Για όλους τους κόμβους του γραφήματος
        for( auto it = newStopIds.begin(); it != newStopIds.end(); it++)
        {

            tNodeIterator uP = Gt.getNodeIterator(tstations[*it].stNode);

            std::map<int, std::vector<TimeEvent_<tNodeIterator>* > > map;

            // Για όλες  τις ακ���ές
            for(tEdgeIterator eP = Gt.beginEdges(uP), lastEdge = Gt.endEdges(uP); eP != lastEdge; ++eP)
            {


              if(eP->TimeTable.size() == 0) continue;


              tNodeIterator vP = Gt.target(eP);

                int k = 0;
                int j = 0;



              int numTimeSlots = uP->first->size();



                while(j < numTimeSlots)
                {
                    if (k >= eP->TimeTable.size() )
                    {
                        TimeEvent_<tNodeIterator> *temp = nullptr;
                        map[j].push_back( temp);

                        ++j;
                    }
                    else if( uP->IndexFirst(j) <= eP->TimeTable[k].depTime)
                    {
                         TimeEvent_<tNodeIterator> *temp = &(eP->TimeTable[k]);
                         map[j].push_back( temp);
                         ++j;
                     }

                    else ++k;
                }

              } // Για όλες τις ακμές του γραφήματος


               for(int m = 0; m < uP->first->size(); ++m)
               {
                 TimeEventContainer<tNodeIterator>& eventsSecond = *(uP->getSecond(m));

                 for(int l = 0; l < map[m].size(); ++l)
                 {
                   eventsSecond.emplace_back(map[m][l]);
                 }
               }
            }// Για όλους τους κόμβους του γραφήματος


            // Για όλους τους κόμβους του γραφήματος
            for( auto it = newStopIds.begin(); it != newStopIds.end(); it++)
            {

                tNodeIterator uP = Gt.getNodeIterator(tstations[*it].stNode);

                // Για όλες  τις ακ���ές
                for(tEdgeIterator eP = Gt.beginEdges(uP), lastEdge = Gt.endEdges(uP); eP != lastEdge; ++eP)
                {

                // Για όλα τα events της ακμής
                for(int i = 0; i < eP->TimeTable.size(); ++i)
                {

                    StationID depStation, arrStation;
                    tNodeIterator t = Gt.target(eP);

                    std::string singleOrDouble;
                    Time exampletime;
                    int mydays;

                    // Βρες την ακμή που βρίσκεται το επόμενο event του event i της ακμής u->t  (αφού μεταφράσεις τα νέα ids σε παλιά)
                    std::map<std::string, std::string>::iterator it = ShowNext.find(std::to_string(oldIds[uP->stId]) + " " + std::to_string(oldIds[t->stId]) + " " + std::to_string(eP->TimeTable[i].trId)+ " " + std::to_string(eP->TimeTable[i].depTime)+ " " + std::to_string(eP->TimeTable[i].sDays));

                    if(it == ShowNext.end()) continue;


                    std::stringstream data(ShowNext[std::to_string(oldIds[uP->stId]) + " " + std::to_string(oldIds[t->stId]) + " " + std::to_string(eP->TimeTable[i].trId)+ " " + std::to_string(eP->TimeTable[i].depTime)+ " " + std::to_string(eP->TimeTable[i].sDays)]);


                    // Το event βρίσκεται στην ακμή depStation->arrStation (με τα παλιά ids)
                    data >> depStation >> arrStation >> singleOrDouble >> exampletime >> mydays;


                    if( (isErased[depStation]) || (isErased[ arrStation])) continue;

                    // Η ακμη που βρίσκεται το επόμενο event (αφού μεταφράσαμε τα παλιά ids σε νέα)
                    tEdgeIterator eNext = Gt.getEdgeIterator(Gt.getNodeIterator(tstations[newIds[depStation]].stNode), Gt.getNodeIterator(tstations[newIds[arrStation]].stNode));


                    int appears=0;
                    int j;


                    // Αφού βρήκαμε την ακμή που περιέχει το επόμενο event, βρίσκουμε το κατάλληλο event (είναι αυτό που έχει το ίδιο trip id και ΔΕΝ είναι μοναδικό)
                    for(j = 0; j < eNext->TimeTable.size(); ++j)
                    {

                        if(eNext->TimeTable[j].trId == eP->TimeTable[i].trId)
                        {
                            ++appears;

                            if(eNext->TimeTable[j].depTime == exampletime && eNext->TimeTable[j].sDays == mydays)//if(e->TimeTable[i].arrTime%1440 <= eNext->TimeTable[j].depTime)
                            {
                              eP->TimeTable[i].next = &(eNext->TimeTable[j]);
                              break;
                            }

                        }
                    }


                }


            }



        }  // Για όλους τους κόμβους του γραφήματος




    }



    //graph station Node
    struct StNodeInfo : DefaultGraphItem
    {
        StNodeInfo() : compId(0), isMarked(0)
        {}

        StationID stId;
        unsigned int compId;
        bool isMarked;
    };

    std::vector<EdgeData> netEdges;
    std::vector<EdgeData> walkEdges;

    //graph station Edge
    struct StEdgeInfo : DefaultGraphItem
    {};

    //graph station InEdge
    struct StInEdgeInfo : DefaultGraphItem
    {};



    typedef DynamicGraph< AdjacencyListImpl, StNodeInfo, StEdgeInfo, StInEdgeInfo>         StGraph;
    typedef typename StGraph::NodeIterator                     StNodeIterator;
    typedef typename StGraph::NodeDescriptor                   StNodeDescriptor;
    typedef typename StGraph::EdgeIterator                     StEdgeIterator;
    typedef typename StGraph::EdgeDescriptor                   StEdgeDescriptor;
    StGraph stG;
  };
