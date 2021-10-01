## Intro 
This project explores one possible way of making a 3d scanner or rather a 3d-scan from a given point. This project shows how this can be done with relatively few low-cost materials. The project uses an arduino, 2 hobby-size servos and one [IR rangefinder](https://www.pololu.com/product/1137) to measure distance. A scan will be made via a pan/tilt mechanism that allows the sensor to read a variety of different distances for different points within the range of the mechanism. 

Here’s a photo of our final results of a scan of the letter “O” in Figure 0

RESULT PHOTO!
<p align="center">
  <img src="images/oScanSetup.jpg" width="49%">
  <img src="mages/oScan.png" width="49%">
  <br/>
  <i>Figure 0 </i>
</p>

Please see these links if oyu are looking for all files used:

[CAD](https://cad.onshape.com/documents/00404c6b682243b94410b231/w/e0565735532057e490157cf3/e/47ad195450b799a5f13700f5)

[Code](https://github.com/teadetime/pieScanner/)

## Testing Equipment
___

### Testing Sensors
The feasibility and accuracy of our system hinges entirely on how precise and accurate of measurements we can read. Servos are very good at accurately positioning themselves however, we were curious how stable the readings from the Sharp IR Rangefinder were. 
We started out simply reading the distance as an analog value with a 10 µF capacitor between 5v and ground as per the [sensor datasheet](https://www.pololu.com/file/0J156/gp2y0a02yk_e.pdf). We tested several scenarios to obtain clean data from the sensor:


__Sample once every 2 seconds__
  - This was our first test to verify the sensor was attached correctly
  - Established range of sensor from reading ~50 to ~600 out of the theoretical 0-1023 [units](https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)
  - Data obviously followed a trend but the readings were slightly different each time (+/- 5 units)

__Average values for 1 second reading every loop without delays__
- This tried to smooth our input and it did in a way but we got ridiculously low values (could have been programming error)
- Results showed more stability, but less resolution despite being stable
- We attributed error to reading too quickly (see next experiment)

__Average values for 1 second reading every loop with a loop time of ~50ms__
- This gave us average values that were similar to what our one-off readings were
- More stable than single readings but still had large jumps occasionally
- This spurred more investigation into the values that we were getting ![IMAGE SHOWING READING EVERY 50 MS with spikes]()
- Obviously there is a periodic spike that we don’t want that could be throwing our average off (See next approach)

__Trim high and low values and average with a loop time ~50ms__
- We will trim out the extremes of our readings(spikes in either direction)
- This requires sorting the values before averaging. Values are stored in an array and then sorted via [qsort](https://en.cppreference.com/w/c/algorithm/qsort) from the C Standard Library
  ``` c 
  short cmpfunc (const void * a, const void * b) {
    return ( *(short*)a - *(short*)b );
  }
  qsort(distArray, numReadings, sizeof(short), cmpfunc);
  readingCounter = 0;
  // Now Average
  for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
    readingCounter += 1;
    totalReading += distArray[i];
  }
  avgReading = totalReading/readingCounter;
  //Now Send
  Serial.println(avgReading);    Serial.print(",");
  ```
- This approach stores consecutive readings rather than summing them (increased memory usage) and takes more time
- Very Accurate and precise. Values are stable and change significantly less than other methods.

__Learning that material matters__

During these tests we were primarily measuring a large piece of cardboard perpendicular to the sensor. However, during some of the tests we used a different tissue box that totally messed up readings.
![TISSUE BOX]()
![PLOT OF SINE WAVE]()
<p align="center">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="49%">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="49%">
  <br/>
  <i>Figure 0 </i>
</p>

Figure XXXXX shows sequential readings over 5 seconds. These exhibit a huge range and are sinusoidal. This was very confusing and we are unsure how this happened except for the fact that the material was somewhat reflective. Based on this info we will be testing with just cardboard and matte surfaces.


#### Calibrating Sensor
Obviously the units that we get from the Arduino are not representative of a distance that we can understand. To understand the relationship between our sensor readings and real-world distance we will create a calibration curve looking at sensor data as a=it relates to a known measurement. This curve will be curvefit over the effective distance range (20-150cm) such that we can estimate a distance based solely on a sensor reading.

Our calibration setup used a large matte object with our sensor perpendicular to it.
![INSERT PICTURE WITH TAPE MEASURE!]()
![PICTURE OF EXCEL TABLE](Resulting data Is shown here)

From the above data we were able to curve fit an equation that related the Arduino AnalogRead voltage to the distance in meters. Figure SOMETHING shows this plot, note that it is not linear! The equation for this relationship is given in FIGURE SOMETIHNG where “v” is the AnalogRead value.
![]Insert phot of equations
![](INSERT PHOTO OD plot)

We wanted to validate how well our calibration curve was working so we took measurements a separate day with data points that were not in our curve fit. This data allowed us to compare our prediction(made using our conversion equation) to the ground truth (measurement via a ruler). FIGURE SOMTHING shows this data. Note that the predicted values are very close to the actual values(The value that is furthest off is <.5”). This validates our calibration curve.
![](FIGURE SOMETHING)

## Mechanism Design
The pan tilt mechanism was 3D-printed and designed from scratch. The CAD can be found [here](https://cad.onshape.com/documents/00404c6b682243b94410b231/w/e0565735532057e490157cf3/e/47ad195450b799a5f13700f5). We focused on making our parts small but still stiff and using press-fit connections instead of additional fasteners. We need the mechanical system to be stiff such that we can get repeatable positioning. 

Another key point is to ensure that all rotation is around the center of the sensor. If the sensor translates in space during movement, we would need to do math to determine the new position of the sensor as opposed to having a constant location for our sensor. For example if we scanned a sphere from it’s centerpoint we would expect that our distance readings should be the same for all points. If our mechanism was designed such that rotation didn’t go through the sensor we would have different distance readings since the sensor would be translating in the sphere during a scan.

The cad model in FIGURE SOMETHING doesn’t show the sensor or servos but is roughly laid out in the correct space.
![INSERT IMAGE WITH FULL CAD]() RIGHT NEXT TO FULL IMAGE OF 3dSENSOR

The pan and tilt parts are shown in greater detail in Figure SOMETHING
![](SIDE BY SIDE OF BOTH OTHER CAD MODELS)

#### Final Circuit Picture and Diagram
![CIRCUIT DIAGRAM]()

FIGURE SOMETHING shows the layout of our circuit. This adds a button for us to start and switch between scanning modes. Our two servos are controlled via the [arduino servo library](https://www.arduino.cc/reference/en/libraries/servo/) and are wired to PWM pins on the Arduino. The IR Rangefinder is an analog sensor so its output is connected to an analog pin on the Arduino.


## Data processing
As mentioned previously while testing the sensor we did a lot of processing to the output o achieve accurate and precise results. One thing that we didn’t mention is the field-of-view of the sensor. We calibrated with large flat pieces of cardboard when in reality we will be reading the edge which may give back weird data. This is something we are ignoring for the purposes of this project but could handle on the software side or simply do multiple scans from different directions.

We mention a “scan” many times but do not define it well. A scan is the movement of our pan/tilt mechanism through a variety of pan and tilt angles such that we are receiving distance measurements for the area that we move through. This data can then be used to create a 3d mapping of those points in space and hence the object/view that our scanner is looking at.

#### Sending/Receiving the Data
The Arduino works great to do simple processing but is not appropriate for doing distance calculation, point conversions, or visualization of our scans. For this reason we are sending the basic data that includes the angle that each servo is pointing(SEE FIGURE SOMETHING LEFT), and the distance recorded by the sensor over serial to our computer. The computer receives this data (FIGURE SOMETHING) in Matlab and is able to do post-processing to convert these polar coordinates into cartesian space. (See Conversion from Polar to Cartesian for more info)
![EXAMPLE OF SENDING DATA]()
``` c
        qsort(distArray, numReadings, sizeof(short), cmpfunc);
        readingCounter = 0;
        // Now Average
        for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
          readingCounter += 1;
          totalReading += distArray[i];
        }
        avgReading = totalReading/readingCounter;
        Serial.print(panPos-panZero);    Serial.print(",");
        Serial.print(tiltPos-tiltZero);    Serial.print(",");
        Serial.print(avgReading);    Serial.print(",");
        Serial.println("0");
```
![EXAMPLE RECEIVING DATA]()
``` matlab

```

#### Preliminary Single Scan
Our scanning method consists of many single scans going from left-to-right and right-to-left(think zigzag grid). We started by doing a single scan with no tilt enabled across the letter “O”. We expect there to be 2 clusters of values with low distance readings (the parts of the O) and the rest of the points to be very far away. FIGURE SOMETHING shows our setup and the resulting distance versus angle graph.
1[INSERT SETUP PHOTO]()
![INSERT GRAPH PHOTO]()

#### Conversion from 3d Polar to Cartesian points (Stella)

``` matlab
% Conversion from Polar to Cartesian Coordinates
distance = polyval(p,values(:,3));                    % Calculate Distance in Meters
scat(:,1) = distance(:,1).* cosd(values(:,1)).*cosd(values(:,2)); % X Zalues
scat(:,2) = distance(:,1).* sind(values(:,1)).*cosd(values(:,2));  % Y Values
scat(:,3) = distance(:,1).* sind(values(:,2));                % Z Values

```

## Other notable code parts
Our entire code can be found [here](https://github.com/teadetime/pieScanner/). The key pieces have already been called out in earlier portions of this doc.

However, we also wanted to highlight some other cool features of our code. For instance in addition to averaging, sorting and trimming values, we wait for the servo to be in position fora specified amount of time. This is shown in FIGURE SOMETHING and ensures that the servo has stopped twitching and the mechanism is at rest
``` c
if(moving){
      // Make sure it's been step delay time since moving
      if((currTime - lastStepTime) > stepDelay ){
        moving = false;
  }
}
else{//NOT MOVING, this means we are reading from sensor!
      if (readingCounter >= numReadings){
            LastStepTime = currTime;
            moving = true;
        …
}
...
}
```

We also programmed a separate mode that passes our array of data over serial for plotting in Matlab. This was used for inspection of readings over time. Figure SOMETHING shows this mode.
``` c
else if(mode == 1){
    if(readingCounter < numReadings){
      distArray[readingCounter] = analogRead(distPin);
      readingCounter += 1;
      delay(readRate); //Take readings 50 ms apart
    }
    else{
     for(int i=0; i<sizeof(distArray) / sizeof(distArray[0]); i++)
     {
        Serial.print("");
        Serial.print(i);
        Serial.print(",");
        Serial.print(distArray[i]);
        Serial.print(";");
     }
     Serial.println("\n");
     qsort(distArray, numReadings, sizeof(short), cmpfunc);
     readingCounter = 0;
     // Now Average
     for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
       readingCounter += 1;
       totalReading += distArray[i];
     }
     Serial.println("\n");
     avgReading = (totalReading/readingCounter);
     Serial.println(avgReading);
        
    readingCounter = 0;
    totalReading = 0;
    delay(2000);
      
    }
  }
```

The core of our scanning code is moving the servo, checking to see if a reading should be sent and if the tilt servo should be moved up. This causes a side to side “grid” scanning motion. 

``` c
void loop() {
  currTime = millis();
if(moving){
      // Make sure its been step delay time since moving
      if((currTime - lastStepTime) > stepDelay ){
        moving = false;
      }
    }
    else{//NOT MOVING, this means we are reading from sensor!
      if (readingCounter >= numReadings){
        lastStepTime = currTime;
        moving = true;
        qsort(distArray, numReadings, sizeof(short), cmpfunc);
        readingCounter = 0;
        // Now Average
        for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
          readingCounter += 1;
          totalReading += distArray[i];
        }
        avgReading = totalReading/readingCounter;
        Serial.print(panPos-panZero);    Serial.print(",");
        Serial.print(tiltPos-tiltZero);    Serial.print(",");
        Serial.print(avgReading);    Serial.print(",");
        Serial.println("0");
        readingCounter = 0; // Reset the array so that it can be repopulated
        avgReading = 0;
        totalReading = 0;

        //NOW DO THE PANNING
        if(panningRight){
          //Set the servo to next position
          panPos += panStep;
          if(panPos > (panMax+panZero)){
            //Time to move up
            panPos = (panMax+panZero);
            movingUp = true;
            panningRight = !panningRight;
          }
        }
        else{
          panPos -= panStep;
          if(panPos < (panMin+panZero)){
            //Time to move up
            panPos = (panMin+panZero);
            movingUp = true;
            panningRight = !panningRight;
          }
        }
        panServo.write(panPos);
        //MOVE UP IF Needed
        if(movingUp){
          tiltPos += tiltStep;
          if(tiltPos > (tiltMax+tiltZero)){
            //Serial.println("ALL DONE");     //Reached the top of the scan Stop sending Message so matlab breaks
            // Do cleanup so another scan can be run
            tiltPos = (tiltMin+tiltZero);
            panPos = (panMin+ panZero);
            mode = 2;
          }
          else{
            tiltServo.write(tiltPos);
          }
          movingUp = false;
        }
      }
      // It's not time to move, do another reading
      else{
        distArray[readingCounter] = analogRead(distPin);
        readingCounter += 1;
        delay(readRate); // Don't sum readings all the time because that makes error
      }
    } 
}
```




























You can use the [editor on GitHub](https://github.com/teadetime/pieScanner/edit/gh-pages/index.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/teadetime/pieScanner/settings/pages). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
