const cards_qty = require('./cards.json')

class Deck {
  constructor() {
    this.cards = new Array();

    function shuffle(a) {
      for (let i = a.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [a[i], a[j]] = [a[j], a[i]];
      }
      return a;
    }

    // do not include locust yet
    this.initialize = function() {
      // 10 of each resource type
      for (let i=0; i<10; i++) {
        this.cards.push("Flax");
        this.cards.push("Papyrus");
        this.cards.push("Lettuce");
        this.cards.push("Castor");
        this.cards.push("Wheat");
      }
      // 2 of each speculation type
      // for (let i=0; i<2; i++) {
      //   this.cards.push("Flax_Papyrus");
      //   this.cards.push("Flax_Lettuce");
      //   this.cards.push("Flax_Castor");
      //   this.cards.push("Flax_Wheat");
      //   this.cards.push("Papyrus_Lettuce");
      //   this.cards.push("Papyrus_Castor");
      //   this.cards.push("Papyrus_Wheat");
      //   this.cards.push("Lettuce_Castor");
      //   this.cards.push("Lettuce_Wheat");
      //   this.cards.push("Castor_Wheat");
      // }
      shuffle(this.cards);
    }

    this.addLocusts = function() {
      this.cards.push("Locusts");
      shuffle(this.cards);
    }

    this.getCard = function() {
      let card = this.cards.pop(); 
      return card;
    }

    this.getCards = function(n) {
      let cards = new Array();
      for (let i=0; i<n; i++) {
        cards.push(this.getCard());
      }
      return cards;
    }

    this.addCard = function(card) {
      this.cards.push(card);
    }
  }
}

module.exports = { Deck }
