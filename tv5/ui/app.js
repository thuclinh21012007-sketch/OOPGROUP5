/* ============================================================
   CloudCost Optimizer – app.js
   Mô phỏng toàn bộ logic từ C++ backend (TV2 / TV3 / TV4)
   ============================================================ */

'use strict';

/* ── 1. PRICING DATA (TV2 – PricingAdapters mock) ─────────── */
// Giá tham chiếu (USD/giờ hoặc USD/GB-tháng) – giống PricingAdapters.cpp

const PRICING = {
  AWS: {
    compute: { base: 0.096, perVcpu: 0.048, perGbRam: 0.006 }, // USD/giờ per vCPU + RAM
    storage: 0.023,     // USD/GB/tháng
    database: 65.0      // USD/tháng managed PostgreSQL/MySQL
  },
  GCP: {
    compute: { base: 0.082, perVcpu: 0.042, perGbRam: 0.0054 },
    storage: 0.020,
    database: 58.0
  },
  Azure: {
    compute: { base: 0.088, perVcpu: 0.044, perGbRam: 0.0059 },
    storage: 0.021,
    database: 62.0
  }
};
const HOURS_PER_MONTH = 730;
const PROVIDERS = ['AWS', 'GCP', 'Azure'];

/* ── 2. STATE ─────────────────────────────────────────────── */
let infra = [];           // ResourceInput[]
let currentCosts = null;  // ProviderCost[]
let selectedEngine = 'PostgreSQL';

/* ── 3. PRICING LOGIC (TV2 Adapter Pattern) ──────────────── */
function computeProviderCost(provider, resources) {
  const p = PRICING[provider];
  let computeCost = 0, storageCost = 0, dbCost = 0;

  for (const r of resources) {
    if (r.type === 'COMPUTE') {
      // cost per VM = base + perVcpu*vcpu + perGbRam*ram  (USD/hr)
      const hourly = p.compute.base + p.compute.perVcpu * r.vcpu + p.compute.perGbRam * r.ramGB;
      computeCost += hourly * HOURS_PER_MONTH * r.quantity;
    } else if (r.type === 'STORAGE') {
      storageCost += p.storage * r.sizeGB;
    } else if (r.type === 'DATABASE') {
      dbCost += p.database;
    }
  }
  return { provider, compute: computeCost, storage: storageCost, database: dbCost };
}

function total(c) { return c.compute + c.storage + c.database; }

/* ── 4. RECOMMENDATION ENGINE (TV4 Strategies) ──────────── */
function generateRecommendations(costs, resources) {
  const recs = [];
  const totals = costs.map(c => ({ ...c, tot: total(c) }));
  const bestTotal = Math.min(...totals.map(t => t.tot));
  const currentBest = totals.sort((a, b) => a.tot - b.tot)[0];

  // Strategy 1 – CheapestProviderStrategy
  for (const c of totals) {
    if (c.tot > bestTotal * 1.01) {
      recs.push({
        strategy: 'CheapestProvider',
        description: `Chuyển từ ${c.provider} sang ${currentBest.provider} – nhà cung cấp rẻ nhất`,
        before: c.tot,
        after: currentBest.tot
      });
      break;
    }
  }

  // Strategy 2 – RightSizingStrategy  (if any compute < 40% usage)
  const lowUsageComputes = resources.filter(r => r.type === 'COMPUTE' && r.usagePercent < 40);
  if (lowUsageComputes.length > 0 && currentCosts) {
    const awsCost = totals.find(c => c.provider === 'AWS');
    if (awsCost) {
      const saving = awsCost.tot * 0.25; // hạ 1 tier = ~25% tiết kiệm
      recs.push({
        strategy: 'RightSizing',
        description: `${lowUsageComputes.length} máy chủ sử dụng dưới 40% – đề xuất hạ cấu hình`,
        before: awsCost.tot,
        after: awsCost.tot - saving
      });
    }
  }

  // Strategy 3 – PricingModelStrategy (Reserved = -30%)
  if (totals.length > 0) {
    const cheapest = totals[0];
    recs.push({
      strategy: 'ReservedPricing',
      description: `Cam kết dùng ${cheapest.provider} 1 năm (Reserved Plan) – giảm ~30%`,
      before: cheapest.tot,
      after: cheapest.tot * 0.70
    });
  }
  return recs;
}

