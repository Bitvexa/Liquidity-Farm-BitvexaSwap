const express = require('express');
const cors = require('cors');
const app = express();
require("dotenv").config();

const api = require('./routes');

app.use(cors());
app.use('/api', api);

app.listen(8080);
