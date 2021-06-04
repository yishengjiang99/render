const app = require("express")();
const spawn = require("child_process").spawn;
const { stdout, stderr, stdin } = spawn("./html.o");

app.get("/", (req, res) => {
  stdout.pipe(res);
  stdin.write("l");
});
app.listen(4444);