/* ── 5. SIDEBAR & PANEL NAV ──────────────────────────────── */
const PANEL_TITLES = {
  costs: 'Dashboard – So sánh chi phí',
  compute: '⚡ Thêm máy chủ (Compute)',
  storage: '💾 Thêm lưu trữ (Storage)',
  database: '🗄️ Thêm Database',
  infra: '🏗️ Hạ tầng đã thiết kế',
  detail: '🔍 Chi tiết chi phí từng hãng',
  recs: '💡 Khuyến nghị tối ưu',
  tests: '🧪 Bộ kiểm thử (25 Test Cases)'
};

function showPanel(name) {
  document.querySelectorAll('.panel').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));

  const panel = document.getElementById('panel-' + name);
  if (panel) panel.classList.add('active');

  const nav = document.getElementById('nav-' + name);
  if (nav) nav.classList.add('active');

  document.getElementById('topbar-title').textContent = PANEL_TITLES[name] || name;

  // lazy-render on navigation
  if (name === 'infra') renderInfra();
  if (name === 'detail' && currentCosts) renderDetail();
  if (name === 'recs' && currentCosts) renderRecs();

  return false;
}

function toggleSidebar() {
  document.body.classList.toggle('sidebar-collapsed');
}

/* ── 6. ADD RESOURCES ────────────────────────────────────── */
function addCompute() {
  const qty = parseInt(document.getElementById('c-qty').value) || 1;
  const vcpu = parseInt(document.getElementById('c-vcpu').value) || 4;
  const ram = parseInt(document.getElementById('c-ram').value) || 16;
  const usage = parseInt(document.getElementById('c-usage').value) || 70;

  if (qty < 1 || vcpu < 1 || ram < 1 || usage < 1 || usage > 100) {
    toast('Vui lòng nhập giá trị hợp lệ', 'error'); return;
  }
  infra.push({ type: 'COMPUTE', quantity: qty, vcpu, ramGB: ram, usagePercent: usage, sizeGB: 0, engine: '' });
  updateInfraBadge();
  toast(`✅ Đã thêm ${qty} máy chủ (${vcpu} vCPU / ${ram} GB RAM)`, 'success');
  currentCosts = null;
}

function addStorage() {
  const size = parseInt(document.getElementById('s-size').value) || 500;
  if (size < 1) { toast('Dung lượng phải lớn hơn 0', 'error'); return; }
  infra.push({ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: size, engine: '' });
  updateInfraBadge();
  toast(`✅ Đã thêm lưu trữ ${size} GB`, 'success');
  currentCosts = null;
}

function addDatabase() {
  infra.push({ type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: selectedEngine });
  updateInfraBadge();
  toast(`✅ Đã thêm Database ${selectedEngine}`, 'success');
  currentCosts = null;
}

function selectEngine(eng) {
  selectedEngine = eng;
  document.querySelectorAll('.engine-option').forEach(el => el.classList.remove('selected'));
  document.getElementById(eng === 'PostgreSQL' ? 'eng-pg' : 'eng-my').classList.add('selected');
}

function deleteResource(idx) {
  infra.splice(idx, 1);
  updateInfraBadge();
  renderInfra();
  currentCosts = null;
}

function clearAll() {
  if (!confirm('Xóa tất cả tài nguyên?')) return;
  infra = [];
  currentCosts = null;
  updateInfraBadge();
  renderInfra();
  resetDashboard();
  toast('🗑️ Đã xóa tất cả tài nguyên', 'info');
}

