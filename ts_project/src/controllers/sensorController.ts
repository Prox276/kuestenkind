import { Database } from 'sqlite';
import { Measurement } from '../models/Measurement';

export const addMeasurement = async (db: Database, data: Measurement) => {
  await db.run(
    'INSERT INTO measurements (temperature, ph) VALUES (?, ?)',
    [data.temperature, data.ph]
  );
};

export const getMeasurements = async (db: Database): Promise<Measurement[]> => {
  return db.all('SELECT * FROM measurements ORDER BY timestamp DESC');
};