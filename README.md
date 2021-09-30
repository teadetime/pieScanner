# TestA 3D-Scanner using Hobby Servos and an IR rangefinder
Stella Stark and Nathan Faber
## Intro 
This project explores one possible way of making a 3d scanner or rather a 3d-scan from a given point. This project shows how this can be done with relatively few low-cost materials. The project uses an arduino, 2 hobby-size servos and one [IR rangefinder](https://www.pololu.com/product/1137) to measure distance. A scan will be made via a pan/tilt mechanism that allows the sensor to read a variety of different distances for different points within the range of the mechanism. 

Here’s a photo of our final results of a scan of the letter “O”


RESULT PHOTO!
<p align="center">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="30%">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="60%">
  <br/>
  TEST
</p>

## Testing Equipment

#### Testing Sensors
The feasibility and accuracy of our system hinges entirely on how precise and accurate of measurements we can read. Servos are very good at accurately positioning themselves however, we were curious how stable the readings from the Sharp IR Rangefinder were. 
We started out simply reading the distance as an analog value with a 10 µF capacitor between 5v and ground as per the [sensor datasheet](https://www.pololu.com/file/0J156/gp2y0a02yk_e.pdf). We tested several scenarios to obtain clean data from the sensor:
