# A 3D-Scanner using Hobby Servos and an IR rangefinder
Stella Stark and Nathan Faber
## Intro 
This project explores one possible way of making a 3d scanner or rather a 3d-scan from a given point. This project shows how this can be done with relatively few low-cost materials. The project uses an arduino, 2 hobby-size servos and one [IR rangefinder](https://www.pololu.com/product/1137) to measure distance. A scan will be made via a pan/tilt mechanism that allows the sensor to read a variety of different distances for different points within the range of the mechanism. 

Here’s a photo of our final results of a scan of the letter “O”

RESULT PHOTO!
<p align="center">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="49%">
  <img src="https://pocket-syndicated-images.s3.amazonaws.com/607f458ececf6.jpg" width="49%">
  <br/>
  <i>Figure 0 </i>
</p>

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
  ```
- This approach stores consecutive readings rather than summing them (increased memory usage) and takes more time

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
