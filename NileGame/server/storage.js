class Storage {
  constructor() {
    this.cards = new Array();
    
    this.addCard = function(card) {
      this.cards.push(card);
      // TODO: sort
    }

    // returns the Storage as an array
    this.asArray = function() {
      return this.cards;
    }
    
    // TODO: implement
    this.draw = function() {

    }
  }
}

module.exports = { Storage }
