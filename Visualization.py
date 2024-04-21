import re
import matplotlib.pyplot as plt

def extract_node_positions(file_path):
    node_positions = {}
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            match = re.match(r'\s*nodes\[([0-9]+)\]\s*=\s*\(Node\)\{([0-9]+),\s*([0-9.-]+),\s*([0-9.-]+)\};', line)
            if match:
                node_id = int(match.group(1))
                lat = float(match.group(3))
                lon = float(match.group(4))
                node_positions[node_id] = (lon, lat)  # Reversed order to match (x, y) convention
    return node_positions

def read_data(filepath):
    """Read the node path from a text file."""
    with open(filepath, 'r') as file:
        lines = file.readlines()
    path_line = lines[-1].strip()
    path_str = path_line.split(': ')[1].split(' <- ')
    path = [int(node) for node in path_str if node.isdigit()]
    return path

def plot_path(path, node_positions):
    """Plot the path using Matplotlib."""
    plt.figure(figsize=(10, 6))
    for i in range(len(path)-1):
        start = node_positions[path[i]]
        end = node_positions[path[i+1]]
        plt.plot([start[0], end[0]], [start[1], end[1]], 'bo-')
        plt.text(start[0], start[1], str(path[i]), color="red", fontsize=12)

    # Mark the final node
    end = node_positions[path[-1]]
    plt.plot(end[0], end[1], 'bo')
    plt.text(end[0], end[1], str(path[-1]), color="red", fontsize=12)

    plt.title('Path Visualization')
    plt.xlabel('Longitude')
    plt.ylabel('Latitude')
    plt.grid(True)
    plt.show()

# Extract node positions from the C code file
node_positions = extract_node_positions('/Users/dobromiriliev/Documents/GitHub/UrbanTree/main.c')

# Read the node path from the output file
path = read_data('/Users/dobromiriliev/Documents/GitHub/UrbanTree/coordinates.csv')

# Plot the path
plot_path(path, node_positions)
