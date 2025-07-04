/**
 * Kuestenkind - server component
 * @author Deiras
 * @created 27.06.2025
 * @lastUpdated 30.06.2025
 * Update: Changed UTC to localtime
 */
import sqlite3 from 'sqlite3';
import { open } from 'sqlite';

export async function initializeDB() {
  return open({
    filename: './sensor.db',
    driver: sqlite3.Database
  });
}

export async function setupDB() {
  const db = await initializeDB();
  await db.exec(`
    CREATE TABLE IF NOT EXISTS measurements (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      temperature REAL NOT NULL,
      ph REAL NOT NULL,
      timestamp DATETIME DEFAULT (datetime('now', 'localtime'))
    )
  `);
  return db;
}