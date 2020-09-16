const Hand = require("./hand.js");
const Storage = require("./storage.js");
const Field = require("./field.js");


class Player {
  constructor() {
    this.hand = new Hand.Hand();
    this.storage = new Storage.Storage();
    this.field = new Field.Field();
    this.selection = new Array();
    
    this.initialize = function() {
    
    }
  }
}

module.exports = { Player }
