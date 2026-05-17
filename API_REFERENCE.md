# MatchKernel — API Reference

Base URL: `http://localhost:8080`

All responses are JSON. All endpoints include CORS headers so browser fetch calls work without restriction.

---

## GET /queue-state

Returns the current state of all five rank queues.

**Request:**
```
GET http://localhost:8080/queue-state
```

**Response:**
```json
{
  "status": "success",
  "queues": {
    "Bronze": [],
    "Silver": [
      {
        "id": 4,
        "username": "Mikasa",
        "rank": "Silver",
        "subrank": "I",
        "waitCycles": 0,
        "expansionRadius": 0
      }
    ],
    "Gold": [
      {
        "id": 1,
        "username": "Ace",
        "rank": "Gold",
        "subrank": "II",
        "waitCycles": 0,
        "expansionRadius": 0
      },
      {
        "id": 2,
        "username": "Zoro",
        "rank": "Gold",
        "subrank": "I",
        "waitCycles": 0,
        "expansionRadius": 0
      }
    ],
    "Platinum": [],
    "Diamond": []
  }
}
```

**Player object fields:**

| Field           | Type    | Description                                      |
|-----------------|---------|--------------------------------------------------|
| id              | integer | Unique player ID assigned at creation            |
| username        | string  | Player name                                      |
| rank            | string  | Current rank tier                                |
| subrank         | string  | Sub-tier within rank                             |
| waitCycles      | integer | How many rounds this player has waited unmatched |
| expansionRadius | integer | Current search radius (waitCycles / 3)           |

---

## GET /matches

Returns all matches formed during this session in order of creation.

**Request:**
```
GET http://localhost:8080/matches
```

**Response:**
```json
{
  "status": "success",
  "matches": [
    {
      "matchId": 1,
      "player1": {
        "id": 1,
        "username": "Ace",
        "rank": "Gold",
        "subrank": "II",
        "waitCycles": 0,
        "expansionRadius": 0
      },
      "player2": {
        "id": 2,
        "username": "Zoro",
        "rank": "Gold",
        "subrank": "I",
        "waitCycles": 0,
        "expansionRadius": 0
      },
      "isCrossRank": false
    }
  ]
}
```

**Match object fields:**

| Field       | Type    | Description                                             |
|-------------|---------|---------------------------------------------------------|
| matchId     | integer | Unique match ID                                         |
| player1     | object  | Full player object                                      |
| player2     | object  | Full player object                                      |
| isCrossRank | boolean | True if players are from different rank tiers           |

**Empty response (no matches yet):**
```json
{
  "status": "success",
  "matches": []
}
```

---

## GET /stats

Returns session-level statistics.

**Request:**
```
GET http://localhost:8080/stats
```

**Response:**
```json
{
  "totalPlayers": 3,
  "totalMatches": 1
}
```

| Field        | Type    | Description                              |
|--------------|---------|------------------------------------------|
| totalPlayers | integer | Players currently waiting in all queues  |
| totalMatches | integer | Total matches formed this session        |

---

## POST /add-player

Adds a single player to the appropriate rank queue.

**Request:**
```
POST http://localhost:8080/add-player
Content-Type: application/x-www-form-urlencoded

username=Ace&rank=Gold&subrank=II
```

**Body parameters:**

| Parameter | Required | Values                                      |
|-----------|----------|---------------------------------------------|
| username  | Yes      | Any non-empty string                        |
| rank      | Yes      | Bronze, Silver, Gold, Platinum, Diamond     |
| subrank   | Yes      | I, II, III                                  |

**Success response:**
```json
{
  "status": "success",
  "success": "Player Ace added to queue"
}
```

**Error response (missing fields):**
```json
{
  "status": "error",
  "message": "Missing fields"
}
```

**curl example:**
```bash
curl -X POST http://localhost:8080/add-player -d "username=Ace&rank=Gold&subrank=II"
```

---

## POST /matchmake

Triggers one complete matchmaking pass through all rank queues.

The engine runs four phases:
1. Same-rank matching with sub-rank proximity priority
2. Wait cycle increment for all unmatched players
3. Expansion matching for players with radius greater than 0
4. Statistics report to server terminal

**Request:**
```
POST http://localhost:8080/matchmake
```

No body required.

**Response:**
```json
{
  "status": "success",
  "message": "Matchmaking executed"
}
```

After this call, fetch `/queue-state` and `/matches` to see updated results.

**curl example:**
```bash
curl -X POST http://localhost:8080/matchmake
```

---

## POST /load-file

Bulk loads players from raw CSV text. Parses each row and adds players to their respective rank queues.

**Request:**
```
POST http://localhost:8080/load-file
Content-Type: text/plain

name,rank,subrank
Ace,Gold,II
Zoro,Gold,I
Levi,Platinum,III
```

**CSV format:**

| Column  | Required | Values                                  |
|---------|----------|-----------------------------------------|
| name    | Yes      | Player username                         |
| rank    | Yes      | Bronze, Silver, Gold, Platinum, Diamond |
| subrank | Yes      | I, II, III                              |

The first row must be the header `name,rank,subrank`. Rows with missing or empty fields are silently skipped. Windows carriage returns (`\r`) are handled automatically.

**Success response:**
```json
{
  "status": "success",
  "message": "12 players loaded"
}
```

**curl example:**
```bash
curl -X POST http://localhost:8080/load-file --data-binary @sample_data.csv
```

---

## Error Handling

All endpoints return a consistent error envelope:

```json
{
  "status": "error",
  "message": "Description of what went wrong"
}
```

Common error scenarios:

| Scenario                        | Endpoint       | Message          |
|---------------------------------|----------------|------------------|
| Missing username/rank/subrank   | /add-player    | Missing fields   |
| Empty CSV body                  | /load-file     | 0 players loaded |

---

## Response Status Values

| Status    | Meaning                              |
|-----------|--------------------------------------|
| success   | Request processed successfully       |
| error     | Validation failed or bad input       |
