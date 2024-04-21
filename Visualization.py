import pandas as pd
import matplotlib.pyplot as plt

# Load the data from CSV file
file_path = '/Users/dobromiriliev/Documents/GitHub/UrbanTree/route_coordinates.csv'
data = pd.read_csv(file_path)

# Extract longitude and latitude from the data
longitude = data['Longitude']
latitude = data['Latitude']

# Create a plot
plt.figure(figsize=(12, 8))
plt.scatter(longitude, latitude, color='red', marker='o', s=10)  # smaller markers
plt.plot(longitude, latitude, color='blue', linewidth=1)  # thinner line

# Adding titles and labels
plt.title('GSMST to Georgia Tech')
plt.xlabel('Longitude')
plt.ylabel('Latitude')

# Show grid
plt.grid(True)

# Optional: Save the plot to a file
plt.savefig('/Users/dobromiriliev/Documents/GitHub/UrbanTree/route_visualization.png')

# Display the plot
plt.show()
