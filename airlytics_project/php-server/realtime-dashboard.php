<!DOCTYPE html>
<html>
<head>
  <title>Airlytics Dashboard</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body { font-family: Arial; text-align: center; padding: 20px; background: #f9f9f9; }
    h1 { margin-bottom: 40px; }
    .sensor-display { display: flex; justify-content: center; gap: 50px; margin-bottom: 30px; }
    .card { background: #fff; padding: 20px 40px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
    .value { font-size: 36px; font-weight: bold; color: #2c3e50; }
    .label { font-size: 16px; color: #888; }
    canvas { max-width: 600px; margin: 0 auto 50px; background: #fff; border-radius: 10px; padding: 15px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); }
  </style>
</head>
<body>
  <h1>Airlytics Realtime Dashboard</h1>
  <div class="sensor-display">
    <div class="card"><div class="label">Temperature</div><div class="value" id="tempValue">-- °C</div></div>
    <div class="card"><div class="label">Humidity</div><div class="value" id="humValue">-- %</div></div>
  </div>
  <canvas id="tempChart"></canvas>
  <canvas id="humChart"></canvas>
  <script>
    const tempCtx = document.getElementById('tempChart').getContext('2d');
    const humCtx = document.getElementById('humChart').getContext('2d');

    const tempChart = new Chart(tempCtx, { type: 'line', data: { labels: [], datasets: [{ label: 'Temperature (°C)', data: [], borderColor: 'red', fill: false }] } });
    const humChart = new Chart(humCtx, { type: 'line', data: { labels: [], datasets: [{ label: 'Humidity (%)', data: [], borderColor: 'blue', fill: false }] } });

    function fetchData() {
      fetch('get-latest.php')
        .then(response => response.json())
        .then(data => {
          const time = new Date().toLocaleTimeString();
          document.getElementById('tempValue').innerText = `${data.temperature.toFixed(1)} °C`;
          document.getElementById('humValue').innerText = `${data.humidity.toFixed(1)} %`;
          tempChart.data.labels.push(time); tempChart.data.datasets[0].data.push(data.temperature);
          humChart.data.labels.push(time); humChart.data.datasets[0].data.push(data.humidity);
          if (tempChart.data.labels.length > 10) { tempChart.data.labels.shift(); tempChart.data.datasets[0].data.shift(); humChart.data.labels.shift(); humChart.data.datasets[0].data.shift(); }
          tempChart.update(); humChart.update();
        });
    }
    setInterval(fetchData, 3000); fetchData();
  </script>
</body>
</html>