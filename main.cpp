#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")
#include <iostream>
#include <random>
#include <boost/program_options.hpp>
#include <Structs/Graphs/dynamicGraph.h>
#include <Structs/Graphs/adjacencyListImpl.h>
#include <Structs/Graphs/adjacencyVectorImpl.h>
#include <Structs/Graphs/packedMemoryArrayImpl.h>
#include <Structs/Graphs/staticForwardStarImpl.h>
#include <Structs/Graphs/staticArrayGraph.h>
#include <Utilities/timer.h>
#include "Structs/tspNetwork.h"
#include "Readers/GTFS2Reader.h"
#include "Algs/OptALTDijkstraPointer.h"


typedef struct NodeInfo Foo;
typedef struct EdgeInfo Boo;
typedef struct inNodeInfo inFoo;
typedef struct inEdgeInfo inBoo;
typedef struct InEdgeInfo Coo;

#define DAYS 1


#define TIMEPERIOD 86400 // SECONDS


typedef StaticArrayGraph< Foo, Boo, Coo>   aGraph;
typedef DynamicGraph< StaticForwardStarImpl, inFoo, inBoo, Coo>  bGraph;

typedef TspNetwork<aGraph> TspNetworkFoo;
typedef TspNetwork<bGraph> InTspNetworkFoo;

typedef typename aGraph::NodeIterator NodeIteratorFoo;
typedef typename bGraph::NodeIterator inNodeIteratorFoo;

//fsGraph

struct inNodeInfo : DefaultGraphItem, TspNode<inNodeIteratorFoo>
{};

struct inEdgeInfo : DefaultGraphItem, TspEdge<inNodeIteratorFoo>
{};

struct NodeInfo : DefaultGraphItem, TspNode<NodeIteratorFoo>
{};

struct EdgeInfo : DefaultGraphItem, TspEdge<NodeIteratorFoo>
{};

struct InEdgeInfo : DefaultGraphItem
{};

TTL algTimestamp = 0;

typedef DynamicGraph< PackedMemoryArrayImpl, NodeInfo, EdgeInfo, InEdgeInfo>   pmaGraph;
typedef DynamicGraph< StaticForwardStarImpl, NodeInfo, EdgeInfo, InEdgeInfo>         fsGraph;
typedef DynamicGraph< AdjacencyListImpl, NodeInfo, EdgeInfo, InEdgeInfo>       Graph;

typedef DynamicGraph<StaticForwardStarImpl, inNodeInfo, inEdgeInfo, InEdgeInfo> InGraphType;
typedef StaticArrayGraph<NodeInfo, EdgeInfo, InEdgeInfo> GraphType;


template<typename GraphType, typename InGraphType>
void shell( int& format, int& mod, int& algorithm, std::string& basePath, std::string& qFile, std::string& rFile, std::string& netName, bool walkEnabled, int numberOfDays, int walkDuration);

template<typename GraphType>
void runQuery( TspNetwork<GraphType>& net, int algorithm, std::string& basePath);

