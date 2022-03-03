# hanover_flipdot_esphome

This is a component for [ESPHome](https://esphome.io/) which presents a Hanover flipdot electromechanical display as a native ESPHome display, meaning you can draw arbitary text and graphics, and it will update automatically as if it was any other display. These displays were used extensively in buses in the UK and other countries, and turn up on eBay, Facebook Marketplace and other places at pretty low prices.  Be aware that they are quite heavy, particularly the large bus front destination displays, and the window on the front is toughened glass, so they are risky to ship. The 28x14 rear bus display is very compact though, and has room inside for a mains power supply and an IEC socket on the side.

The code is intended to be used with the "7090" control board, which communicates over RS485 using a proprietary protocol which is [quite well documented](https://engineer.john-whittington.co.uk/2017/11/adventures-flippy-flip-dot-display/) online. I think this board was used in the majority of the signs made.

I'm a huge fan of ESPHome and for something like this it's particularly well-suited, because with just a few easy lines of config you can have it displaying all sorts of data, eg. listen to an MQTT topic, sync sensor data from Home Assistant, or attach a sensor to the ESP and use that directly to trigger something.

## Hardware

I have used this with several displays in various sizes, and have used it with multiple ESPs and all combinations seem to work well.

The easiest way to connect them up is using a pre-built RS485-TTL module, which can be bought off AliExpress or eBay very cheaply. 

On the 7090 board the RS485-in is pins 1 A(+) and 2 is B(-). If the system is still wired up, A+ will be red. Connect these to the appropriate pins of your RS485 module. Terminate the RS485 output (pins 3 and 4) using a resistor, approx 120 ohms.  Then connect the RS485 module to whatever ESP device you are using. We are only transmitting data, so RXD can be left unconnected. If you module has DE/RE pins then they will need to be connected to put it in driver mode (I think connecting them together and pulling high does that).

(This is a WIP, I will add photos etc soon)

## Configuration 

I use ESPHome as an add-on within Home Assistant, and these instructions reflect that.

* Create a new folder `custom_components` within the `esphome` folder, and then copy the `hanover_flipdot` folder into it.  
* Here's a partial config - it assumes you already know how to configure and upload an ESPHome device - if not then they have some great Getting Started guides!

``` yaml
external_components:
  - source:
      type: local
      path: custom_components # The name of the folder we created

uart:
  - id: uart1
    tx_pin: GPIO2
    baud_rate: 4800
  
display:
  - platform: hanover_flipdot
    id: flipdot1
    uart_id: uart1
    
    width: 28 
    height: 14
    # Self-explanatory!

    address: 0x01 
    # This should match the rotary switch on the control board
    
    update_interval: 10s
    # This is how often the display is updated. The flipdots take some time to update, it makes makes a whining noise, and presumably does stress the components somewhat, so it can't be done too rapidly, especially on larger displays. You will need to adjust this depending on your use. eg. if displaying a temperature you might only need to update it every couple of minutes.

    offset: 1 
    # Shifts the display up or down by 1 pixel. I need to do this on only one of my displays and I haven't yet figured out why...
    
    rotation: 180 
    # This is a built-in feature of ESPHome and works off the display buffer, the display cases are wedge shaped and when freestanding are best sat upside-down!

# Below is all pretty standard ESPHome display code. I'd recommend buying a cheap mini OLED display to get used to programming it.

    pages:
      - id: page1
        lambda: |-
          it.print(it.get_width() /2, it.get_height()/2, id(_0705N1E1), TextAlign::CENTER, "Hello");
          
      - id: page2
        lambda: |-
          it.print(it.get_width() /2, it.get_height()/2, id(_0705N1E1), TextAlign::CENTER, "World");

interval:
 - interval: 10s
   then:
     - display.page.show_next: flipdot1
     - component.update: flipdot1

```

