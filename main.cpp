#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <string>
#include <cmath>
#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <stddef.h>
#include <geos_c.h>

using namespace std;
using namespace osmium;
using namespace osmium::handler;

struct MyAppNode {
    int id;
    double lat, lon; // Latitude and Longitude
};

struct Edge {
    int target;
    double cost;
};

typedef vector<vector<Edge>> Graph;

class RoadHandler : public Handler {
public:
    unordered_map<long long, Location> node_locations;
    Graph graph;
    map<int, MyAppNode> nodes;
    int node_count = 0;

    RoadHandler() {
        geosContext = initGEOS_r();
    }

    ~RoadHandler() {
        finishGEOS_r(geosContext);
    }

    void node(const osmium::Node& node) {
        node_locations[node.id()] = node.location();
        nodes[node_count++] = {static_cast<int>(node.id()), node.location().lat(), node.location().lon()};
    }

    void way(const Way& way) {
        const char* highway = way.tags()["highway"];
        if (highway) {
            vector<long long> nodes_in_way;
            for (const auto& node : way.nodes()) {
                if (node_locations.find(node.ref()) != node_locations.end()) {
                    nodes_in_way.push_back(node.ref());
                }
            }
            for (size_t i = 0; i < nodes_in_way.size() - 1; ++i) {
                int from = static_cast<int>(nodes_in_way[i]);
                int to = static_cast<int>(nodes_in_way[i + 1]);
                double cost = calculateDistance(node_locations[from].lat(), node_locations[from].lon(),
                                                node_locations[to].lat(), node_locations[to].lon());
                graph[from].push_back({to, cost});
                graph[to].push_back({from, cost}); // If bidirectional
            }
        }
    }

    GEOSContextHandle_t geosContext;

    GEOSGeom createPoint(double lat, double lon) {
        GEOSCoordSequence* seq = GEOSCoordSeq_create_r(geosContext, 1, 2);
        GEOSCoordSeq_setX_r(geosContext, seq, 0, lon);
        GEOSCoordSeq_setY_r(geosContext, seq, 0, lat);
        GEOSGeom point = GEOSGeom_createPoint_r(geosContext, seq);  // The GEOSGeom owns the sequence
        return point;
    }

    double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        GEOSGeom p1 = createPoint(lat1, lon1);
        GEOSGeom p2 = createPoint(lat2, lon2);
        double distance;
        GEOSDistance_r(geosContext, p1, p2, &distance);
        GEOSGeom_destroy_r(geosContext, p1);
        GEOSGeom_destroy_r(geosContext, p2);
        return distance;
    }
};

double heuristic(const MyAppNode& a, const MyAppNode& b) {
    return hypot(a.lat - b.lat, a.lon - b.lon);
}

vector<int> a_star_search(const Graph& graph, const map<int, MyAppNode>& nodes, int start, int goal) {
    priority_queue<tuple<double, int, vector<int>>, vector<tuple<double, int, vector<int>>>, greater<>> frontier;
    frontier.emplace(0, start, vector<int>{start});

    map<int, double> cost_so_far;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        auto [current_cost, current, path] = frontier.top();
        frontier.pop();

        if (current == goal) {
            return path;
        }

        for (const auto& edge : graph[current]) {
            double new_cost = current_cost + edge.cost;
            if (!cost_so_far.count(edge.target) || new_cost < cost_so_far[edge.target]) {
                cost_so_far[edge.target] = new_cost;
                double priority = new_cost + heuristic(nodes.at(edge.target), nodes.at(goal));
                vector<int> new_path = path;
                new_path.push_back(edge.target);
                frontier.emplace(priority, edge.target, new_path);
            }
        }
    }
    return {}; // Return empty path if no path is found
}

void read_osm_data(const string& filename) {
    io::Reader reader(filename);
    RoadHandler handler;
    osmium::apply(reader, handler);
    reader.close();
}

int main() {
    string osm_filename = "map.osm"; // Specify your OSM data file
    double start_lat, start_lon, end_lat, end_lon;
    cout << "Enter start latitude: ";
    cin >> start_lat;
    cout << "Enter start longitude: ";
    cin >> start_lon;
    cout << "Enter end latitude: ";
    cin >> end_lat;
    cout << "Enter end longitude: ";
    cin >> end_lon;

    int start_node_id = 0;  // Example, map this from real coordinates
    int goal_node_id = 10;  // Example, map this from real coordinates

    read_osm_data(osm_filename);
    RoadHandler handler;
    auto path = a_star_search(handler.graph, handler.nodes, start_node_id, goal_node_id);
    for (int node_id : path) {
        cout << "Node ID: " << node_id << " - Lat: " << handler.nodes[node_id].lat << ", Lon: " << handler.nodes[node_id].lon << endl;
    }

    return 0;
}
