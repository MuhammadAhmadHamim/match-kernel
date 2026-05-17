<div align="center">

<img width="100%" src="https://capsule-render.vercel.app/api?type=waving&color=0:07000f,40:1a0028,100:07000f&height=240&section=header&text=MatchKernel&fontSize=58&fontColor=bf00ff&fontAlignY=40&desc=Rank-Based%20Multiplayer%20Matchmaking%20in%20C%2B%2B&descAlignY=62&descColor=39ff14&animation=fadeIn&fontFamily=Georgia" alt="MatchKernel banner"/>

<br/>

![Language](https://img.shields.io/badge/C%2B%2B-17-bf00ff?style=for-the-badge&logo=cplusplus&logoColor=white)
![Frontend](https://img.shields.io/badge/Frontend-Vanilla%20JS-39ff14?style=for-the-badge&logo=javascript&logoColor=black)
![Type](https://img.shields.io/badge/Type-DS%20Semester%20Project-1a0028?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-MinGW%20g%2B%2B-bf00ff?style=for-the-badge&logo=windows&logoColor=white)
![Status](https://img.shields.io/badge/Status-Complete-39ff14?style=for-the-badge&logoColor=black)

<br/><br/>

> *"In matchmaking, the queue is the data structure. The wait is the cost. The match is the proof."*

<br/>

</div>

---

## ◈ What This Is

A rank-based **multiplayer matchmaking system** built as the Data Structures semester project. MatchKernel simulates the exact logic behind real game matchmaking — rank queues, sub-rank proximity, wait-time expansion, and cross-rank matching — all powered by queue-based data structures, a C++17 HTTP backend, and a live browser UI.

Not a simulation of matchmaking. An actual matchmaking engine.

---

## ◈ How the Engine Works

```
Phase 1  →  Same-rank players matched first
             Sub-rank proximity is the tiebreaker (I beats III)
             ↓
Phase 2  →  Unmatched players accumulate wait cycles
             ↓
Phase 3  →  Every 3 wait cycles, expansion radius grows by +1
             Players can now match one rank tier further away
             ↓
Phase 4  →  Remaining unmatched players reported
             Cross-rank matches flagged with a purple badge in UI
```

This mirrors how real matchmaking handles long queue times — the longer you wait, the wider the net casts.

---

## ◈ Tech Stack

<div align="center">

| Layer | Technology | Role |
|:---:|:---:|:---|
| **Backend** | C++17 + cpp-httplib | HTTP server, all endpoints, matchmaking engine |
| **Frontend** | HTML5, CSS3, Vanilla JS | Live UI, queue visualization, match display |
| **Build** | g++ MinGW, build.bat | One-command compile and launch |
| **Data** | In-memory + CSV input | Queue state, match history, bulk player loading |

</div>

---

## ◈ Project Structure

```
MatchKernel/
│
├── backend/
│   ├── main.cpp               ←  HTTP server and all endpoints
│   ├── httplib.h              ←  single-header server library
│   ├── Player.h               ←  Player struct and rank enums
│   ├── Match.h                ←  Match struct
│   ├── MatchmakingSystem.h    ←  core matchmaking engine
│   ├── JsonHelper.h           ←  JSON serialization
│   └── Utils.h                ←  string to enum parsers
│
├── frontend/
│   ├── index.html             ←  UI layout
│   ├── style.css              ←  dark theme styling
│   └── script.js              ←  API calls and DOM rendering
│
├── sample_data.csv            ←  demo player data
├── build.bat                  ←  one command build and launch
├── TECHNICAL_DOCS.md
├── API_REFERENCE.md
└── README.md
```

---

## ◈ The Matchmaking Logic — In Code

```cpp
// Phase 1 — Same-rank matching with sub-rank proximity priority
for(int i = 0; i < totalRanks; i++) {
    if(rankQueues[i].size() < 2) continue;

    // Convert queue to vector for flexible access
    std::vector<Player> bucket;
    while(!rankQueues[i].empty()) {
        bucket.push_back(rankQueues[i].front());
        rankQueues[i].pop();
    }

    std::vector<bool> matched(bucket.size(), false);

    for(int j = 0; j < (int)bucket.size(); j++) {
        if(matched[j]) continue;

        int bestIndex = -1;
        int bestDistance = INT_MAX;

        // Search all unmatched players after j
        for(int k = j + 1; k < (int)bucket.size(); k++) {
            if(matched[k]) continue;

            int distance = abs(static_cast<int>(bucket[j].subRank) -
                                        static_cast<int>(bucket[k].subRank));
            if(distance < bestDistance) {
                bestDistance = distance;
                bestIndex = k;
            }
        }

        // If a valid match was found
        if(bestIndex != -1) {
            matched[j] = true;
            matched[bestIndex] = true;

            Match m(++matchIdCounter, bucket[j], bucket[bestIndex]);
            formedMatches.push_back(m);
            matchesThisRound++;

            std::cout << "MATCHED (same-rank, sub-priority): ";
            m.display();
        }
    }

    // Push unmatched players back into queue
    for(int j = 0; j < (int)bucket.size(); j++) {
        if(!matched[j]) {
            rankQueues[i].push(bucket[j]);
        }
    }
}

// Phase 3 — Expansion: wait cycles unlock cross-rank matching
// For each rank bucket, check if front player can expand
for(int i = 0; i < totalRanks; i++){
    if(rankQueues[i].empty()) continue;

    Player p1 = rankQueues[i].front();
    int radius = p1.expansionRadius();
    if(radius == 0) continue;

    // Search adjacent buckets within radius
    bool matched = false;
    for(int offset = 1; offset <= radius && !matched; offset++){
        // Check bucket above
        int above = i + offset;
        if(above < totalRanks && !rankQueues[above].empty()){
            rankQueues[i].pop();
            Player p2 = rankQueues[above].front();
            rankQueues[above].pop();
            Match m(++matchIdCounter, p1, p2);
            formedMatches.push_back(m);
            matchesThisRound++;
            matched = true;
            std::cout << "Matched (EXPANDED):";
            m.display();
        }

        // Check bucket below
        int below = i - offset;
        if(!matched && below >= 0 && !rankQueues[below].empty()){
            rankQueues[i].pop();
            Player p2 = rankQueues[below].front();
            rankQueues[below].pop();
            Match m(++matchIdCounter, p1, p2);
            formedMatches.push_back(m);
            matchesThisRound++;
            matched = true;
            std::cout << "Matched (EXPANDED):";
            m.display();
        }
    }
}

```

---

## ◈ API Reference

<div align="center">

| Method | Endpoint | What It Does |
|:---:|:---:|:---|
| `GET` | `/queue-state` | Returns current queue as JSON |
| `GET` | `/matches` | Returns all formed matches |
| `GET` | `/stats` | Total players and match count |
| `POST` | `/add-player` | Adds one player to the queue |
| `POST` | `/matchmake` | Triggers one full matchmaking pass |
| `POST` | `/load-file` | Bulk loads players from CSV |

</div>

```bash
# Add a single player
curl -X POST http://localhost:8080/add-player \
     -d "username=Ace&rank=Gold&subrank=II"

# Bulk load from CSV
curl -X POST http://localhost:8080/load-file \
     --data-binary @sample_data.csv
```

---

## ◈ CSV Format

```csv
name,rank,subrank
Ace,Gold,II
Zoro,Gold,I
Levi,Platinum,III
Mikasa,Silver,I
```

<div align="center">

| Field | Valid Values |
|:---:|:---|
| **rank** | Bronze · Silver · Gold · Platinum · Diamond |
| **subrank** | I · II · III |

</div>

---

## ◈ How to Run

**Requirements:** Windows 10+ · MinGW g++ with C++17 support

```bash
# Clone
git clone https://github.com/MuhammadAhmadHamim/MatchKernel.git

# Build and launch — one command does everything
.\build.bat
```

Server compiles, starts on `http://localhost:8080`, and the browser opens automatically.

---

## ◈ Demo Flow

```
1.  UI loads → rank queues display automatically
2.  Add players manually OR upload sample_data.csv
3.  Watch rank queues populate in real time
4.  Hit Run Matchmaking → matches appear instantly
5.  Cross-rank matches flagged with a purple badge
6.  Run multiple rounds → watch expansion logic activate
```

---

## ◈ Data Structures Applied

<div align="center">

| Structure | Where It Appears |
|:---:|:---|
| **Queue** | Per-rank player queues — core of the matchmaking engine |
| **Vector** | Match history, unmatched player tracking |
| **Vector of Queues** | Rank-to-queue mapping for O(1) queue access |
| **Enum** | Rank and subrank type safety across the entire system |
| **Struct** | Player and Match as clean, typed data containers |

</div>

---

## ◈ Skills This Project Demonstrates

<div align="center">

![](https://img.shields.io/badge/C%2B%2B-Queue%20Based%20Engine-1a0028?style=flat-square&logo=cplusplus&logoColor=bf00ff)
![](https://img.shields.io/badge/C%2B%2B-REST%20API%20Server-bf00ff?style=flat-square&logo=cplusplus&logoColor=white)
![](https://img.shields.io/badge/DSA-Rank%20Expansion%20Logic-39ff14?style=flat-square&logo=buffer&logoColor=black)
![](https://img.shields.io/badge/JavaScript-Live%20UI%20Rendering-1a0028?style=flat-square&logo=javascript&logoColor=39ff14)
![](https://img.shields.io/badge/System%20Design-Multi%20Phase%20Engine-bf00ff?style=flat-square&logo=buffer&logoColor=white)
![](https://img.shields.io/badge/CSV-Bulk%20Data%20Loading-39ff14?style=flat-square&logo=files&logoColor=black)
![](https://img.shields.io/badge/HTTP-cpp--httplib%20Integration-1a0028?style=flat-square&logo=fastapi&logoColor=bf00ff)
![](https://img.shields.io/badge/Build-bat%20Automation-bf00ff?style=flat-square&logo=windows&logoColor=white)

</div>

---

## ◈ A Note on This Project

Every matchmaking system in every game you've ever played runs on some version of this logic — ranked queues, wait time expansion, cross-tier fallback. MatchKernel is that logic stripped to its core, built from scratch in C++, and wired to a live UI that shows it working in real time.

Data structures aren't theory here. They're the engine.

---

<div align="center">

<img width="100%" src="https://capsule-render.vercel.app/api?type=waving&color=0:1a0028,50:07000f,100:1a0028&height=120&section=footer&animation=fadeIn" alt="footer"/>

*The queue decides everything. The algorithm decides the rest.*

</div>