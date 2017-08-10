//
//  main.cpp
//  HelloRapidMix
//
//  Created by mzed on 23/05/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#include <iostream>
#include "rapidmix.h"

int main(int argc, const char * argv[]) {
    
    //Machine Learning
    rapidmix::staticRegression mtofRegression; //Create a machine learning object
    rapidmix::trainingData myData;  //Create a place to hold training Data
    
    //Setting up the first element of training data
    std::vector<double> input = { 48 };
    std::vector<double> output = { 130.81 };
    myData.addElement(input, output);
    
    //More elements
    input = { 54 };
    output = { 185.00 };
    myData.addElement(input, output);
    
    input = { 60 };
    output = { 261.63 };
    myData.addElement(input, output);
    
    input = { 66 };
    output = { 369.994 };
    myData.addElement(input, output);
    
    input = { 72 };
    output = { 523.25 };
    myData.addElement(input, output);
    
    //Train the machine learning model with the data
    mtofRegression.train(myData);
    
    //Get some user input
    int newNote = 0;
    std::cout << "Type a MIDI note number.\n"; std::cin >> newNote;
    
    //Run the trained model on the user input
    std::vector<double> inputVec = { double(newNote) };
    double freqHz = mtofRegression.run(inputVec)[0];
    
    std::cout << "MIDI note " << newNote << " is " << freqHz << " Hertz" << std::endl;
    
    return 0;
    
}