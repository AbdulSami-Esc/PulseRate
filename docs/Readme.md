# PulseRoute

**Reroute only what broke.**

PulseRoute is a hospital-robot routing engine written in C++ from scratch. When a corridor gets blocked, it finds the few robots that were actually using that corridor and re-plans only their routes, instead of recalculating the whole building.

Built for the Data Structures course (CS2001) at FAST-NUCES, Karachi, by Muhammad Hashaam, Abdul Sami and Shaheer Hassan (BAI-3C).

---

## Why this exists

Hospitals already use autonomous robots to carry medicines, lab samples and supplies through their corridors. The best-known examples are Aethon's TUG robots (made in Pittsburgh, USA, and now part of Singapore's ST Engineering), Swisslog's RoboCourier, and Diligent Robotics' Moxi. They are used in hospitals across the US, and in other countries too.

Hospital corridors are unpredictable. A stretcher, a cleaning cart or a lockdown can close a hallway at any moment. The simple way to handle that is to recompute everyone's route. We wanted to see how much cheaper it is to fix only the robots that were affected, and to build every data structure ourselves to understand why it works.

PulseRoute is a course project and a simulation. It is not affiliated with those companies, and their real systems are far more complex than this one.

---

## How it works

Follow one delivery from start to finish:

1. **Load the map.** `nodes.csv` (rooms, junctions, elevators) and `edges.csv` (corridors with travel seconds) are read into a weighted graph stored as adjacency lists. Each room name goes into a hash table so `"ICU_Room_4"` becomes node `4` instantly.
2. **A request arrives.** For example: *blood sample to ICU, STAT*. Requests wait in a priority queue built on a min-heap, so emergencies go before routine supplies. Requests that wait too long slowly become more urgent (aging), so nothing starves.
3. **A free robot is chosen.** The dispatcher gives the most urgent request to an idle robot.
4. **The route is calculated.** Dijkstra's algorithm, powered by our own min-heap, finds the fastest path. Elevators are just normal nodes, so multi-floor routes need no special code.
5. **We remember who uses what.** A reverse index maps each corridor to the robots whose remaining route passes through it.
6. **A corridor gets blocked.** One hash lookup in the reverse index returns only the affected robots. Each is re-routed from its current position. Everyone else is untouched. If no route exists, the robot waits as "stuck" and recovers when the corridor reopens.
7. **The system adapts.** Every tick, corridor costs are recomputed as base time, plus a congestion cost for each robot planning to use it, plus a risk penalty learned from past blockages with an exponential moving average. Dijkstra is unchanged because it reads these live costs.
8. **You operate it.** A command-line interface drives everything, with a trie for room-name suggestions. A browser viewer shows the robots moving.

```
nodes.csv + edges.csv
        |
   MapLoader ──> Graph (adjacency lists)      HashTable (name -> id)
                        |                            |
 requests ──> DispatchQueue (MinHeap) ──> Dispatcher
                                              |
                      RobotFleet ── Dijkstra (MinHeap) ── Router
                           |
                     ReverseIndex   "who uses corridor A-B?"
                           |
 block A B ──> reroute only those robots
                           |
 WeightModel (congestion + learned risk) ──> live edge weights
```

---

## Data structures from the course, and where we use them

No `std::vector`, `std::map`, `std::unordered_map` or `std::priority_queue` anywhere. Everything below is our own code.

| Data structure / concept | File | What it does in PulseRoute |
|---|---|---|
| Dynamic safe array, Rule of Three | `DynamicArray.h` | Our own `vector`. Holds adjacency lists, robot routes, heap storage and hash buckets. Bounds-checked, with deep copy. |
| Hash table, chaining, rehashing | `HashTable.h` | Room name to node ID. Also maps corridors to slots in the reverse index. |
| Min-heap (complete binary tree in an array) | `MinHeap.h` | Gives Dijkstra the closest node next, and gives dispatch the most urgent request. |
| Priority queue | `DispatchQueue.h` | Orders requests by urgency, with arrival order as tie-break and aging. |
| Graph, adjacency list | `Graph.h` | The hospital map. Each corridor is two directed edges so each direction can have its own cost. |
| BFS (queue-based traversal) | `Graph.h` | Checks the map is connected and counts reachable rooms. |
| Shortest path (Dijkstra) | `Dijkstra.h` | Fastest route, skipping blocked corridors, starting from any node. |
| Hashing + lists | `ReverseIndex.h` | Corridor to robots. This is the core of local rerouting. |
| Trie (multi-way tree) | `Trie.h` | Prefix search for room names ("Did you mean..."). Recursive collect and destroy. |
| Queue ADT / simulation | `Simulator.h` | Tick loop: update weights, age requests, dispatch, move robots. |
| Recursion | `Trie.h`, `Dijkstra.h` | Trie traversal, and rebuilding a path from the `prev` array. |

