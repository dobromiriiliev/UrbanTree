#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <string>
#include <cmath>
#include <limits>
#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace osmium;
using namespace osmium::handler;

struct MyAppNode {
    int id;
    double lat, lon;
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
    shared_ptr<geos::geom::GeometryFactory> factory;

    RoadHandler() : factory(geos::geom::GeometryFactory::create()) {}

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

    double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        geos::geom::Coordinate c1(lon1, lat1);
        geos::geom::Coordinate c2(lon2, lat2);
        auto p1 = factory->createPoint(c1);
        auto p2 = factory->createPoint(c2);
        return p1->distance(p2.get());
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

// CURL callback function to collect received data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Geocode address using Google Maps API
std::pair<double, double> geocodeAddress(const std::string& address) {
    CURL *curl;
    CURLcode res;
    string readBuffer;
    curl = curl_easy_init();
    if (curl) {
        char *escapedAddress = curl_easy_escape(curl, address.c_str(), address.length());
        if (escapedAddress) {
            string api_url = "https://maps.googleapis.com/maps/api/geocode/json?address=" + string(escapedAddress) + "&key=AIzaSyDJx_mZW11ZsmGxI71uPS31mG76r8OESoo";
            curl_free(escapedAddress);

            curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK) {
                json response = json::parse(readBuffer);
                double lat = response["results"][0]["geometry"]["location"]["lat"];
                double lon = response["results"][0]["geometry"]["location"]["lng"];
                return {lat, lon}; // Return parsed latitude and longitude
            }
        }
    }

    return {0.0, 0.0}; // Return default if CURL fails or no data
}

int findClosestNode(double lat, double lon, map<int, MyAppNode>& nodes) {
    double min_distance = numeric_limits<double>::max();
    int closest_node_id = -1;

    for (const auto& [node_id, node] : nodes) {
        double distance = hypot(node.lat - lat, node.lon - lon);
        if (distance < min_distance) {
            min_distance = distance;
            closest_node_id = node_id;
        }
    }
    return closest_node_id;
}

void read_osm_data(const string& filename, RoadHandler& handler) {
    io::Reader reader(filename);
    osmium::apply(reader, handler);
    reader.close();
}

int main() {
    string osm_filename = "/Users/dobromiriliev/Documents/GitHub/UrbanTree/us-latest.osm.pbf";
    string start_address, end_address;

    cout << "Enter start address: ";
    getline(cin, start_address);
    cout << "Enter end address: ";
    getline(cin, end_address);

    auto [start_lat, start_lon] = geocodeAddress(start_address);
    auto [end_lat, end_lon] = geocodeAddress(end_address);

    RoadHandler handler;
    read_osm_data(osm_filename, handler);

    int start_node_id = findClosestNode(start_lat, start_lon, handler.nodes);
    int goal_node_id = findClosestNode(end_lat, end_lon, handler.nodes);

    if (start_node_id == -1 || goal_node_id == -1) {
        cout << "Error: Address not found on map." << endl;
        return 1;
    }

    auto path = a_star_search(handler.graph, handler.nodes, start_node_id, goal_node_id);
    for (int node_id : path) {
        cout << "Node ID: " << node_id << " - Lat: " << handler.nodes[node_id].lat << ", Lon: " << handler.nodes[node_id].lon << endl;
    }

    return 0;
}