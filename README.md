# 🏠 SMART HOUSE IoT PLATFORM

## 📌 Project Overview
Smart House application designed to collect, process, and display
real-time data from sensors connected to an ESP32.

## 🎯 Objectives
- Monitor temperature, humidity, luminosity, and gas levels
- Store sensor data in MongoDB (Dockerized)
- Provide real-time dashboard
- Control actuators remotely
- Trigger alerts when thresholds are exceeded

## 🛠️ Tech Stack
- ESP32
- Node.js (Express 5)
- MongoDB
- Docker
- Firebase Admin
- JWT Authentication

## 🏗️ Architecture
ESP32 → Backend API → MongoDB → Web Dashboard

## 🚀 How to Run

### 1️⃣ Clone
git clone https://github.com/AyoubCharbaji/Smart_House_Project.git

### 2️⃣ Backend
cd backend
npm install
npm run dev

### 3️⃣ Docker (MongoDB)
docker-compose up -d