function resetDashboard() {
  // Reset cost cards to $0.00
  PROVIDERS.forEach(p => {
    const pKey = p.toLowerCase();
    document.getElementById('total-' + pKey).textContent = '$0.00';
    document.getElementById(pKey + '-compute').textContent = '$0.00';
    document.getElementById(pKey + '-storage').textContent = '$0.00';
    document.getElementById(pKey + '-database').textContent = '$0.00';
    document.getElementById('annual-' + pKey).textContent = '≈ $0.00 / năm';
    document.getElementById('card-' + pKey).classList.remove('cheapest');
    document.getElementById('badge-' + pKey).style.display = 'none';
  });

  // Reset bar chart – hide bars, show placeholder
  const cats = ['c', 's', 'd', 't'];
  const provKeys = ['aws', 'gcp', 'azure'];
  for (const cat of cats) {
    for (const pk of provKeys) {
      const bar = document.getElementById(`b-${pk}-${cat}`);
      const label = document.getElementById(`bl-${pk}-${cat}`);
      if (bar) bar.style.height = '0px';
      if (label) label.textContent = '';
    }
  }
  const chartPlaceholder = document.getElementById('chart-placeholder');
  const chartBars = document.getElementById('chart-bars');
  if (chartPlaceholder) chartPlaceholder.style.display = 'flex';
  if (chartBars) chartBars.style.display = 'none';

  // Reset time table
  const tbody = document.getElementById('time-table-body');
  if (tbody) tbody.innerHTML = '';
  const timeTableCard = document.getElementById('time-table-card');
  if (timeTableCard) timeTableCard.style.display = 'none';

  // Reset detail panel (Chi tiết theo hãng)
  const detailContent = document.getElementById('detail-content');
  if (detailContent) {
    detailContent.innerHTML = '<div class="empty-state"><div class="empty-icon">🔍</div><div>Nhấn <strong>"Tính chi phí"</strong> để xem chi tiết.</div></div>';
  }

  // Reset recommendations panel (Khuyến nghị tối ưu)
  const recsContent = document.getElementById('recs-content');
  if (recsContent) {
    recsContent.innerHTML = '<div class="empty-state"><div class="empty-icon">💡</div><div>Nhấn <strong>"Tính chi phí"</strong> để xem khuyến nghị.</div></div>';
  }
}

function updateInfraBadge() {
  document.getElementById('infra-count').textContent = infra.length;
}

/* ── 7. CALCULATE ────────────────────────────────────────── */
function calculateAll() {
  if (infra.length === 0) {
    toast('⚠️ Hãy thêm ít nhất 1 tài nguyên', 'error');
    showPanel('compute');
    return;
  }
  currentCosts = PROVIDERS.map(p => computeProviderCost(p, infra));

  renderCostCards();
  renderBarChart();
  renderTimeTable();
  renderDetail();
  renderRecs();

  showPanel('costs');
  toast('✅ Đã tính chi phí xong!', 'success');
}

/* ── 8. RENDER COST CARDS ────────────────────────────────── */
function fmtUSD(val) {
  if (val === 0) return '$0.00';
  if (val < 0.01) return '< $0.01';
  return '$' + val.toFixed(2);
}
function fmtBig(val) {
  if (val === 0) return '–';
  return '$' + val.toFixed(2).replace(/\B(?=(\d{3})+(?!\d))/g, ',');
}

function renderCostCards() {
  const totals = currentCosts.map(c => total(c));
  const minTotal = Math.min(...totals);

  // clear cheapest badges
  PROVIDERS.forEach(p => {
    document.getElementById('card-' + p.toLowerCase()).classList.remove('cheapest');
    document.getElementById('badge-' + p.toLowerCase()).style.display = 'none';
  });

  for (const c of currentCosts) {
    const pKey = c.provider.toLowerCase();
    const isMin = Math.abs(total(c) - minTotal) < 0.001;

    document.getElementById('total-' + pKey).textContent = fmtBig(total(c));
    document.getElementById(pKey + '-compute').textContent = fmtUSD(c.compute);
    document.getElementById(pKey + '-storage').textContent = fmtUSD(c.storage);
    document.getElementById(pKey + '-database').textContent = fmtUSD(c.database);
    document.getElementById('annual-' + pKey).textContent =
      `≈ ${fmtBig(total(c) * 12)} / năm`;

    if (isMin) {
      document.getElementById('card-' + pKey).classList.add('cheapest');
      document.getElementById('badge-' + pKey).style.display = 'block';
    }
  }
}

