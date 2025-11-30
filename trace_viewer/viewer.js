let trace = [];
let currentCycle = 0;
let isPlaying = false;
let playInterval = null;
let playSpeed = 1000; // milliseconds between cycles
let executionHistory = [];

// Initialize application
document.addEventListener('DOMContentLoaded', () => {
  loadTraceList();

  // Setup Back Button
  document.getElementById('back-btn').addEventListener('click', () => {
    stopPlayback();
    document.getElementById('viewer-container').classList.add('hidden');
    document.getElementById('trace-selection').classList.remove('hidden');
    window.history.pushState({}, document.title, window.location.pathname);
  });

  // Setup Slider
  document.getElementById('slider').addEventListener('input', function (e) {
    const cycle = parseInt(e.target.value, 10);
    currentCycle = cycle;
    render(cycle);
    updateExecutionHistory(cycle);
  });

  // Setup Playback Controls
  document.getElementById('first-btn').addEventListener('click', () => {
    stopPlayback();
    goToFirst();
  });

  document.getElementById('prev-btn').addEventListener('click', () => {
    stopPlayback();
    goToPrevious();
  });

  document.getElementById('play-pause-btn').addEventListener('click', togglePlayback);

  document.getElementById('next-btn').addEventListener('click', () => {
    stopPlayback();
    goToNext();
  });

  document.getElementById('last-btn').addEventListener('click', () => {
    stopPlayback();
    goToLast();
  });

  document.getElementById('reset-btn').addEventListener('click', () => {
    stopPlayback();
    goToFirst();
  });

  // Setup Speed Control
  document.getElementById('speed-select').addEventListener('change', function (e) {
    playSpeed = parseInt(e.target.value, 10);
    if (isPlaying) {
      // Restart with new speed
      stopPlayback();
      startPlayback();
    }
  });

  // Keyboard shortcuts
  document.addEventListener('keydown', (e) => {
    if (document.getElementById('viewer-container').classList.contains('hidden')) return;

    switch (e.key) {
      case ' ':
        e.preventDefault();
        togglePlayback();
        break;
      case 'ArrowRight':
        e.preventDefault();
        stopPlayback();
        goToNext();
        break;
      case 'ArrowLeft':
        e.preventDefault();
        stopPlayback();
        goToPrevious();
        break;
      case 'Home':
        e.preventDefault();
        stopPlayback();
        goToFirst();
        break;
      case 'End':
        e.preventDefault();
        stopPlayback();
        goToLast();
        break;
    }
  });

  // Check URL params for direct link
  const params = new URLSearchParams(window.location.search);
  const traceFile = params.get('trace');
  if (traceFile) {
    loadTrace(traceFile);
  }
});

// Fetch and display list of available traces
function loadTraceList() {
  const listContainer = document.getElementById('trace-list');

  fetch('../build/traces/traces.json')
    .then(response => {
      if (!response.ok) throw new Error('Manifest not found');
      return response.json();
    })
    .then(files => {
      listContainer.innerHTML = '';
      if (files.length === 0) {
        listContainer.innerHTML = '<p>No traces found. Run demo.sh to generate them.</p>';
        return;
      }

      files.forEach(file => {
        const btn = document.createElement('button');
        btn.className = 'trace-btn';

        // Extract the base name without timestamp and .json extension
        const baseName = file.replace('.json', '').replace(/_\d{8}_\d{6}$/, '');

        // Format for display: replace underscores with spaces and capitalize
        const displayName = baseName.replace(/_/g, ' ')
          .split(' ')
          .map(word => word.charAt(0).toUpperCase() + word.slice(1))
          .join(' ');

        btn.innerHTML = `<div class="trace-icon">📊</div><div class="trace-name">${displayName}</div>`;
        btn.dataset.originalName = baseName; // Store original name
        btn.onclick = () => loadTrace(file, baseName);
        listContainer.appendChild(btn);
      });
    })
    .catch(err => {
      console.error('Error loading trace list:', err);
      listContainer.innerHTML = '<p>Error loading traces. Make sure you are running via demo.sh</p>';
    });
}

