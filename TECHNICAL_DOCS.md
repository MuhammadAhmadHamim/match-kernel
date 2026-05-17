# MatchKernel — Technical Documentation

---

## Problem Statement

Modern multiplayer games require intelligent matchmaking that balances fairness with queue efficiency. A naive system that matches anyone with anyone produces unfair games. A system that only matches identical ranks produces long wait times. MatchKernel solves this by implementing a tiered approach — strict same-rank matching first, with intelligent expansion for players who wait too long — simulating how real production matchmaking systems operate.

---

## System Architecture

```
Browser (HTML + CSS + Vanilla JS)
          ↕  HTTP fetch() calls
C++ Server (cpp-httplib, port 8080)
          ↕  function calls
MatchmakingSystem class
          ↕  operates on
vector<queue<Player>>  +  vector<Match>
```

The system is divided into four clean layers with strict separation of concerns:

- Frontend — rendering, user input, API consumption
- HTTP Server — receiving requests, sending responses, CORS handling
- MatchmakingSystem — all game logic, data management
- Data Structures — Player, Match, the queue system

---

## Data Structure Justification

### Core Structure: vector<queue<Player>>

A vector of queues where each index corresponds to a rank tier:

```
rankQueues[0] → Bronze queue
rankQueues[1] → Silver queue
rankQueues[2] → Gold queue
rankQueues[3] → Platinum queue
rankQueues[4] → Diamond queue
```

Why this structure specifically:

**Vector** provides O(1) index access to each rank bucket. The Rank enum assigns integer values 0-4 to each tier, so `rankQueues[static_cast<int>(rank)]` is a direct, zero-overhead lookup. No searching, no hashing.

**Queue** enforces FIFO — the first player to join a rank tier is the first to be matched. This is fairness by design. A player cannot be skipped by someone who joined after them within the same rank.

**Alternative considered: map<Rank, queue<Player>>** — rejected because map provides O(log n) access and requires a comparator. The enum integer indexing into a vector is cleaner, faster, and more readable.

**Alternative considered: priority_queue** — rejected because priority queues reorder elements by weight, which would break FIFO fairness. Wait time is tracked separately through the waitCycles field rather than baked into a single priority value.

### Secondary Structure: vector<Match>

A flat vector that accumulates every match formed across all matchmaking rounds. Never cleared during a session. Serves as complete match history, directly serialized for the GET /matches endpoint.

---

## File by File Breakdown

### Player.h

**Enums:**

`Rank` — five rank tiers as named integers: Bronze=0, Silver=1, Gold=2, Platinum=3, Diamond=4. Integer values are intentional — they enable mathematical distance calculation between ranks during expansion logic. `abs(rank1 - rank2)` gives the tier distance between any two players.

`SubRank` — three sub-tiers as I=1, II=2, III=3. Integer values enable sub-rank distance calculation for priority matching. `abs(subrank1 - subrank2)` gives proximity score, 0 being a perfect sub-rank match.

**struct Player fields:**

- `playerId` — unique integer identifier assigned by MatchmakingSystem's counter. Allows tracking individual players even when names collide
- `name` — player username as string
- `rank` — stored as Rank enum, not string
- `subRank` — stored as SubRank enum, not string
- `waitCycles` — incremented every matchmaking pass the player sits unmatched, starts at 0

**struct Player methods:**

- `expansionRadius()` — returns `waitCycles / 3`. The intelligence function of the entire engine. Every 3 unmatched cycles the search radius grows by 1. Integer division means the jump is discrete and predictable — radius 0 for cycles 0-2, radius 1 for cycles 3-5, radius 2 for cycles 6-8, and so on
- `display()` — formatted terminal output for debugging and server logs
- `rankToString()` — enum to string conversion via switch statement. Rank::Gold becomes "Gold". Required for JSON serialization since enums cannot be directly streamed
- `subRankToString()` — same pattern for sub-rank. SubRank::II becomes "II"

### Match.h

**struct Match fields:**

- `player1`, `player2` — full Player objects stored by value, not pointer. Match is self-contained
- `matchId` — unique integer assigned at creation time by MatchmakingSystem counter
- `isCrossRank` — boolean computed at construction: `p1.rank != p2.rank`. Set automatically, never manually. True means the match came from expansion logic, false means same-rank match