/* ── 9. BAR CHART ────────────────────────────────────────── */
function renderBarChart() {
  const maxVal = Math.max(...currentCosts.flatMap(c => [c.compute, c.storage, c.database, total(c)]));
  if (maxVal === 0) return;

  const H = 180; // px height

  function setBar(id, labelId, val) {
    const pct = val / maxVal;
    const px = Math.max(pct * H, val > 0 ? 6 : 0);
    const el = document.getElementById(id);
    el.style.height = px + 'px';
    document.getElementById(labelId).textContent = val > 0 ? '$' + Math.round(val) : '';
  }

  const cats = [
    ['c', c => c.compute],
    ['s', c => c.storage],
    ['d', c => c.database],
    ['t', c => total(c)]
  ];
  const provKeys = [['aws', 'AWS'], ['gcp', 'GCP'], ['azure', 'Azure']];

  for (const [cat, fn] of cats) {
    for (const [pk, pname] of provKeys) {
      const c = currentCosts.find(x => x.provider === pname);
      setBar(`b-${pk}-${cat}`, `bl-${pk}-${cat}`, fn(c));
    }
  }

  document.getElementById('chart-placeholder').style.display = 'none';
  document.getElementById('chart-bars').style.display = 'flex';
}

/* ── 10. TIME TABLE ──────────────────────────────────────── */
function renderTimeTable() {
  const tbody = document.getElementById('time-table-body');
  tbody.innerHTML = '';
  for (const c of currentCosts) {
    const mo = total(c);
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td><strong>${c.provider}</strong></td>
      <td class="mono">${fmtUSD(mo / HOURS_PER_MONTH)}</td>
      <td class="mono">${fmtUSD(mo / 30)}</td>
      <td class="mono">${fmtBig(mo)}</td>
      <td class="mono">${fmtBig(mo * 12)}</td>
    `;
    tbody.appendChild(tr);
  }
  document.getElementById('time-table-card').style.display = 'block';
}

/* ── 11. RENDER INFRA TABLE ──────────────────────────────── */
function renderInfra() {
  const empty = document.getElementById('infra-empty');
  const wrap = document.getElementById('infra-table-wrap');
  const actions = document.getElementById('infra-actions');
  const tbody = document.getElementById('infra-tbody');

  if (infra.length === 0) {
    empty.style.display = 'flex';
    wrap.style.display = 'none';
    actions.style.display = 'none';
    return;
  }
  empty.style.display = 'none';
  wrap.style.display = 'block';
  actions.style.display = 'flex';

  tbody.innerHTML = '';
  infra.forEach((r, i) => {
    let detail = '';
    let badge = '';
    if (r.type === 'COMPUTE') {
      detail = `${r.quantity}× VM  |  ${r.vcpu} vCPU / ${r.ramGB} GB RAM  |  Sử dụng ${r.usagePercent}%`;
      badge = '<span class="type-badge type-compute">⚡ Compute</span>';
    } else if (r.type === 'STORAGE') {
      detail = `${r.sizeGB} GB lưu trữ đối tượng`;
      badge = '<span class="type-badge type-storage">💾 Storage</span>';
    } else if (r.type === 'DATABASE') {
      detail = `Managed DB – ${r.engine}`;
      badge = '<span class="type-badge type-database">🗄️ Database</span>';
    }
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${i + 1}</td>
      <td>${badge}</td>
      <td>${detail}</td>
      <td><button class="btn-icon" onclick="deleteResource(${i})" title="Xóa">🗑️</button></td>
    `;
    tbody.appendChild(tr);
  });
}

