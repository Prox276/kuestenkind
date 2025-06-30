/**
 * Kuestenkind - server component
 * @author Deiras
 * @created 27.06.2025
 * @lastUpdated 27.06.2025
 * Update:
 */

import { Request, Response, Router, NextFunction, RequestHandler } from 'express';
import { Database } from 'sqlite';
import * as sensorController from '../controllers/sensorController';
import { Measurement } from '../models/Measurement';

export const sensorRouter = (db: Database): Router => {
  const router = Router();

  // GET-Route
  router.get('/data', (async (req: Request, res: Response, next: NextFunction) => {
    try {
      const measurements = await sensorController.getMeasurements(db);
      res.json(measurements);
    } catch (error) {
      console.error('Database error:', error);
      res.status(500).json({ error: 'Server error' });
    }
  }) as RequestHandler);

  // POST-Route
  router.post('/add', (async (req: Request, res: Response, next: NextFunction) => {
    const body = req.body as Partial<Measurement>;
    
    // Validation
    if (typeof body.temperature !== 'number' || typeof body.ph !== 'number') {
      return res.status(400).json({
        error: 'Invalid data format. Expected { temperature: number, ph: number }'
      });
    }

    try {
      await sensorController.addMeasurement(db, {
        temperature: body.temperature,
        ph: body.ph
      });
      res.status(201).json({ message: 'Data saved' });
    } catch (error) {
      console.error('Database error:', error);
      res.status(500).json({ error: 'Error saving data' });
    }
  }) as RequestHandler);

  return router;
};