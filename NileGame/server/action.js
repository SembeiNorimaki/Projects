const game = require("./game");
const field = require("./field");
const hand = require("./hand");
const player = require("./player");

class Action {
  constructor() {
    this.isCrop = function(card) {
      return ["Flax", "Papyrus", "Lettuce", "Castor", "Wheat"].includes(card);
    }
    this.isSpeculation = function(card) {
      return ["Flax_Papyrus", "Flax_Lettuce", "Flax_Castor", "Flax_Wheat", "Papyrus_Lettuce", "Papyrus_Castor", "Papyrus_Wheat", "Lettuce_Castor", "Lettuce_Wheat", "Castor_Wheat"].includes(card);
    }

    this.plant = function(cards, game) {
      console.log("Received plant action");
      if (cards.length == 0) {
        console.log("ERROR, cannot plant 0 cards");
        throw "ERROR, cannot plant 0 cards";
      }
      for (let card of cards) {
        // check that all cards are crops
        if (!this.isCrop(card)) {
          console.log("ERROR, all cards must be crops");
          throw "ERROR, all cards must be crops";
        }
        // check that no card is flooded
        if (card == game.floodedCard) {
          console.log("ERROR, cannot plant a flooded crop");
          throw "ERROR, cannot plant a flooded crop";
        }
      }
      // if a single card is planted it must already exist in field
      if (cards.length == 1) {
        if (game.player[game.turn].field[cards[0]] > 0) {
          // good we can plant it
          game.player[game.turn].field.addCard(cards[0]);
          game.player[game.turn].hand.removeCard(cards[0]);
          this.nextTurn(game);
          return;
        }
        else {
          console.log("ERROR, planting only 1 crop requires having already a field of that type");
          throw("ERROR, planting only 1 crop requires having already a field of that type");
        }
      }
      // if >2 cards, then all crops must be the same type
      if (cards.length > 2) {
        for (let i=1; i<cards.length; i++) {
          if (cards[i] != cards[0]) {
            console.log("ERROR, Planting more than 2 cards require all cards to be of the same type");
            throw("ERROR, Planting more than 2 cards require all cards to be of the same type");
          }
        }
        // Planting more than 2 crops of the same type. 
        // We must check that no oponent has a >= field of the same crop
        for (let i=0; i<game.nplayers; i++) {
          if (game.turn == i) {
            continue;
          }
          if (game.player[i].field.cards[cards[0]] >= cards.length) {
            console.log("ERROR, another player has already a field of this size or bigger");
            throw("ERROR, another player has already a field of this size or bigger");
          } else {
            // ok, we can plant
            game.player[game.turn].field.addCards(cards);
            game.player[game.turn].hand.removeCards(cards);
            this.nextTurn(game);
            return;
          }
        }      
      }
      // Planting exactly 2 crops
      if (cards[0] == cards[1]) {
        // 2 cards of the same crop
        // check that no other player has a >= field
        for (let i=0; i<game.nplayers; i++) {
          if (game.turn == i) {
            continue;
          }
          if (game.player[i].field.cards[cards[0]] >= cards.length) {
            console.log("ERROR, another player has already a field of this size or bigger");
            throw("ERROR, another player has already a field of this size or bigger");
          } else {
            // ok, we can plant
            game.player[game.turn].field.addCards(cards);
            game.player[game.turn].hand.removeCards(cards);
            this.nextTurn(game);
            return;
          }
        }
      } else { 
        // two different crops, 
        // check that no other player has a >= field for each crop
        for (let i=0; i<game.nplayers; i++) {
          if (game.turn == i) {
            continue;
          }
          if ((game.player[i].field.cards[cards[0]] > 0) && (game.player[i].field.cards[cards[1]] > 0)) {
            console.log("ERROR, another player has already a field of this size or bigger");
            throw("ERROR, another player has already a field of this size or bigger");
          } else {
            // ok, we can plant
            game.player[game.turn].field.addCards(cards);
            game.player[game.turn].hand.removeCards(cards);
            this.nextTurn(game);
            return;
          }
        }
      }
    }
    
    this.speculate = function(cards, game) {
      console.log("Received speculate action");
      if (cards.length == 0 || cards.length > 2) {
        console.log("ERROR, you can only speculate with 1 or 2 cards");
        throw "ERROR, you can only speculate with 1 or 2 cards";
      }
      for (let card of cards) {
        // check that no crop is in the card (so there are all speculation)
        console.log(card);
        if (!this.isSpeculation(card)) {
          console.log("ERROR, You can only speculate with speculation cards");
          throw "ERROR, You can only speculate with speculation cards";
        }
      }
      // do the speculation
      // remove cards from player hand
      game.player[game.turn].hand.removeCards(cards);
      console.log("A");
      // flood a new card
      this.newFlood(game);
      console.log("A");
      // check speculation result
      for (let card of cards) {
        console.log("A");
        if (true) { // TODO: check successful speculation
          game.player[game.turn].hand.addCards(game.deck.getCards(3));
        }
      }
    }
    
    this.market = function(cards, game) {
      if (cards.length != 2) {
        console.log("ERROR, exactly 2 cards needed for market");
        throw "ERROR, exactly 2 cards needed for market";
      }
      //  discard the two cards from player's hand
      for (let card of cards) {
        game.player[game.turn].hand.removeCard(card);
        game.discard.addCard(card);
      }
      // give the player a new card from the deck
      // TODO: we have to check for locusts
      game.player[game.turn].hand.addCard(game.deck.getCard());
    }
    
    this.offer = function(cards, game) {
      console.log("Received offer action");
      if (cards.length != 2) {
        console.log("ERROR, exactly 2 cards needed for offer");
        throw "ERROR, exactly 2 cards needed for offer";       
      }
      //  discard the two cards from player's hand
      for (let card of cards) {
        game.player[game.turn].hand.removeCard(card);
        game.discard.addCard(card);
      }
      this.newFlood(game);
    }

    this.pass = function(game) {
      console.log("Received pass action");
      if (game.phase == 0) {
        game.phase = 1;
      } else {
        game.phase = 0;
        game.turn++;
        if (game.turn >= game.nplayers) {
          game.turn = 0;
        }
      }
    }

    this.nextTurn = function(game) {
      // when turn must change:
      // player gets 2 cards
      game.player[game.turn].hand.addCards(game.deck.getCards(2));
      // new flood (lucusts is checked in newFlood)
      this.newFlood(game);
      // speculation is checked

      // we update the turn
      game.turn = ++game.turn % game.nplayers;
      game.phase = 0;
      
    }

    this.nextPhase = function(game) {
      if (game.phase == 0) {
        game.phase++;
      } else {
        this.nextTurn(game);
      }
    }

    this.newFlood = function(game) {
      // discard previous flood
      game.discard.addCard(game.floodedCard);
      // get a new flood
      game.floodedCard = game.deck.getCard();
      // check for locusts
      if (game.floodedCard == "locusts") {
        this.locustsRevealed(game);
      }
      // harvest
      for (let i=0; i<game.nplayers; i++) {
        if(game.player[i].field.cards[game.floodedCard] > 0) {
          game.player[i].field.removeCard(game.floodedCard);
          game.player[i].storage.addCard(game.floodedCard);
          // only one player can have this crop, so we can break here
          break;
        }
      }
    }

    this.locustsRevealed = function(game) {
      // destroy the biggest fields
      // check the size of the biggest field
      let biggest = 0;
      for (let field of fields) {
        
      }
    }
  }
}

module.exports = { Action }
