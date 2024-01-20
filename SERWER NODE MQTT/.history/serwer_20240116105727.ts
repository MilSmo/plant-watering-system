import mqtt from 'mqtt';
import express, { Request, Response } from 'express';
import { open, Database } from 'sqlite';
import sqlite3 from 'sqlite3';