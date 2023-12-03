# Tamagotchi-like Life Simulation with UXN

## Overview

This open-source project aims to create a simple life simulation reminiscent of Tamagotchi using UXN, a minimalistic virtual machine language. UXN is developed by [Hundred Rabbits](https://100r.co/site/home.html) and you can learn more about it [here](https://100r.co/site/uxn.html).


## Rules 

For each stat, 100 is max and 0 is lowest. 
Being at 100 is good. 

**stats**:
* happy: the creature's happiness. Will die if < 0
* hunger: the creature's hunger
* strain: the creature's strain

The creature will also poop and need cleaning sometimes.  

* When eating, we loose 10 poop-counter points 
* If goes asleep by its own, we loose 10 happy 
* Loose 2% happy per min by poop
* Gain 1 happy per min if hunger>50 and strain>50

### Egg

Nothing to do here, just wait. 

### Baby

Will start with hunger at 25 and strain at 19.

**happy**: if hungry or tired: -5x + 100

**hunger**: -75/30x + 100 = hunger at 25 after 30min  

**strain**: -75/40x + 100 = strain at 25 after 10min 

**poop**: -5x + 100 = a poop per 20min

Will take a nap after 10min by itself, and then wake up in 10min. 

## Contributing

We welcome contributions! Feel free to fork the repository, make your changes, and submit a pull request.

## Issues

If you encounter any issues or have suggestions, please open an issue on the GitHub repository.

## License

This extension is released under the CC0 1.0 Universal (CC0 1.0) Public Domain Dedication. You are free to use, modify, and distribute this software for any purpose without any restrictions. See the [LICENSE](LICENSE) file for more details.

## Acknowledgments

Special thanks to the UXN community for its invaluable support and contributions.

