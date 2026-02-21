const axios = require("axios");

function simulateValues() {
  const t = 23 + Math.random() * 6; // température 20–30
  const h = 35 + Math.random() * 20;        // humidité 40–60
  const gas = 300 + Math.random() * 500;   // gaz 200–500
  const light = Math.floor(Math.random() * 4095); // LDR 0–4095

  return { room: "Salon", temperature: t, humidity: h, gas: gas, light: light };
}

async function sendData() {
  const data = simulateValues();

  try {
    await axios.post("http://localhost:3000/api/sensor", data);
    console.log("✔ Données envoyées :", data);
  } catch (err) {
    console.error("Erreur :", err.message);
  }
}

// envoie chaque 3 secondes
setInterval(sendData, 3000);
