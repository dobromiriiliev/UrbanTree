# UrbanTree

A Python-based application for analyzing and visualizing urban tree coverage, designed by Dobromir Iliev.

## Overview

UrbanTree is a tool that enables users to:

- Import and process geospatial tree data within urban environments  
- Visualize tree distribution across neighborhoods or city areas  
- Analyze tree density, species diversity, and spatial patterns

The project leverages GIS libraries to provide insights into urban forestry, green space planning, and environmental monitoring.

## Features

- **Data Import**: Load tree datasets (e.g., CSV with latitude/longitude, species, diameter)  
- **Visualization**: Map-based graphics showcasing tree locations, with overlays for heatmaps or clustering  
- **Analysis Tools**:
  - Tree density calculations per region or ZIP code
  - Species distribution statistics
  - Interactive map filtering by species, size, or other attributes

## Requirements

- Python 3.8+  
- Required libraries:
  - `geopandas`
  - `shapely`
  - `matplotlib`
  - `folium` _(optional for interactive maps)_
  - `pandas`

Install dependencies via:

```bash
pip install geopandas shapely matplotlib folium pandas
