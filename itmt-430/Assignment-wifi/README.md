<h1>Simon Game with 4 leds and 4 buttons</h1>
<h2>Assignment-wifi</h2>
### Algorithm
<img src="https://github.com/jkim264/jian/blob/master/itmt-430/Assignment-wifi/picture/Simon_Game-wifi.png" width = 50%></img>
### Advanced Web Server & Simon Game code
<p>I use Advanced Web Server cod of sample. And I change the part of that.</p>
### How to play & How to see on Web
<p>To start Simon Game, player has to push the yellow button.</p>
<p>Then, One note after another is repeated three times.</p>
<p>Basically, it's same to usual simon game.</p>
<p>If the red color is bright, then player should push the red button.</p>
<p>If player connect to Ip adress on Serial Print window, they can see what they pushed.</p>
### How can I do
<p>In Arduino code, I wrote draw() function like sample code.</p>
<p>In the function, computer read buttonState value using digitalRead() function.</p>
<p>Then, if the red button's buttonState is LOW, then changed the .svg file to red color.</p>
