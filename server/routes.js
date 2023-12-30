const express = require('express');
const api = express.Router();

const LiquidityFarming = require('./liquidity-farming');

api.use(express.json());

api.post('/liquidity-farming/sync', LiquidityFarming.sync);
api.post('/liquidity-farming/sendreward', LiquidityFarming.sendReward);

module.exports = api;