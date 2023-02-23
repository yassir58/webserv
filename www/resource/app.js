let username = "yassir58";
var request = new XMLHttpRequest();
request.open('GET', 'https://api.github.com/users/' + username);

var data = JSON.parse(request.responseText);

console.log (data);