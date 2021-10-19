# mRNA vaccine badge

A small badge showing a schematic of an mRNA vaccine nanoparticle and blinking the RNA sequence of the Moderna or Pfizer vaccines. (Switch between vaccines by long pressing (>1s) the button to enter the menu, short pressing to switch, and long pressing to save).

There are a few units on Tindie in case you just want to buy one:  <a href='https://www.tindie.com/products/petl/mrna-vaccine-badge-rechargeable/'>tindie</a>

[![](https://github.com/petl/mrna_vaccine_badge/blob/main/documents/photos_sorted/IMG_20210912_193253.jpg)
The unit in action (VIDEO)](https://vimeo.com/603445821 "badge in action")


The RNA for the moderna vaccine is from is from https://github.com/NAalytics/Assemblies-of-putative-SARS-CoV2-spike-encoding-mRNA-sequences-for-vaccines-BNT-162b2-and-mRNA-1273 with an added poly-A tail of similar length to the Pfizer one (just a guess). The Pfizer sequence is from https://berthub.eu/articles/11889.doc

There are always two nucleotides shown, one on the left 4 LEDs and one on the right:

    A = Green
    G = Yellow
    U/T = Red
    C = Blue

The micro USB on the back is for recharging a LiPo cell. The circuitry features over charge-/discharge, temperature and short circuit protection.

It takes a couple of minutes to run through the whole sequence, but the battery should last over 100hours. It plays the sequence once and goes to sleep afterwards, the vaccine can be changed (Moderna / Pfizer) by keeping the button pressed while turning the badge on. 


![](https://github.com/petl/mrna_vaccine_badge/blob/main/documents/photos_sorted/IMG_20210912_202905.jpg)
Backside with LiPo cell soldered on]


The sequences are stored encoded into 2 bits per nucleotide using [encode_genome.py](encode_genome.py).

### Schematics:
![](https://github.com/petl/mrna_vaccine_badge/blob/main/documents/schematics_v1.2.png)
Version 1.2 of the schematics. 

* Microcontroller can be programmed via Arduino over the Atmel ICSP programming pinout. Can be drop-in replaced with ATTiny44/ATTiny84.
* Li-ion LiPo battery has a charge/discharge/overtemperature and short circuit protection.
* Battery charging indicators on the backside. 
* Micro USB for charging. 
* Button (Momentary Switch) is hooked to an interrupt. 
* LEDs are not multi(/charlie)plexed for easier programming. 

### Components

BOM (Bill of Materials) to be added later on if needed. 

