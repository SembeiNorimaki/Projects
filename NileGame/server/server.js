const express = require("express");
const cors = require("cors");
const monk = require("monk");

const app = express();
app.use(express.static("client"));
const server = app.listen(3000, () => {
  console.log("Listening on http://localhost:3000")
});
let socket = require("socket.io");
let io = socket(server);

// relates each player id to a connection socker
let connections = new Array();

//const db = monk("localhost/gameDB");
const Game = require("./game.js");
let game;


io.sockets.on("connection", (socket) => {
  console.log("New connection: " + socket.id);
  connections.push(socket);
  io.to(socket.id).emit("setPlayerName", "Player " + 
                        (connections.length-1));
  socket.on('Market', marketAction);  
  socket.on('Offer', offerAction);
  socket.on('Plant', plantAction);
  socket.on('Speculate', speculateAction);
  socket.on('Pass', passAction);
  socket.on('disconnect', () => {
    console.log(socket.id + ' disconnected');
    // if a player disconnects we finish the game
    for (let i=0; i<connections.length; i++) {
      io.to(connections[i].id).emit("errorMessage", "Player disconnected. Game finished");
      connections = [];
      game = null;
    }
    //connections.splice(connections.indexOf(socket), 1);
    //connections[connections.indexOf(socket)] = null;
  });

  if (connections.length >= 2) {
    startGame(2);
  }
});

function marketAction(cards) {
  console.log("Market action received with data:");
  console.log(cards);
  try {
    // do the market action
    game.action.market(cards, game);
    // send the updated status to client
    // io.to(connections[game.turn].id).emit("update", 
    //   game.getInfo(game.turn));
    io.to(connections[game.turn].id).emit("update", 
      game.getInfo(game.turn));
  } catch(err) {
    console.log("Exception catched");
    io.to(connections[game.turn].id).emit("errorMessage", err);
  }
}
function offerAction(cards) {
  console.log("Offer action received with data:");
  console.log(cards);
  try {
    // do the offer action
    game.action.offer(cards, game);
    // send the updated status to client
    io.to(connections[game.turn].id).emit("update", 
      game.getInfo(game.turn));
  } catch(err) {
    console.log("Exception catched");
    io.to(connections[game.turn].id).emit("errorMessage", err);
  }  
}
function plantAction(cards) {
  console.log("Plant action received with data:");
  console.log(cards);
  try {
    // TODO: player 0 and 1 must be turn and noturn
    game.action.plant(cards, game);
    for(let i=0; i<game.nplayers; i++) {
      io.to(connections[i].id).emit("update", game.getInfo(i));
    }
  } catch(err) {
    console.log("Exception catched");
    io.to(connections[game.turn].id).emit("errorMessage", err);
  }
}
function speculateAction(cards) {
  console.log("Speculate action received with data:");
  console.log(cards);
  try {
    game.action.speculate(cards, game);
    io.to(connections[game.turn].id).emit("update", 
      game.getInfo(game.turn));
  } catch(err) {
    console.log("Exception catched");
    io.to(connections[game.turn].id).emit("errorMessage", err);
  }
}
function passAction() {
  console.log("Pass action received");
  try {
    game.action.pass(game);
    for (let i=0; i<game.nplayers; i++) {
      io.to(connections[i].id).emit("update", 
        game.getInfo(i));
    }
  } catch(err) {
    console.log("Exception catched");
    io.to(connections[game.turn].id).emit("errorMessage", err);
  }
}

function startGame(nplayers) {
  console.log("Game started");
  game = new Game.Game(nplayers);
  game.initialize();
  for (let i=0; i<nplayers; i++) {
    io.to(connections[i].id).emit("update", game.getInfo(i));
  } 
}
