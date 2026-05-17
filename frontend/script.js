const API_BASE = 'http://localhost:8080';

async function fetchJSON(url, options = {}) {
  try {
    const response = await fetch(url, options);
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return await response.json();
  } catch (err) {
    console.error(`Error fetching ${url}:`, err);
    return null;
  }
}
async function getQueueState() {
  const data = await fetchJSON(`${API_BASE}/queue-state`);
  if (data && data.status === 'success') return data.queues;
  return null;
}
async function getMatches() {
  const data = await fetchJSON(`${API_BASE}/matches`);
  if (data && data.status === 'success') return data.matches;
  return [];
}
async function getStats() {
  const data = await fetchJSON(`${API_BASE}/stats`);
  if (data) return data;
  return { totalPlayers: 0, totalMatches: 0 };
}
async function addPlayer(username, rank, subrank) {
  const params = new URLSearchParams({ username, rank, subrank });
  const result = await fetchJSON(`${API_BASE}/add-player`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: params.toString()
  });
  return result;
}
async function runMatchmaking() {
  const result = await fetchJSON(`${API_BASE}/matchmake`, { method: 'POST' });
  return result;
}
async function loadFile(csvText) {
  const result = await fetchJSON(`${API_BASE}/load-file`, {
    method: 'POST',
    headers: { 'Content-Type': 'text/plain' },
    body: csvText
  });
  return result;
}
function renderQueues(queuesData) {
  const container = document.getElementById('queuesContainer');
  if (!queuesData) {
    container.innerHTML = '<div class="empty-queue">❌ -Failed to load queues</div>';
    return;
  }
  const rankOrder = ['Bronze', 'Silver', 'Gold', 'Platinum', 'Diamond'];
  let html = '';
  for (const rank of rankOrder) {
    const players = queuesData[rank] || [];
    const playerCount = players.length;
    const playerTags = players.map(p => 
      `<div class="player-tag">${escapeHtml(p.username)} (${p.subrank})</div>`
    ).join('');
    html += `
      <div class="rank-card">
        <div class="rank-title">
          <span>🏅 ${rank}</span>
          <span class="rank-badge">${playerCount} waiting</span>
        </div>
        <div class="player-list">
          ${playerCount === 0 ? '<div class="empty-queue">— empty —</div>' : playerTags}
        </div>
      </div>
    `;
  }
  container.innerHTML = html;
}

function renderMatches(matches) {
  const container = document.getElementById('matchesList');
  if (!matches || matches.length === 0) {
    container.innerHTML = '<div class="empty-matches">✨ No matches yet. Run matchmaking!</div>';
    return;
  }
  
  const matchCards = matches.slice().reverse().map(m => {
    const crossTag = m.isCrossRank ? '<div class="cross-rank">⚔️ Cross-rank battle</div>' : '';
    return `
      <div class="match-card">
        <div class="match-header">Match #${m.matchId}</div>
        <div class="match-players">
          <span>${escapeHtml(m.player1.username)} (${m.player1.rank} ${m.player1.subrank})</span>
          <span>VS</span>
          <span>${escapeHtml(m.player2.username)} (${m.player2.rank} ${m.player2.subrank})</span>
        </div>
        ${crossTag}
      </div>
    `;
  }).join('');
  container.innerHTML = matchCards;
}

function updateStats() {
  getStats().then(stats => {
    document.getElementById('totalPlayers').innerText = stats.totalPlayers ?? 0;
    document.getElementById('totalMatches').innerText = stats.totalMatches ?? 0;
  });
}

async function refreshAll() {
  // show loading state for queues (optional)
  document.getElementById('queuesContainer').innerHTML = '<div class="loading-placeholder">Refreshing...</div>';
  const [queues, matches] = await Promise.all([getQueueState(), getMatches()]);
  renderQueues(queues);
  renderMatches(matches);
  updateStats();
}
async function handleAddPlayer() {
  const username = document.getElementById('username').value.trim();
  const rank = document.getElementById('rankSelect').value;
  const subrank = document.getElementById('subrankSelect').value;
  
  if (!username) {
    alert('Please enter a username');
    return;
  }
  const result = await addPlayer(username, rank, subrank);
  if (result && result.status === 'success') {
    document.getElementById('username').value = '';
    refreshAll();
  } else {
    alert('Failed to add player: ' + (result?.message || 'unknown error'));
  }
}
async function handleMatchmake() {
  const result = await runMatchmaking();
  if (result && result.status === 'success') {
    refreshAll();
  } else {
    alert('Matchmaking triggered but server response invalid.');
    refreshAll();
  }
}
async function handleLoadFile() {
  const fileInput = document.getElementById('csvFile');
  const msgDiv = document.getElementById('loadFileMsg');

  if (!fileInput.files || fileInput.files.length === 0) {
    msgDiv.style.color = '#f87171';
    msgDiv.innerText = 'Please select a CSV file first.';
    return;
  }

  const file = fileInput.files[0];

  // Read file as text
  const csvText = await new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = e => resolve(e.target.result);
    reader.onerror = () => reject(new Error('Failed to read file'));
    reader.readAsText(file);
  });

  const result = await loadFile(csvText);

  if (result && result.status === 'success') {
    msgDiv.style.color = '#4ade80';
    msgDiv.innerText = '✔ ' + result.message;
    fileInput.value = ''; // reset file input
    refreshAll();
  } else {
    msgDiv.style.color = '#f87171';
    msgDiv.innerText = '✘ Failed to load file.';
  }
}
function escapeHtml(str) {
  if (!str) return '';
  return str.replace(/[&<>]/g, function(m) {
    if (m === '&') return '&amp;';
    if (m === '<') return '&lt;';
    if (m === '>') return '&gt;';
    return m;
  });
}
document.addEventListener('DOMContentLoaded', () => {
  refreshAll();
  document.getElementById('refreshBtn').addEventListener('click', refreshAll);
  document.getElementById('addPlayerBtn').addEventListener('click', handleAddPlayer);
  document.getElementById('matchmakeBtn').addEventListener('click', handleMatchmake);
  document.getElementById('loadFileBtn').addEventListener('click', handleLoadFile);
});