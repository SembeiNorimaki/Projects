const MARGIN = 10;
const CARD_W = 90;
const CARD_H = 126;
const HAND_X = 20;
const HAND_Y = 380;
const HAND_W = 400;
const HAND_H = CARD_H;
const FLOOD_X = 10;
const FLOOD_Y = 100;
const STORAGE_X = HAND_X;
const STORAGE_Y = HAND_Y + HAND_H + 3*MARGIN;
const STORAGE_W = HAND_W;
const STORAGE_H = CARD_H;

const FIELD_W = 400;
const FIELD_H = CARD_H;

const FIELD1_X = 300;
const FIELD1_Y = 65;
const FIELD2_X = FIELD1_X + FIELD_W + 3*MARGIN;
const FIELD2_Y = FIELD1_Y;
const FIELD3_X = FIELD1_X;
const FIELD3_Y = FIELD1_Y + FIELD_H + 3*MARGIN;
const FIELD4_X = FIELD2_X;
const FIELD4_Y = FIELD3_Y;

const MAX_HAND = 10;
const BGCOL = 240;


let socket;
let imgs = {};
let config;
let data;
let selected = new Array(10).fill(false);
let buttons = [];


function setup() {
  
}

function preload() {
  config = loadJSON('config.json');
  imgs.Flax = loadImage("images/flax.jpg");
  imgs.Papyrus = loadImage("images/papyrus.jpg");
  imgs.Lettuce = loadImage("images/lettuce.jpg");
  imgs.Castor = loadImage("images/castor.jpg");
  imgs.Wheat = loadImage("images/wheat.jpg");
  imgs.Locusts = loadImage("images/locusts.jpg");
  imgs.Flood = loadImage("images/flood.jpg");
  imgs.Reverse = loadImage("images/reverse.jpg");
  imgs.Flax_Papyrus = loadImage("images/Flax_Papyrus.jpg");
  imgs.Flax_Lettuce = loadImage("images/Flax_Lettuce.jpg");
  imgs.Flax_Castor = loadImage("images/Flax_Castor.jpg");
  imgs.Flax_Wheat = loadImage("images/Flax_Wheat.jpg");
  imgs.Papyrus_Lettuce = loadImage("images/Papyrus_Lettuce.jpg");
  imgs.Papyrus_Castor = loadImage("images/Papyrus_Castor.jpg");
  imgs.Papyrus_Wheat = loadImage("images/Papyrus_Wheat.jpg");
  imgs.Lettuce_Castor = loadImage("images/Lettuce_Castor.jpg");
  imgs.Lettuce_Wheat = loadImage("images/Lettuce_Wheat.jpg");
  imgs.Castor_Wheat = loadImage("images/Castor_Wheat.jpg");
}

function setup(){
  noLoop();
  createCanvas(1500, 700);
  background(config.BGCOL);
  //showInfoMessage('Connecting to server...');

  socket = io.connect("http://localhost:3000");
  socket.on("setPlayerName", printPlayerName);
  socket.on("update", update);
  socket.on("errorMessage", showErrorMessage);
}

function showInfoMessage(infoMsg) {
  push();
  fill("white");
  rect(10, 10, 500, 30);
  fill("black");
  text(infoMsg, 20, 30);
  pop();
}

function showErrorMessage(errorMsg) {
  push();
  fill("red");
  rect(10, 10, 500, 30);
  fill("black");
  text(errorMsg, 20, 30);
  pop();
  console.log(errorMsg);
}

function printPlayerName(data) {
  showInfoMessage("You are player " + data + ".  Waiting for the game to start...");
}

function drawButtons() {
  if (data.buttons.length == 3) {
    button1 = createButton(data.buttons[0]).position(600, 20).mousePressed(button1Pressed);
    button2 = createButton(data.buttons[1]).position(750, 20).mousePressed(button2Pressed);
    button3 = createButton(data.buttons[2]).position(900, 20).mousePressed(button3Pressed);
    console.log(button1);
  }
}

function getSelectedCards(selected) {
  let selection = new Array();
  for (let i=0; i<MAX_HAND; i++) {
    if (selected[i]) {
      selection.push(data.hand[i]);
    }
  }
  return selection;
}

isMyTurn = function() {
  return data.turn == data.playerIdx;
}

function button1Pressed() {
  console.log("turn: " + data.turn);
  console.log("playerIdx: " + data.playerIdx);

  if (isMyTurn()) {
    console.log("Sending " + data.buttons[0] + " action to server");
    socket.emit(data.buttons[0], getSelectedCards(selected));
    resetSelected();
  } else {
    showErrorMessage("ERROR, it's not your turn");
  }
  selected.fill(false);
}
function button2Pressed() {
  if (isMyTurn()) {
    console.log("Sending " + data.buttons[1] + " action to server");
    socket.emit(data.buttons[1], getSelectedCards(selected));
    resetSelected();
  } else {
    showErrorMessage("ERROR, it's not your turn");
  }
  selected.fill(false);
}
function button3Pressed() {
  if (isMyTurn()) {
    console.log("Sending " + data.buttons[2] + " action to server");
    socket.emit(data.buttons[2]);
    resetSelected();
  } else {
    showErrorMessage("ERROR, it's not your turn");
  }
  selected.fill(false);
}

