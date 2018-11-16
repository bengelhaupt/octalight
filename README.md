<h5><a href="http://bensoft.de/projects/smartmirror">Visit my webpage! (images etc.)</a></h5>
<h1>OctaLight</h1>
						<h2>Description</h2>
						<p>
							OctaLight is a DIY project. It consists of an led strip wired in an octagon shape with a cross in the middle of it. It is powered by a NodeMCU chip with WiFi capabilities. Through implementation of a simple HTTP server, the light reacts to GET requests. It can be controlled via the in-built web control panel.
						</p>
						<p>
							Required libraries:<br>
							<ul>
								<li><a href="https://github.com/Makuna/NeoPixelBus">NeoPixelBus by Makuna</a></li>
							</ul>
						</p>
						<h2>GET request endpoints</h2>
						<p>
							<table class="bordered">
								<tr>
								<td><b>Route</b></td>
								<td><b>Description</b></td>
								<td colspan="2"><b>Arguments</b></td>
								<td><b>Response</b></td>
								</tr>
								<tr>
								<td>*</td>
								<td>&nbsp;</td>
								<td>page=[<em>true</em> | <em>false</em>]</td>
								<td>Whether to respond with the control panel.</td>
								<td><em>success or error message</em></td>
								</tr>
								<tr>
								<td>/</td>
								<td>Show the control panel.</td>
								<td colspan="2">&nbsp;</td>
								<td><em>control panel</em></td>
								</tr>
								<tr>
								<td>/get</td>
								<td>Get the average light color.</td>
								<td colspan="2">&nbsp;</td>
								<td>color in <em>RRGGBBWW</em></td>
								</tr>
								<tr>
								<td>/on</td>
								<td>Set the light color to a warm white.</td>
								<td colspan="2">&nbsp;</td>
								<td>&nbsp;</td>
								</tr>
								<tr>
								<td>/off</td>
								<td>Set the light color to black (off).</td>
								<td colspan="2">&nbsp;</td>
								<td>&nbsp;</td>
								</tr>
								<tr>
								<td rowspan="3">/color</td>
								<td rowspan="3">Set the light color.</td>
								<td>color=[<em>RRGGBBWW</em>] (required)</td>
								<td>The color.</td>
								<td rowspan="3">&nbsp;</td>
								</tr>
								<tr>
								<td>transition=[<em>int</em>]</td>
								<td>The transition time in ms.</td>
								</tr>
								<tr>
								<td>mode=[<em>all</em> |&nbsp;<em>inner</em> | <em>outer</em>]</td>
								<td>Where to set color.</td>
								</tr>
								<tr>
								<td rowspan="4">/alternate</td>
								<td rowspan="4">Alternating circles animation of different colors on the outer pixels.</td>
								<td>colors=[<em>RRGGBBWWRRGGBBWWRR...</em>] (required)</td>
								<td>The colors.</td>
								<td rowspan="4">&nbsp;</td>
								</tr>
								<tr>
								<td>duration=[<em>int</em>]</td>
								<td>Duration for one color in ms.</td>
								</tr>
								<tr>
								<td>inner=[<em>RRGGBBWW</em>]</td>
								<td>The color on the inner pixels.</td>
								</tr>
								<tr>
								<td>cycles=[<em>int</em>]</td>
								<td>Cycle count. "-1" for infinity.</td>
								</tr>
								<tr>
								<td rowspan="3">/ripple</td>
								<td rowspan="3">Ripple animation from in to out of different colors.</td>
								<td>colors=[<em>RRGGBBWWRRGGBBWWRR...</em>] (required)</td>
								<td>The colors.</td>
								<td rowspan="3"></td>
								</tr>
								<tr>
								<td>duration=[<em>int</em>]</td>
								<td>Duration for one color in ms.</td>
								</tr>
								<tr>
								<td>cycles=[<em>int</em>]</td>
								<td>Cycle count. "-1" for infinity.</td>
								</tr>
								<tr>
								<td rowspan="2">/spectrum</td>
								<td rowspan="2">Waving through the RGB spectrum.</td>
								<td>duration=[<em>int</em>]</td>
								<td>Duration for one cycle in ms.</td>
								<td rowspan="2">&nbsp;</td>
								</tr>
								<tr>
								<td>cycles=[<em>int</em>]</td>
								<td>Cycle count. "-1" for infinity.</td>
								</tr>
								<tr>
								<td>/randomness</td>
								<td>Random colors on all pixels.</td>
								<td>frequency=[<em>int</em>]</td>
								<td>Number of new random pixels per second.</td>
								<td>&nbsp;</td>
								</tr>
								<tr>
								<td rowspan="3">/starlight</td>
								<td rowspan="3">A sky full of stars.</td>
								<td>color=[<em>RRGGBBWW</em>]</td>
								<td>Pixel color.</td>
								<td rowspan="3">&nbsp;</td>
								</tr>
								<tr>
								<td>frequency=[<em>int</em>]</td>
								<td>Number of pixel switches per minute.</td>
								</tr>
								<tr>
								<td>max=[<em>int</em>]</td>
								<td>Maximum number of pixels on at the same time.</td>
								</tr>
								<tr>
								<td rowspan="4">/stroboscope</td>
								<td rowspan="4">Fast-flashing stroboscopic light.</td>
								<td>colors=[<em>RRGGBBWWRRGGBBWWRR...</em>] (required)</td>
								<td>The colors.</td>
								<td rowspan="4">&nbsp;</td>
								</tr>
								<tr>
								<td>frequency=[<em>int</em>]</td>
								<td>Number of flashes per second.</td>
								</tr>
								<tr>
								<td>blacks=[<em>int</em>]</td>
								<td>Number of black phases between flashes.</td>
								</tr>
								<tr>
								<td>mode=[<em>random</em> | <em>random_spectrum&nbsp;</em>| <em>order</em>]</td>
								<td>
								<p>The ordering mode of the <em>colors.</em><br /><em>random:&nbsp;</em>random order<br /><em>random_spectrum:</em>&nbsp;random colors<br /><em>order:</em>&nbsp;<em>or</em>dered as in&nbsp;<em>colors</em></p>
								</td>
								</tr>
							</table>
						</p>
						<h2>Structure</h2>
						<p>
							Pixel zero is situated at the controller and the outer pixels (red) range counterclockwise back to it. From there the inner pixels (green) form a bow. Not all the strips can be connected directly: There has to be a cable connection to connect the two inner strands (black-green).
						</p>
						<h2>Parts list</h2>
						<p>
							<ul>
								<li>
									Wooden bars as base (5m length, 1-2cm wide) [5€]
								</li>
								<li>
									<a href="https://www.aliexpress.com/item/Best-Price-1m-5m-Addressable-SK6812-RGBW-led-strip-WWA-4-Color-in-1-Led-Waterproof/32768952249.html">Per-LED-controllable LED strip with 4 colors (5m, 30 LEDs/m, temperature per your liking)</a> [17€]
								</li>
								<li>
									<a href="https://www.aliexpress.com/item/5V-12V-24V-LED-Power-Supply-2A-3A-5A-7A-8A-10A-For-5V-12V-24V/32694173521.html">Power adapter for the strip (5V 8A)</a> [8€]
								</li>
								<li>
									<a href="https://www.aliexpress.com/item/ASCELINA-1-pcs-DC-Power-female-Connectors-Plug-5-5-x-2-1mm-Jack-Adapter-Connector/32799252247.html">Wire adapter</a> [&lt;1€]
								</li>
								<li>
									<a href="https://www.aliexpress.com/item/NodeMcu-Lua-WIFI-development-board-based-on-the-ESP8266-Internet-of-things/32443964726.html">NodeMCU ESP8266 development board</a> [3€]
								</li>
								<li>
									Wires, tape, glue, tools
								</li>
							</ul>
						</p>
            
