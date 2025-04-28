-- Database and table setup for Airlytics
CREATE DATABASE IF NOT EXISTS airlytics;
USE airlytics;
CREATE TABLE IF NOT EXISTS readings (
  id INT AUTO_INCREMENT PRIMARY KEY,
  temperature FLOAT,
  humidity FLOAT,
  timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
