const express = require('express');
const admin = require('firebase-admin');
const cors = require('cors');

// ==== 1. Colle ici le contenu du fichier JSON que tu as téléchargé ====
const serviceAccount = require('./firebase-key.json');

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "Lien du firebase"
});

const db = admin.database();
const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(__dirname)); // sert tous les fichiers du dossier

// Stockage en mémoire des dernières données
let latestData = [];
let lastData = {};

// Écoute les changements dans Firebase
db.ref("/smart_house/sensor_data/last").on('value', (snapshot) => {
  const data = snapshot.val();
  if (data) {
    // Conversion pour correspondre à ton dashboard actuel
    const converted = {
      temperature: data.temp || 0,
      humidity: data.hum || 0,
      gas: data.gas || 0,
      light: data.light || 0,
      room: "Salon",
      timestamp: new Date().toISOString(), // ou tu peux garder millis() et convertir côté front
      alarm: data.alarm,
      led1: data.led1,
      led2: data.led2,
      led3: data.led3,
      led4: data.led4
    };

    lastData = converted;

    // Historique (optionnel, on garde les 50 dernières)
    latestData.unshift(converted);
    if (latestData.length > 50) latestData.pop();
  }
});

// Route que ton dashboard appelle
app.get('/api/sensor', (req, res) => {
  res.json(latestData.length > 0 ? latestData : [lastData]);
});

app.get('/api/sensor/latest', (req, res) => {
  res.json(lastData);
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Serveur démarré sur http://localhost:${PORT}`);
  console.log(`Dashboard → http://localhost:${PORT}/public/index.html (tu peux servir le fichier)`);
});