// Load a specific trace file
function loadTrace(filename, baseName) {
  const path = '../build/traces/' + filename;

  fetch(path)
    .then(response => {
      if (!response.ok) throw new Error('Failed to load trace file');
      return response.json();
    })
    .then(data => {
      trace = data;
      currentCycle = 0;
      executionHistory = [];

      // Update UI
      document.getElementById('trace-selection').classList.add('hidden');
      document.getElementById('viewer-container').classList.remove('hidden');

      // Use the base name for consistent display
      const displayName = baseName.replace(/_/g, ' ')
        .split(' ')
        .map(word => word.charAt(0).toUpperCase() + word.slice(1))
        .join(' ');
      document.getElementById('current-trace-name').textContent = displayName;

      // Setup Slider
      const maxCycle = Math.max(0, trace.length - 1);
      document.getElementById('slider').max = maxCycle;
      document.getElementById('slider').value = 0;

      // Update stats
      document.getElementById('total-cycles').textContent = `Total Cycles: ${trace.length}`;
      updateProgress();

      // Update URL without reloading
      const newUrl = new URL(window.location);
      newUrl.searchParams.set('trace', filename);
      window.history.pushState({}, '', newUrl);

      render(0);
      updateExecutionHistory(0);
    })
    .catch(err => {
      console.error('Error loading trace:', err);
      alert('Failed to load trace: ' + filename);
    });
}

// Playback control functions
function togglePlayback() {
  if (isPlaying) {
    stopPlayback();
  } else {
    startPlayback();
  }
}

function startPlayback() {
  isPlaying = true;
  const playBtn = document.getElementById('play-pause-btn');
  playBtn.textContent = '⏸️ Pause';
  playBtn.classList.add('running');

  if (playSpeed === 0) {
    // Max speed - use requestAnimationFrame
    const runMaxSpeed = () => {
      if (!isPlaying) return;
      if (currentCycle < trace.length - 1) {
        goToNext();
        requestAnimationFrame(runMaxSpeed);
      } else {
        stopPlayback();
      }
    };
    requestAnimationFrame(runMaxSpeed);
  } else {
    // Normal speed with interval
    playInterval = setInterval(() => {
      if (currentCycle < trace.length - 1) {
        goToNext();
      } else {
        stopPlayback();
      }
    }, playSpeed);
  }
}

function stopPlayback() {
  isPlaying = false;
  if (playInterval) {
    clearInterval(playInterval);
    playInterval = null;
  }
  const playBtn = document.getElementById('play-pause-btn');
  playBtn.textContent = '▶️ Run';
  playBtn.classList.remove('running');
}

function goToFirst() {
  currentCycle = 0;
  document.getElementById('slider').value = 0;
  render(0);
  updateExecutionHistory(0);
}

function goToPrevious() {
  if (currentCycle > 0) {
    currentCycle--;
    document.getElementById('slider').value = currentCycle;
    render(currentCycle);
    updateExecutionHistory(currentCycle);
  }
}

function goToNext() {
  if (currentCycle < trace.length - 1) {
    currentCycle++;
    document.getElementById('slider').value = currentCycle;
    render(currentCycle);
    updateExecutionHistory(currentCycle);
  }
}

function goToLast() {
  currentCycle = trace.length - 1;
  document.getElementById('slider').value = currentCycle;
  render(currentCycle);
  updateExecutionHistory(currentCycle);
}

function updateProgress() {
  if (trace.length === 0) return;
  const percent = ((currentCycle / (trace.length - 1)) * 100).toFixed(1);
  document.getElementById('progress-percent').textContent = `Progress: ${percent}%`;
}

// Execution history tracking
function updateExecutionHistory(idx) {
  if (!trace || !trace[idx]) return;

  const c = trace[idx];
  let instrText = 'N/A';

  if (c.instr) {
    let opcodeNum = parseInt(c.instr.opcode, 10);
    let mnemonic = (opcodeNum >= 0 && opcodeNum < OPCODE_MNEMONICS.length) ? OPCODE_MNEMONICS[opcodeNum] : "?";
    instrText = `${mnemonic} (PC: ${formatValue(c.pc, 4)})`;
  }

  executionHistory.push({
    cycle: c.cycle,
    instruction: instrText,
    isCurrent: true
  });

  // Mark all previous as not current
  for (let i = 0; i < executionHistory.length - 1; i++) {
    executionHistory[i].isCurrent = false;
  }

  // Keep only last 10
  if (executionHistory.length > 10) {
    executionHistory.shift();
  }

  renderExecutionLog();
}

function renderExecutionLog() {
  const logContainer = document.getElementById('execution-log');
  if (executionHistory.length === 0) {
    logContainer.innerHTML = '<p style="color: var(--text-secondary); font-style: italic;">No execution history yet</p>';
    return;
  }

  let html = '';
  for (let i = executionHistory.length - 1; i >= 0; i--) {
    const entry = executionHistory[i];
    const cssClass = entry.isCurrent ? 'log-entry current' : 'log-entry';
    html += `<div class="${cssClass}">Cycle ${entry.cycle}: ${entry.instruction}</div>`;
  }
  logContainer.innerHTML = html;
}

