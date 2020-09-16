const CROP_TYPES = ["Flax", "Papyrus", "Lettuce", "Castor", "Wheat",
  "Flax_Papyrus", "Flax_Lettuce", "Flax_Castor", "Flax_Wheat",
  "Papyrus_Lettuce", "Papyrus_Castor", "Papyrus_Wheat",
  "Lettuce_Castor", "Lettuce_Wheat", "Castor_Wheat"];

class Hand {
  constructor() {
    this.cards = {};

    this.emptyHand = function() {
      for (let crop of CROP_TYPES) {
        this.cards[crop] = 0;
      }
    }
    this.setHand = function(cards) {
      this.emptyHand();
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
      return card;
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
  }
}

module.exports = { Hand }
