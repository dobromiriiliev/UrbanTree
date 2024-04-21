import pandas as pd
import matplotlib.pyplot as plt

# Load the data from CSV file
file_path = '/Users/dobromiriliev/Documents/GitHub/UrbanTree/route_coordinates.csv'
data = pd.read_csv(file_path)

# Extract longitude and latitude from the data
longitude = data['Longitude']
latitude = data['Latitude']

# Create a plot
plt.figure(figsize=(10, 6))
plt.scatter(longitude, latitude, color='Green', marker='o')  # Plot points
plt.plot(longitude, latitude, color='Black')  # Connect points with a line

# Adding titles and labels
plt.title('GSMST to Georgia Tech Route')
plt.xlabel('Longitude')
plt.ylabel('Latitude')

# Show grid
plt.grid(True)

# Display the plot
plt.show()
