const http = require(’http’);
const fs = require(’fs’);
const url = require(’url’);
const path = require(’path’);
const server = http.createServer( (request, response) => {
let pathname = url.parse(request.url).pathname;
console.log("Request for " + pathname + " received.");
const safePath = path.normalize(path.join(process.cwd(), pathna
fs.readFile(safePath, (err, data) => {
if (err) {
console.error(err);
response.writeHead(404, { ’Content-Type’: ’text/html’ });
response.end(’<h1>404 - File Not Found</h1>’);
return;
}
response.writeHead(200, { ’Content-Type’: ’text/html’ });
response.end(data);
});
});
server.listen(3000, () => {
console.log(’Serwer nasłuchuje na http://127.0.0.1:3000/’);
});