int main(int argc, char* argv[])
{

    // ---- Default  ----
    std::string netName = "athens";
    int walkDuration = 0;
    int numberOfDays = 1;
    int graphVariant = 3;
    int algorithm = 2;
    int format = 2;
    int mod = 1;
    bool walkEnabled = true;
    // ---------------------------------

    std::string homePath, basePath;
    std::string qFile, rFile;
    homePath = std::string( getenv("HOME"));
    basePath = "../../../temp/"; //homePath + "/Projects/Graphs/";

    // program options
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("net,n", po::value<std::string>(), "Name of transport network. Default:'athens'.")
        ("format,f", po::value<int>(), "Format. GTFS[1] (london), GTFS[2] (berlin,athens). Default:2.")
        ("algorithm,a", po::value<int>(), "Algorithm. PlainDijkstra[1], OptDijkstra[2], ALT-Dijkstra[3]. Default:2.")
        ("graph structure,g", po::value<int>(), "Graph structure. Adjacency List[1], Dynamic Forward Star[2], Packed Memory Graph[3]. Default:2.")
        ("mod,o", po::value<int>(), "Run shpath queries manually[1], random queries from file[2]. Default:1")
        ("qFile,q", po::value<std::string>(), "Query file path. Default:./netName.queries")
        ("rFile,r", po::value<std::string>(), "Result file path. Default:./netName.RTE.results")
        ("walk,w", po::value<bool>(), "Enable walking. Default:0")
        ("number_of_days,x", po::value<int>(), "Number of Days. Default:1.")
        ("walk_duration,y", po::value<int>(), "walkDuration. Default:0.")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify( vm);

    if ( vm.empty())
    {
        std::cout << desc << "\n";
        return 0;
    }

    if ( vm.count("net"))
        netName = vm["net"].as<std::string>();

    if ( vm.count("number_of_days"))
        numberOfDays = vm["number_of_days"].as<int>();

    if ( vm.count("walk_duration"))
        walkDuration = vm["walk_duration"].as<int>();

    if ( vm.count("format"))
        format = vm["format"].as<int>();

    if ( vm.count("algorithm"))
        algorithm = vm["algorithm"].as<int>();

    if ( vm.count("graph structure"))
        graphVariant = vm["graph structure"].as<int>();

    if ( vm.count("mod"))
        mod = vm["mod"].as<int>();

    if ( vm.count("qFile"))
        qFile = vm["qFile"].as<std::string>();

    if ( vm.count("rFile"))
        rFile = vm["rFile"].as<std::string>();

    if ( vm.count("walk"))
        walkEnabled = vm["walk"].as<bool>();


    // GTFS Path
    if( format == 1)
        basePath = basePath + "GTFS1/" + netName + "/";
    else
        basePath = basePath /*+ "GTFS2/"*/ + netName + "/";

    qFile = "./" + netName + ".queries";
    rFile = "./" + netName + ".DTM.results";


    // Graph Data
    std::cout<<"--------------------------------------------------------------------------------"<<std::endl;
    std::cout<<"\t\t\t  Time Dependent Model"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------------"<<std::endl;

    std::cout<<"[-] Format: "<<format<<std::endl;
    std::cout<<"[-] NetName: "<<netName<<std::endl;
    std::cout<<"[-] GraphVariant: "<<graphVariant<<std::endl;

    shell<aGraph, bGraph>( format, mod, algorithm, basePath, qFile, rFile, netName, walkEnabled, numberOfDays, walkDuration);

    return 0;
}

template<typename GraphType, typename InGraphType>
void shell( int& format, int& mod, int& algorithm, std::string& basePath, std::string& qFile, std::string& rFile, std::string& netName, bool walkEnabled, int numberOfDays, int walkDuration)
{
    typedef TspNetwork<InGraphType> InTspNetwork;
    typedef TspNetwork<GraphType> TspNetwork;

    typedef typename GraphType::NodeIterator NodeIterator;

    Timer timer;
    timer.start();

    // Create Graph
    TspNetwork net;
    InTspNetwork  InNet;

    GTFS2Reader<TspNetwork, InTspNetwork> data( net, InNet, basePath + "routes.txt", basePath + "trips.txt",
                                           basePath + "stops.txt", basePath + "stop_times.txt", basePath + "transfers.txt", basePath + "calendar.txt",
                                           basePath + "walkPaths_d"+std::to_string(numberOfDays)+"_s4_5_r"+std::to_string(walkDuration)+"_min.txt", basePath + "calendar_dates.txt", walkEnabled, netName);
        data.read();

    // Graph Memory
    std::cout << "Graph has " << (double) net.G.memUsage() / 1048576.0
              << " Mbytes. Time spent to read:\t"
              << timer.getElapsedTime() << "secs\n";

    // ALT File
    std::string filename;
    filename = "ALT_d"+std::to_string(numberOfDays)+"_w"+std::to_string(walkDuration)+"_FIN+.txt";


    // Mod : 1-> queries from file, 2-> random queries
    if( mod == 1) runQueriesFromFile( net, algorithm, basePath, filename, netName, numberOfDays, walkDuration);
    else runRandomQueries(net, algorithm, basePath, filename);

}


