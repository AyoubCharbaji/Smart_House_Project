const axios = require("axios");

// Ton vrai Firebase
const databaseURL = "Lien du firebase";
const apiKey = "Ton API KEY";

// Ton backend local
const backendURL = "http://localhost:3000/api/sensor";

// Fonction pour lire la valeur depuis Firebase
async function getValue(path) {
  try {
    const url = `${databaseURL}${path}.json?auth=${apiKey}`;
    const res = await axios.get(url);
    return res.data;
  } catch (err) {
    console.error(`Erreur Firebase (${path}) :`, err.message);
    return null;
  }
}

// Fonction principale : lire Firebase et envoyer au backend
async function bridge() {
  const t = await getValue("/TEMPERATURE");
  const h = await getValue("/HUMIDITY");
  const gas = await getValue("/GAZ");
  const light = await getValue("/LDR_VALUE");

  if (t === null || h === null || gas === null || light === null) return;

  const payload = {
    room: "Maison",
    temperature: t,
    humidity: h,
    gas: gas,
    light: light
  };

  try {
    await axios.post(backendURL, payload);
    console.log("✔ Transféré dans MongoDB :", payload);
  } catch (err) {
    console.error("Erreur d’envoi backend :", err.message);
  }
}

// Lancer toutes les 2 secondes
setInterval(bridge, 2000);
