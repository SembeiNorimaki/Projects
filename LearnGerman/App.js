import React, { Component } from 'react';
import {
  StyleSheet,
  View,
  Text,
  TouchableOpacity
} from 'react-native';

import GestureRecognizer, {swipeDirections} from 'react-native-swipe-gestures';


import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import Home from './screens/Home';
import Quiz from './screens/Quiz';
const Stack = createStackNavigator();


const dictionary = require('./dictionary.json');

export default class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      question : null,
      options : [],
      showAnswer : false,
      bg : ['gray', 'gray', 'gray', 'gray'],
      correctIncorrect : [0, 0],
      remaining: 0
    }
  }

  componentDidMount() {
    this.newWord();
  }

  shuffleArray(array) {
    for (let i = array.length - 1; i > 0; i--) {
      const j = Math.floor(Math.random() * (i + 1));
      [array[i], array[j]] = [array[j], array[i]];
    }
  }

  newWord() {
    let keys = Object.keys(dictionary);
    let question = keys[ keys.length * Math.random() << 0];


    let options = [dictionary[question]];  // first option is the correct one
    for (let i=0; i<3; i++) {  // add three other options
      options.push( dictionary[keys[ keys.length * Math.random() << 0]] );
    }
    this.shuffleArray(options);  // then shuffle

    this.setState({
      question: question,
      options: options,
      bg: ['gray', 'gray', 'gray', 'gray'],
      remaining: keys.length
    });
  }

  isCorrect(question, answer) {
    return dictionary[question] == answer;
  }

  processAnswer(answer) {
    let bg = this.state.bg;
    let correctIncorrect = this.state.correctIncorrect;
    
    for (let i=0; i<4; i++) {
      if (this.isCorrect(this.state.question, this.state.options[i])) {
        bg[i] = 'green';
      } else if (i == answer){
        bg[i] = 'red';
      } else {
        bg[i] = 'gray';
      }
    }

    if (this.isCorrect(this.state.question, this.state.options[answer])) {
      correctIncorrect[0]++;
      // remove key from dictionary
      delete dictionary[this.state.question];
    } else {
      correctIncorrect[1]++;
    }
    
    this.setState({
      bg: bg,
      correctIncorrect: correctIncorrect
    });  
  }

  render() {   
    const config = {
      velocityThreshold: 0.3,
      directionalOffsetThreshold: 800
    }; 

    return (      
      <GestureRecognizer
        onSwipeLeft={() => this.newWord()}
        config={config}
        style={styles.container}
      >
        <View style={styles.header}>
          <Text>Richtig: {this.state.correctIncorrect[0]}</Text>
          <Text>Falsch: {this.state.correctIncorrect[1]}</Text>
          <Text>Verbleibend: {this.state.remaining}</Text>
        </View>
        <Text>{this.state.question}</Text>
        
        <TouchableOpacity 
          style={[{backgroundColor: this.state.bg[0]}, styles.button]} 
          activeOpacity={1}
          onPress={() => {
            this.processAnswer(0);
          }
        }>
          <Text>{this.state.options[0]}</Text>
        </TouchableOpacity>

        <TouchableOpacity 
          style={[{backgroundColor: this.state.bg[1]}, styles.button]}
          activeOpacity={1}
          onPress={() => {
            this.processAnswer(1);
          }
        }>
          <Text>{this.state.options[1]}</Text>
        </TouchableOpacity>

        <TouchableOpacity 
          style={[{backgroundColor: this.state.bg[2]}, styles.button]} 
          activeOpacity={1}
          onPress={() => {
            this.processAnswer(2);
          }
        }>
          <Text>{this.state.options[2]}</Text>
        </TouchableOpacity>

        <TouchableOpacity 
          style={[{backgroundColor: this.state.bg[3]}, styles.button]} 
          activeOpacity={1}
          onPress={() => {
            this.processAnswer(3);
          }
        }>
          <Text>{this.state.options[3]}</Text>
        </TouchableOpacity>
        
        <View style={styles.footer}>
        <Text text-align="right">Swipe right for a new word</Text>
        
        </View>
      </GestureRecognizer>
    );
  }
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: "center",
    justifyContent: "center",
    backgroundColor: 'white'
  },
  header: {
    flex: 1,
    flexDirection: 'row',
    alignItems: "center",
    justifyContent: "space-around",
    position: 'absolute',
    height:30,
    width: '100%',
    marginBottom: 200,
    top: 30,
    backgroundColor: 'white'
  },
  footer: {
    flex: 1,
    flexDirection: 'row',
    alignItems: "center",
    justifyContent: "space-around",
    position: 'absolute',
    height:30,
    width: '100%',
    marginBottom: 20,
    bottom: 30,
    backgroundColor: 'red'
  },
  button: {
    borderRadius: 15,
    width: 200,
    margin: 20,
    height: 50,
    alignItems: "center",
    justifyContent: "center"
  }
});