template<typename GraphType>
void runQueriesFromFile( TspNetwork<GraphType>& net, int algorithm, std::string& basePath, std::string filename, std::string& netName, int numberOfDays, int walkDuration)
{
    typedef TspNetwork<GraphType> TspNetwork;
    Timer timer;

    StationID sourceStationId, targetStationId;
    Time startTime;
    int numVehicles =  net.vehicles.size();
    int numSelectedVehicles = 0;

    std::map<VehicleTypeID, bool> selectedVehicles;

    char opt;

    typedef typename GraphType::NodeIterator NodeIterator;
    typedef typename GraphType::EdgeIterator EdgeIterator;
    typedef TimeEvent_<NodeIterator>  TimeEvent;

    std::ifstream infile;
    std::ofstream outfile;

    std::string line;

    std::cout << "\nReading query files "  << " ... " << std::flush;

    std::string inFileS = "voula_queries_"+netName+"_d"+std::to_string(numberOfDays)+"_w"+std::to_string(walkDuration)+"_wFin+.txt";
    std::string outFileS = "translated"+netName+"_d"+std::to_string(numberOfDays)+"_w"+std::to_string(walkDuration)+"Results_wFin+.txt";

    infile.open (inFileS);
    std::cout << inFileS << std::endl;
    outfile.open (outFileS);

    if( !infile)
    {
      std::cerr << "\nError opening " << inFileS << std::endl;
      exit(-1);
    }

    if( !outfile)
    {
      std::cerr << "\nError opening " << outFileS << std::endl;
      exit(-1);
    }

    OptALTDijkstraPointer<TspNetwork> alg3( net, &algTimestamp, basePath + filename);
    float mysum = 0;
    float sum = 0;
    float max = 0;
    int counter = 0;

    while(std::getline( infile, line))
    {
       std::stringstream data(line);

       data >> sourceStationId >> targetStationId >> startTime;

       timer.start();
       alg3.runQuery( sourceStationId, targetStationId, startTime);
       timer.stop();

       NodeIterator t3 = net.G.getNodeIterator(net.stations[targetStationId].stNode);
       NodeIterator t2 = net.G.getNodeIterator(net.stations[sourceStationId].stNode);

       outfile << sourceStationId << " " << targetStationId << " "<< startTime << " " << timer.getElapsedTimeInMilliSec() << " " << (t3->dist-t2->dist) << "\n";

       sum+=timer.getElapsedTimeInMilliSec();

      if (timer.getElapsedTimeInMilliSec()>max) max = timer.getElapsedTimeInMilliSec();

      ++counter;

    }



    std::cout << "average time: " << (sum/counter)<< "ms" << std::endl;
    std::cout << "max time: " << max<< "ms" << std::endl;

    outfile.close();
    infile.close();

}



template<typename GraphType>
void runRandomQueries( TspNetwork<GraphType>& net, int algorithm, std::string& basePath, std::string filename)
{

    typedef TspNetwork<GraphType> TspNetwork;
    typedef typename GraphType::NodeIterator NodeIterator;
    typedef typename GraphType::EdgeIterator EdgeIterator;
    typedef TimeEvent_<NodeIterator>  TimeEvent;


            OptALTDijkstraPointer<TspNetwork> alg( net, &algTimestamp, basePath + filename);

            StationID dep, arr;
            Time time_ = 0;
            float sum = 0;
            int max =0;
            int experiments = 100000;
	          int snodes =0;

            boost::mt19937 gen(time(NULL));
            boost::uniform_real<> dist(0, 1);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<> > random(gen, dist);



float sumspecial=0;
            // Πειράματα
            for(int i =0; i < experiments; i++)
            {

                Timer t1;
                dep = random() *net.stations.size();
                arr = random() * net.stations.size();
                time_ = random() * TIMEPERIOD*DAYS;

                t1.start();
                alg.runQuery(dep, arr,time_);
                t1.stop();

                NodeIterator t3 = net.G.getNodeIterator(net.stations[dep].stNode);
                NodeIterator t2 = net.G.getNodeIterator(net.stations[arr].stNode);


                alg.PrintShortestPath();



                if (t1.getElapsedTimeInMilliSec() > max) max = t1.getElapsedTimeInMilliSec();

                sum+=t1.getElapsedTimeInMilliSec();


             }

             std::cout << "average time: " << (sum/experiments)<< "ms" << std::endl;
             std::cout << "max time: " << max<< "ms" << std::endl;
             std::cout << "snodes: " << (snodes/experiments) << std::endl;
             std::cout << "special sum: " << (sumspecial/experiments) << std::endl;

}
