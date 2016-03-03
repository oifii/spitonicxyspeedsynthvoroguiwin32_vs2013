/*
 * Copyright (c) 2015-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
//
//  ControlSnapToScaleTestSynth.cpp
//  TonicDemo
//
//  Created by Morgan Packard on 3/24/13.

//

#ifndef TonicDemo_ControlSnapToScaleTestSynth_cpp
#define TonicDemo_ControlSnapToScaleTestSynth_cpp

#include "Tonic.h"

using namespace Tonic;

class ControlSnapToScaleTestSynth : public Synth{
  public:
  ControlSnapToScaleTestSynth(){
   
    float scaleArr[6] = {0,2,3,7,10};
    vector<float> scale(scaleArr, scaleArr + sizeof scaleArr / sizeof scaleArr[0]);
    
    ControlGenerator speed = addParameter("speed", 0.85); //original
	//ControlGenerator speed = addParameter("speed", 2.2); //spi

    ControlMetro switchIt = ControlMetro().bpm(30 * speed);
    
	/*
    ControlMetro metro = ControlMetro().bpm(
        ControlStepper()
          .bidirectional(1)
          .start(550)
          .end(650)
          .step(100)
          .trigger(switchIt)
        * speed
      );
    */
	ControlMetro metro = ControlMetro().bpm(
		ControlStepper()
		.bidirectional(1)
		.start(550)
		.end(1050)
		.step(100)
		.trigger(switchIt)
		* speed
		); //spi

    ADSR env = ADSR(0.01, 0.5, 0, 0)
      .trigger(metro)
      .doesSustain(false)
      .legato(true)
      .decay(
        ControlStepper()
        .start(0.05)
        .end(0.5)
        .step(0.01)
        .trigger(metro)
      );
    
    //ControlGenerator stepperStart = addParameter("stepperStart") * 30 + 43; //original
	ControlGenerator stepperStart = addParameter("stepperStart", 1.0f) * 30 + 43; //spi

    ControlStepper stepper1 = ControlStepper()
    .bidirectional(1)
    .start( stepperStart )
	//.end(stepperStart + 50 * addParameter("stepperSpread")) //original
	.end(stepperStart + 50 * addParameter("stepperSpread", 1.0f)) //spi
    .step(
      ControlStepper()
      .bidirectional(1)
      .start(4)
      .end(
        ControlRandom()
        .min(9)
        .max(15)
        .trigger(
		//ControlMetro().bpm(11) //original
		ControlMetro().bpm(50) //spi
        )
      )
      .step(1)
      .trigger(metro)
    )
    .trigger(metro);
   
    ControlSnapToScale scaleSnapper1 =  ControlSnapToScale()
      .setScale(scale)
      .input( stepper1 );
    
    //Generator delayMix = ControlRandom().min(0).max(0.1).trigger(switchIt).smoothed(); //original
	Generator delayMix = ControlRandom().min(0).max(1.0).trigger(switchIt).smoothed(); //spi

    //Reverb verb = Reverb().inputHPFCutoff(200.f).decayTime(0.8).roomSize(0.1).density(0.8).wetLevel(dBToLin(-30.f)); //original
	Reverb verb = Reverb().inputHPFCutoff(200.f).decayTime(0.8).roomSize(0.1).density(0.8).wetLevel(dBToLin(-20.0f)); //spi

    Generator outputGen = ((
    
		SineWave().freq( //original
		//SawtoothWave().freq( //spi
        ControlMidiToFreq().input(scaleSnapper1 + -12).smoothed().length(
          ControlRandom().min(0).max(0.03).trigger(switchIt)
        )
      ) 
      
      +
    
      SineWave().freq(
        ControlMidiToFreq().input(scaleSnapper1 + 12)
      //) * 0.1 //original
	  ) * 0.5 //spi

    )
    
    * 0.3 * env) >> StereoDelay(0.3,0.33).feedback(0.3).dryLevel(1.0 - delayMix).wetLevel(delayMix) >> verb;
    
    setOutputGen(outputGen);
    
  }
};


TONIC_REGISTER_SYNTH(ControlSnapToScaleTestSynth);

#endif