function drawBackground() {
  push();
  fill(BGCOL);
  stroke(255);
  strokeWeight(4);
  // HAND
  rect(HAND_X-MARGIN, HAND_Y-MARGIN, HAND_W+2*MARGIN, HAND_H+2*MARGIN, MARGIN);
  // STORAGE
  rect(STORAGE_X-MARGIN, STORAGE_Y-MARGIN, STORAGE_W+2*MARGIN, STORAGE_H+2*MARGIN, MARGIN);
  // FIELD 1
  rect(FIELD1_X-MARGIN, FIELD1_Y-MARGIN, FIELD_W+2*MARGIN, FIELD_H+2*MARGIN, MARGIN);
  // FIELD 2
  rect(FIELD2_X-MARGIN, FIELD2_Y-MARGIN, FIELD_W+2*MARGIN, FIELD_H+2*MARGIN, MARGIN);
  // FIELD 3
  rect(FIELD3_X-MARGIN, FIELD3_Y-MARGIN, FIELD_W+2*MARGIN, FIELD_H+2*MARGIN, MARGIN);
  // FIELD 4
  rect(FIELD4_X-MARGIN, FIELD4_Y-MARGIN, FIELD_W+2*MARGIN, FIELD_H+2*MARGIN, MARGIN);
  
  pop();
}

function draw(){
}

function drawLog() {
  text("Lorem ipsum", 1200, 20);
}

function update(updated_data) {

  data = updated_data;
  console.log("Update data from server:");
  console.log(data);
  drawBackground();
  showInfoMessage("You are player " + data.playerIdx + 
    "  TURN: " + data.turn + "  PHASE: " + data.phase);
  drawButtons();
  drawLog();
  drawHand(data.hand);
  drawFlood(data.flood);
  drawPiles(data.storage, STORAGE_X, STORAGE_Y);
  drawPiles(data.fields[0], FIELD1_X, FIELD1_Y);
  drawPiles(data.fields[1], FIELD2_X, FIELD2_Y);
}

function drawCard(card, x, y) {
  image(imgs[card], x+2, y+2, CARD_W-4, CARD_H-4);
}

function drawFlood(card) {
  drawCard("Reverse", FLOOD_X, FLOOD_Y);
  drawCard("Reverse", FLOOD_X+4*1, FLOOD_Y+4*1);
  drawCard("Reverse", FLOOD_X+4*2, FLOOD_Y+4*2);
  drawCard("Reverse", FLOOD_X+4*3, FLOOD_Y+4*3);
  drawCard("Flood", FLOOD_X+CARD_W+20, FLOOD_Y);
  drawCard(card, FLOOD_X+CARD_W+20, FLOOD_Y + 20);
  text("CARDS LEFT: " + data.cardsInDeck, FLOOD_X, FLOOD_Y+CARD_W+7*MARGIN);
  text("RESHUFFLES: " + data.nreshuffles, FLOOD_X, FLOOD_Y+CARD_W+8*MARGIN);
}

function drawHand(cards) {
  let x = HAND_X, y = HAND_Y;
  for (let card of cards) {
    drawCard(card, x, y);
    x += CARD_W;
  }
}

function drawPiles(cards, xi, yi) {
  if (cards.length == 0)
    return;
  let x = xi, y = yi;
  drawCard(cards[0], x, y);
  for (let i=1; i<cards.length; i++) {
    if (cards[i] == cards[i-1]) {
      x += 15;
      // y += 25;
    } else {
      x += CARD_W + 5;
      y = yi;
    }
    drawCard(cards[i], x, y);
  }
}

function resetSelected() {
  for(let i=0; i<10; i++) {
    selected[i] = false;
  }
}
function mouseClicked() {
  // determine which card in hand has been clickedp5.BandPass()
  if ((mouseY > HAND_Y) && (mouseY < HAND_Y + CARD_H) && (mouseX > HAND_X)) {
    let i = floor((mouseX - HAND_X) / CARD_W);
    if (i < data.hand.length) {
      selected[i] = !selected[i];
      console.log(selected);
      push();
      if (selected[i]) {
        stroke("blue");
      } else {
        stroke("white");
      }
      strokeWeight(4);
      rect(HAND_X + i * CARD_W, HAND_Y, CARD_W, CARD_H);
      pop();
      drawHand(data.hand);
    }
  }
}

function onBeforeUnload(e) {
  if (thereAreUnsavedChanges()) {
      e.preventDefault();
      e.returnValue = '';
      return;
  }

  delete e['returnValue'];
}

window.addEventListener('beforeunload', onBeforeUnload);