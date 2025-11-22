<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <title>AIRIQ – DHT22 Sensor Monitoring</title>

    <!-- Icon & Tailwind -->
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.0/font/bootstrap-icons.css"
    />
    <script src="https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4"></script>

    <!-- Chart.js -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  </head>

  <body class="bg-linear-to-br from-gray-800 via-gray-900 to-black h-screen">
    <div class="text-white max-h-screen flex flex-col">
      <!-- Header -->
      <header class="bg-white/10 backdrop-blur-md shadow-md py-4 text-center border-b border-white/10">
        <h1 class="text-3xl font-bold tracking-widest text-[#1E90FF] text-shadow-sm text-shadow-[#00bfff80] font-sans">AIRIQ</h1>
        
        <p class="text-sm text-gray-300 mt-1">DHT22 Sensor Monitoring</p>
      </header>

      <!-- Card Section -->
      <main
        class="flex-1 flex flex-col md:flex-row gap-6 justify-center items-stretch p-6 md:max-w-300 mx-auto w-full"
      >
        <!-- Temperature Card -->
        <div class="w-full md:w-1/2 bg-white/10 backdrop-blur-2xl border border-white/10 rounded-2xl p-6 text-center shadow-[0_8px_25px_rgba(255,255,255,0.05)] hover:shadow-[0_14px_25px_rgba(239,68,68,0.25)] hover:scale-[1.02] transition-all duration-300 h-44 md:h-48 flex flex-col justify-center">
          <div class="flex flex-col items-center space-y-2">
            <i class="bi bi-thermometer-half text-4xl text-red-400"></i>

            <h2 class="text-base font-semibold tracking-wide text-gray-300">
              Temperature
            </h2>

            <p class="text-4xl font-extrabold text-red-400">
              <span id="temperature">--</span>°C
            </p>
          </div>

          <div>
            <p id="temp-notif" class=""></p>
          </div>
        </div>

        <!-- Humidity Card -->
        <div class="w-full md:w-1/2 bg-white/10 backdrop-blur-2xl border border-white/10 rounded-2xl p-6 text-center shadow-[0_8px_25px_rgba(255,255,255,0.05)] hover:shadow-[0_14px_25px_rgba(56,189,248,0.25)] hover:scale-[1.02] transition-all duration-300 h-44 md:h-48 flex flex-col justify-center">
          <div class="flex flex-col items-center space-y-2">
            <i class="bi bi-droplet text-4xl text-sky-500"></i>

            <h2 class="text-base font-semibold tracking-wide text-gray-300">
              Humidity
            </h2>

            <p class="text-4xl font-extrabold text-sky-400">
              <span id="humidity">--</span>%
            </p>
          </div>

          <div>
            <p id="humi-notif" class=""></p>
          </div>
        </div>
      </main>

      <!-- Chart Section -->
      <section class="max-w-6xl mx-auto w-11/12 h-auto md:w-full md:h-auto bg-white/10 backdrop-blur-2xl border border-white/10 rounded-2xl shadow-[0_8px_25px_rgba(255,255,255,0.05)] hover:shadow-[0_14px_25px_rgba(249,248,246,0.5)] p-6 mt-4 mb-6 hover:scale-[1.02] transition-all duration-300">
        <h2 class="text-center text-lg font-semibold tracking-wide text-gray-300 mb-4">
          Real-time Sensor Chart
        </h2>

        <canvas id="sensorChart" height="100"></canvas>
      </section>

      <!-- Footer -->
      <footer class="text-center md:absolute md:bottom-0 md:left-1/2 md:-translate-x-1/2 py-3 text-gray-500 text-xs md:text-sm">
        <p>© 2025 AIRIQ | Real-time DHT22 Sensor Data</p>
      </footer>
    </div>

    <!-- Script Section -->
    <script>
        // ====== CARD UPDATE (setiap 0.5 detik) ======
        async function updateCards() {
            try {
              const res = await fetch("/get-data");
              const data = await res.json();

              const tempNotif = document.getElementById("temp-notif");
              const humiNotif = document.getElementById("humi-notif");

              document.getElementById("temperature").innerText =
                  data.temperature ?? "--";
              document.getElementById("humidity").innerText =
                  data.humidity ?? "--";
              
              const setNotif = (el, text, color) => {
                el.innerText = text;
                el.className = `mt-3 text-xs sm:text-sm md:text-base font-medium text-${color}-300 bg-${color}-900/40 border border-${color}-700/40 px-3 sm:px-4 py-1 sm:py-2 rounded-lg shadow-md transition-all duration-300 text-center`.trim();
              }

              if (data.temperature >= 30) {
                setNotif(tempNotif, "⚠️ Suhu udara terlalu panas", "red");
              } else if (data.temperature <= 21) {
                setNotif(tempNotif, "❄️ Suhu udara terlalu dingin", "blue");
              } else {
                setNotif(tempNotif, "😊 Suhu udara normal", "green");
              }

              if (data.humidity >= 65) {
                 setNotif(humiNotif, "⚠️ Kelembapan diatas batas normal", "red");
              } else if (data.humidity <= 30) {
                setNotif(humiNotif, "⚠️ Kelembapan dibawah batas normal", "blue");
              } else {
                setNotif(humiNotif, "😊 Kelembapan normal", "green");
              }
            } catch (err) {
              console.error("❌ Gagal mengambil data untuk card:", err);
            }
        }

        updateCards();
        setInterval(updateCards, 1000);

        // ====== INISIALISASI CHART ======
        const ctx = document.getElementById("sensorChart").getContext("2d");
        const sensorChart = new Chart(ctx, {
            type: "line",
            data: {
              labels: [],
              datasets: [
                  {
                    label: "Temperature (°C)",
                    borderColor: "rgba(239,68,68,1)",
                    backgroundColor: "rgba(239,68,68,0.2)",
                    data: [],
                    tension: 0.3,
                    fill: true,
                  },
                  {
                    label: "Humidity (%)",
                    borderColor: "rgba(56,189,248,1)",
                    backgroundColor: "rgba(56,189,248,0.2)",
                    data: [],
                    tension: 0.3,
                    fill: true, 
                  },
              ],
            },
            options: {
              responsive: true,
              scales: {
                  x: {
                      ticks: { color: "#9ca3af" },
                      grid: { color: "rgba(255,255,255,0.05)" },
                  },
                  y: {
                      ticks: { color: "#9ca3af" },
                      grid: { color: "rgba(255,255,255,0.05)" },
                  },
              },
              plugins: {
                  legend: { labels: { color: "#f3f4f6" } },
              },
            },
        });

        // ====== CHART UPDATE (setiap 10 detik) ======
        async function updateChart() {
            try {
              const res = await fetch("/get-data");
              const data = await res.json();
              const now = new Date().toLocaleTimeString();

              sensorChart.data.labels.push(now);
              sensorChart.data.datasets[0].data.push(data.temperature ?? 0);
              sensorChart.data.datasets[1].data.push(data.humidity ?? 0);

              if (sensorChart.data.labels.length > 15) {
                sensorChart.data.labels.shift();
                sensorChart.data.datasets.forEach((ds) => ds.data.shift());
              }

              sensorChart.update();
            } catch (err) {
              console.error("❌ Gagal mengambil data untuk chart:", err);
            }
        }

        updateChart();
        setInterval(updateChart, 10000);

        // ====== CHART LABELS DELETE (2 labels setiap 60 detik) ======
        setInterval(() => {
            const removeCount = 2; 

            if (sensorChart.data.labels.length > removeCount) {
              sensorChart.data.labels.splice(0, removeCount);
              sensorChart.data.datasets.forEach((ds) =>
                ds.data.splice(0, removeCount)
              );
              sensorChart.update();
            }
        }, 60000); 
    </script>
  </body>
</html>