const OPCODE_MNEMONICS = [
  "NOP", "HALT", "MOV", "LOAD", "STORE", "ADD", "SUB", "AND", "OR", "XOR",
  "CMP", "SHL", "SHR", "JMP", "JZ", "JNZ", "JC", "JNC", "JN", "CALL",
  "RET", "PUSH", "POP", "IN", "OUT"
];

const MODE_NAMES = [
  "REG", "IMM", "DIR", "IND", "OFF", "REL"
];

function render(idx) {
  if (!trace || !trace[idx]) return;
  const c = trace[idx];

  // Update cycle display
  document.getElementById('cycle').textContent = `Cycle: ${c.cycle} / ${trace.length - 1}`;
  updateProgress();

  // Registers
  let regsHtml = '<table>';
  for (let r in c.registers) {
    regsHtml += `<tr><td>${r}</td><td>${formatValue(c.registers[r], 4)}</td></tr>`;
  }

  // Flags
  regsHtml += `<tr><td>FLAGS</td><td>${renderFlags(c.flags)}</td></tr>`;

  // System / internal registers (if present)
  if (c.sp !== undefined) {
    regsHtml += `<tr><td>SP</td><td>${formatValue(c.sp, 4)}</td></tr>`;
  }
  if (c.ir !== undefined) regsHtml += `<tr><td>IR</td><td>${formatValue(c.ir, 4)}</td></tr>`;
  if (c.mar !== undefined) regsHtml += `<tr><td>MAR</td><td>${formatValue(c.mar, 4)}</td></tr>`;
  if (c.mdr !== undefined) regsHtml += `<tr><td>MDR</td><td>${formatValue(c.mdr, 4)}</td></tr>`;

  regsHtml += `</table>`;
  document.getElementById('registers').innerHTML = regsHtml;

  // Instruction
  let instrHtml = '';
  if (c.instr) {
    let opcodeNum = parseInt(c.instr.opcode, 10);
    let modeNum = parseInt(c.instr.mode, 10);
    let mnemonic = (opcodeNum >= 0 && opcodeNum < OPCODE_MNEMONICS.length) ? OPCODE_MNEMONICS[opcodeNum] : "?";
    let modeName = (modeNum >= 0 && modeNum < MODE_NAMES.length) ? MODE_NAMES[modeNum] : "?";

    instrHtml += `<pre>PC:     ${formatValue(c.pc, 4)}
Opcode: ${mnemonic} (${opcodeNum})
Mode:   ${modeName} (${modeNum})
Rd:     ${c.instr.rd}
Rs:     ${c.instr.rs}
Extra:  ${formatValue(c.instr.extra, 4)}</pre>`;
  } else {
    instrHtml += `<pre>PC: ${formatValue(c.pc, 4)}

(No instruction data available)</pre>`;
  }
  document.getElementById('instr').innerHTML = instrHtml;

  // Memory Writes
  let memHtml = '';
  if (c.mem_writes && c.mem_writes.length) {
    memHtml += '<ul>';
    c.mem_writes.forEach(m => memHtml += `<li>Addr ${formatValue(m.addr, 4)}: ${formatValue(m.old, 2)} → ${formatValue(m.new, 2)}</li>`);
    memHtml += '</ul>';
  } else {
    memHtml += '<p style="color: var(--text-secondary); font-style: italic;">No memory writes this cycle</p>';
  }
  document.getElementById('mem').innerHTML = memHtml;
}

function toHexNumber(v, width = 4) {
  let n = Number(v) & 0xFFFF;
  return '0x' + n.toString(16).padStart(width, '0').toUpperCase();
}

function formatValue(val, width = 4) {
  if (val === undefined || val === null) return '-';
  if (typeof val === 'string') {
    if (val.startsWith('0x') || val.startsWith('0X')) return val.toUpperCase();
    let n = Number(val);
    if (!Number.isNaN(n)) return toHexNumber(n, width);
    return val;
  }
  if (typeof val === 'number') return toHexNumber(val, width);
  return String(val);
}

function renderFlags(flags) {
  let num = 0;
  if (typeof flags === 'string') {
    if (flags.startsWith('0x') || flags.startsWith('0X')) num = parseInt(flags, 16);
    else num = parseInt(flags, 10);
  } else if (typeof flags === 'number') {
    num = flags;
  }
  num = num & 0x0F;
  let names = ['Z', 'N', 'C', 'V'];
  let out = '';
  for (let i = 0; i < names.length; ++i) {
    let bit = (num >> (3 - i)) & 1;
    let color = bit ? 'var(--success-color)' : 'var(--text-secondary)';
    let weight = bit ? 'bold' : 'normal';
    out += `<span style="color:${color}; font-weight:${weight}; margin-right:8px; font-size:1.1em">${names[i]}</span>`;
  }
  return out;
}