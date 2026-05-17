# Contributing to MatchKernel

---

## Project Overview

MatchKernel is a rank-based matchmaking system with a C++ backend and vanilla JS frontend. The backend exposes a REST API consumed by the frontend via fetch calls.

Full architecture and design decisions are documented in `TECHNICAL_DOCS.md`.
All endpoints are documented in `API_REFERENCE.md`.

---

## Requirements

- Windows 10 or later
- MinGW g++ with C++17 support
- A modern browser (Chrome, Firefox, Edge)

---

## Building

From the project root:

```bash
.\build.bat
```

This compiles the backend, launches the server on port 8080, and opens the frontend in your default browser automatically.

To compile only without launching:

```bash
g++ backend/main.cpp -std=c++17 -o server -D_WIN32_WINNT=0x0A00 -lws2_32 -lwininet -static-libgcc -static-libstdc++
```

---

## Project Structure

```
MatchKernel/
├── backend/
│   ├── main.cpp               ← HTTP server and endpoint handlers
│   ├── httplib.h              ← cpp-httplib single header library
│   ├── Player.h               ← Player struct, Rank and SubRank enums
│   ├── Match.h                ← Match struct
│   ├── MatchmakingSystem.h    ← core engine, queue management
│   ├── JsonHelper.h           ← JSON serialization helpers
│   └── Utils.h                ← string to enum conversion
├── frontend/
│   ├── index.html             ← page structure and element IDs
│   ├── style.css              ← dark theme, component styles
│   └── script.js              ← API layer, render layer, event wiring
├── sample_data.csv            ← demo CSV with 12 players
├── build.bat                  ← build and launch script
├── .gitignore
├── README.md
├── TECHNICAL_DOCS.md
└── API_REFERENCE.md
```

---

## Adding a New Backend Endpoint

1. Add your handler in `backend/main.cpp` following the existing pattern:

```cpp
svr.Get("/your-endpoint", [](const httplib::Request& req,
                              httplib::Response& res) {
    std::cout << "GET /your-endpoint hit!\n";
    setCORSHeaders(res);
    // your logic here
    res.set_content(yourJson, "application/json");
});
```

2. Call `setCORSHeaders(res)` at the start of every handler — without this the browser will block the response.

3. If your endpoint needs POST body params, use the getValue lambda pattern:

```cpp
auto getValue = [&](const string& key) -> string {
    string search = key + "=";
    size_t start = body.find(search);
    if(start == string::npos) return "";
    start += search.length();
    size_t end = body.find('&', start);
    if(end == string::npos) end = body.length();
    return body.substr(start, end - start);
};
```

4. Add the corresponding function in `frontend/script.js` following the API layer pattern:

```javascript
async function yourEndpoint() {
  const data = await fetchJSON(`${API_BASE}/your-endpoint`);
  if (data && data.status === 'success') return data.yourField;
  return null;
}
```

5. Document the new endpoint in `API_REFERENCE.md`.

---

## Adding a New Field to Player

1. Add the field to the `Player` struct in `backend/Player.h`
2. Initialize it in the constructor initializer list
3. Add it to `playerToJson()` in `backend/JsonHelper.h`
4. Update `display()` in `Player.h` if it should appear in terminal output
5. Update the frontend render functions in `script.js` if it should appear in the UI

---

## Code Conventions

**Backend (C++):**
- All header files use include guards (`#ifndef / #define / #endif`)
- All JSON serialization goes through `JsonHelper.h` — never build JSON strings in endpoint handlers directly
- All string to enum conversion goes through `Utils.h`
- All game logic goes in `MatchmakingSystem.h` — `main.cpp` handles only HTTP concerns
- Use `std::` prefix explicitly — avoid `using namespace std` in header files

**Frontend (JS):**
- All fetch calls go through the central `fetchJSON()` wrapper
- All user-generated content injected into DOM must go through `escapeHtml()`
- UI updates always go through `refreshAll()` after any data-mutating action
- No inline event handlers in HTML — all wiring happens in `DOMContentLoaded`

---

## What Not To Commit

The `.gitignore` covers these but worth knowing explicitly:

- `server.exe` — compiled binary, machine-specific, rebuild with build.bat
- `*.exe`, `*.o`, `*.obj` — all build artifacts
- `.vscode/` — editor settings, personal to each developer

---

## Testing an Endpoint

Use curl from the project root directory:

```bash
# GET endpoints
curl http://localhost:8080/queue-state
curl http://localhost:8080/matches
curl http://localhost:8080/stats

# POST add-player
curl -X POST http://localhost:8080/add-player -d "username=Ace&rank=Gold&subrank=II"

# POST matchmake
curl -X POST http://localhost:8080/matchmake

# POST load-file
curl -X POST http://localhost:8080/load-file --data-binary @sample_data.csv
```

Run one command at a time. Each should return a JSON response with `"status":"success"`.
