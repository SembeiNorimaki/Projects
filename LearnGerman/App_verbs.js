import React, { Component } from 'react';
import { StyleSheet, View, TextInput, Text, TouchableOpacity } from 'react-native';
import { Table, Row, Rows, TableWrapper, Col } from 'react-native-table-component';

//const dictionary = require('./verbs.json');

export default class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      tableHead: ['Head'],
      tableTitle: ['Ich', 'Du', 'Er/sie/es', 'Wir', 'Ihr', 'Sie'],
      tableData: [
        ['1'],
        ['a'],
        ['1'],
        ['1'],
        ['1'],
        ['a']
      ],
      answers: ['','','','','',''],
      correctAnswer = ['bin', 'bist', 'ist', 'sein', 'seid', 'sind']
    }
  }
 
  componentDidMount() {
    this.setState({tableData: [
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[0] = text;
        this.setState({answers: answers})}}/>],
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[1] = text;
        this.setState({answers: answers})}}/>],
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[2] = text;
        this.setState({answers: answers})}}/>],
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[3] = text;
        this.setState({answers: answers})}}/>],
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[4] = text;
        this.setState({answers: answers})}}/>],
      [<TextInput onChangeText={text => {
        let answers = this.state.answers;
        answers[5] = text;
        this.setState({answers: answers})}}/>]
    ]});
    // this.setState({tableData: [
    //   ['2'],
    //   ['a'],
    //   ['1'],
    //   ['1'],
    //   ['1'],
    //   ['a']
    // ]});
  }

  processAnswer() {
    console.log(this.state.answers);
  }

  render() {
    const state = this.state;
    return (
      <View style={styles.container}>
        <Table borderStyle={{borderWidth: 1}}>
          <Row data={state.tableHead} style={styles.head} textStyle={styles.text}/>
          <TableWrapper style={styles.wrapper}>
            <Col data={state.tableTitle} style={styles.title} textStyle={styles.text}/>
            <Rows data={state.tableData} style={styles.row} textStyle={styles.text}/>
          </TableWrapper>
        </Table>

        <TouchableOpacity 
          style={styles.button} 
          onPress={() => {
            this.processAnswer();
          }
        }>
          <Text>Check!</Text>
        </TouchableOpacity>

      </View>
    )
  }
}
 
const styles = StyleSheet.create({
  container: { flex: 1, padding: 16, paddingTop: 30, backgroundColor: '#fff' },
  head: { height: 40, backgroundColor: '#f1f8ff' },
  title: {  width: 50, backgroundColor: '#f6f8fa' },
  wrapper: { flexDirection: 'row' },
  row: {  height: 50, width:290},
  text: { textAlign: 'center' },
  button: {
    
    flexDirection: "row",
    
    borderRadius: 15,
    width: "100%",
    top:20,
    height: 50,
    alignItems: "center",
    justifyContent: "center",
    backgroundColor: "green"
  }
});