**struct Match methods:**

- `Match(p1, p2, id)` — constructor uses initializer list. `isCrossRank` is evaluated inline during initialization: `isCrossRank(p1.rank != p2.rank)`
- `display()` — prints both players with ranks and appends [CROSS-RANK] tag conditionally

### JsonHelper.h

All methods are static. No instance needed.

- `playerToJson(Player)` — serializes one Player to JSON object string. String values wrapped in escaped quotes, numeric values left bare. Produces valid JSON directly consumable by frontend's JSON.parse()
- `matchToJson(Match)` — serializes one Match. Nests two full player JSON objects. `isCrossRank` serialized as literal `true` or `false` not as a string
- `matchesToJson(vector<Match>)` — serializes entire match history to JSON array. Handles comma separation, returns `[]` for empty vector
- `queueToJson(queue<Player>)` — takes queue by value (copy) so original is never modified. Drains the copy via front/pop to serialize in FIFO order
- `successResponse(key, value)` — wraps content in `{"status":"success","key":value}` envelope. Consistent response shape across all successful endpoints
- `errorResponse(message)` — produces `{"status":"error","message":"..."}`. Used when request validation fails

### Utils.h

All methods are static.

- `parseRank(string)` — inward journey function. Converts "Gold" to Rank::Gold. Returns Rank::Bronze as default fallback for unrecognized input
- `parseSubRank(string)` — converts "II" to SubRank::II. Returns SubRank::I as fallback

These are the inverse of Player's toString methods. Player.h handles enum to string (output). Utils.h handles string to enum (input).

### MatchmakingSystem.h

**Private fields:**

- `rankQueues` — the core vector<queue<Player>> structure, 5 elements
- `formedMatches` — vector<Match> persisting all matches across rounds
- `totalRanks` — integer constant 5, bounds all rank loops
- `matchIdCounter` — increments with every match formed
- `playerIdCounter` — increments with every player added

**Public methods:**

`MatchmakingSystem()` — constructor calls `rankQueues.resize(5)` to initialize 5 empty queues before any player arrives.

`addPlayer(name, rank, subRank)` — creates Player with internal counter, pushes into correct bucket via `rankQueues[static_cast<int>(rank)].push(p)`.

`loadPlayersFromCSV(csvContent)` — takes raw CSV string from HTTP body, skips header row, parses each line by comma delimiter, strips Windows carriage returns, calls addPlayer per valid row, returns loaded count.

`runMatchmaking()` — the heart of the engine. Four phases:

Phase 1 — Same-rank matching with sub-rank priority. Converts each queue to a vector for random access. Uses nested loop with running minimum pattern — outer loop picks each unmatched player, inner loop finds closest sub-rank among remaining unmatched players. `matched[]` flag array tracks state within the round. Unmatched players pushed back into queue after the pass.

Phase 2 — Increments waitCycles for all remaining unmatched players. Uses copy-drain-rebuild pattern since queue only exposes front element. waitCycles increment indirectly grows expansionRadius.

Phase 3 — Expansion matching. For each bucket's front player, checks expansionRadius. If zero, skips entire bucket (FIFO guarantees front player waited longest — if they haven't expanded, nobody behind them has either). For radius > 0, checks adjacent buckets outward from nearest to farthest. Forms cross-rank match on first candidate found.

Phase 4 — Reports unmatched players and round statistics to terminal.

`findBestMatch(player, candidates)` — running minimum helper for Phase 1. Calculates `abs(subrank1 - subrank2)` for each candidate, tracks lowest distance and its index, returns index of best match. Returns -1 if no candidates. Uses INT_MAX as initial bestDistance so first candidate always sets the baseline.

`getQueueStateJson()` — iterates all 5 buckets, serializes each via JsonHelper::queueToJson, wraps in JSON object with rank names as keys.

`getMatchesJson()` — passes formedMatches to JsonHelper::matchesToJson.

`getStatsJson()` — builds JSON with getTotalPlayers() and getTotalMatches() counts.

`getTotalPlayers()` — sums all queue sizes.

`getTotalMatches()` — returns formedMatches.size().

### main.cpp

**Global state:**

`matchSystem` — single global MatchmakingSystem instance. Persists across all HTTP requests for the server lifetime. In-memory state resets when server restarts.

