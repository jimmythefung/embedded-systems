from neopixel import NeoPixel
from utime import sleep
import random

NEOPIXEL_PIN = 0
NUMBER_PIXELS = 60

# Color RGB values
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
orange = (255, 60, 0) # Gamma corrected from G=128 to be less like yellow
yellow = (255, 150, 0)
indigo = (75, 0, 130) # purple?
violet = (138, 43, 226) # mostly pink
off = (0, 0, 0)

class Strip():
    def __init__(self, pixel_pin, pixel_length):
        self.strip = NeoPixel(machine.Pin(pixel_pin), pixel_length)
        self.pixel_pin   = pixel_pin
        self.pixel_length= pixel_length


    def blink(self, pixels, interval_seconds, color):
        self.set_pixels(pixels, color)
        sleep(interval_seconds)
        self.set_pixels(pixels, off)
        sleep(interval_seconds)


    def set_pixels(self, pixels, color):
        for p in pixels:
            if 0 <= p < self.pixel_length:
                self.strip[p] = color
        self.strip.write()


    def turn_on(self, color):
        pixels = list(range(self.pixel_length))
        self.set_pixels(pixels, color)


    def demo_blink_odd_even_colors(self, color1, color2, seconds):
        # Example usage: strip.demo_blink_odd_even_colors(violet, orange, 1)
        even_pixels = [n for n in range(self.pixel_length) if n%2==0]
        odd_pixels = [n for n in range(self.pixel_length) if n%2==1]
        b = False
        while True:
            if b is True:
                self.blink(even_pixels, seconds, color1)
            else:
                self.blink(odd_pixels, seconds, color2)
            b = True if b is False else False


    def demo_back_and_forth(self, color, t, light_width):
        self.moveup(color, t, light_width)
        self.movedown(color, t, light_width)


    def demo_deccelerated_bounce(self, light_width):
        t_up = 0
        t_down = 0.001
        self.moveup(green, t_up, light_width, is_deccel=True)
        self.movedown(green, t_down, light_width)


    def moveup(self, color, t, width, is_deccel=False):
        i = 0
        while i < self.pixel_length:
            pixels = [i+j for j in range(width)]
            if is_deccel:
                t += 0.0005
            self.blink(pixels, t, color)
            i += 1


    def movedown(self, color, t, width):
        i = self.pixel_length-1
        while i >= 0:
            pixels = [i+j for j in range(width)]
            self.blink(pixels, t, color)
            i-=1


    def inward(self, color, t, width):
        left = 0
        right = self.pixel_length-1
        while right >= 0:
            right_pixels = [right+j for j in range(width)]
            left_pixels  = [left-j for j in range(width)] 
            self.blink(right_pixels, t, color)
            self.blink(left_pixels, t, color)
            right-=1
            left+=1


    def get_dimmed_light(self, brightness, color):
        # Constraint: 0 < brightness <= 1
        if 0 < brightness <= 1:
            return tuple([int(brightness * value) for value in color])
        return color


def example():
    strip = Strip(NEOPIXEL_PIN, NUMBER_PIXELS)
    color = strip.get_dimmed_light(0.2, blue)
    while True:
        strip.demo_back_and_forth(color, t=0.01, light_width=10)
        strip.demo_deccelerated_bounce(light_width=1)
        strip.inward(color=indigo, t=0.01, width=8)


#########################
# Main Program
#########################
strip = Strip(NEOPIXEL_PIN, NUMBER_PIXELS)
color_wheel = [red, green, blue, orange, yellow, indigo, violet]

while True:
    # Obtain color and brightness
    mycolor = random.choice(color_wheel)
    b = round(random.uniform(0.1, 1), 2)
    color = strip.get_dimmed_light(brightness=b, color=mycolor)
    
    # Set the light
    strip.turn_on(color)
    
    t = round(random.uniform(0.5, 4), 2)
    sleep(t)