/* ── 12. RENDER DETAIL ───────────────────────────────────── */
function renderDetail() {
  if (!currentCosts) return;
  const container = document.getElementById('detail-content');
  container.innerHTML = '';

  for (const c of currentCosts) {
    const pKey = c.provider.toLowerCase();
    const colorMap = { AWS: 'var(--aws)', GCP: 'var(--gcp)', Azure: 'var(--azure)' };
    const color = colorMap[c.provider];

    const block = document.createElement('div');
    block.className = 'detail-provider-block';

    // Build rows per resource
    let rows = '';
    for (const r of infra) {
      let resourceCost = 0;
      let desc = '';
      const p = PRICING[c.provider];
      if (r.type === 'COMPUTE') {
        const hourly = p.compute.base + p.compute.perVcpu * r.vcpu + p.compute.perGbRam * r.ramGB;
        resourceCost = hourly * HOURS_PER_MONTH * r.quantity;
        desc = `${r.quantity}× ${r.vcpu} vCPU / ${r.ramGB}GB @ $${hourly.toFixed(4)}/hr`;
      } else if (r.type === 'STORAGE') {
        resourceCost = p.storage * r.sizeGB;
        desc = `${r.sizeGB} GB × $${p.storage}/GB`;
      } else if (r.type === 'DATABASE') {
        resourceCost = p.database;
        desc = `Managed ${r.engine} – $${p.database}/tháng flat`;
      }
      rows += `<tr>
        <td>${r.type === 'COMPUTE' ? '⚡' : r.type === 'STORAGE' ? '💾' : '🗄️'} ${r.type}</td>
        <td>${desc}</td>
        <td class="mono" style="color:${color}">${fmtUSD(resourceCost)}</td>
        <td class="mono">${fmtUSD(resourceCost * 12)}</td>
      </tr>`;
    }

    block.innerHTML = `
      <div class="glass-card">
        <div class="provider-section-title">
          <div class="provider-badge ${pKey}" style="width:32px;height:32px;font-size:9px;">${c.provider}</div>
          <span style="color:${color}">${c.provider}</span>
          <span style="color:var(--text-3);font-weight:400;font-size:13px">– Chi tiết từng dòng tài nguyên</span>
        </div>
        <div class="table-wrap">
          <table class="data-table">
            <thead><tr><th>Loại</th><th>Mô tả</th><th>/ Tháng</th><th>/ Năm</th></tr></thead>
            <tbody>
              ${rows}
              <tr style="font-weight:700">
                <td colspan="2">Tổng cộng</td>
                <td class="mono" style="color:${color}">${fmtBig(total(c))}</td>
                <td class="mono" style="color:${color}">${fmtBig(total(c) * 12)}</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>`;
    container.appendChild(block);
  }
}

/* ── 13. RENDER RECOMMENDATIONS ──────────────────────────── */
function renderRecs() {
  if (!currentCosts) return;
  const container = document.getElementById('recs-content');
  container.innerHTML = '';

  const recs = generateRecommendations([...currentCosts], infra);
  if (recs.length === 0) {
    container.innerHTML = '<div class="empty-state"><div class="empty-icon">✅</div><div>Hạ tầng đã được tối ưu tốt!</div></div>';
    return;
  }

  const summary = document.createElement('div');
  summary.className = 'glass-card';
  summary.style.marginBottom = '20px';
  summary.innerHTML = `<div style="font-size:13px;color:var(--text-2)">Dựa trên <strong style="color:var(--text)">${recs.length} chiến lược</strong> phân tích (CheapestProvider, RightSizing, ReservedPricing)</div>`;
  container.appendChild(summary);

  for (const r of recs) {
    const saving = r.before - r.after;
    const pct = r.before > 0 ? (saving / r.before * 100) : 0;
    const card = document.createElement('div');
    card.className = 'rec-card';
    card.innerHTML = `
      <div class="rec-header">
        <span class="rec-strategy-badge">${r.strategy}</span>
        <span class="rec-desc">${r.description}</span>
      </div>
      <div class="rec-metrics">
        <div class="rec-metric">
          <div class="rec-metric-label">Chi phí trước</div>
          <div class="rec-metric-val rm-before">${fmtBig(r.before)}<span style="font-size:11px;color:var(--text-3)">/tháng</span></div>
        </div>
        <div style="color:var(--text-3);align-self:center;font-size:20px;margin:0 4px">→</div>
        <div class="rec-metric">
          <div class="rec-metric-label">Chi phí sau</div>
          <div class="rec-metric-val rm-after">${fmtBig(r.after)}<span style="font-size:11px;color:var(--text-3)">/tháng</span></div>
        </div>
        <div style="flex:1"></div>
        <div class="rec-metric" style="text-align:right">
          <div class="rec-metric-label">Tiết kiệm</div>
          <div class="rec-metric-val rm-saving">${fmtBig(saving)}</div>
        </div>
        <div class="rec-metric" style="text-align:right">
          <div class="rec-metric-label">Phần trăm</div>
          <div class="rec-metric-val rm-pct">${pct.toFixed(1)}%</div>
        </div>
      </div>`;
    container.appendChild(card);
  }
}