---

## The "smart" part

This is lightweight, explainable adaptation, not deep learning. It needs no GPU and almost no memory, which makes it a good fit for small edge devices.

- **Congestion-aware routing.** `weight = base + 5s × robots planning to use the corridor`. Crowded corridors become less attractive before a jam forms.
- **Blockage-risk learning.** Each block updates `risk = 0.8 × risk + 0.2`, and risk fades slowly each tick. Corridors that keep blocking get a time penalty, so routes lean away from them.
- **Priority aging.** Waiting requests gradually become more urgent.

---

## Try it

You need a C++17 compiler (g++) and a browser. No other libraries.

```bash
g++ -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined src/main.cpp -o pulseroute
./pulseroute data
```

Sample session:

```
> dispatch ICU_Room_4 STAT Blood_sample
> dispatch Ward_201 ROUTINE Cleaning supplies
> tick 2
> robots
> block Junction_B ICU_Room_4
> risk
> find ICU
```

Commands: `dispatch`, `block`, `unblock`, `tick [n]`, `status`, `robots`, `risk`, `find`, `help`, `quit`.

**Web viewer:** open `web/index.html` in any browser. Click a corridor to block it and watch the affected robot take a new route. The viewer is a JavaScript re-implementation of the same logic, so it works offline. It is not yet fed by the C++ program's output.

**Tests:** each data structure has its own test in `tests/`, built with AddressSanitizer on, so leaks and invalid memory access are caught. The routing test also checks Dijkstra against a slow reference solution on a random 200-node map.

---

## Design decisions

| Choice | Why |
|---|---|
| Adjacency list, not matrix | Hospitals are sparse (2 to 4 corridors per room), so memory scales with corridors, not rooms squared. |
| `blocked` flag instead of infinite weight | The original travel time is kept, so reopening restores the corridor exactly. |
| `baseWeight` and `weight` stored separately | Congestion and risk change `weight`, so Dijkstra never had to change. |
| Lazy duplicates in the heap | No decrease-key needed. Stale entries are skipped when popped. Simpler and still O((V+E) log V). |
| Chaining for collisions | Simple, never "full", and matches the course. |
| Robots hop node to node, one node per tick | Keeps the simulation easy to reason about. |
| Two-way corridors | Each direction is stored separately, so costs can differ. |

---

## Limitations

- It is a simulation. There is no real hardware and no physics.
- Robots are points that jump between nodes, so a blockage never hits a robot mid-corridor.
- Corridors are two-way. Single-lane locking is a possible extension.
- When a corridor reopens, only stuck robots are re-planned. Moving robots keep their route.
- The test map is small and hand-made.

## What we learned

- Why the Rule of Three exists: a shallow copy gives a double free, and AddressSanitizer shows it immediately.
- A stale index is worse than no index. Keeping the reverse index in sync on every assign, move and reroute was the hardest part.
- Keeping `baseWeight` and `weight` apart in week one saved us a rewrite in the final phase.

---

## Project structure

```
PulseRoute/
├── include/   all headers (data structures, graph, routing, fleet, dispatch, CLI)
├── src/       main.cpp
├── data/      nodes.csv, edges.csv
├── tests/     one test per component
├── web/       index.html (browser viewer)
└── README.md
```

## Coming next

Benchmarks on 1,000+ node maps (local re-route vs full recalculation, and risk model on vs off) and a demo video. Results will be added here once measured.

---

*Course: CS2001 Data Structures, FAST-NUCES Karachi, Fall 2026.*