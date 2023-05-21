"use strict";

const app = require("../app");
const PORT = process.env.PORT || 3000; // 포트


app.listen(PORT, async () => {
  console.log(`Server running on port ${PORT}`);
});