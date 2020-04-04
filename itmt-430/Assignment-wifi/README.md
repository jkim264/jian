<h1>Simon Game with 4 leds and 4 buttons</h1>
<h2>Assignment-wifi</h2>
<h3> Algorithm</h3>
<img src="https://github.com/jkim264/jian/blob/master/itmt-430/Assignment-wifi/pictures/Simon%20Game-wifi.png" width = 50%></img>
<h3> Advanced Web Server & Simon Game code</h3>
<p>I use Advanced Web Server cod of sample. And I change the part of that.</p>
<h3> How to play & How to see on Web</h3>
<p>To start Simon Game, player has to push the yellow button.</p>
<p>Then, One note after another is repeated three times.</p>
<p>Basically, it's same to usual simon game.</p>
<p>If the red color is bright, then player should push the red button.</p>
<p>If player connect to Ip adress on Serial Print window, they can see what they pushed.</p>
<h3> How can I do</h3>
<p>In Arduino code, I wrote draw() function like sample code.</p>
<p>In the function, computer read buttonState value using digitalRead() function.</p>
<p>Then, if the red button's buttonState is LOW, then changed the .svg file to red color.</p>
