// Modules
const express = require("express");
const app = express();
const bodyParser = require("body-parser");
const dotenv = require("dotenv");
dotenv.config();

// Routers
const home = require("./src");

// App setting
app.set('view engine', 'ejs'); // template engine
app.set("views", "./src/view"); // view file path

// Middleware
app.use(bodyParser.json({ extended: true })); // body-parser
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.raw({ extended: true }));

// static folder
app.use(express.static(`${__dirname}/images`));
app.use(express.static(`${__dirname}/src/css`));

app.use("/", home);

module.exports = app;