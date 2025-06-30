/**
 * Kuestenkind - server component
 * @author Deiras
 * @created 27.06.2025
 * @lastUpdated 30.06.2025
 * Update: cors hinzugefügt, komplette überarbeitung für kompabilität und komfortabilität, sowie API hinzugefügt
 */
import express from 'express';
import { setupDB } from './db/database';
import { sensorRouter } from './routes/sensor';
import dotenv from 'dotenv';
import cors from 'cors';

dotenv.config();

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'));

async function startServer() {
  const db = await setupDB();
  
  // API-routen
  app.use('/api', sensorRouter(db));

  // Start server
  const server = app.listen(PORT, () => {
    console.log(`Server läuft auf http://localhost:${PORT}`);
    console.log(`ESP8266 Endpoint: http://${getLocalIp()}:${PORT}`);
  });

  return server;
}

// hilfsfunktion für ip-ermittlung
function getLocalIp(): string {
  const interfaces = require('os').networkInterfaces();
  for (const name in interfaces) {
    for (const iface of interfaces[name]) {
      if (iface.family === 'IPv4' && !iface.internal) {
        return iface.address;
      }
    }
  }
  return 'localhost';
}

startServer();

// Ins Terminal: npx nodemon src/server.ts , eingeben