import React from 'react';

import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';

import Home from './screens/Home';
import Quiz from './screens/Quiz';

const Stack = createStackNavigator();

function NavStack() {
  return (
    <Stack.Navigator
      screenOptions={{
        headerShown: false,
        headerTitleAlign: 'center',
        headerStyle: {
          backgroundColor: '#621FF7',
        },
        headerTintColor: '#fff',
        headerTitleStyle :{
          fontWeight: 'bold',
        }
      }}
    >
      <Stack.Screen 
        name="Home" 
        component={Home} 
        options={{ 
          title: 'Home2',
          animationEnabled: false
        }}
      />
      <Stack.Screen 
        name="Quiz" 
        component={Quiz} 
        options={{ 
          title: 'Quiz',
          animationEnabled: false
        }}
      />
    </Stack.Navigator>
  );
}

export default function App() {
  return (
    <NavigationContainer>
      <NavStack />
    </NavigationContainer>
  );
}

