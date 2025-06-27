import express, { json, Express } from 'express';
import { setupDB } from './db/database';
import { sensorRouter } from './routes/sensor';
import dotenv from 'dotenv';

dotenv.config();

const app: Express = express();
const PORT = process.env.PORT || 5500;
/* 
VSCode verwendet Port 5500, es kann sein,
dass du einen anderen Port verwenden wirst,
standardmäßig auf 3000 eigentlich.
*/
// Middleware muss vor Routen!!
app.use(json());
app.use(express.static('public'));

async function startServer() {
  const db = await setupDB();
  
  // Sensor-Routen
  app.use('/api', sensorRouter(db));

  app.listen(PORT, () => {
    console.log(`Server läuft auf http://localhost:${PORT}`);
  });
}

startServer();

// Ins Terminal: npx nodemon src/server.ts , eingeben