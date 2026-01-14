# Campus Compass

Campus Compass is a C++ command-line application that combines a campus navigation
graph with a student schedule manager. It loads campus edges and class metadata
from CSV files, then processes commands to manage students, validate schedules,
and compute route/graph outputs.

## Project Structure (matches source files)
- `main.cpp` — CLI entry point; loads `data/edges.csv` + `data/classes.csv`, then reads commands
- `CampusCompass.cpp/.h` — command parsing + orchestration across graph/catalog/student systems
- `Graph.cpp/.h` — adjacency-list graph with:
  - BFS connectivity checks
  - Dijkstra shortest paths (open edges only)
  - Edge open/close toggling + edge status checks
  - MST cost via Prim’s algorithm on induced subgraphs
- `ClassCatalog.cpp/.h` — stores class info (locationId, start/end times)
- `StudentManager.cpp/.h` + `Student.h` — student records + schedule operations

## Data Files

This project expects the following CSVs:

### `data/edges.csv`
Columns:
- `LocationID_1`, `LocationID_2` (integer node IDs)
- `Name_1`, `Name_2` (building/location names)
- `Time` (edge weight / travel time)

### `data/classes.csv`
Columns:
- `ClassCode` (e.g., COP3502)
- `LocationID` (building/location node ID)
- `Start Time (HH:MM)`, `End Time (HH:MM)`

The program loads `data/edges.csv` and `data/classes.csv` at runtime.

## Supported Commands
Commands are parsed in `CampusCompass` and include:
- `insert`
- `remove`
- `dropClass`
- `replaceClass`
- `removeClass`
- `toggleEdgesClosure`
- `checkEdgeStatus`
- `isConnected`
- `printShortestEdges`
- `printStudentZone`
- `verifySchedule`

## Notes
This project was completed as part of a Data Structures & Algorithms course and focuses on
graph algorithms, data parsing, and command-driven program design.
Data Note: Building names and course codes used are publicly available UF information.
