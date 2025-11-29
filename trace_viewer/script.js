let trace = [];

// Check for trace file in query parameter
function loadTraceFromUrl() {
  const params = new URLSearchParams(window.location.search);
  const tracePath = params.get('trace');
  if (tracePath) {
    // Fetch the trace file from the provided path
    fetch('file://' + tracePath)
      .then(response => {
        if (!response.ok) throw new Error('Failed to load trace file');
        return response.text();
      })
      .then(text => {
        try {
          trace = JSON.parse(text);
          document.getElementById('slider').max = Math.max(0, trace.length - 1);
          render(0);
          console.log('Trace loaded from URL:', tracePath);
        } catch (e) {
          console.error('Failed to parse trace JSON:', e);
        }
      })
      .catch(err => console.error('Error loading trace:', err));
  }
}

// Load trace on page load if URL parameter present
document.addEventListener('DOMContentLoaded', loadTraceFromUrl);

document.getElementById('file').addEventListener('change', function(e){
  const f = e.target.files[0];
  if (!f) return;
  const reader = new FileReader();
  reader.onload = function(ev){
    trace = JSON.parse(ev.target.result);
    document.getElementById('slider').max = Math.max(0, trace.length - 1);
    render(0);
  };
  reader.readAsText(f);
});

document.getElementById('slider').addEventListener('input', function(e){
  render(parseInt(e.target.value,10));
});


const OPCODE_MNEMONICS = [
  "NOP", "HALT", "MOV", "LOAD", "STORE", "ADD", "SUB", "AND", "OR", "XOR",
  "CMP", "SHL", "SHR", "JMP", "JZ", "JNZ", "JC", "JNC", "JN", "CALL",
  "RET", "PUSH", "POP", "IN", "OUT"
];

const MODE_NAMES = [
  "REG", "IMM", "DIR", "IND", "OFF", "REL"
];

function render(idx){
  if (!trace || !trace[idx]) return;
  const c = trace[idx];
  document.getElementById('cycle').textContent = `Cycle: ${c.cycle} PC: ${c.pc}`;

  // Registers
  let regsHtml = '<h2>Registers</h2><table>';
  for (let r in c.registers) {
    regsHtml += `<tr><td>${r}</td><td>${formatValue(c.registers[r],4)}</td></tr>`;
  }
  regsHtml += `</table>`;
  // Flags
  regsHtml += `<p>Flags: ${renderFlags(c.flags)}</p>`;

  // System / internal registers (if present)
  if (c.sp !== undefined) {
    regsHtml += `<p>SP: ${formatValue(c.sp,4)}</p>`;
  }
  if (c.ir !== undefined || c.mar !== undefined || c.mdr !== undefined) {
    regsHtml += '<h3>Internal</h3>';
    if (c.ir !== undefined) regsHtml += `<p>IR: ${formatValue(c.ir,4)}</p>`;
    if (c.mar !== undefined) regsHtml += `<p>MAR: ${formatValue(c.mar,4)}</p>`;
    if (c.mdr !== undefined) regsHtml += `<p>MDR: ${formatValue(c.mdr,4)}</p>`;
  }

  document.getElementById('registers').innerHTML = regsHtml;

  // Instruction
  let instrHtml = '<h2>Instruction</h2>';
  if (c.instr) {
    let opcodeNum = parseInt(c.instr.opcode, 10);
    let modeNum = parseInt(c.instr.mode, 10);
    let mnemonic = (opcodeNum >=0 && opcodeNum < OPCODE_MNEMONICS.length) ? OPCODE_MNEMONICS[opcodeNum] : "?";
    let modeName = (modeNum >=0 && modeNum < MODE_NAMES.length) ? MODE_NAMES[modeNum] : "?";
    instrHtml += `<pre>opcode: ${opcodeNum} (${mnemonic})\nmode: ${modeNum} (${modeName}) rd: ${c.instr.rd} rs: ${c.instr.rs} extra: ${c.instr.extra}</pre>`;
  }
  document.getElementById('instr').innerHTML = instrHtml;

  // Memory Writes
  let memHtml = '<h2>Memory Writes</h2>';
  if (c.mem_writes && c.mem_writes.length) {
    memHtml += '<ul>';
    c.mem_writes.forEach(m => memHtml += `<li>${m.addr} : ${m.old} → ${m.new}</li>`);
    memHtml += '</ul>';
  } else memHtml += '<p>(none)</p>';
  document.getElementById('mem').innerHTML = memHtml;

  document.getElementById('slider').value = idx;
}

function toHexNumber(v, width=4) {
  let n = Number(v) & 0xFFFF;
  return '0x' + n.toString(16).padStart(width,'0').toUpperCase();
}

function formatValue(val, width=4) {
  if (val === undefined || val === null) return '(none)';
  if (typeof val === 'string') {
    // already a hex string like "0x1234"
    if (val.startsWith('0x') || val.startsWith('0X')) return val.toUpperCase();
    // try parse as number
    let n = Number(val);
    if (!Number.isNaN(n)) return toHexNumber(n, width);
    return val;
  }
  if (typeof val === 'number') return toHexNumber(val, width);
  // fallback
  return String(val);
}

function renderFlags(flags) {
  // Accept flags as number or hex string (e.g., "0x01") and display Z N C V bits
  let num = 0;
  if (typeof flags === 'string') {
    if (flags.startsWith('0x') || flags.startsWith('0X')) num = parseInt(flags, 16);
    else num = parseInt(flags, 10);
  } else if (typeof flags === 'number') {
    num = flags;
  }
  num = num & 0x0F;
  let names = ['Z','N','C','V'];
  let out = '';
  for (let i = 0; i < names.length; ++i) {
    let bit = (num >> (3 - i)) & 1; // keep same order Z N C V
    out += names[i] + ':' + (bit ? '1' : '0') + ' ';
  }
  return out.trim();
}