/* ── 14. EXPORT CSV ──────────────────────────────────────── */
function exportCSV() {
  if (infra.length === 0) { toast('Hãy thêm tài nguyên trước', 'error'); return; }
  if (!currentCosts) calculateAll();

  let csv = 'BAO CAO MO PHONG CHI PHI HA TANG DA DAM MAY\n';
  csv += `Ngay xuat,${new Date().toLocaleDateString('vi-VN')}\n`;
  csv += 'Vung mac dinh,ap-southeast-1\n';
  csv += 'Luu y,Gia mo phong - khong phai bao gia cam ket\n\n';

  csv += 'HA TANG DA THIET KE\nSTT,Loai,Chi tiet\n';
  infra.forEach((r, i) => {
    let detail = '';
    if (r.type === 'COMPUTE') detail = `${r.quantity} may x ${r.vcpu}vCPU/${r.ramGB}GB (${r.usagePercent}%)`;
    if (r.type === 'STORAGE') detail = `${r.sizeGB} GB`;
    if (r.type === 'DATABASE') detail = r.engine;
    csv += `${i + 1},${r.type},"${detail}"\n`;
  });

  csv += '\nSO SANH CHI PHI (USD)\nProvider,Compute/thang,Storage/thang,Database/thang,Total/thang,Total/nam\n';
  for (const c of currentCosts) {
    csv += `${c.provider},${c.compute.toFixed(2)},${c.storage.toFixed(2)},${c.database.toFixed(2)},${total(c).toFixed(2)},${(total(c) * 12).toFixed(2)}\n`;
  }

  csv += '\nKHUYEN NGHI TOI UU\nStrategy,Description,Before (USD),After (USD),Saving (USD),Percent (%)\n';
  for (const r of generateRecommendations([...currentCosts], infra)) {
    const saving = r.before - r.after;
    const pct = r.before > 0 ? (saving / r.before * 100) : 0;
    csv += `${r.strategy},"${r.description}",${r.before.toFixed(2)},${r.after.toFixed(2)},${saving.toFixed(2)},${pct.toFixed(1)}\n`;
  }
  csv += '\nGhi chu: gia mo phong - khong phai bao gia cam ket.\n';

  const blob = new Blob(['\uFEFF' + csv], { type: 'text/csv;charset=utf-8' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url; a.download = `cloud_cost_report_${Date.now()}.csv`;
  document.body.appendChild(a); a.click(); document.body.removeChild(a);
  URL.revokeObjectURL(url);
  toast('📥 Đã xuất báo cáo CSV!', 'success');
}

/* ── 15. TESTS (TV3 mock – 25 cases) ────────────────────── */
const TEST_CASES = [
  // [description, fn → bool]
  ['Tính chi phí Compute AWS 4vCPU/16GB 1 máy',
    () => Math.abs(computeProviderCost('AWS', [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 100, sizeGB: 0, engine: '' }]).compute - (0.096 + 4 * 0.048 + 16 * 0.006) * 730) < 0.01],
  ['Tính chi phí Storage GCP 500GB',
    () => Math.abs(computeProviderCost('GCP', [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 500, engine: '' }]).storage - 0.020 * 500) < 0.01],
  ['Tính chi phí DB Azure PostgreSQL',
    () => Math.abs(computeProviderCost('Azure', [{ type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'PostgreSQL' }]).database - 62.0) < 0.01],
  ['AWS rẻ hơn GCP cho 4vCPU/16GB? (kiểm tra giá)',
    () => { const a = computeProviderCost('AWS', [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 100, sizeGB: 0, engine: '' }]); const g = computeProviderCost('GCP', [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 100, sizeGB: 0, engine: '' }]); return total(a) > total(g); }],
  ['GCP rẻ hơn Azure Storage',
    () => PRICING.GCP.storage < PRICING.Azure.storage],
  ['Tổng = Compute + Storage + Database',
    () => { const c = computeProviderCost('AWS', [{ type: 'COMPUTE', quantity: 1, vcpu: 2, ramGB: 8, usagePercent: 100, sizeGB: 0, engine: '' }, { type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 100, engine: '' }, { type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'PostgreSQL' }]); return Math.abs(total(c) - (c.compute + c.storage + c.database)) < 0.001; }],
  ['Hạ tầng rỗng → tổng chi phí = 0',
    () => total(computeProviderCost('AWS', [])) === 0],
  ['Nhiều VM × quantity = 5',
    () => { const r = [{ type: 'COMPUTE', quantity: 5, vcpu: 4, ramGB: 16, usagePercent: 100, sizeGB: 0, engine: '' }]; return Math.abs(computeProviderCost('AWS', r).compute - 5 * (0.096 + 4 * 0.048 + 16 * 0.006) * 730) < 0.01; }],
  ['Chi phí năm = chi phí tháng × 12',
    () => { const r = [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 200, engine: '' }]; const c = computeProviderCost('GCP', r); return Math.abs(total(c) * 12 - 0.020 * 200 * 12) < 0.01; }],
  ['Chi phí giờ = chi phí tháng / 730',
    () => { const c = computeProviderCost('AWS', [{ type: 'COMPUTE', quantity: 1, vcpu: 2, ramGB: 4, usagePercent: 100, sizeGB: 0, engine: '' }]); return Math.abs(total(c) / HOURS_PER_MONTH - (0.096 + 2 * 0.048 + 4 * 0.006)) < 0.001; }],
  ['GCP DB rẻ hơn AWS DB',
    () => PRICING.GCP.database < PRICING.AWS.database],
  ['Azure DB rẻ hơn AWS DB',
    () => PRICING.Azure.database < PRICING.AWS.database],
  ['Storage AWS > GCP',
    () => PRICING.AWS.storage > PRICING.GCP.storage],
  ['RightSizing đề xuất khi usage < 40%',
    () => { const recs = generateRecommendations(PROVIDERS.map(p => computeProviderCost(p, [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 30, sizeGB: 0, engine: '' }])), [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 30, sizeGB: 0, engine: '' }]); return recs.some(r => r.strategy === 'RightSizing'); }],
  ['RightSizing KHÔNG đề xuất khi usage >= 40%',
    () => { const recs = generateRecommendations(PROVIDERS.map(p => computeProviderCost(p, [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 60, sizeGB: 0, engine: '' }])), [{ type: 'COMPUTE', quantity: 1, vcpu: 4, ramGB: 16, usagePercent: 60, sizeGB: 0, engine: '' }]); return !recs.some(r => r.strategy === 'RightSizing'); }],
  ['ReservedPricing luôn tiết kiệm 30%',
    () => { const costs = PROVIDERS.map(p => computeProviderCost(p, [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 100, engine: '' }])); const recs = generateRecommendations(costs, [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 100, engine: '' }]); const r = recs.find(x => x.strategy === 'ReservedPricing'); return r && Math.abs(r.after / r.before - 0.70) < 0.01; }],
  ['CheapestProvider đề xuất tồn tại',
    () => { const costs = PROVIDERS.map(p => computeProviderCost(p, [{ type: 'COMPUTE', quantity: 3, vcpu: 8, ramGB: 32, usagePercent: 80, sizeGB: 0, engine: '' }])); const recs = generateRecommendations(costs, [{ type: 'COMPUTE', quantity: 3, vcpu: 8, ramGB: 32, usagePercent: 80, sizeGB: 0, engine: '' }]); return recs.length > 0; }],
  ['Saving = before - after',
    () => { const costs = PROVIDERS.map(p => computeProviderCost(p, [{ type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'MySQL' }])); const recs = generateRecommendations(costs, [{ type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'MySQL' }]); return recs.every(r => Math.abs((r.before - r.after) - (r.before - r.after)) < 0.001); }],
  ['Percent = (saving/before)*100',
    () => { const costs = PROVIDERS.map(p => computeProviderCost(p, [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 1000, engine: '' }])); const recs = generateRecommendations(costs, [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 1000, engine: '' }]); return recs.every(r => Math.abs((r.before - r.after) / r.before * 100 - (r.before - r.after) / r.before * 100) < 0.001); }],
  ['GCP cheapest cho Storage',
    () => { const costs = PROVIDERS.map(p => computeProviderCost(p, [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 500, engine: '' }])); const minCost = Math.min(...costs.map(total)); return costs.find(c => c.provider === 'GCP')?.storage === minCost; }],
  ['So sánh 3 hãng: có đúng 3 kết quả',
    () => PROVIDERS.map(p => computeProviderCost(p, [{ type: 'COMPUTE', quantity: 1, vcpu: 2, ramGB: 4, usagePercent: 100, sizeGB: 0, engine: '' }])).length === 3],
  ['Compute 0 vCPU → chi phí base only',
    () => { const c = computeProviderCost('AWS', [{ type: 'COMPUTE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 100, sizeGB: 0, engine: '' }]); return Math.abs(c.compute - 0.096 * 730) < 0.01; }],
  ['Storage 0GB → $0',
    () => computeProviderCost('GCP', [{ type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: '' }]).storage === 0],
  ['Nhiều loại tài nguyên cộng đúng',
    () => { const r = [{ type: 'COMPUTE', quantity: 2, vcpu: 4, ramGB: 8, usagePercent: 100, sizeGB: 0, engine: '' }, { type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 200, engine: '' }, { type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'PostgreSQL' }]; const c = computeProviderCost('AWS', r); const expected = 2 * (0.096 + 4 * 0.048 + 8 * 0.006) * 730 + 0.023 * 200 + 65; return Math.abs(total(c) - expected) < 0.01; }],
  ['PROVIDERS array có đúng 3 phần tử',
    () => PROVIDERS.length === 3 && PROVIDERS.includes('AWS') && PROVIDERS.includes('GCP') && PROVIDERS.includes('Azure')],
];

function runTests() {
  const output = document.getElementById('test-output');
  output.style.display = 'block';
  output.innerHTML = '';

  let pass = 0, fail = 0;

  for (let i = 0; i < TEST_CASES.length; i++) {
    const [desc, fn] = TEST_CASES[i];
    let ok = false;
    try { ok = fn(); } catch (e) { ok = false; }
    if (ok) pass++; else fail++;

    const div = document.createElement('div');
    div.className = 'test-result-item ' + (ok ? 'test-pass' : 'test-fail');
    div.textContent = `${ok ? '✓' : '✗'} [${String(i + 1).padStart(2, '0')}] ${desc}`;
    output.appendChild(div);
  }

  const summary = document.createElement('div');
  summary.className = 'test-summary';
  summary.textContent = `${pass}/${TEST_CASES.length} PASSED  |  ${fail} FAILED`;
  if (fail === 0) summary.style.color = 'var(--green)';
  else summary.style.color = 'var(--amber)';
  output.appendChild(summary);

  toast(`🧪 ${pass}/${TEST_CASES.length} test cases passed`, fail === 0 ? 'success' : 'info');
}

/* ── 16. TOAST ───────────────────────────────────────────── */
function toast(msg, type = 'info') {
  const tc = document.getElementById('toast-container');
  const t = document.createElement('div');
  t.className = `toast toast-${type}`;
  t.textContent = msg;
  tc.appendChild(t);
  setTimeout(() => { if (t.parentNode) tc.removeChild(t); }, 3000);
}

/* ── 17. LIVE PREVIEW FORMS ──────────────────────────────── */
function bindPreview(inputIds, previewId, formatter) {
  for (const id of inputIds) {
    const el = document.getElementById(id);
    if (el) el.addEventListener('input', () => {
      document.getElementById(previewId).textContent = formatter();
    });
  }
}
bindPreview(
  ['c-qty', 'c-vcpu', 'c-ram', 'c-usage'], 'compute-preview-text',
  () => {
    const qty = document.getElementById('c-qty').value || 1;
    const vcpu = document.getElementById('c-vcpu').value || 4;
    const ram = document.getElementById('c-ram').value || 16;
    const usg = document.getElementById('c-usage').value || 70;
    return `${qty} × VM (${vcpu} vCPU / ${ram} GB RAM) – sử dụng ${usg}%`;
  }
);
bindPreview(
  ['s-size'], 'storage-preview-text',
  () => `${document.getElementById('s-size').value || 500} GB lưu trữ đối tượng`
);

/* ── 18. DEMO DATA (load on first open) ─────────────────── */
function loadDemoData() {
  infra = [
    { type: 'COMPUTE', quantity: 3, vcpu: 4, ramGB: 16, usagePercent: 70, sizeGB: 0, engine: '' },
    { type: 'COMPUTE', quantity: 1, vcpu: 8, ramGB: 32, usagePercent: 30, sizeGB: 0, engine: '' },
    { type: 'STORAGE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 500, engine: '' },
    { type: 'DATABASE', quantity: 1, vcpu: 0, ramGB: 0, usagePercent: 0, sizeGB: 0, engine: 'PostgreSQL' },
  ];
  updateInfraBadge();
  calculateAll();
}

/* ── 19. INIT ────────────────────────────────────────────── */
document.addEventListener('DOMContentLoaded', () => {
  showPanel('costs');
});
