# Campus Compass

Campus Compass is a C++ command-line application that combines a campus navigation
graph with a student schedule manager. The system loads campus map data and class
metadata from CSV files and processes commands to manage students, verify schedules,
and compute graph-based queries.

## Project Components
- `main.cpp` — program entry point; loads CSV data and processes commands
- `CampusCompass.*` — command parsing and system orchestration
- `Graph.*` — adjacency-list graph supporting:
  - Breadth-First Search (BFS)
  - Dijkstra’s shortest-path algorithm
  - Edge open/close toggling
  - Minimum spanning tree cost (Prim’s algorithm)
- `ClassCatalog.*` — class metadata (location, start/end time)
- `StudentManager.*` / `Student.h` — student records and schedule validation

## Data Files
The program expects the following CSV files:

### `data/edges.csv`
Columns:
- `LocationID_1`, `LocationID_2`
- `Name_1`, `Name_2`
- `Time` (edge weight / travel time)

### `data/classes.csv`
Columns:
- `ClassCode`
- `LocationID`
- `Start Time (HH:MM)`
- `End Time (HH:MM)`

Building names and class codes are based on publicly available University of Florida
information.

## How to Run
Compile all source files:
```bash
g++ src/*.cpp -o campus-compass
