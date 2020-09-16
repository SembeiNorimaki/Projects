const Game = require("./game.js"); 

let game = new Game.Game();
game.initialize();
//console.log(game.players[0]);
console.log(game.getInfo(0));
console.log(game.getInfo(1));