**Helper functions:**

`setCORSHeaders(res)` — adds Access-Control-Allow-Origin: *, Methods, and Headers to every response. Called at the start of every handler so browser fetch calls are never blocked regardless of origin.

`getValue(body, key)` — lambda defined inside POST /add-player handler. Manually parses URL-encoded body string by finding key= and extracting value until next & or end of string. Used because cpp-httplib's `get_param_value()` reads URL query strings, not POST body params.

**Endpoints:**

- `OPTIONS .*` — handles browser preflight CORS checks, returns 204
- `GET /queue-state` — calls getQueueStateJson(), wraps in success envelope
- `GET /matches` — calls getMatchesJson(), wraps in success envelope
- `GET /stats` — calls getStatsJson(), returns directly
- `POST /add-player` — parses body manually via getValue lambda, validates all three fields, calls addPlayer, returns confirmation
- `POST /matchmake` — calls runMatchmaking(), returns success message
- `POST /load-file` — passes req.body directly to loadPlayersFromCSV, returns count

---

## Matchmaking Algorithm — Time Complexity

| Phase | Operation | Complexity |
|-------|-----------|------------|
| Phase 1 | Same-rank matching per bucket | O(n²) where n = players in bucket |
| Phase 2 | Wait cycle increment | O(n) total unmatched players |
| Phase 3 | Expansion search | O(r) where r = expansion radius, max 4 |
| Overall | Per matchmaking round | O(n) dominated by Phase 2 |

Phase 1 is technically O(n²) but in practice n per bucket is small. Players cluster in the same rank tier and sub-rank combinations are limited to 3 values, so realistic bucket sizes stay in single digits.

---

## Key Design Decisions

### Why expansion radius grows in the same round it's earned

waitCycles increments in Phase 2, expansionRadius is consumed in Phase 3 of the same pass. A player earns their expanded radius and uses it immediately rather than waiting until the next round. This minimizes wait time and is more player-friendly.

### Why queue is drained to vector for Phase 1

Queues only expose their front element — random access is impossible. Converting to vector enables the nested loop pairing algorithm. The queue is rebuilt from unmatched vector elements after the pass. This is a deliberate tradeoff: O(n) conversion cost for clean matching logic.

### Why FIFO guarantees the expansion skip optimization

In Phase 3, if the front player of a bucket has expansionRadius 0, the entire bucket is skipped. This works because queue FIFO guarantees the front player waited longest. If they haven't expanded, nobody behind them has. This turns a potential O(n) scan into an O(1) early exit.

### Why manual JSON building over a library

At this project's scale, string concatenation JSON is sufficient and transparent. Every field is visible and debuggable without library abstraction. It also demonstrates understanding of the JSON format rather than importing magic.

### Why manual body parsing over get_param_value()

cpp-httplib's `get_param_value()` reads URL query strings (?key=value), not POST body params. Manual parsing with a getValue lambda correctly reads the URL-encoded body that the frontend sends via URLSearchParams.

---

## Frontend Architecture

### API Layer
All fetch calls go through `fetchJSON()` — a central wrapper with try/catch, HTTP status checking, and null return on failure. Every endpoint has a dedicated function that calls fetchJSON and extracts the relevant field from the response.

### Render Layer
`renderQueues()` iterates ranks in fixed order (not object key order which is unreliable across browsers). `renderMatches()` reverses the array with `.slice().reverse()` so newest matches appear first. `escapeHtml()` sanitizes all user-generated content before DOM injection, preventing XSS.

### Coordination
`refreshAll()` uses `Promise.all()` to fire queue state and matches requests simultaneously rather than sequentially. Both resolve before either renders, keeping the UI consistent.

### Event Layer
All button wiring happens inside `DOMContentLoaded`. `refreshAll()` fires once on load so the UI populates immediately without user interaction.

---

## What Would Be Added With More Time

- Persistent storage — SQLite or file-based so match history survives server restarts
- Player authentication — unique accounts with match history per player
- Real-time updates — WebSocket push so the UI updates without polling
- Team matchmaking — 5v5 formation instead of 1v1 pairs
- Match result recording — win/loss tracking that affects rank over time
- Rate limiting — prevent spam adding players via the API
