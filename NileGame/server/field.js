const CROP_TYPES = ["Flax", "Papyrus", "Lettuce", "Castor", "Wheat"];

class Field {
  constructor() {
    this.cards = {}; 
    

    this.emptyField = function() {
      for (let crop of CROP_TYPES) {
        this.cards[crop] = 0;
      }
    }
    this.setField = function(cards) {
      this.emptyField();
      this.addCards(cards);
    }
    this.addCard = function(card) {
      this.cards[card]++;
    }
    this.addCards = function(cards) {
      for(let card of cards) {
        this.addCard(card);
      }
    }
    this.removeCard = function(card) {
      if (this.cards[card] == 0) {
        return false;
      }
      this.cards[card]--;
      return true;
    }
    this.removeCards = function(cards) {
      for (let card of cards) {
        this.removeCard(card);
      }
    }
    // returns the Hand as an array
    this.asArray = function() {
      let result = new Array();
      for (let crop of CROP_TYPES) {
        for (let i=0; i<this.cards[crop]; i++) {
          result.push(crop);
        }
      }
      return result;
    }
    this.getQty = function(card) {
      return this.cards[card];
    }
    // TODO: implement this
    this.draw = function() {

    }
    this.emptyField();
  }
}

module.exports = { Field }
