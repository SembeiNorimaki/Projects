const Deck = require("./deck.js");
const Action = require("./action.js");
const Player = require("./player.js");

class Game {
  constructor(nplayers) {
    this.nplayers = nplayers;
    this.player = new Array();
    this.deck = new Deck.Deck();
    this.discard = new Deck.Deck();
    this.action = new Action.Action();  // reserved word?
    this.floodedCard;
    this.turn = 0;
    this.phase = 0;

    this.initialize = function() {
      this.deck.initialize();
      for (let i=0; i<this.nplayers; i++) {
        this.player.push(new Player.Player("Player " + i));
        this.player[i].hand.setHand(this.deck.getCards(5));
      }

      // set the initial flood card
      this.floodedCard = this.deck.getCard();
      
      // after giving cards to players we add locusts to the deck
      this.deck.addLocusts();      
    }

    // returns the data to be sent to playerId
    this.getInfo = function(playerIdx) {
      let data = {
        playerIdx: playerIdx,
        turn: this.turn,
        phase: this.phase,
        cardsInDeck: this.deck.cards.length,
        nreshuffles: 2,
        flood: this.floodedCard,
        hand: this.player[playerIdx].hand.asArray(),
        storage: this.player[playerIdx].storage.asArray(),
        fields: [
          this.player[0].field.asArray(),
          this.player[1].field.asArray()],
        buttons: []        
      }
      if (this.phase == 0) {
        data.buttons = ["Market", "Offer", "Pass"]
      } else {
        data.buttons = ["Plant", "Speculate", "Pass"]
      }
      return data;
    }     
    
    // this.newFlood = function() {
    //   // get the old flooded card into the discard pile
    //   this.discard.push(this.floodedCard);
    //   // flood a new card from the deck
    //   this.floodedCard = game.deck.getCard();
    
    //   // players get crops
    //   for (player in this.player) {
    //     if (player.field.getQty(this.floodedCard) > 0) {
    //       // remove card from field
    //       player.field.removeCard(this.floodedCard.name);
    //       // and place it into storage
    //       player.storage.addCard(this.floodedCard.name);
    //     }
    //   }
    // }
  }
}

module.exports = { Game }